#ifndef __MUSIC_DATABASE_H__
#define __MUSIC_DATABASE_H__
#ifdef _WIN32
#pragma once
#define MUSICLIBRARY		"c:\\MusicLibrary.dat"
#define CLOSEHANDLE(x)	CloseHandle(x)
#define __I_TAVI__ 1
#else
#include <stdio.h>
#include <string.h>
#include <Component/String.h>

#define TRUE					1
#define FALSE					0
#define INVALID_HANDLE_VALUE		-1
#define MUSICLIBRARY		"/hd1/Music/Music.db"
#define MAX_PATH			256
#define MAX_ENTITIES			4000
#define CLOSEHANDLE	(__h)	fatfs_fclose( (RMuint32)__h )

#define wcscpy( __x, __y )	ustrcpy( (uchar_t*)__x, (uchar_t*)__y )
#define wcsncpy( _x, _y, _n )	ustrncpy( (uchar_t*)_x, (uchar_t*)_y, _n )
#define wcscmp( __x, __y )	ustrcmp( (uchar_t*)__x, (uchar_t*)__y )
#define wcslen( __x )			ustrlen( (uchar_t*)__x )

#define FILE_BEGIN			SEEK_SET
#define FILE_CUR				SEEK_CUR
#define FILE_END				SEEK_END
#define SetFilePointer( _h, _a1, _a2, _a3 )		fatfs_fseek( (RMuint32)_h, (RMint32)_a1, (RMint32)_a3 )
#define ReadFile( _h, _b, _s, _p, _d )			fatfs_fread( (RMuint32)_h, (RMuint8*)_b, (RMuint32)_s, (RMuint32*)_p )

#define TRACE				

typedef unsigned short WCHAR;
typedef unsigned short* LPWSTR;
typedef char* LPCSTR;
typedef int HANDLE;
typedef bool BOOL;
typedef int DWORD;
#endif

/////////////////////////////////////////////////////////
// Music DB for TAVI 020
// New Media Life Inc.
// 2005. 10. 12
// By Bornbest
//////////////////////////////////////////////////////////
#include "PointerList.h"

enum GROUP_TYPE {  ARTIST=0, ALBUM, GENRE};

extern unsigned int g_i_CurrentMemPos;
extern unsigned int g_i_TotalMusicMemSize;
extern char *m_pMusicMem;

#define MUSICMAJORVER 1
#define MUSICMINORVER 4

typedef struct 
{
	unsigned int i_totalcount;
	char s_cpcode[8];
	int i_majorversion;
	int i_minorversion;
}ST_DEVICEHEADER;


typedef struct 
{
unsigned int i_musickey;
int i_musicindex;
}ST_DEVICEMUSICHASH;


typedef struct 
{
	WCHAR s_title[128];
	WCHAR s_filepath[260];
	WCHAR s_artist[64];
	WCHAR s_album[64];
	WCHAR s_genre[64];

}ST_DEVICEFILEINFO;

typedef struct 
{
	WCHAR *s_ntitle;
	WCHAR *s_nfilepath;
	WCHAR *s_nartist;
	WCHAR *s_nalbum;
	WCHAR *s_ngenre;

}ST_DEVICEMUSICFILEINFO;


typedef struct 
{
	unsigned int i_musickey;

	ST_DEVICEMUSICFILEINFO st_musicfileinfo;

	unsigned int i_artistkey;
	unsigned int i_albumkey;
	unsigned int i_genrekey;

	void *pprev_artist_music;
	void *pnext_artist_music;
	void *pprev_album_music;
	void *pnext_album_music;
	void *pprev_genre_music;
	void *pnext_genre_music;

}ST_DEVICEMUSICINFO;

typedef struct 
{
unsigned int i_groupkey;
ST_DEVICEMUSICINFO *st_pfirstmusic;
ST_DEVICEMUSICINFO *st_plastmusic;
int i_includemusiccount;
}ST_DEVICEGROUPHASH;


