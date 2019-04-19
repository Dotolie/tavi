//************************ Copyright(C) New Media Life Inc. *******************
//
// Description : Byte stream class
//
//  This class is used to save battery power when a media is playing.
//  
// $Source: /home/cvsroot/TAVI020/tavi/bytestream.cpp,v $
// $Revision: 1.7 $
// $Date: 2006/06/01 06:03:07 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: bytestream.cpp,v $
// Revision 1.7  2006/06/01 06:03:07  zstein
// - fixed end-of-file bug.
//
// Revision 1.6  2006/05/10 02:41:11  zstein
// - return to previous version.
//
// Revision 1.5  2006/04/19 04:11:15  zstein
// *** empty log message ***
//
// Revision 1.4  2006/04/17 04:38:43  zstein
// - improved buffering alg.
//
// Revision 1.3  2006/04/06 04:16:31  zstein
// - fixed bug.
//
// Revision 1.2  2006/04/05 02:03:03  zstein
// - fixed bug.
//
// Revision 1.1  2006/03/20 01:44:16  zstein
// - Core of file buffering alg.
//
//*****************************************************************************

#include <stdio.h>
#include <string.h>

#include "bytestream.h"
#include "rmbasic.h"
#include "fatfs.h"
#include "fs_fat.h"

#define BYTESTREAM_DEBUG    0

#if BYTESTREAM_DEBUG
#define BS_DPRINTF      printf
#else
#define BS_DPRINTF  
#endif

#define JITTER_SIZE     4*1024*1024
#define JITTER_COUNT    (JITTER_SIZE/PAGE_SIZE)

ByteStream::ByteStream( 
    int handle, 
    unsigned long npages,
    int( *hddsleep_func )(void),
    int( *hddwakeup_func )(void) )
{
    m_npages = npages;
    
    m_streamsize = PAGE_SIZE*m_npages;
    if( m_streamsize > STREAMBUFFER_LIMIT ) {
        m_streamsize = STREAMBUFFER_LIMIT;
        m_npages = m_streamsize/PAGE_SIZE;
    }
    // The size of cache is bigger than PAGE_SIZE
    if( m_streamsize < PAGE_SIZE ) {
        m_streamsize = PAGE_SIZE;
        m_npages = 1;
    }
    m_streambuffer = new char[m_streamsize];
    m_jitterbuffer = new char[JITTER_SIZE];

    // the array of page information
    m_pageused = new PageInfo[m_npages];
    memset( (void*)m_pageused, 0, sizeof(PageInfo)*m_npages );

    m_wjittercount = 0;
    m_wjitterpos = 0;
    m_jitterstop = false;
    m_cursec = 0;
    m_curmsec = 0;
    m_rstreampos = 0;
    m_wstreampos = 0;
    m_curpage = -1;
    m_dist = 0;
    m_eof = false;
    m_hddsleeped = false;
    m_hddsleep_func = hddsleep_func;
    m_hddwakeup_func = hddwakeup_func;
    memset( (void*)m_pageused, 0, sizeof(PageInfo)*m_npages );
   

    m_handle = handle;

    fatfs_ftell64( (RMuint32)m_handle, (RMint64*)&m_filepos );
    fatfs_fsize64( (RMuint32)m_handle, (RMint64*)&m_filesize );
}


ByteStream::~ByteStream( void )
{
    WakeupHDD2();
    
    if( m_streambuffer ) {
        delete[] m_streambuffer;
    }

    if( m_jitterbuffer ) {
        delete[] m_jitterbuffer;
    }

    if( m_pageused ) {
        delete[] m_pageused;
    }
}


void ByteStream::SleepHDD( void )
{
    if( !m_hddsleeped ) {
        if( m_hddsleep_func ) {
            m_hddsleep_func();
            m_hddsleeped = true;
        }
    }
}


void ByteStream::WakeupHDD( void )
{
    if( m_hddsleeped ) {
        if( m_hddwakeup_func ) {
            m_hddwakeup_func();
            m_hddsleeped = false;
        }
    }
}


void ByteStream::WakeupHDD2( void )
{
    if( m_hddsleeped ) {
        fs_fat_powerup();
        m_hddsleeped = false;
    }
}


