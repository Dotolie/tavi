//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Music ID Database library
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/AudioPlayer/ID3/IDDB.cpp,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $
//
//******************************************************************************
// $Log: IDDB.cpp,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.7  2005/09/28 02:52:00  zstein
// - update UI & fixed bugs
//
// Revision 1.6  2005/09/20 12:22:48  zstein
// - update
//
// Revision 1.5  2005/09/16 06:25:07  zstein
// - update db.
//
// Revision 1.4  2005/09/16 05:39:06  zstein
// - update db
//
// Revision 1.3  2005/09/14 02:22:18  zstein
// - update IDDB & UI
//
// Revision 1.2  2005/09/12 03:09:42  zstein
// - updated DB
//
// Revision 1.1  2005/09/09 05:53:39  zstein
// - new IDDB
//
//*******************************************************************************

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <rmbasic.h>
#include <fatfs.h>
#include <linux/autoconf.h>
#include <Task/tavi_global.h>
#include "IDDB.h"

#define OCCUPIED( __key )	( (__key==-1) ? 0 : 1 )

IDNode* AllocFreeNode( IDDB_Desc* desc )
{
	int idx, n;

	DBDEBUG( "+AllocFreeNode\n" );
	DBDEBUG( "nListIdx: %d, idx: %d\n", desc->nLinkIdx, desc->pFreeLinkList[desc->nLinkIdx] );
	n = desc->nLinkIdx;
	if( n < 0 || n >= (IDDB_FILE_HASH_NUM-desc->pHeader->nTotal) ) return 0;
	idx = desc->pFreeLinkList[n];
	// not any more node.
	if( idx < 0 ) return 0;

	desc->nLinkIdx++;
	desc->nLinkIdx %= IDDB_FILE_HASH_NUM;
	desc->pHeader->nFreeLinkHead = desc->pFreeLinkList[desc->nLinkIdx];
	desc->pNode->nIdx = idx;
	desc->pNode->nArtistNext = desc->pNode->nArtistPrev = -1;
	desc->pNode->nAlbumNext = desc->pNode->nAlbumPrev = -1;
	desc->pNode->nGenreNext = desc->pNode->nGenrePrev = -1;
	DBDEBUG( "-AllocFreeNode(linkhead: %d, nIdx: %d)\n", desc->pHeader->nFreeLinkHead, desc->pNode->nIdx );
	return desc->pNode;
}

static int Hash( HashTable* pht, int key )
{
	if( !pht ) return -1;
	int h = (unsigned int)key % pht->nHashSize;
	return h;
}

int IDDB_FindKey( HashTable* htable, int key )
{
	int bucket = Hash( htable, key );
	bool exist = false;
	int s = bucket;
	
	DBDEBUG( "+IDDB_FindKey(bucket:%d, 0x%x)\n", bucket, key );
	while( OCCUPIED(htable->pht[bucket].nKey) ) {
		if( htable->pht[bucket].nKey == key ) {
			exist = true;
			break;
		}
		bucket++;
		bucket %= htable->nHashSize;
		if( s ==  bucket ) break;
	}

	bucket = ( exist ? bucket : -1 );
	DBDEBUG( "-IDDB_FindKey(%d, %d)\n", bucket, exist );

	return bucket;
}

static int AllocBucket( HashTable* htable, int key )
{
	int bucket = -1;
	int end, c;

	bucket = Hash( htable, key );
	end = bucket;
	c = bucket;

	DBDEBUG( "+AllocBucket(%d, key:0x%x)\n", c, key );

	if( htable->pht[bucket].nKey == -1 ) {
		htable->pht[bucket].nPrevSlot = -1;
		htable->pht[bucket].nNextSlot = -1;
		htable->pht[bucket].nStartPointer = -1;
		htable->pht[bucket].nEndPointer = -1;
	}
	else {
		while( OCCUPIED(htable->pht[c].nKey) ) {
			c++;
			c %= htable->nHashSize;
			if( c == end ) {
				c = -1;
				break;
			}
			DBDEBUG( "finding free bucket ----> %d\n", c );
		}
	}

	if( c < 0 ) return -1;

	bucket = c;
	// link slot
	if( htable->nStart == -1 ) {
		htable->pht[c].nPrevSlot = -1;
		htable->pht[c].nNextSlot = -1;
	}
	else {
		htable->pht[htable->nEnd].nNextSlot = c;
		htable->pht[c].nPrevSlot = htable->nEnd;
		htable->pht[c].nNextSlot = -1;
	}
	
	if( htable->nStart == -1 ) {
		htable->nStart = c;
	}
	htable->nEnd = c;

	if( bucket != -1 ) htable->nUsed++;

	DBDEBUG( "-AllocBucket(bucket: %d)\n", bucket );

	return bucket;
}

