#ifdef _WIN32
#include "StdAfx.h"
#else
#include <rmbasic.h>
#include <fatfs.h>
#include <Lib/LibWideString.h>
#endif  /* _WIN32 */

#include "MusicDB.h"

CDeviceMusicDB::CDeviceMusicDB(void)
{
	m_h_MusicLibFile = INVALID_HANDLE_VALUE;

	g_i_CurrentMemPos=0;
	g_i_TotalMusicMemSize=0;
	m_pMusicMem=NULL;
}

CDeviceMusicDB::~CDeviceMusicDB(void)
{
/*	m_a_ArtistList.RemoveAll();
	m_a_AlbumList.RemoveAll();
	m_a_GenreList.RemoveAll();
	m_a_CurrentAlbumList.RemoveAll();
		
	int i;
	int i_count = (int)m_a_MusicInfo.GetCount();

	for(i=0;i<i_count;i++)
	{
		ST_DEVICEMUSICINFO *st_pmusicinfo = (ST_DEVICEMUSICINFO *)m_a_MusicInfo.GetAt(i);
		delete st_pmusicinfo->st_musicfileinfo.s_nalbum;
		delete st_pmusicinfo->st_musicfileinfo.s_nartist;
		delete st_pmusicinfo->st_musicfileinfo.s_nfilepath;
		delete st_pmusicinfo->st_musicfileinfo.s_ngenre;
		delete st_pmusicinfo->st_musicfileinfo.s_ntitle;
		delete st_pmusicinfo;
	}

	m_a_MusicInfo.RemoveAll();


	for(i=0;i<MUSICHASHSIZE;i++)
	{
		if(m_a_MusicHash[i])
		{
			delete m_a_MusicHash[i];
		}
	}
	for(i=0;i<CATEGORYHASHSIZE;i++)
	{
		if(m_a_ArtistHash[i])
		{
			delete m_a_ArtistHash[i];
		}
		if(m_a_AlbumHash[i])
		{
			delete m_a_AlbumHash[i];
		}
		if(m_a_GenreHash[i])
		{
			delete m_a_GenreHash[i];
		}
	}
*/
	if(m_pMusicMem)
		delete[] m_pMusicMem;
}

BOOL CDeviceMusicDB::Init_Hash()
{
	MUSICHASHSIZE = 10007;
	CATEGORYHASHSIZE = 3001;

	for(int i=0;i<MUSICHASHSIZE;i++)
	{
		m_a_MusicHash[i] = NULL;
	}

	for(int i=0;i<CATEGORYHASHSIZE;i++)
	{
		m_a_ArtistHash[i] = NULL;
		m_a_AlbumHash[i] = NULL;
		m_a_GenreHash[i] = NULL;
	}

	return TRUE;
}


BOOL CDeviceMusicDB::Open_MusicLibraryFile()
{
	#ifdef _WIN32

	if(m_s_DBFilePath.IsEmpty())return FALSE;

	m_h_MusicLibFile = ::CreateFileW(m_s_DBFilePath.AllocSysString(), GENERIC_WRITE|GENERIC_READ, NULL, NULL,OPEN_EXISTING, FILE_ATTRIBUTE_HIDDEN, NULL);
	
	if(m_h_MusicLibFile==INVALID_HANDLE_VALUE)
	{
		return FALSE; // occured error
	}
	#else
	FATFS_ERROR err;
	TRACE( "Open_MusicLibraryFile\n" );
	err = fatfs_fopen( (RMuint8*)MUSICLIBRARY, _O_RDONLY, (RMuint32*)&m_h_MusicLibFile );
	if( err != FATFS_ERROR_NO_ERROR ) {
		TRACE( "FAILED TO OPEN MUSIC DATABASE(0x%x)\n", err );
		m_h_MusicLibFile = INVALID_HANDLE_VALUE;
		return FALSE;
	}
	#endif

	
	return TRUE;
}



void CDeviceMusicDB::Close_MusicLibraryFile()
{
	#if _WIN32
	if (m_h_MusicLibFile != INVALID_HANDLE_VALUE)
	{
		CLOSEHANDLE(m_h_MusicLibFile);
		m_h_MusicLibFile=INVALID_HANDLE_VALUE;
	}
	#else
	fatfs_fclose( (RMuint32)m_h_MusicLibFile );
	#endif
}






