#ifndef __THUMDB_H__
#define __THUMDB_H__

#include <Component/String.h>

#define THUMBHASHSIZE 		30011
#define THUMBDB_ID			"NML_PDB"
#define THUMBDB_FILE		"/hd1/Photo/photo.db"

#define THUMB_M_WIDTH		56
#define THUMB_M_HEIGHT		40
#define THUMB_M_YSIZE		(THUMB_M_WIDTH*THUMB_M_HEIGHT)
#define THUMB_M_UVSIZE		(THUMB_M_YSIZE/2)
#define THUMB_M_SIZE		(THUMB_M_YSIZE+THUMB_M_UVSIZE)

typedef struct 
{
	unsigned int i_thumbkey;
	int i_thumbindex;
}ST_THUMBHASH;

typedef struct 
{
	unsigned int i_totalcount;
	char s_cpcode[8];
	int i_majorversion;
	int i_minorversion;
}ST_P_HEADER;

class CThumbDB {
protected:
	ST_THUMBHASH* m_pHash;
	ST_P_HEADER m_header;
	int m_hFile;
public:
	enum {
		SMALL,
		MIDDLE,
		BIG
	};
	CThumbDB( void );
	//CThumbDB( const CString& path );
	~CThumbDB( void );

	//bool Load( const CString& path );
	bool Load( void );
	bool UnLoad( void );
	//bool FetchThumb( const CString& filename, char* pY, char* pUV );
	bool FetchThumb( const uchar_t* filename, char* pY, char* pUV );
};

#endif /* __THUMBDB_H__ */