IDNode* IDDB_AllocFileNode( IDDB_Desc* desc )
{
	int bucket;
	bool exist;
	IDNode* pNode = 0;

	if( !desc ) return 0;

	if( (pNode=AllocFreeNode(desc)) < 0 ) return 0;
	pNode->nDirty = 1;
	pNode->nFreeLink = -1;

	return pNode;
}


static int InsertIDHash( IDDB_Desc* desc, HashTable* ht, int id, int key, IDNode* pnode )
{
	//DBDEBUG( "+InsertIDHash(id:%d, key:0x%x, nidx:%d)\n", id, key, pnode->nIdx );
	if( !ht || !pnode || !desc ) return -1;

	int bucket = IDDB_FindKey( ht, key );
	if( bucket == -1 ) {
		bucket = AllocBucket( ht, key );
	}

	// NO MORE bucket in the hash table!!
	if( bucket == -1 ) {
		printf( "NO MORE BUCKET\n" );
		return -1;
	}
	
	if( ht->pht[bucket].nStartPointer == -1 ) { // new bucket
		ht->pht[bucket].nStartPointer = pnode->nIdx;
		ht->pht[bucket].nKey = key;
		ht->pht[bucket].nMark = 1;
		//ht->pht[bucket].nPrevSlot = -1;
		//ht->pht[bucket].nNextSlot = -1;
	}

	if( ht->pht[bucket].nEndPointer == - 1 ) {
		ht->pht[bucket].nEndPointer = pnode->nIdx;
		switch( id ) {
		case HASH_ARTIST:
			pnode->nArtistNext = -1;
			pnode->nArtistPrev = -1;
			break;
		case HASH_ALBUM:
			pnode->nAlbumPrev = -1;
			pnode->nAlbumNext = -1;
			break;
		case HASH_GENRE:
			pnode->nGenrePrev = -1;
			pnode->nGenreNext = -1;
			break;
		}
	}
	else {
		// load last node
		IDNode node;
		int nbytes;
		int end = ht->pht[bucket].nEndPointer;
		int offset = IDDB_HASH_BLOCK_SIZE + sizeof(IDNode)*end;
		short* next;
		short* prev;
		short* p;
		IDDB_ScanMode( desc );
		fatfs_fseek( (RMuint32)desc->nFD, offset, SEEK_SET );
		fatfs_fread( (RMuint32)desc->nFD, (RMuint8*)&node, sizeof(IDNode), (RMuint32*)&nbytes );
		// update endpoint
		ht->pht[bucket].nEndPointer = pnode->nIdx;
		switch( id ) {
		case HASH_ARTIST:
			node.nArtistNext = pnode->nIdx;
			pnode->nArtistPrev = node.nIdx;
			break;
		case HASH_ALBUM:
			node.nAlbumNext = pnode->nIdx;
			pnode->nAlbumPrev = node.nIdx;
			break;
		case HASH_GENRE:
			node.nGenreNext = pnode->nIdx;
			pnode->nGenrePrev = node.nIdx;
			break;
		}
		IDDB_UpdateMode( desc );
		fatfs_fseek( (RMuint32)desc->nFD, offset, SEEK_SET );
		fatfs_fwrite( (RMuint32)desc->nFD, (RMuint8*)&node, sizeof(IDNode), (RMuint32*)&nbytes );
	}

	//DBDEBUG( "-InsertIDHash\n" );

	return 0;
}

/*
 * @cnode: current file node
 * @pnode: previous file node
 * @nnode: next file node
 */
