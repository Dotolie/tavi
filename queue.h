//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : TAVI putChunkQueue Class Header
// 
// $Source: /home/cvsroot/TAVI020/tavi/queue.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: queue.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.2  2005/10/13 01:25:21  etnlwind
// Update for Header Info
//
//*****************************************************************************



#ifndef _QUEUE_H
#define _QUEUE_H

#include "rmbasic.h"

#define CHUNKQ_MAX_ELEMENTS		(1024)

typedef struct tagputChunk
{
	RMuint8		*buffer;
	RMuint32	length;
	struct tagputChunk *next;
} putChunk;

class putChunkQueue
{
	putChunk	m_Elements[CHUNKQ_MAX_ELEMENTS];
	putChunk	*m_headFree;
	putChunk	*m_headBusy;

public:
	putChunkQueue ();
	~putChunkQueue ();
	
	// places all elements on the freeQ
	void Reset ();
	// returns the head of the busyQ
	putChunk *DeQueue ();
	// returns the head of the freeQ
	putChunk *CanQueue ();
	// returns 0 if the queue succeeded
	// returns !0 if the queue did not succeed
	RMuint32 Queue (RMuint8 *pbuffer, RMuint32 length);
};

#endif
