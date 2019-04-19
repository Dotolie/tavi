//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : OGG Tag Parser
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/AudioPlayer/ID3/OGGTag.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: OGGTag.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.1  2005/06/01 05:43:01  zstein
// *** empty log message ***
//
//*******************************************************************************

#ifndef __OGG_TAG_PARSER_H__
#define __OGG_TAG_PARSER_H__

namespace NML {

class COGGTag {
private:
protected:
	char* m_szTitle;
	char* m_szAlbum;
	char* m_szArtist;
	char* m_szGenre;
	int m_nTitleLen;
	int m_nAlbumLen;
	int m_nGenreLen;
	int m_nArtistLen;
	int m_nError;
public:
	COGGTag( void );
	~COGGTag( void );

	enum {
		OGGTAG_ERROR_NO_ERROR,
		OGGTAG_ERROR_OPEN,
		OGGTAG_ERROR_READ,
		OGGTAG_ERROR_SEEK,
		OGGTAG_ERROR_CLOSE
	};
	
	bool Parse( const char* path );
	char* GetTitle( int& len ) const { len = m_nTitleLen; return m_szTitle; }
	char* GetAlbum( int& len ) const { len = m_nAlbumLen; return m_szAlbum; }
	char* GetArtist( int& len ) const { len = m_nArtistLen; return m_szAlbum; }
	char* GetGenre( int& len ) const { len = m_nGenreLen; return m_szGenre; }
	int GetError( void ) const { return m_nError; }
};

}; /* END OF NAMESPACE */

#endif /* __OGG_TAG_PARSER_H__ */