static int DeleteIDHash( HashTable* ht, IDNode* cnode, IDNode* pnode, IDNode* nnode, int id )
{
	int k, b, n, p;
	struct link { short prev; short next; } *clink, *plink, *nlink;

	plink = nlink = 0;
	switch( id ) {
	case HASH_ARTIST:
		k = IDDB_GenKey( cnode->szArtist, cnode->nArtistSize );
		clink = ( struct link* )&cnode->nArtistPrev;
		if( pnode ) plink = ( struct link* )&pnode->nArtistPrev;
		if( nnode ) nlink = ( struct link* )&nnode->nArtistPrev;
		break;
	case HASH_ALBUM:
		k = IDDB_GenKey( cnode->szAlbum, cnode->nAlbumSize );
		clink = ( struct link* )&cnode->nAlbumPrev;
		if( pnode ) plink = ( struct link* )&pnode->nAlbumPrev;
		if( nnode ) nlink = ( struct link* )&nnode->nAlbumPrev;
		break;
	case HASH_GENRE:
		k = IDDB_GenKey( cnode->szGenre, cnode->nGenreSize );
		clink = ( struct link* )&cnode->nGenrePrev;
		if( pnode ) plink = ( struct link* )&pnode->nGenrePrev;
		if( nnode ) nlink = ( struct link* )&nnode->nGenrePrev;
		break;
	}

	b = IDDB_FindKey( ht, k );
	if( b < 0 ) {
		DBDEBUG( "IDDB_Delete<%d>: FAILED TO FIND ARTIST KEY(0x%x)\n", __LINE__, k );
		return -1;
	}

	if( !pnode ) { // head
		ht->pht[b].nStartPointer = clink->next;
	}
	else {
		plink->next = clink->next;
	}
	if( !nnode ) { // tail
		ht->pht[b].nEndPointer = clink->prev;
	}
	else {
		nlink->prev = clink->prev;
	}

	DBDEBUG( "+ht->nStart: %d, ht->nEnd: %d\n", ht->nStart, ht->nEnd );
	if( ht->pht[b].nStartPointer == -1 ) { 
		n = ht->pht[b].nNextSlot;
		p = ht->pht[b].nPrevSlot;
		ht->pht[b].nKey = -1;
		if( p != -1 ) {
			ht->pht[p].nNextSlot = ht->pht[b].nNextSlot;
		}
		else {
			ht->nStart = n;
		}
		if( n != -1 ) {
			ht->pht[n].nPrevSlot = p;
		}
		else {
			ht->nEnd = p;
		}
		ht->nUsed--;
	}
	DBDEBUG( "-ht->nStart: %d, ht->nEnd: %d, b: %d, n: %d, p: %d\n", 
		ht->nStart, ht->nEnd, b, n, p );

	return 0;
}

int IDDB_GenKey( const char* sz, int size )
{
	int i_key=0;
	int i;
	unsigned char* p = ( unsigned char* )sz;
	
	if( !p ) return -1;

	for (i=0; i<size; ++i) {
		i_key += p[i];
		i_key += (i_key << 10);
		i_key ^= (i_key >> 6);
	}
	i_key += (i_key << 3);
	i_key ^= (i_key >> 11);
	i_key += (i_key << 15);

	return i_key;
}