ST_DEVICEMUSICINFO *CDeviceMusicDB::Get_Music(unsigned int i_musickey)
{
	int i_musicindex = Get_MusicIndex(i_musickey);

	ST_DEVICEMUSICINFO *st_pmusicinfo = NULL;
	
	if(i_musicindex>=0)
	{
		st_pmusicinfo = (ST_DEVICEMUSICINFO *)m_a_MusicInfo.GetAt(i_musicindex);
		if(st_pmusicinfo && i_musickey==st_pmusicinfo->i_musickey)
		{
			return st_pmusicinfo;
		}
	}

	return NULL;
}

ST_DEVICEMUSICINFO *CDeviceMusicDB::Get_Music(LPWSTR s_filepath)
{
	unsigned int i_musickey = Generate_Key(s_filepath);
	int i_musicindex = Get_MusicIndex(i_musickey);
	ST_DEVICEMUSICINFO *st_pmusicinfo = NULL;
	
	if(i_musicindex>=0)	
		st_pmusicinfo = (ST_DEVICEMUSICINFO *)m_a_MusicInfo.GetAt(i_musicindex);

	return st_pmusicinfo;
}



BOOL CDeviceMusicDB::Exist_Music(unsigned int i_musickey)
{
	int i_musicindex = Get_MusicIndex(i_musickey);

	ST_DEVICEMUSICINFO *st_pmusicinfo = NULL;
	
	if(i_musicindex>=0)
	{
		st_pmusicinfo = (ST_DEVICEMUSICINFO *)m_a_MusicInfo.GetAt(i_musicindex);
		if(st_pmusicinfo && i_musickey==st_pmusicinfo->i_musickey)
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CDeviceMusicDB::Exist_Music(LPWSTR s_filepath)
{
	unsigned int i_musickey = Generate_Key(s_filepath);
	int i_musicindex = Get_MusicIndex(i_musickey);
	ST_DEVICEMUSICINFO *st_pmusicinfo = NULL;
	
	if(i_musicindex>=0)	
		return TRUE;

	return FALSE;
}



BOOL CDeviceMusicDB::Load_AllMusicFromLibFile()
{
	if(Open_MusicLibraryFile()==TRUE)
	{
		DWORD i_read=0;

		DWORD pos1 = SetFilePointer (m_h_MusicLibFile, 0,0, FILE_BEGIN);  
		BOOL bResult = ReadFile(m_h_MusicLibFile,&m_st_MusicDBHeader,sizeof(ST_DEVICEHEADER),&i_read,NULL);
		if (bResult &&  i_read == 0) 
		{ 
			TRACE( "FAIL1 bResult: %d, i_read: %d\n", bResult, i_read );
			Close_MusicLibraryFile();
			return FALSE;
		}

		if(m_st_MusicDBHeader.i_majorversion != MUSICMAJORVER || 
			m_st_MusicDBHeader.i_minorversion != MUSICMINORVER)
		{
			TRACE( "FAIL2 Ver.%d.%d\n", 
				m_st_MusicDBHeader.i_majorversion,
				m_st_MusicDBHeader.i_minorversion );
			Close_MusicLibraryFile();
			return FALSE;
		}

		pos1 = SetFilePointer(m_h_MusicLibFile,(m_st_MusicDBHeader.i_totalcount*sizeof(ST_DEVICEFILEINFO)+sizeof(ST_DEVICEHEADER)),0, FILE_BEGIN);  

		//if (pos1 != INVALID_SET_FILE_POINTER) 
		if ( !pos1 ) 
		{
			bResult = ReadFile(
						m_h_MusicLibFile,
						&g_i_TotalMusicMemSize,
						sizeof(g_i_TotalMusicMemSize),
						&i_read,
						NULL);

			//122090
			if(g_i_TotalMusicMemSize)
			{
				TRACE("g_i_TotalMusicMemSize = %d \r\n",g_i_TotalMusicMemSize);

				m_pMusicMem = new char[g_i_TotalMusicMemSize];
				memset(m_pMusicMem,0, g_i_TotalMusicMemSize);
				g_i_CurrentMemPos = 0;
			}
			else
			{
				Close_MusicLibraryFile();
				return FALSE; 
			}


			TRACE("m_st_MusicDBHeader.i_totalcount = %d \r\n",m_st_MusicDBHeader.i_totalcount);

			if(strcmp(m_st_MusicDBHeader.s_cpcode,"NML_MDB"))return FALSE;

			int i_count = m_st_MusicDBHeader.i_totalcount;

			pos1 = SetFilePointer (m_h_MusicLibFile, sizeof(ST_DEVICEHEADER),0, FILE_BEGIN);  

			for(int i=0;i<i_count;i++)
			{
				ST_DEVICEFILEINFO st_musicfileinfo;
				BOOL bResult = ReadFile(
								m_h_MusicLibFile,
								&st_musicfileinfo,
								sizeof(ST_DEVICEFILEINFO),
								&i_read,
								NULL);
				if (bResult &&  i_read == 0) 
				{ 
					break;
				}

				Add_MusicToLib(st_musicfileinfo);
			}
		}
		else
		{
			// Music DB Memory size가 없다. 
			Close_MusicLibraryFile();
			return FALSE;
		}

		Close_MusicLibraryFile();
	}
	else
	{
		return FALSE;
	}

	TRACE("g_i_CurrentMemPos = %d \r\n",g_i_CurrentMemPos);

	return TRUE;

}

static unsigned long Align4( unsigned long addr )
{
	addr = (addr&~0x03) + 4;
	return addr;
}

ST_DEVICEMUSICINFO *CDeviceMusicDB::Add_MusicToLib( ST_DEVICEFILEINFO &st_fileinfo)
{
	ST_DEVICEMUSICINFO *st_pMusicinfo=NULL;

	unsigned int i_musickey = Generate_Key(st_fileinfo.s_filepath);

	if(Get_MusicIndex(i_musickey)==-1) // 기존에 없는 경우 
	{
		int i_musicindex=-1;

		ST_DEVICEMUSICINFO *st_pMusicinfolast;
		st_pMusicinfolast = (ST_DEVICEMUSICINFO *)m_a_MusicInfo.GetTail();

		if(st_pMusicinfolast)
		{
			i_musicindex = m_a_MusicInfo.GetCount();//Get_MusicIndex(st_pMusicinfolast->i_musickey);
		}

		if(i_musicindex==-1)
		{
			i_musicindex=0;
		}


//		st_pMusicinfo = new ST_DEVICEMUSICINFO;
//		memset(st_pMusicinfo,0,sizeof(ST_DEVICEMUSICINFO));

		st_pMusicinfo = (ST_DEVICEMUSICINFO *)(m_pMusicMem+g_i_CurrentMemPos);
		g_i_CurrentMemPos+=sizeof(ST_DEVICEMUSICINFO);
		g_i_CurrentMemPos = Align4( g_i_CurrentMemPos );
		
		st_pMusicinfo->st_musicfileinfo.s_nalbum = (WCHAR *)(m_pMusicMem+g_i_CurrentMemPos);
		g_i_CurrentMemPos+=sizeof(WCHAR) * (wcslen(st_fileinfo.s_album)+1);
		g_i_CurrentMemPos = Align4( g_i_CurrentMemPos );
		
		st_pMusicinfo->st_musicfileinfo.s_nartist = (WCHAR *)(m_pMusicMem+g_i_CurrentMemPos);
		g_i_CurrentMemPos+=sizeof(WCHAR) * (wcslen(st_fileinfo.s_artist)+1);
		g_i_CurrentMemPos = Align4( g_i_CurrentMemPos );
		
		st_pMusicinfo->st_musicfileinfo.s_nfilepath = (WCHAR *)&m_pMusicMem[g_i_CurrentMemPos];
		g_i_CurrentMemPos+=sizeof(WCHAR) * (wcslen(st_fileinfo.s_filepath)+1);
		g_i_CurrentMemPos = Align4( g_i_CurrentMemPos );
		
		st_pMusicinfo->st_musicfileinfo.s_ngenre = (WCHAR *)&m_pMusicMem[g_i_CurrentMemPos];
		g_i_CurrentMemPos+=sizeof(WCHAR) * (wcslen(st_fileinfo.s_genre)+1);
		g_i_CurrentMemPos = Align4( g_i_CurrentMemPos );
		
		st_pMusicinfo->st_musicfileinfo.s_ntitle = (WCHAR *)&m_pMusicMem[g_i_CurrentMemPos];
		g_i_CurrentMemPos+=sizeof(WCHAR) * (wcslen(st_fileinfo.s_title)+1);
		g_i_CurrentMemPos = Align4( g_i_CurrentMemPos );

		wcsncpy(st_pMusicinfo->st_musicfileinfo.s_nalbum,st_fileinfo.s_album,wcslen(st_fileinfo.s_album));
		wcsncpy(st_pMusicinfo->st_musicfileinfo.s_nartist,st_fileinfo.s_artist,wcslen(st_fileinfo.s_artist));
		wcsncpy(st_pMusicinfo->st_musicfileinfo.s_nfilepath, st_fileinfo.s_filepath,wcslen(st_fileinfo.s_filepath));
		wcsncpy(st_pMusicinfo->st_musicfileinfo.s_ngenre, st_fileinfo.s_genre,wcslen(st_fileinfo.s_genre));
		wcsncpy(st_pMusicinfo->st_musicfileinfo.s_ntitle,st_fileinfo.s_title,wcslen(st_fileinfo.s_title));

		st_pMusicinfo->i_musickey = i_musickey;
		Update_MusicHash(st_pMusicinfo->i_musickey,i_musicindex);
		Add_ArtistHash(st_pMusicinfo);
		Add_AlbumHash(st_pMusicinfo);
		Add_GenreHash(st_pMusicinfo);

		m_a_MusicInfo.AddTail((void *)st_pMusicinfo);
	}
	else
	{
		st_pMusicinfo=NULL;
	}

	

	return st_pMusicinfo;
}


int CDeviceMusicDB::Update_MusicHash(unsigned int i_musickey,int i_musicindex)
{
	int i_hashindex=-1;
	int i_insert=-1;
	int i;

	i_hashindex = i_musickey%MUSICHASHSIZE;

	for(i=i_hashindex;i<MUSICHASHSIZE;i++)
	{
		if(m_a_MusicHash[	i]==NULL || /*m_a_MusicHash[i]->i_musickey==0 ||*/ m_a_MusicHash[	i]->i_musickey==i_musickey)
		{
			i_insert = i;
			break;
		}
	}

	if(i_insert==-1) // Key를 넣을 곳이 없을 경우 
	{
		for(i=0;i<i_hashindex;i++)
		{
			if(m_a_MusicHash[	i]==NULL || /*m_a_MusicHash[i]->i_musickey==0 ||*/ m_a_MusicHash[	i]->i_musickey==i_musickey)
			{
				i_insert = i;
				break;
			}
		}
	}

	if(i_insert!=-1)
	{
		if(m_a_MusicHash[	i]==NULL)
		{
//			m_a_MusicHash[i] = new ST_DEVICEMUSICHASH;
			m_a_MusicHash[i] = (ST_DEVICEMUSICHASH *)&m_pMusicMem[g_i_CurrentMemPos];
			g_i_CurrentMemPos+=sizeof(m_a_MusicHash[i]);

		}
		m_a_MusicHash[i_insert]->i_musickey = i_musickey;
		m_a_MusicHash[i_insert]->i_musicindex = i_musicindex;
	}

	return i_hashindex;
}




int CDeviceMusicDB::Get_MusicIndex(unsigned int i_musickey)
{
	int i_pos = i_musickey%MUSICHASHSIZE;
	int i=0;

	for(i=i_pos;i<MUSICHASHSIZE;i++)
	{
		if(m_a_MusicHash[i])		
		{
			if(m_a_MusicHash[i]->i_musickey==i_musickey)
				return m_a_MusicHash[i]->i_musicindex;
		}
		else
		{
			return -1;
		}
	}

	if(i_musickey==0)
	{
		for(i=0;i<i_pos;i++)
		{
			if(m_a_MusicHash[i])
			{
				if(m_a_MusicHash[i]->i_musickey==i_musickey)
					return m_a_MusicHash[i]->i_musicindex;
			}
			else
			{
				return -1;
			}
		}
	}

	return -1;
}


unsigned int CDeviceMusicDB::Generate_Key( LPWSTR s_sz)
{
	unsigned int i_length=(unsigned int)wcslen(s_sz)*2;
	unsigned int i_key=0;
	unsigned int i;
	unsigned char* p = ( unsigned char* )s_sz;
	
	if( !p ) return 0;

	for (i=0; i<i_length; ++i) 
	{
		i_key += p[i];
		i_key += (i_key << 10);
		i_key ^= (i_key >> 6);
	}
	i_key += (i_key << 3);
	i_key ^= (i_key >> 11);
	i_key += (i_key << 15);

	return i_key;
}


LPWSTR CDeviceMusicDB::Get_ArtistName(unsigned int i_artistkey)
{
	ST_DEVICEGROUPHASH *phash = Get_ArtistHash(i_artistkey);
	if(phash && phash->st_pfirstmusic)
	{
		ST_DEVICEMUSICINFO *pinfo = (ST_DEVICEMUSICINFO *)phash->st_pfirstmusic;//Get_Music(phash->i_firstmusickey);
		return pinfo->st_musicfileinfo.s_nartist;
	}
	return NULL;
}

LPWSTR CDeviceMusicDB::Get_AlbumName(unsigned int i_albumkey)
{
	ST_DEVICEGROUPHASH *phash = Get_AlbumHash(i_albumkey);
	if(phash && phash->st_pfirstmusic)
	{
		ST_DEVICEMUSICINFO *pinfo = (ST_DEVICEMUSICINFO *)phash->st_pfirstmusic;//Get_Music(phash->i_firstmusickey);
		return pinfo->st_musicfileinfo.s_nalbum;
	}
	return NULL;
}

LPWSTR CDeviceMusicDB::Get_GenreName(unsigned int i_genrekey)
{
	ST_DEVICEGROUPHASH *phash = Get_GenreHash(i_genrekey);
	if(phash && phash->st_pfirstmusic)
	{
		ST_DEVICEMUSICINFO *pinfo = (ST_DEVICEMUSICINFO *)phash->st_pfirstmusic;//Get_Music(phash->i_firstmusickey);
		return pinfo->st_musicfileinfo.s_ngenre;
	}
	return NULL;
}


ST_DEVICEGROUPHASH *CDeviceMusicDB::Get_ArtistHash(unsigned int i_artistkey)
{
	int i_pos=0;
	int i;

	i_pos = i_artistkey%CATEGORYHASHSIZE;

	for(i=i_pos;i<CATEGORYHASHSIZE;i++)
	{
		if(m_a_ArtistHash[i] && m_a_ArtistHash[i]->i_groupkey==i_artistkey)
		{
			return m_a_ArtistHash[i];
		}
	}

	for(i=0;i<i_pos;i++)
	{
		if( m_a_ArtistHash[i] && m_a_ArtistHash[i]->i_groupkey==i_artistkey)
		{
			return m_a_ArtistHash[i];
		}
	}

	return NULL;
}


ST_DEVICEGROUPHASH *CDeviceMusicDB::Get_AlbumHash(unsigned int i_albumkey)
{
	int i_pos=0;

	i_pos = i_albumkey%CATEGORYHASHSIZE;
	int i;

	for(i=i_pos;i<CATEGORYHASHSIZE;i++)
	{
		if(m_a_AlbumHash[i] && m_a_AlbumHash[i]->i_groupkey==i_albumkey)
		{
			return m_a_AlbumHash[i];
		}
	}

	for(i=0;i<i_pos;i++)
	{
		if(m_a_AlbumHash[i] && m_a_AlbumHash[i]->i_groupkey==i_albumkey)
		{
			return m_a_AlbumHash[i];
		}
	}

	return NULL;
}


ST_DEVICEGROUPHASH *CDeviceMusicDB::Get_GenreHash(unsigned int i_genrekey)
{
	int i_pos=0;
	int i;

	i_pos = i_genrekey%CATEGORYHASHSIZE;

	for(i=i_pos;i<CATEGORYHASHSIZE;i++)
	{
		if(m_a_GenreHash[i] && m_a_GenreHash[i]->i_groupkey==i_genrekey)
		{
			return m_a_GenreHash[i];
		}
	}

	for(i=0;i<i_pos;i++)
	{
		if(m_a_GenreHash[i] && m_a_GenreHash[i]->i_groupkey==i_genrekey)
		{
			return m_a_GenreHash[i];
		}
	}

	return NULL;
}


unsigned int CDeviceMusicDB::Add_ArtistHash(ST_DEVICEMUSICINFO *st_pMusicinfo)
{
	if(wcslen(st_pMusicinfo->st_musicfileinfo.s_nartist)==0)
	{
		return 0;
	}
//	if(st_pMusicinfo->st_musicfileinfo.s_nartist[0]==0)return 0;

	st_pMusicinfo->i_artistkey = Generate_Key(st_pMusicinfo->st_musicfileinfo.s_nartist);

	int i_pos=0;
	int i_insert=-1;
	int i;
	BOOL b_new = FALSE;

	i_pos = st_pMusicinfo->i_artistkey%CATEGORYHASHSIZE;

	for(i=i_pos;i<CATEGORYHASHSIZE;i++)
	{
		if(m_a_ArtistHash[i]==NULL || m_a_ArtistHash[i]->i_groupkey==st_pMusicinfo->i_artistkey)
		{
			if(m_a_ArtistHash[i]==NULL)
			{
				i_insert = i;
				b_new = TRUE;
				break;
			}
			else
			{
				i_insert = i;
				break;
			}
		}
	}

	if(i_insert==-1) // Key를 넣을 곳이 없을 경우 
	{
		for(i=0;i<i_pos;i++)
		{
			if(m_a_ArtistHash[i]==NULL || m_a_ArtistHash[i]->i_groupkey==st_pMusicinfo->i_artistkey)
			{
				if(m_a_ArtistHash[i]==NULL)
				{
					i_insert = i;
					b_new = TRUE;
					break;
				}
				else
				{
					i_insert = i;
					break;
				}
			}
		}
	}

	if(i_insert>=0)
	{
		if(b_new)
		{
			// create artist hash element
//			m_a_ArtistHash[i_insert] = new ST_DEVICEGROUPHASH;

			m_a_ArtistHash[i_insert] = (ST_DEVICEGROUPHASH *)&m_pMusicMem[g_i_CurrentMemPos];
			g_i_CurrentMemPos+=sizeof(ST_DEVICEGROUPHASH);

			m_a_ArtistHash[i_insert]->i_groupkey = st_pMusicinfo->i_artistkey;
			m_a_ArtistHash[i_insert]->st_pfirstmusic = st_pMusicinfo;
			m_a_ArtistHash[i_insert]->st_plastmusic = st_pMusicinfo;
			m_a_ArtistHash[i_insert]->i_includemusiccount=0;

			m_a_ArtistList.AddTail((void *)(st_pMusicinfo->st_musicfileinfo.s_nartist));
		}
		else
		{
			// update exitist hash element

			ST_DEVICEMUSICINFO *pcurrmusic = m_a_ArtistHash[i_insert]->st_plastmusic;
			
			if(pcurrmusic)
			{
				ST_DEVICEMUSICINFO *st_pMusicinfo_prevlast = pcurrmusic;
				st_pMusicinfo_prevlast->pnext_artist_music = st_pMusicinfo;
				st_pMusicinfo->pnext_artist_music = st_pMusicinfo_prevlast;
				st_pMusicinfo->pnext_artist_music = NULL;
							
				m_a_ArtistHash[i_insert]->st_plastmusic = st_pMusicinfo;
			}

		}	
		m_a_ArtistHash[i_insert]->i_includemusiccount+=1;
	}
	else
	{
		return 0;
	}

	return st_pMusicinfo->i_artistkey;
}


unsigned int CDeviceMusicDB::Add_AlbumHash(ST_DEVICEMUSICINFO *st_pMusicinfo)
{
	//if(st_pMusicinfo->st_musicfileinfo.s_album[0]==0)return 0;
	if(wcslen(st_pMusicinfo->st_musicfileinfo.s_nalbum)==0)return 0;

	st_pMusicinfo->i_albumkey = Generate_Key(st_pMusicinfo->st_musicfileinfo.s_nalbum);

	int i_pos=0;
	int i_insert=-1;
	int i;
	BOOL b_new = FALSE;

	i_pos = st_pMusicinfo->i_albumkey%CATEGORYHASHSIZE;

	for(i=i_pos;i<CATEGORYHASHSIZE;i++)
	{
		if(m_a_AlbumHash[i]==NULL || m_a_AlbumHash[i]->i_groupkey==st_pMusicinfo->i_albumkey)
		{
			if(m_a_AlbumHash[i]==NULL)
			{
				i_insert = i;
				b_new = TRUE;
				break;
			}
			else
			{
				i_insert = i;
				break;
			}
		}
	}

	if(i_insert==-1) // Key를 넣을 곳이 없을 경우 
	{
		for(i=0;i<i_pos;i++)
		{
			if(m_a_AlbumHash[i]==NULL || m_a_AlbumHash[i]->i_groupkey==st_pMusicinfo->i_albumkey)
			{
				if(m_a_AlbumHash[i]==NULL)
				{
					i_insert = i;
					b_new = TRUE;
					break;
				}
				else
				{
					i_insert = i;
					break;
				}
			}
		}
	}

	if(i_insert>=0)
	{
		if(b_new)
		{
			// create album hash element
//			m_a_AlbumHash[i_insert] = new ST_DEVICEGROUPHASH;

			m_a_AlbumHash[i_insert] = (ST_DEVICEGROUPHASH *)&m_pMusicMem[g_i_CurrentMemPos];
			g_i_CurrentMemPos+=sizeof(ST_DEVICEGROUPHASH);

			m_a_AlbumHash[i_insert]->i_groupkey = st_pMusicinfo->i_albumkey;
			m_a_AlbumHash[i_insert]->st_pfirstmusic = st_pMusicinfo;
			m_a_AlbumHash[i_insert]->st_plastmusic = st_pMusicinfo;
			m_a_AlbumHash[i_insert]->i_includemusiccount=0;

			m_a_AlbumList.AddTail((void *)(st_pMusicinfo->st_musicfileinfo.s_nalbum));
		}
		else
		{
			// update exist hash element

			ST_DEVICEMUSICINFO *pcurrmusic = m_a_AlbumHash[i_insert]->st_plastmusic;
			
			if(pcurrmusic)
			{
				ST_DEVICEMUSICINFO *st_pMusicinfo_prevlast = pcurrmusic;
				st_pMusicinfo_prevlast->pnext_album_music = st_pMusicinfo;
				st_pMusicinfo->pprev_album_music = st_pMusicinfo_prevlast;
				st_pMusicinfo->pnext_album_music = NULL;
							
				m_a_AlbumHash[i_insert]->st_plastmusic = st_pMusicinfo;
			}

		}	
		m_a_AlbumHash[i_insert]->i_includemusiccount+=1;
	}
	else
	{
		return 0;
	}

	return st_pMusicinfo->i_albumkey;
}

unsigned int CDeviceMusicDB::Add_GenreHash(ST_DEVICEMUSICINFO *st_pMusicinfo)
{
//	if(st_pMusicinfo->st_musicfileinfo.s_genre[0]==0)return 0;
	if(wcslen(st_pMusicinfo->st_musicfileinfo.s_ngenre)==0)return 0;

	st_pMusicinfo->i_genrekey = Generate_Key(st_pMusicinfo->st_musicfileinfo.s_ngenre);
	int i_pos=0;
	int i_insert=-1;
	int i;
	BOOL b_new = FALSE;

	i_pos = st_pMusicinfo->i_genrekey%CATEGORYHASHSIZE;

	for(i=i_pos;i<CATEGORYHASHSIZE;i++)
	{
		if(m_a_GenreHash[i]==NULL  || m_a_GenreHash[i]->i_groupkey==st_pMusicinfo->i_genrekey)
		{
			if(m_a_GenreHash[i]==NULL)
			{
				i_insert = i;
				b_new = TRUE;
				break;
			}
			else
			{
				i_insert = i;
				break;
			}
		}
	}

	if(i_insert==-1) // Key를 넣을 곳이 없을 경우 
	{
		for(i=0;i<i_pos;i++)
		{
			if(m_a_GenreHash[i]==NULL || m_a_GenreHash[i]->i_groupkey==st_pMusicinfo->i_genrekey)
			{
				if(m_a_GenreHash[i]==NULL)
				{
					i_insert = i;
					b_new = TRUE;
					break;
				}
				else
				{
					i_insert = i;
					break;
				}
			}
		}
	}

	if(i_insert>=0)
	{
		if(b_new)
		{
			// create genre hash element
//			m_a_GenreHash[i_insert] = new ST_DEVICEGROUPHASH;
			m_a_GenreHash[i_insert] = (ST_DEVICEGROUPHASH *)&m_pMusicMem[g_i_CurrentMemPos];
			g_i_CurrentMemPos+=sizeof(ST_DEVICEGROUPHASH);

			m_a_GenreHash[i_insert]->i_groupkey = st_pMusicinfo->i_genrekey;
			m_a_GenreHash[i_insert]->st_pfirstmusic = st_pMusicinfo;
			m_a_GenreHash[i_insert]->st_plastmusic = st_pMusicinfo;
			m_a_GenreHash[i_insert]->i_includemusiccount=0;

			m_a_GenreList.AddTail((void *)st_pMusicinfo->st_musicfileinfo.s_ngenre);
		}
		else
		{
			// update exist hash element

			ST_DEVICEMUSICINFO *pcurrmusic = m_a_GenreHash[i_insert]->st_plastmusic;
			
			if(pcurrmusic)
			{
				ST_DEVICEMUSICINFO *st_pMusicinfo_prevlast = pcurrmusic;
				st_pMusicinfo_prevlast->pnext_genre_music = st_pMusicinfo;
				st_pMusicinfo->pprev_genre_music = st_pMusicinfo_prevlast;
				st_pMusicinfo->pnext_genre_music = NULL;
							
				m_a_GenreHash[i_insert]->st_plastmusic = st_pMusicinfo;
			}


		}	
		m_a_GenreHash[i_insert]->i_includemusiccount+=1;
	}
	else
	{
		return 0;
	}

	return st_pMusicinfo->i_genrekey;
}

int CDeviceMusicDB::Get_MusicCount()
{
	return (int)(m_a_MusicInfo.GetCount());
}

int CDeviceMusicDB::Get_MusicCountFromGroup(int i_type, unsigned int i_groupkey) //  Artist, Album, Genre별로 뮤직의 개수를 얻는다. 
{
	ST_DEVICEGROUPHASH *st_phash = NULL;
	
	switch(i_type)
	{
	case 0 : // artist
		st_phash = Get_ArtistHash(i_groupkey);
		break;
	case 1: // album
		st_phash = Get_AlbumHash(i_groupkey);
		break;
	case 2 : // genre
		st_phash = Get_GenreHash(i_groupkey);
		break;
	}

	if(st_phash==NULL)return NULL;
	return st_phash->i_includemusiccount;
}










BOOL CDeviceMusicDB::Get_AlbumListFromArtist(LPWSTR s_artistname) //  특정 Artist목록내의 모든 앨범을 읽어 온다 
{
	unsigned int i_artistkey = Generate_Key(s_artistname);
	Get_AlbumListFromArtist(i_artistkey);

	return TRUE;
}

BOOL CDeviceMusicDB::Get_AlbumListFromArtist(unsigned int i_artistkey) //  특정 Artist목록내의 모든 앨범을 읽어 온다 
{
	ST_DEVICEGROUPHASH *phash = Get_ArtistHash(i_artistkey);

	int i_count = phash->i_includemusiccount;

	ST_DEVICEMUSICINFO *st_Musicinfo = NULL;

	ST_DEVICEMUSICINFO *st_music = phash->st_pfirstmusic;

	m_a_CurrentAlbumList.RemoveAll();
	
	for(int i=0;i<i_count;i++)
	{
		st_Musicinfo = st_music;

		Add_CurrentAlbumList(st_Musicinfo->i_albumkey);
		
		st_music = (ST_DEVICEMUSICINFO *)st_Musicinfo->pnext_artist_music;
		if(st_music==NULL)break;
	}

	return TRUE;
}

BOOL CDeviceMusicDB::Add_CurrentAlbumList(unsigned int &i_albumkey)
{
	int i_count = (int)m_a_CurrentAlbumList.GetCount();
	BOOL b_isnew = TRUE;
	int i;

	unsigned int *i_pkey=NULL;

	for(i=0;i<i_count;i++)
	{
		unsigned int *i_pkey = (unsigned int *)m_a_CurrentAlbumList.GetAt(i);

		if(*i_pkey==i_albumkey)
		{
			b_isnew=FALSE;
			break;
		}
	}

	if(b_isnew==TRUE)
	{
		m_a_CurrentAlbumList.AddTail((void *)&(i_albumkey));
	}
	
	return TRUE;
}
