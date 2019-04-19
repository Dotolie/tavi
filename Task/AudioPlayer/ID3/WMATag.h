//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : WMA Tag 
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/AudioPlayer/ID3/WMATag.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: WMATag.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.1  2005/06/01 05:43:01  zstein
// *** empty log message ***
//
//******************************************************************************

#ifndef __WMA_TAG_H__
#define __WMA_TAG_H__

#include <sys/types.h> // for u_char_t

namespace NML {

typedef int( *pfnOpen )( const char* path );
typedef int( *pfnRead )( int hFile, void* pBuffer, int nSize );
typedef int( *pfnSeek )( int hFile, int nPos, int nWhence );
typedef int( *pfnClose )( int hFile );

typedef struct FileOps_t {
	pfnOpen open;
	pfnRead read;
	pfnSeek seek;
	pfnClose close;
} FileOps_t;

typedef struct GUID_t {
	u_int32_t g1;
	u_int16_t g2;
	u_int16_t g3;
	u_int8_t g4[8];
} __attribute__ ((packed)) GUID_t ;

typedef struct FileProp_t {
	GUID_t fileID;
	u_int64_t fileSize;
	u_int64_t cDate; // creation date
	u_int64_t packetCnt;
	u_int64_t playDuration; // playing time
	u_int64_t sendDuration;
	u_int64_t preroll;
	u_int32_t flags; // broadcast flag:1(lsb), seekable flag:1 
	u_int32_t minDPacketSize;
	u_int32_t maxDPacketSize;
	u_int32_t maxBitrate;
} FileProp_t;

typedef struct ContentDescSize_t {
	u_int16_t TitleLength;
	u_int16_t AuthorLength;
	u_int16_t CopyrightLength;
	u_int16_t DescriptionLength;
	u_int16_t RatingLength;
} __attribute__((packed)) ContentDescSize_t ;

typedef unsigned short UTF16_t;

// CWMATag
// This class captures string of aritist, album, genre and title in the wma file.
// NOT support the others.
class CWMATag {
private:
protected:
	// m_pszXXX member variables are null-terminated string.
	UTF16_t* m_pszArtist;
	UTF16_t* m_pszAlbum;
	UTF16_t* m_pszGenre;
	UTF16_t* m_pszTitle;
	int m_nArtistLen;
	int m_nAlbumLen;
	int m_nGenreLen;
	int m_nTitleLen;
	FileOps_t m_Fops;
	int m_nError;
	int m_nPlayingTime; // seconds

	enum TopLevelASFGUIDs {
		ASF_Header_Object,
		ASF_Data_Object,
		ASF_Simple_Index_Object,
		ASF_Index_Object,
		ASF_Media_Object_Index_Object,
		ASF_Timecode_Index_Object
	};
	enum HeaderObjectGUIDs {
		ASF_File_Properties_Object,
		ASF_Stream_Properties_Object,
		ASF_Content_Description_Object,
		ASF_Extended_Content_Description_Object,
		ASF_Header_Extension_Object,
		ASF_Codec_List_Object,
		ASF_Script_Command_Object,
		ASF_Marker_Object,
		ASF_Bitrate_Mutual_Exclusion_Object,
		ASF_Error_Correction_Object,
		ASF_Content_Branding_Object,
		ASF_Stream_Bitrate_Properties_Object,
		ASF_Content_Encryption_Object,
		ASF_Extended_Content_Encryption_Object,
		ASF_Digital_Signature_Object,
		ASF_Padding_Object
	};
public:
	CWMATag( void );
	~CWMATag( void );

	enum {
		WMA_ERROR_NO_ERROR,
		WMA_ERROR_OPEN,
		WMA_ERROR_READ,
		WMA_ERROR_SEEK,
		WMA_ERROR_NO_WMA,
		WMA_ERROR_NO_FILEOPS
	};

	enum ExtDescriptionDataType {
		DataType_UTF16,
		DataType_BARRAY,
		DataType_BOOL,
		DataType_DWORD,
		DataType_QWORD,
		DataType_WORD
	};

	void RegisterFileOps( FileOps_t* ops );
	bool Parse( const char* path );
	UTF16_t* GetArtist( int& len ) const { len = m_nArtistLen; return m_pszArtist; }
	UTF16_t* GetAlbum( int& len ) const { len = m_nAlbumLen; return m_pszAlbum; }
	UTF16_t* GetGenre( int& len ) const { len = m_nGenreLen; return m_pszGenre; }
	UTF16_t* GetTitle( int& len ) const { len = m_nTitleLen; return m_pszTitle; }
	int GetTotalTime( void ) const { return m_nPlayingTime; }
	int GetError( void ) const { return m_nError; }
};

}; /* end of namespace NML */

#endif /* __WMA_TAG_H__ */