int IDDB_Create( IDDB_Desc* desc )
{
	int ret = 0;
	RMuint32 fd, nBytes;
	FATFS_ERROR err;
	IDNode* node;

	err = fatfs_fopen( (RMuint8*)desc->path, _O_WRONLY | _O_CREAT | _O_TRUNC, &fd );
	if( err != FATFS_ERROR_NO_ERROR ) {
		err = fatfs_fopen( (RMuint8*)desc->path, _O_WRONLY | _O_TRUNC, &fd );
	}

	if( err != FATFS_ERROR_NO_ERROR ) {
		DBDEBUG( "failed to open(0x%x)\n", err );
		return -1;
	}

	// Make header
	DBHeader* header = desc->pHeader;
	strcpy( header->szMagicKey, IDDB_MAGIC );
	header->nMajor = IDDB_VER_MAJOR;
	header->nMinor = IDDB_VER_MINOR;
	header->nTotal = 0;
	header->nFreeLinkHead = 0;

	// Make file hash table
	HashTable* ht = ( HashTable* )(desc->pCache + IDDB_HEADER_SIZE);
	memset( (void*)ht, -1, IDDB_FILE_HASH_BLOCK );
	ht->nHashSize = IDDB_FILE_HASH_NUM;
	ht->nUsed = 0;
	ht->nStart = -1;
	ht->nEnd = -1;

	// Make Aritst hash table
	ht = ( HashTable* )( desc->pCache + IDDB_HEADER_SIZE + IDDB_FILE_HASH_BLOCK );
	memset( (void*)ht, -1, IDDB_ID_HASH_BLOCK );
	ht->nHashSize = IDDB_ID_HASH_NUM;
	ht->nUsed = 0;
	ht->nStart = -1;
	ht->nEnd = -1;

	// Make album hash table
	ht = ( HashTable* )( desc->pCache+IDDB_HEADER_SIZE+IDDB_FILE_HASH_BLOCK+IDDB_ID_HASH_BLOCK );
	memset( (void*)ht, -1, IDDB_ID_HASH_BLOCK );
	ht->nHashSize = IDDB_ID_HASH_NUM;
	ht->nUsed = 0;
	ht->nStart = -1;
	ht->nEnd = -1;

	// Make genre hash table
	ht = ( HashTable* )( desc->pCache+IDDB_HEADER_SIZE+IDDB_FILE_HASH_BLOCK+2*IDDB_ID_HASH_BLOCK );
	memset( (void*)ht, -1, IDDB_ID_HASH_BLOCK );
	ht->nHashSize = IDDB_ID_HASH_NUM;
	ht->nUsed = 0;
	ht->nStart = -1;
	ht->nEnd = -1;

	// Write hash block
	err = fatfs_fwrite( fd, (RMuint8*)desc->pCache, IDDB_HASH_BLOCK_SIZE, &nBytes );
	if( err != FATFS_ERROR_NO_ERROR ) ret = -1;
	
	// Create free file nodes
	IDNode* pnode = ( IDNode* )( desc->pCache + IDDB_HASH_BLOCK_SIZE );
	int m = IDDB_FILE_HASH_NUM%IDDB_CACHE_NUM;
	int n = (IDDB_FILE_HASH_NUM+m)/IDDB_CACHE_NUM;
	int i;
	for( i=0; i<n; i++ ) {
		for( int j=0; j<IDDB_CACHE_NUM; j++ ) {
			pnode[j].nDirty = 0;
			pnode[j].nIdx = i*IDDB_CACHE_NUM+j;
			pnode[j].nFreeLink = i*IDDB_CACHE_NUM + j+1;
		}
		if( i == n-1 ) {
			int last = IDDB_FILE_HASH_NUM%IDDB_CACHE_NUM;
			pnode[last-1].nFreeLink = -1;
		}
		fatfs_fwrite( fd, (RMuint8*)pnode, IDDB_CACHE_NUM*sizeof(IDNode), &nBytes );
	}
	
	for( int i=0; i<IDDB_FILE_HASH_NUM; i++ ) {
		desc->pFreeLinkList[i] = i;
	}
	int offset = IDDB_HASH_BLOCK_SIZE+IDDB_FNODE_SIZE*IDDB_FILE_HASH_NUM;
	fatfs_fseek( fd, (RMuint32)offset, SEEK_SET );
	fatfs_fwrite( fd, (RMuint8*)desc->pFreeLinkList, sizeof(short)*IDDB_FILE_HASH_NUM, &nBytes );
	desc->nFD = 0;
	fatfs_fclose( fd );

	return ret;
}