class CDeviceMusicDB
{
public:
	CDeviceMusicDB(void);
	~CDeviceMusicDB(void);

	ST_DEVICEMUSICHASH *m_a_MusicHash[10007];
	ST_DEVICEGROUPHASH *m_a_ArtistHash[3001];
	ST_DEVICEGROUPHASH *m_a_AlbumHash[3001];
	ST_DEVICEGROUPHASH *m_a_GenreHash[3001];

	CFirmPointerList m_a_MusicInfo;
	CFirmPointerList m_a_ArtistList;
	CFirmPointerList m_a_AlbumList;
	CFirmPointerList m_a_GenreList;
	CPointerList m_a_CurrentAlbumList;

	HANDLE m_h_MusicLibFile;

	unsigned int Add_GenreHash(ST_DEVICEMUSICINFO *st_pmusicinfo);
	unsigned int Add_AlbumHash(ST_DEVICEMUSICINFO *st_pmusicinfo);
	unsigned int Add_ArtistHash(ST_DEVICEMUSICINFO *st_pmusicinfo);

	static unsigned int Generate_Key( LPWSTR s_sz);
	int Get_MusicIndex(unsigned int i_musickey);

	int Update_MusicHash(unsigned int i_musickey,int i_musicindex);
	
	BOOL Load_AllMusicFromLibFile();


	ST_DEVICEMUSICINFO *Get_Music(unsigned int i_musickey);
	ST_DEVICEMUSICINFO *Get_Music(LPWSTR s_filepath);

	BOOL Exist_Music(unsigned int i_musickey);
	BOOL Exist_Music(LPWSTR s_filepath);

	CString m_s_DBFilePath;

	void Set_MusicDBFile(LPCSTR s_dbfile)
	{
		m_s_DBFilePath = s_dbfile;
	}

	ST_DEVICEMUSICINFO *Add_MusicToLib( ST_DEVICEFILEINFO &st_fileinfo);

#ifdef __I_TAVI__
#endif /* __I_TAVI__ */

	void Close_MusicLibraryFile();
	BOOL Open_MusicLibraryFile();
#ifdef __I_TAVI__
#endif /* __I_TAVI__ */

	ST_DEVICEGROUPHASH *Get_ArtistHash(unsigned int i_artistkey);
	ST_DEVICEGROUPHASH *Get_AlbumHash(unsigned int i_albumkey);
	ST_DEVICEGROUPHASH *Get_GenreHash(unsigned int i_genrekey);

//	BOOL Get_AllMusicFromGroup(GROUP_TYPE i_type, unsigned int i_groupkey);

	ST_DEVICEHEADER m_st_MusicDBHeader;

	int Get_MusicCount();

	int Get_MusicCountFromGroup(int i_type, unsigned int i_groupkey);
	BOOL Get_MusicFromGroup(int i_type, unsigned int i_groupkey);
	BOOL Get_AllMusic();

	BOOL Get_AlbumListFromArtist(LPWSTR s_artistname); //  특정 Artist목록내의 모든 앨범을 읽어 온다 
	BOOL Get_AlbumListFromArtist(unsigned int i_artistkey); //  특정 Artist목록내의 모든 앨범을 읽어 온다 
	BOOL Add_CurrentAlbumList(unsigned int &i_albumkey);

	LPWSTR Get_ArtistName(unsigned int i_artistkey);
	LPWSTR Get_AlbumName(unsigned int i_albumkey);
	LPWSTR Get_GenreName(unsigned int i_genrekey);

	BOOL Init_Hash();

//	struct CompSort : public std::binary_function<ST_DEVICEMUSICTITLE, ST_DEVICEMUSICTITLE, bool>
//	void Sort_MusicByTitle(BOOL seq);

	int MUSICHASHSIZE;
	int CATEGORYHASHSIZE;


};

#endif
