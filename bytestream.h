//************************ Copyright(C) New Media Life Inc. *******************
//
// Description : Byte stream class
//
//  This class is used to save battery power when a media is playing.
//  
// $Source: /home/cvsroot/TAVI020/tavi/bytestream.h,v $
// $Revision: 1.4 $
// $Date: 2006/05/10 02:41:11 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: bytestream.h,v $
// Revision 1.4  2006/05/10 02:41:11  zstein
// - return to previous version.
//
// Revision 1.3  2006/04/17 04:38:43  zstein
// - improved buffering alg.
//
// Revision 1.2  2006/04/05 02:03:44  zstein
// - fixed bugs.
//
// Revision 1.1  2006/03/20 01:44:16  zstein
// - Core of file buffering alg.
//
//*****************************************************************************


#ifndef __NML_BYTESTREAM_CLASS_H__
#define __NML_BYTESTREAM_CLASS_H__

#define STREAMBUFFER_LIMIT     (1024*1024*28)      // 28MB
#define PAGE_SIZE              (1<<18)             // 256

typedef struct {
    unsigned short used;
    unsigned short full;
} PageInfo;

class ByteStream {
private:
protected:
    unsigned long long m_filepos;
    // The distance between 0 and 0 offset of stream buffer in the file.
    unsigned long long m_dist;
    unsigned long m_rstreampos;
    unsigned long m_wstreampos;
    unsigned long m_rjitterpos;
    unsigned long m_wjitterpos;
    unsigned long m_wjittercount;
    bool          m_jitterstop;

    // The size of a file.
    long long m_filesize;
    bool m_eof;
    bool m_hddsleeped;
    // The m_handle is a handle of a file.
    // m_handle is set when called constructor.
    int m_handle;
    // The size of a cache buffer.
    unsigned long m_streamsize;

    unsigned long m_cursec;  /* seconds */
    unsigned long m_curmsec; /* milliseconds */

    unsigned long m_npages;
    int m_curpage;
    PageInfo* m_pageused;

    char* m_streambuffer;
    char* m_jitterbuffer;

    int( *m_hddsleep_func )( void );
    int( *m_hddwakeup_func )( void );

    int FillStreamBuffer( int pageno );
    int FillJitterBuffer( void );

public:
    typedef enum {
        BS_SEEK_SET,
        BS_SEEK_CUR,
        BS_SEEK_END
    } Seek_t;
    
    ByteStream( 
        int handle, 
        unsigned long streamsize, 
        int( *sleep_func )(void), 
        int( *wakeup_func )(void) );
    ~ByteStream( void );

    int GetBytes( char* buf, unsigned long size );
    int Seek( Seek_t whence, int pos );
    unsigned long Tell( void );
    void Schedule( unsigned long sec, unsigned long msec /* millisecond */ );
    bool eof( void );
    void WakeupHDD( void );
    void WakeupHDD2( void );
    void SleepHDD( void );
    bool IsSleep( void ) {
        return m_hddsleeped;
    }
};

#endif /* __NML_BYTESTREAM_CLASS_H__ */