int ByteStream::GetBytes( char* buf, unsigned long size )
{
  bool refill = false;
  int r_bytes;
  int c_size;
  int err;
  int pageno;
  bool update_w;
  char* p = buf;

  c_size = 0;
  update_w = false;

  BS_DPRINTF( "m_rstreampos: %d, m_wstreampos: %d, size: %d\n",
                m_rstreampos, m_wstreampos, size );
                
  if( m_eof ) {
    return 0;
  }

  while( size ) {
    // There are no read data.
    pageno = m_rstreampos / PAGE_SIZE;
    printf( "pageno: %d, m_curpage: %d\n", pageno, m_curpage );
    if( pageno != m_curpage ) {
      m_curpage = pageno;
      update_w = true;
    }

    if( m_curpage == m_npages ) {
      m_rstreampos = 0;
      m_wstreampos = 0;
      m_curpage = 0;
      m_dist += m_streamsize;
      // clear page info array.
      memset( (void*)m_pageused, 0, sizeof(PageInfo)*m_npages );

      if( (err=FillStreamBuffer(m_curpage)) < 0 ) {
          return err;
      }
    }

    //BS_DPRINTF( "m_curpage: %d, used: %d, wpos: %d, rpos: %d, size: %d, mdist: %d\n",
    //    m_curpage, m_pageused[m_curpage].used, 
    //    m_wstreampos, m_rstreampos, size,
    //    m_dist );
    if( m_pageused[m_curpage].used == 0 ) {
        // Fill stream buffer at the start pointer
        // error on reading a file.
        if( (err=FillStreamBuffer(m_curpage)) < 0 ) {
            return err;
        }
    }
    else {
      // Have been filled already.
      int pageno = m_wstreampos/PAGE_SIZE;
      int wpageno=0;
      int bytes=0;
      if( update_w ) {
        wpageno = m_wstreampos/PAGE_SIZE;
        if( wpageno == m_curpage ) {
          bytes = (m_pageused[m_curpage].full) ? 
                       PAGE_SIZE : m_filesize%PAGE_SIZE;
          if( (m_dist+m_wstreampos+bytes) <= m_filesize ) {
            m_wstreampos += bytes;
          }
        }
        update_w = false;
      }
      
    }

    if( m_pageused[m_curpage].full ) {
        r_bytes = (m_curpage+1)*PAGE_SIZE - m_rstreampos;
    }
    else {
        r_bytes = m_wstreampos - m_rstreampos;
    }

    // EOF or an error.
    if( r_bytes <= 0 ) {
      m_eof = true;
      break;
    }

    if( size < r_bytes ) {
      r_bytes = size;
    }

    // DO COPY
    memcpy( p, &m_streambuffer[m_rstreampos], r_bytes );

    // updates infos.
    size -= r_bytes;
    c_size += r_bytes;

    m_rstreampos += r_bytes;
    p += c_size;
  }

  return c_size;
}


/* ++

    FillStreamBuffer( void )

Description:
    The member method FillStreamBuffer() reads PAGE_SIZE bytes from a file
    and fills stream buffer with read data. If the remainder of a file is 
    less than PAGE_SIZE, FillStreamBuffer() will read the remainder and marks 
    End-Of-File.

    After reading a file, m_filepos and m_wstreampos will be increased by the
    number of bytes read(equal to PAGE_SIZE or less)

Arguments:
    None

Return:
    Returns the number of bytes successfully read. On failure, It returns negative
    value. The negative value is a error of file operation.
    
-- */
int ByteStream::FillStreamBuffer( int page )
{
    int r_bytes;
    int nread;
    int remainder;
    int err;

    
    if( m_eof ) {
        return 0;
    }

    if( m_filepos == m_filesize ) {
        return 0;
    }

    // If stream buffer is full.
    if( m_wstreampos == m_streamsize ) {
        return 0;
    }

    
    m_filepos = m_dist+page*PAGE_SIZE;
    m_wstreampos = m_filepos%m_streamsize;
    
    remainder = m_streamsize - m_wstreampos;
    r_bytes = PAGE_SIZE;
    if( remainder < PAGE_SIZE ) {
        r_bytes = remainder;
    }
    BS_DPRINTF( "FillStreamBuffer: page(%d), m_filepos(%lld), r_bytes(%d)\n", 
                page, m_filepos, r_bytes );

    
    if( m_wjittercount ) {
        WakeupHDD();
        if( m_wjitterpos > PAGE_SIZE ) {
            nread = PAGE_SIZE;
        }
        else {
            nread = m_wjitterpos;
        }
       
        memcpy( &m_streambuffer[page*PAGE_SIZE], m_jitterbuffer+m_rjitterpos, nread );
        m_wjittercount--;
        m_wjitterpos -= nread;
        m_rjitterpos += nread;
    }
    else {
      RMint64 pos;
  		if( m_rstreampos < m_rjitterpos ) {
  			return 0;
  		}
      
      m_jitterstop = false;
      fatfs_ftell64( (RMuint32)m_handle, &pos );
      if( pos != m_filepos ) {
          err = (int)fatfs_fseek64( (RMuint32)m_handle, m_filepos, SEEK_SET );
          if( err ) {
              printf( "failed to fatfs_fseek64: 0x%x\n", err );
          }
      }
      err = (int)fatfs_fread( 
              (RMuint32)m_handle, 
              (RMuint8*)&m_streambuffer[page*PAGE_SIZE], 
              (RMuint32)r_bytes, 
              (RMuint32*)&nread );

      if( err ) {
          printf( "<1>failed to read: 0x%x: nread: %d\n", err, nread );
          return 0;
      }
    }
    
    m_pageused[page].used = 1;
    if( nread == PAGE_SIZE ) {
        m_pageused[page].full = 1;
    }
    else {
        m_pageused[page].full = 0;
    }
    m_wstreampos += nread;
    m_filepos += nread;

    return nread;
}


