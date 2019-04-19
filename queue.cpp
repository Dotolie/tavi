//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : TAVI putChunkQueue Class Source
// 
// $Source: /home/cvsroot/TAVI020/tavi/queue.cpp,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: queue.cpp,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.2  2005/10/13 01:25:21  etnlwind
// Update for Header Info
//
//*****************************************************************************


#include "queue.h"

#if 0
#include <stdio.h>
static void debug_break (void)
{
}
#define ASSERT(exp)					((void)((exp)?1:(printf ("***ASSERT failed: line %d, file %s\n", __LINE__,__FILE__), debug_break(), 0)))
#define DEBUGMSG(cond,printf_exp)	((void)((cond)?(printf printf_exp),1:0))
#else
#define ASSERT(exp)
#define DEBUGMSG(cond,printf_exp)
#endif


putChunkQueue::putChunkQueue ()
{
}

putChunkQueue::~putChunkQueue ()
{
}
	
// places all elements on the freeQ
void putChunkQueue::Reset ()
{
	RMint32 i;
	m_headBusy = 0;
	m_headFree = &m_Elements[0];
	for (i=0; i<CHUNKQ_MAX_ELEMENTS-1; i++)
	{
		m_Elements[i].buffer = 0;
		m_Elements[i].length = 0;
		m_Elements[i].next = &m_Elements[i+1];
	}
	m_Elements[i].buffer = 0;
	m_Elements[i].length = 0;
	m_Elements[i].next = 0;
}

// returns the head of the busyQ
putChunk *putChunkQueue::DeQueue ()
{
	putChunk *tmp;
	// dequeue from busyQ
	tmp = m_headBusy;
	if (m_headBusy)
		m_headBusy = m_headBusy->next;
	// requeue back to freeQ
	if (tmp)
	{
		tmp->next = m_headFree;
		m_headFree = tmp;
	}
	return tmp;
}

putChunk *putChunkQueue::CanQueue ()
{
	return m_headFree; 
}

// returns 0 if the queue succeeded
// returns !0 if the queue did not succeed
RMuint32 putChunkQueue::Queue (RMuint8 *pbuffer, RMuint32 length)
{
	// dequeue something from the freeQ
	putChunk *tmp, *p;
	tmp = m_headFree;
	if (m_headFree)
		m_headFree = m_headFree->next;
	if (tmp == 0)
	{
		// no free elements left
		ASSERT (0);
		return 1;
	}
	tmp->buffer = pbuffer;
	tmp->length = length;
	tmp->next = 0;
	// queue to tail of busyQ
	p = m_headBusy;
	if (p)
	{
		while (p->next)
			p = p->next;
		p->next = tmp;
	}
	else
	{
		m_headBusy = tmp;
	}
	return 0;
}