IDDB_Desc* IDDB_AllocDesc( void )
{
	IDDB_Desc* desc;

	desc = new IDDB_Desc;
	// attach shared memory
	desc->shmid = shmget( SHMKEY, SHMSIZE, 0666 );
	if( desc->shmid < 0 ) {
		delete desc;
		desc = 0;
		perror( "shmget" );
		DBDEBUG( "failed to open shared memory\n" );
		return 0;
	}
	if( (desc->pCache=(char*)shmat(desc->shmid, NULL, 0)) == (char*)-1 ) {
		delete desc;
		desc = 0;
		DBDEBUG( "failed to attach shared memory\n" );
		return 0;
	}

	desc->nLinkIdx = -1;
	desc->pHeader = ( DBHeader* )desc->pCache;
	desc->f_HashTable = ( HashTable* )(desc->pCache+IDDB_HEADER_SIZE);
	desc->a_HashTable = ( HashTable* )(desc->pCache+IDDB_HEADER_SIZE+IDDB_FILE_HASH_BLOCK);
	desc->al_HashTable = ( HashTable* )( (char*)desc->a_HashTable + IDDB_ID_HASH_BLOCK );
	desc->g_HashTable = ( HashTable* )( (char*)desc->al_HashTable + IDDB_ID_HASH_BLOCK );
	desc->pNode = ( IDNode* )( (char*)desc->g_HashTable + IDDB_ID_HASH_BLOCK );
	desc->pFreeLinkList = ( short* )((char*)desc->pNode + IDDB_CACHE_NUM*IDDB_FNODE_SIZE+2*sizeof(IDNode));

	return desc;	
}

int IDDB_Load( IDDB_Desc* desc, const char* path )
{
	FATFS_ERROR err;
	RMuint32 fd, nBytes;
	int len, ret=0;

	DBDEBUG( "+IDDB_Load(%s)\n", path );
	
	if( !desc ) {
		DBDEBUG( "INVALID ARGUEMENT desc\n" );
		return -1;
	}

	len = strlen( path );
	desc->path = new char[len+1];
	strcpy( desc->path, path );


	err = fatfs_fopen( (RMuint8*)path, _O_RDONLY, &fd );
	if( err != FATFS_ERROR_NO_ERROR ) {
		DBDEBUG( "failed to open %s(0x%x)\n", path, err );
		return -FATFS_ERROR_PATH_NAME_NOT_FOUND;
	}

	err = fatfs_fread( fd, (RMuint8*)desc->pCache, IDDB_HASH_BLOCK_SIZE, &nBytes );
	if( err != FATFS_ERROR_NO_ERROR ) {
		DBDEBUG( "failed to read %s\n", path );
		return -1;
	}

	// Check Header
	DBHeader* pHeader = ( DBHeader* )desc->pCache;
	if( strncmp( pHeader->szMagicKey, IDDB_MAGIC, IDDB_MAGIC_SIZE ) ) {
		DBDEBUG( "NOT NMLIDDB: %s\n", pHeader->szMagicKey );
		ret = -1;
		goto __end;
	}

	DBDEBUG( "NMLIDDB VERSION %d.%d\n", pHeader->nMajor, pHeader->nMinor );
	if( pHeader->nMajor != IDDB_VER_MAJOR || pHeader->nMinor != IDDB_VER_MINOR ) {
		ret = -1;
		goto __end;
	}

	desc->nFD = fd;

__end:
	fatfs_fclose( fd );
	desc->nFD = 0;

	DBDEBUG( "-IDDB_Load\n" );

	return ret;
}

int IDDB_UpdateMode( IDDB_Desc* desc )
{
	if( !desc ) {
		DBDEBUG( "desc is NULL\n" );
		return -1;
	}
	if( !desc->path ) {
		DBDEBUG( "path is NULL\n" );
		return -1;
	}
	if( desc->nFD ) {
		if( desc->mode ) return 0; 
		fatfs_fclose( (RMuint32)desc->nFD );
	}

	FATFS_ERROR err;
	err = fatfs_fopen( (RMuint8*)desc->path, _O_WRONLY, (RMuint32*)&desc->nFD );
	if( err != FATFS_ERROR_NO_ERROR ) {
		DBDEBUG( "FAILED TO OPEN(0x%x)\n", err );
		return -1;
	}

	desc->mode = 1;

	return 0;
}

int IDDB_ScanMode( IDDB_Desc* desc )
{
	if( !desc ) {
		DBDEBUG( "desc is NULL\n" );
		return -1;
	}
	if( !desc->path ) {
		DBDEBUG( "path is NULL\n" );
		return -1;
	}
	if( desc->nFD ) {
		if( desc->mode == 0 ) return 0;
		fatfs_fclose( (RMuint32)desc->nFD );
	}

	FATFS_ERROR err;
	err = fatfs_fopen( (RMuint8*)desc->path, _O_RDONLY, (RMuint32*)&desc->nFD );
	if( err != FATFS_ERROR_NO_ERROR ) {
		DBDEBUG( "FAILED TO OPEN(0x%x)\n", err );
		return -1;
	}
	desc->mode = 0;

	return 0;
}

