//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Music ID Database library
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/AudioPlayer/ID3/IDDB.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $
//
//******************************************************************************
// $Log: IDDB.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.5  2005/09/16 06:25:07  zstein
// - update db.
//
// Revision 1.4  2005/09/16 05:39:06  zstein
// - update db
//
// Revision 1.3  2005/09/14 02:22:19  zstein
// - update IDDB & UI
//
// Revision 1.2  2005/09/12 03:09:42  zstein
// - updated DB
//
// Revision 1.1  2005/09/09 05:53:39  zstein
// - new IDDB
//
//*******************************************************************************

#ifndef __MUSIC_IDDB_H__
#define __MUSIC_IDDB_H__

#define IDDB_MAGIC				"NMLIDDB"
#define IDDB_MAGIC_SIZE		7
#define IDDB_VER_MAJOR			0
#define IDDB_VER_MINOR			10
#define IDDB_TEMP_NODE			456

typedef struct {
	char szMagicKey[8];	// "NMLDB"
	short nMajor;			// DB Version major number
	short nMinor;			// DB Version minor number
	int nTotal;			// The number of total file nodes
	int nFreeLinkHead;	// The head of free nodes
} __attribute__ ((packed)) DBHeader;
#define IDDB_HEADER_SIZE		sizeof(DBHeader)

typedef struct {
	int nKey;
	short nMark; // 1: exist, 0: non-exist
	short nStartPointer; // first node index 
	short nEndPointer; // last node index
	short nPrevSlot;
	short nNextSlot;
} __attribute__ ((packed)) HashSlot;
#define MAX_FILE_HASH_NUM		4096
#define IDDB_FILE_HASH_NUM	4091 // PRIME NUMBER
#define IDDB_FILE_HASH_BLOCK	(MAX_FILE_HASH_NUM*sizeof(HashSlot))
#define MAX_ID_HASH_NUM		1024
#define IDDB_ID_HASH_NUM		1021
#define IDDB_ID_HASH_BLOCK		(MAX_ID_HASH_NUM*sizeof(HashSlot))
#define IDDB_CACHE_NUM			256
#define IDDB_HASH_BLOCK_SIZE	(IDDB_HEADER_SIZE+IDDB_FILE_HASH_BLOCK+3*IDDB_ID_HASH_BLOCK)

enum {
	HASH_ARTIST,
	HASH_ALBUM,
	HASH_GENRE,
	HASH_TITLE
};

typedef struct {
	short nHashSize;
	short nUsed;
	short nStart; // head bucket in the hash table
	short nEnd; // tail bucket in the hash table
	HashSlot pht[1]; // hash table
} __attribute__ ((packed)) HashTable;

typedef struct {
	short size;
	char szName[64];
} IDTagName;

typedef struct {
	short nArtistPrev;
	short nArtistNext;
	short nAlbumPrev;
	short nAlbumNext;
	short nGenrePrev;
	short nGenreNext;
	short nDirty; // used or not
	short nIdx;
	short nFreeLink;
	short nUnicode_A; // artist
	short nArtistSize;
	char szArtist[64];
	short nUnicode_Al; // album
	short nAlbumSize;
	char szAlbum[64];
	short nUnicode_G; // genre
	short nGenreSize;
	char szGenre[64];
	short nUnicode_T;
	short nTitleSize;
	char szTitle[64];
	int nPathLen;
	char szFullpath[1754];
} __attribute__ ((packed)) IDNode; // 2KB-block
#define IDDB_FNODE_SIZE		(sizeof(IDNode))

typedef struct {
	char* path;
	int nFD; // file descriptor
	int mode; // 0 read, 1: write
	int shmid;
	char* pCache;
	short nLinkIdx;
	DBHeader* pHeader;
	HashTable* f_HashTable; // file hash table
	HashTable* a_HashTable; // artist hash table
	HashTable* al_HashTable; // album hash table
	HashTable* g_HashTable; // genre hash table
	IDNode* pNode;
	short* pFreeLinkList;
} IDDB_Desc;

typedef int bucket_t;

#undef DEBUG
#define DEBUG			0
#if DEBUG
#define DBDEBUG			printf
#else
#define DBDEBUG
#endif

extern int IDDB_GenKey( const char* sz, int size );
extern int IDDB_Load( IDDB_Desc* desc, const char* path );
extern int IDDB_Create( IDDB_Desc* desc );
extern IDNode* IDDB_AllocFileNode( IDDB_Desc* desc );
extern int IDDB_ScanMode( IDDB_Desc* desc );
extern int IDDB_UpdateMode( IDDB_Desc* desc );
extern int IDDB_Insert( IDDB_Desc* desc, IDNode* pNode, int key );
extern int IDDB_Delete( IDDB_Desc* desc, int bucket, int key, IDNode* pnode );
extern int IDDB_Close( IDDB_Desc** desc );
extern IDNode* IDDB_FetchNode( IDDB_Desc* desc, int node );
extern int IDDB_FindKey( HashTable* htable, int key );
extern IDDB_Desc* IDDB_AllocDesc( void );
extern IDNode* IDDB_FetchID( IDDB_Desc* desc, int id, int start, int& bucket );
extern int IDDB_Release( IDDB_Desc* desc );


#endif /* __MUSIC_IDDB_H__ */