int ByteStream::FillJitterBuffer( void )
{
    int r_bytes;
    int nread;
    int err;

    if( m_eof || m_jitterstop ) {
        return 0;
    }

    if( m_filepos == m_filesize ) {
        return 0;
    }

    // If stream buffer is full.
    if( m_wjitterpos ==  JITTER_SIZE ) {
        return 0;
    }
    err = (int)fatfs_fread( 
            (RMuint32)m_handle, 
            (RMuint8*)&m_jitterbuffer[m_wjitterpos], 
            (RMuint32)PAGE_SIZE, 
            (RMuint32*)&nread );

    if( err ) {
        printf( "<2>failed to read: 0x%x: nread: %d\n", err, nread );
        return 0;
    }
    
    m_wjitterpos += nread;

    return nread;
}


/* ++

    Schedule( void )

Description:
    The Schedule() method fills stream buffer with PAGE_SIZE bytes 
    whenever this methods is called.

    If a stream buffer is full, this methods will make a HDD sleep.

    Note that this method MUST be called in idle time.

Arguments:
    None

Return:
    None

-- */
void ByteStream::Schedule( unsigned long sec, unsigned long msec )
{
    bool bexpire = false;
    int page = m_wstreampos / PAGE_SIZE;
    int diff_sec = sec - m_cursec;
    int diff_msec = msec - m_curmsec;
    int nbytes = 1;
    static int sleepdelay;


    if( m_cursec == 0 && m_curmsec == 0 ) {
        bexpire = true;
    }
    else {
        if( diff_msec < 0 ) {
            diff_msec += 1000;
        }
        diff_msec += diff_sec*1000;
        if( diff_msec > 200 ) {
            bexpire = true;
        }
    }

    if( bexpire ) {
        m_cursec = sec;
        m_curmsec = msec;
        sleepdelay++;
        if( page < m_npages && m_pageused[page].used == 0 ) {
			nbytes = FillStreamBuffer( page );
        }
        else {
            nbytes = 0;
        }
    }

    
    if( page >= m_npages && !nbytes ) {
        int n=0;
        if( m_wjittercount < JITTER_COUNT ) {
            if( (n=FillJitterBuffer()) ) {
                m_wjittercount++;
                m_rjitterpos = 0;
            }
            else {
              m_jitterstop = true;
            }
        }

        if( m_wjittercount >= JITTER_COUNT || n == 0 ) {
          SleepHDD();
        }
    }
}


/* ++

    Seek( Seek_t whence, int pos )

Description:
    It is similar with fseek. 

Arguments:
    Arguments of Seek(whence, pos) is same as ones of fseek.

Return:
    Returns absolute position of file. On a failure returns negative value as error number.

-- */
int ByteStream::Seek( Seek_t whence, int pos )
{
    int abs_pos;
    int stream_pos;

    m_rjitterpos = 0;
    m_wjitterpos = 0;
    m_wjittercount = 0;
    m_jitterstop = false;

    stream_pos = m_dist + m_rstreampos;
    switch( whence ) {
    case BS_SEEK_SET:
        abs_pos = pos;
        break;
    case BS_SEEK_CUR:
        abs_pos = stream_pos + pos;
        break;
    case BS_SEEK_END:
        abs_pos = m_filesize;
        break;
    default:
        return -1;
    }

    

    if( abs_pos > m_filesize ) {
        abs_pos = m_filesize;
    }

    if( abs_pos < 0 ) {
        return -1;
    }

    /*
        checks whether abs_pos is in the boundary of stream buffer
    */
    int s, e;
    
    s = m_dist;
    e = m_dist + m_streamsize;

    pos = abs_pos%m_streamsize;
    m_curpage = pos / PAGE_SIZE;
    if( s > abs_pos || abs_pos >= e ) {
        // out of boundary of the stream buffer
        m_dist = abs_pos - (abs_pos%m_streamsize);
        // clear the page information array
        memset( (void*)m_pageused, 0, sizeof(PageInfo)*m_npages );
        m_wstreampos = (m_curpage+1)*PAGE_SIZE;
    }
    m_rstreampos = abs_pos%m_streamsize;

    int err;
    unsigned long long pos_r = abs_pos;

    WakeupHDD2();

    pos_r = abs_pos%PAGE_SIZE;
    pos_r = abs_pos - pos_r ;
    err = (int)fatfs_fseek64( (RMuint32)m_handle, pos_r, (RMint32)whence );
    if( err ) {
        printf( "failed to seek(%d, 0x%x)\n", abs_pos, err );
        return -err;
    }

    m_filepos = abs_pos;

    if( m_eof ) {
      if( abs_pos < m_filesize ) {
        m_eof = false;
      }
    }

    BS_DPRINTF( "<seek>abs_pos: %d, pos: %d, m_rstreampos: %d, m_wstreampos: %d, m_dist: %lld\n",
            abs_pos, pos, m_rstreampos, m_wstreampos, m_dist );

    return abs_pos;
}


unsigned long ByteStream::Tell( void )
{
    return (m_dist+m_rstreampos);
}


bool ByteStream::eof( void )
{
    return m_eof;
}