int IDDB_Insert( IDDB_Desc* desc, IDNode* pNode, int key )
{
	int ret = 0;
	int nbytes;

	DBDEBUG( "+IDDB_Insert" );
	if( !desc ) {
		DBDEBUG( "desc is NULL\n" );
		return -1;
	}

	HashTable* ht = desc->f_HashTable;
	// Save file bucket
	int bucket = AllocBucket( ht, key );
	// NO MORE free bucket
	if( bucket < 0 ) {
		DBDEBUG( "FAILED TO ALLOCATE BUCKET\n" );
		return -1;
	}

	ht->pht[bucket].nKey = key;
	ht->pht[bucket].nStartPointer = pNode->nIdx;
	
	// Insert artist bucket
	key = IDDB_GenKey( pNode->szArtist, pNode->nArtistSize );
	ht = desc->a_HashTable;
	InsertIDHash( desc, ht, HASH_ARTIST, key, pNode );

	// Insert album bucket;
	key = IDDB_GenKey( pNode->szAlbum, pNode->nAlbumSize );
	ht = desc->al_HashTable;
	InsertIDHash( desc, ht, HASH_ALBUM, key, pNode );

	// Insert genre bucket
	key = IDDB_GenKey( pNode->szGenre, pNode->nGenreSize );
	ht = desc->g_HashTable;
	InsertIDHash( desc, ht, HASH_GENRE, key, pNode );

	int offset = IDDB_HASH_BLOCK_SIZE + sizeof(IDNode)*pNode->nIdx;

	// Insert file-node 
	IDDB_UpdateMode( desc );
	fatfs_fseek( (RMuint32)desc->nFD, (RMuint32)offset, SEEK_SET );
	fatfs_fwrite( (RMuint32)desc->nFD, (RMuint8*)pNode, sizeof(IDNode), (RMuint32*)&nbytes );
	
	DBDEBUG( "-IDDB_Insert\n" );
	
	return 0;
}

int IDDB_Delete( IDDB_Desc* desc, int bucket, int key, IDNode* pnode )
{
	int ret = 0;
	int nbytes;

	DBDEBUG( "+IDDB_Delete\n" );
	// Delete file hash
	HashTable* ht = desc->f_HashTable;
	//int bucket = IDDB_FindKey( ht, key );
	//if( bucket < 0 ) {
	//	printf( "FAILED TO FIND KEY: %x\n", key );
	//	return -1;
	//}
	
	// delete artist hash table
	// change read-mode
	IDNode* ap, *alp, *gp;
	IDNode* an, *aln, *gn;
	int offset;
	int k , b, n, p;

	ap = alp = gp = 0;
	an = aln = gn = 0;
	// delete tag id
	// Artist hash table
	if( pnode->nArtistPrev != -1 ) {
		offset = IDDB_HASH_BLOCK_SIZE + sizeof(IDNode)*pnode->nArtistPrev;
		ap = new IDNode;
		fatfs_fseek( (RMuint32)desc->nFD, (RMuint32)offset, SEEK_SET );
		fatfs_fread( (RMuint32)desc->nFD, (RMuint8*)ap, sizeof(IDNode), (RMuint32*)&nbytes );
	}
	if( pnode->nArtistNext != -1 ) {
		offset = IDDB_HASH_BLOCK_SIZE + sizeof(IDNode)*pnode->nArtistNext;
		an = new IDNode;
		fatfs_fseek( (RMuint32)desc->nFD, (RMuint32)offset, SEEK_SET );
		fatfs_fread( (RMuint32)desc->nFD, (RMuint8*)an, sizeof(IDNode), (RMuint32*)&nbytes );
	}
	DeleteIDHash( desc->a_HashTable, pnode, ap, an, HASH_ARTIST );
	IDDB_UpdateMode( desc );
	if( ap ) {
		offset = IDDB_HASH_BLOCK_SIZE + sizeof(IDNode)*pnode->nArtistPrev;
		fatfs_fseek( (RMuint32)desc->nFD, (RMuint32)offset, SEEK_SET );
		fatfs_fwrite( (RMuint32)desc->nFD, (RMuint8*)ap, sizeof(IDNode), (RMuint32*)&nbytes );
		delete ap;
	}
	if( an ) {
		offset = IDDB_HASH_BLOCK_SIZE + sizeof(IDNode)*pnode->nArtistNext;
		fatfs_fseek( (RMuint32)desc->nFD, (RMuint32)offset, SEEK_SET );
		fatfs_fwrite( (RMuint32)desc->nFD, (RMuint8*)an, sizeof(IDNode), (RMuint32*)&nbytes );
		delete an;
	}

	// Album
	IDDB_ScanMode( desc );
	if( pnode->nAlbumPrev != -1 ) {
		offset = IDDB_HASH_BLOCK_SIZE + sizeof(IDNode)*pnode->nAlbumPrev;
		alp = new IDNode;
		fatfs_fseek( (RMuint32)desc->nFD, (RMuint32)offset, SEEK_SET );
		fatfs_fread( (RMuint32)desc->nFD, (RMuint8*)alp, sizeof(IDNode), (RMuint32*)&nbytes );
	}
	if( pnode->nAlbumNext != -1 ) {
		offset = IDDB_HASH_BLOCK_SIZE + sizeof(IDNode)*pnode->nAlbumNext;
		aln = new IDNode;
		fatfs_fseek( (RMuint32)desc->nFD, (RMuint32)offset, SEEK_SET );
		fatfs_fread( (RMuint32)desc->nFD, (RMuint8*)aln, sizeof(IDNode), (RMuint32*)&nbytes );
	}
	DeleteIDHash( desc->al_HashTable, pnode, alp, aln, HASH_ALBUM );
	IDDB_UpdateMode( desc );
	if( alp ) {
		offset = IDDB_HASH_BLOCK_SIZE + sizeof(IDNode)*pnode->nAlbumPrev;
		fatfs_fseek( (RMuint32)desc->nFD, (RMuint32)offset, SEEK_SET );
		fatfs_fwrite( (RMuint32)desc->nFD, (RMuint8*)alp, sizeof(IDNode), (RMuint32*)&nbytes );
		delete alp;
	}
	if( aln ) {
		offset = IDDB_HASH_BLOCK_SIZE + sizeof(IDNode)*pnode->nAlbumNext;
		fatfs_fseek( (RMuint32)desc->nFD, (RMuint32)offset, SEEK_SET );
		fatfs_fwrite( (RMuint32)desc->nFD, (RMuint8*)aln, sizeof(IDNode), (RMuint32*)&nbytes );
		delete aln;
	}

	// Genre
	IDDB_ScanMode( desc );
	if( pnode->nGenrePrev != -1 ) {
		offset = IDDB_HASH_BLOCK_SIZE + sizeof(IDNode)*pnode->nGenrePrev;
		gp = new IDNode;
		fatfs_fseek( (RMuint32)desc->nFD, (RMuint32)offset, SEEK_SET );
		fatfs_fread( (RMuint32)desc->nFD, (RMuint8*)gp, sizeof(IDNode), (RMuint32*)&nbytes );
	}
	if( pnode->nGenreNext != -1 ) {
		offset = IDDB_HASH_BLOCK_SIZE + sizeof(IDNode)*pnode->nGenreNext;
		gn = new IDNode;
		fatfs_fseek( (RMuint32)desc->nFD, (RMuint32)offset, SEEK_SET );
		fatfs_fread( (RMuint32)desc->nFD, (RMuint8*)gn, sizeof(IDNode), (RMuint32*)&nbytes );
	}
	DeleteIDHash( desc->g_HashTable, pnode, gp, gn, HASH_GENRE );
	IDDB_UpdateMode( desc );
	if( gp ) {
		offset = IDDB_HASH_BLOCK_SIZE + sizeof(IDNode)*pnode->nGenrePrev;
		fatfs_fseek( (RMuint32)desc->nFD, (RMuint32)offset, SEEK_SET );
		fatfs_fwrite( (RMuint32)desc->nFD, (RMuint8*)gp, sizeof(IDNode), (RMuint32*)&nbytes );
		delete gp;
	}
	if( gn ) {
		offset = IDDB_HASH_BLOCK_SIZE + sizeof(IDNode)*pnode->nGenreNext;
		fatfs_fseek( (RMuint32)desc->nFD, (RMuint32)offset, SEEK_SET );
		fatfs_fwrite( (RMuint32)desc->nFD, (RMuint8*)gn, sizeof(IDNode), (RMuint32*)&nbytes );
		delete gn;
	}

	// delete file node
	p = ht->pht[bucket].nStartPointer;
	offset = IDDB_HASH_BLOCK_SIZE + sizeof(IDNode)*p;
	pnode->nArtistPrev = pnode->nArtistNext = -1;
	pnode->nAlbumPrev = pnode->nAlbumNext = -1;
	pnode->nGenrePrev = pnode->nGenreNext = -1;
	pnode->nDirty = 0;
	pnode->nFreeLink = desc->pHeader->nFreeLinkHead;
	desc->pHeader->nFreeLinkHead = pnode->nIdx;
	//desc->nLinkIdx--;
	//if( desc->nLinkIdx < 0 ) desc->nLinkIdx = IDDB_FILE_HASH_NUM-1;
	desc->pFreeLinkList[IDDB_FILE_HASH_NUM-desc->pHeader->nTotal] = pnode->nIdx;
	desc->pHeader->nTotal--;

	// delete file hash table
	desc->f_HashTable->nUsed--;
	desc->f_HashTable->pht[bucket].nKey = -1;
	n = desc->f_HashTable->pht[bucket].nNextSlot;
	p = desc->f_HashTable->pht[bucket].nPrevSlot;
	if( p == -1 ) { // head 
		desc->f_HashTable->nStart = n;
	}
	else {
		desc->f_HashTable->pht[p].nNextSlot = n;
	}
	if( n == -1 ) { // tail
		desc->f_HashTable->nEnd = p;
	}
	else {
		desc->f_HashTable->pht[n].nPrevSlot = p;
	}
	offset = IDDB_HASH_BLOCK_SIZE + sizeof(IDNode)*pnode->nIdx;
	fatfs_fseek( (RMuint32)desc->nFD, (RMuint32)offset, SEEK_SET );
	fatfs_fwrite( (RMuint32)desc->nFD, (RMuint8*)pnode, sizeof(IDNode), (RMuint32*)&nbytes );
	
	DBDEBUG( "-IDDB_Delete\n" );
	return 0;
}

int IDDB_Close( IDDB_Desc** desc )
{
	IDDB_Desc* h = *desc;
	if( h->nFD ) {
		fatfs_fclose( (RMuint32)h->nFD );
	}

	if( h->path ) delete[] h->path;
	if( h->pCache != (char*)-1 ) {
		shmdt( h->pCache );
	}
	
	delete h;
	*desc = 0;

	return 0;
}

IDNode* IDDB_FetchNode( IDDB_Desc* desc, int node )
{
	int offset = IDDB_HASH_BLOCK_SIZE + sizeof(IDNode)*node;
	int nbytes;
	IDNode* pnode = &desc->pNode[256];
	FATFS_ERROR err;

	if( node == -1 ) return 0;
	fatfs_fseek( (RMuint32)desc->nFD, (RMuint32)offset, SEEK_SET );
	err = fatfs_fread( (RMuint32)desc->nFD, (RMuint8*)pnode, sizeof(IDNode), (RMuint32*)&nbytes );
	if( err != FATFS_ERROR_NO_ERROR ) {
		DBDEBUG( "read error(0x%x)\n", err );
		return 0;
	}

	if( pnode->nDirty == 0 ) pnode = 0;

	return pnode;
}

int IDDB_Release( IDDB_Desc* desc )
{
	int err;
	if( desc->nFD ) {
		err = fatfs_fclose( (RMuint32)desc->nFD );
		if( err != FATFS_ERROR_NO_ERROR ) printf( "IDDB_Release: 0x%x\n", err );
	}
	desc->nFD = 0;
	DBDEBUG( "IDDB_Release\n" );
	return 0;
}

