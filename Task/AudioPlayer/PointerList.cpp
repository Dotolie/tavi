#ifdef _WIN32
#include "StdAfx.h"
#else
#include <stdio.h>
#include <string.h>
#endif

#include "PointerList.h"

unsigned int g_i_CurrentMemPos;
unsigned int g_i_TotalMusicMemSize;
char* m_pMusicMem;

static unsigned long Align4( unsigned long addr )
{
	addr = (addr&~0x03) + 4;
	return addr;
}

CFirmPointerList::CFirmPointerList(void)
{
	m_i_Count=0;
	m_pHeadPtr=NULL;
	m_pTailPtr=NULL;
}

CFirmPointerList::~CFirmPointerList(void)
{
}

void CFirmPointerList::AddHead(void *ptr)
{
	ST_PTRITEM *pitem;

//	pitem = new ST_PTRITEM;

	pitem = (ST_PTRITEM *)&m_pMusicMem[g_i_CurrentMemPos];
	g_i_CurrentMemPos+=sizeof(ST_PTRITEM);
	g_i_CurrentMemPos = Align4( g_i_CurrentMemPos );

//	memset(pitem,0,sizeof(ST_PTRITEM));

	pitem->ptr = ptr;

	if(m_pTailPtr==NULL)
	{
		m_pTailPtr = pitem;
	}

	if(m_pHeadPtr)
	{
		pitem->pnext = (void *)m_pHeadPtr;
		m_pHeadPtr->pprev = (ST_PTRITEM *)pitem;

		m_pHeadPtr = pitem;
	}
	else
	{
		m_pHeadPtr = pitem;
	}

	m_i_Count++;
}

void CFirmPointerList::AddTail(void *ptr)
{
	ST_PTRITEM *pitem;

//	pitem = new ST_PTRITEM;
//	memset(pitem,0,sizeof(ST_PTRITEM));

	pitem = (ST_PTRITEM *)&m_pMusicMem[g_i_CurrentMemPos];
	g_i_CurrentMemPos+=sizeof(ST_PTRITEM);

	pitem->ptr = ptr;

	if(m_pTailPtr)
	{
		m_pTailPtr->pnext = pitem;
		pitem->pprev = m_pTailPtr;

		m_pTailPtr = pitem;
	}
	else
	{
		m_pTailPtr = pitem;
	}

	if(m_pHeadPtr==NULL)
	{
		m_pHeadPtr = pitem;
	}

	m_i_Count++;
}


void *CFirmPointerList::GetHead()
{
	if(m_pHeadPtr==NULL)return NULL;

	return m_pHeadPtr->ptr;
}

void *CFirmPointerList::GetTail()
{
	if(m_pTailPtr==NULL)return NULL;

	return m_pTailPtr->ptr;
}

void *CFirmPointerList::GetHeadPtr()
{
	if(m_pHeadPtr==NULL)return NULL;

	return m_pHeadPtr;
}

void *CFirmPointerList::GetTailPtr()
{
	if(m_pTailPtr==NULL)return NULL;

	return m_pTailPtr;
}


void *CFirmPointerList::GetAt(int i_index)
{
	if(m_pHeadPtr==NULL) return NULL;

	if(m_i_Count-1<i_index)return NULL;

	ST_PTRITEM *pitem = m_pHeadPtr;

	for(int i=0;i<i_index;i++)
	{
		if(pitem->pnext==NULL)
		{
			break;
		}
		pitem = (ST_PTRITEM *)pitem->pnext;
	}

	return pitem->ptr;
}

int CFirmPointerList::Find(void *ptr)
{
	if(m_pHeadPtr==NULL) return -1;

	ST_PTRITEM *pitem = m_pHeadPtr;

	for(int i=0;i<m_i_Count;i++)
	{
		if(pitem->ptr == ptr)
		{
			return i;
		}
		pitem = (ST_PTRITEM *)pitem->pnext;
	}

	return -1;
}



ST_PTRITEM *CFirmPointerList::GetPtrItemAt(int i_index)
{
	if(m_pHeadPtr==NULL) return NULL;

	ST_PTRITEM *pitem = m_pHeadPtr;

	for(int i=0;i<i_index;i++)
	{
		if(pitem->pnext==NULL)
		{
			break;
		}
		pitem = (ST_PTRITEM *)pitem->pnext;
	}

	return pitem;
}



BOOL CFirmPointerList::IsEmpty()
{
	if(m_i_Count>0)return FALSE;
	return TRUE;
}




void *CFirmPointerList::Get_NextPtrItem(void *pt)
{
	ST_PTRITEM *pitem = (ST_PTRITEM *)pt;

	if(pitem && pitem->pnext)
		return pitem->pnext;

	return NULL;
}

void *CFirmPointerList::Get_PrevPtrItem(void *pt)
{
	ST_PTRITEM *pitem = (ST_PTRITEM *)pt;

	if(pitem && pitem->pprev)
		return pitem->pprev;

	return NULL;
}

void *CFirmPointerList::Get_ItemFromPtrItem(void *pt)
{
	ST_PTRITEM *pitem = (ST_PTRITEM *)pt;

	if(pitem && pitem)
		return pitem->ptr;

	return NULL;
}


CPointerList::CPointerList(void)
{
	m_i_Count=0;
	m_pHeadPtr=NULL;
	m_pTailPtr=NULL;
}

CPointerList::~CPointerList(void)
{
	RemoveAll();
}

void CPointerList::AddHead(void *ptr)
{
	#if 0
	ST_PTRITEM *pitem;

	pitem = new ST_PTRITEM;
	memset(pitem,0,sizeof(ST_PTRITEM));

	pitem->ptr = ptr;

	if(m_pTailPtr==NULL)
	{
		m_pTailPtr = pitem;
	}

	if(m_pHeadPtr)
	{
		pitem->pnext = (void *)m_pHeadPtr;
		m_pHeadPtr->pprev = (ST_PTRITEM *)pitem;

		m_pHeadPtr = pitem;
	}
	else
	{
		m_pHeadPtr = pitem;
	}

	m_i_Count++;
	#endif
}

void CPointerList::AddTail(void *ptr)
{
	ST_PTRITEM *pitem;

	pitem = new ST_PTRITEM;

	memset(pitem,0,sizeof(ST_PTRITEM));

	pitem->ptr = ptr;

	if(m_pTailPtr)
	{
		m_pTailPtr->pnext = pitem;
		pitem->pprev = m_pTailPtr;

		m_pTailPtr = pitem;
	}
	else
	{
		m_pTailPtr = pitem;
	}

	if(m_pHeadPtr==NULL)
	{
		m_pHeadPtr = pitem;
	}

	m_i_Count++;
}


void *CPointerList::GetHead()
{
	if(m_pHeadPtr==NULL)return NULL;

	return m_pHeadPtr->ptr;
}

void *CPointerList::GetTail()
{
	if(m_pTailPtr==NULL)return NULL;

	return m_pTailPtr->ptr;
}

void *CPointerList::GetHeadPtr()
{
	if(m_pHeadPtr==NULL)return NULL;

	return m_pHeadPtr;
}

void *CPointerList::GetTailPtr()
{
	if(m_pTailPtr==NULL)return NULL;

	return m_pTailPtr;
}


void *CPointerList::GetAt(int i_index)
{
	if(m_pHeadPtr==NULL) return NULL;

	if(m_i_Count-1<i_index)return NULL;

	ST_PTRITEM *pitem = m_pHeadPtr;

	for(int i=0;i<i_index;i++)
	{
		if(pitem->pnext==NULL)
		{
			break;
		}
		pitem = (ST_PTRITEM *)pitem->pnext;
	}

	return pitem->ptr;
}

int CPointerList::Find(void *ptr)
{
	if(m_pHeadPtr==NULL) return -1;

	ST_PTRITEM *pitem = m_pHeadPtr;

	for(int i=0;i<m_i_Count;i++)
	{
		if(pitem->ptr == ptr)
		{
			return i;
		}
		pitem = (ST_PTRITEM *)pitem->pnext;
	}

	return -1;
}



ST_PTRITEM *CPointerList::GetPtrItemAt(int i_index)
{
	if(m_pHeadPtr==NULL) return NULL;

	ST_PTRITEM *pitem = m_pHeadPtr;

	for(int i=0;i<i_index;i++)
	{
		if(pitem->pnext==NULL)
		{
			break;
		}
		pitem = (ST_PTRITEM *)pitem->pnext;
	}

	return pitem;
}



BOOL CPointerList::IsEmpty()
{
	if(m_i_Count>0)return FALSE;
	return TRUE;
}

void CPointerList::RemoveAt(int i_index)
{
	if(m_pHeadPtr==NULL) return;

	ST_PTRITEM *pitem = m_pHeadPtr;

	for(int i=0;i<i_index;i++)
	{
		if(pitem->pnext==NULL)
		{
			return;
		}
		pitem = (ST_PTRITEM *)pitem->pnext;
	}

	if(pitem)
	{
		if(m_pHeadPtr==pitem)
		{
			RemoveHead();
		}
		else if(m_pTailPtr==pitem)
		{
			RemoveTail();
		}
		else 
		{
			if(pitem->pnext)
			{
				((ST_PTRITEM *)(pitem->pnext))->pprev = pitem->pprev;
			}
			if(pitem->pprev)
			{
				((ST_PTRITEM *)(pitem->pprev))->pnext = pitem->pnext;
			}
			delete pitem;
			m_i_Count--;

			if(m_i_Count==0)
			{
				m_pHeadPtr=NULL;
				m_pTailPtr=NULL;
			}

		}
	}
}

void CPointerList::RemoveHead()
{
	ST_PTRITEM *pitem = m_pHeadPtr;

	if(m_pHeadPtr)
	{
		if(m_pHeadPtr->pnext)
		{
			m_pHeadPtr = (ST_PTRITEM *)m_pHeadPtr->pnext;
			m_pHeadPtr->pprev = NULL;
		}
		else
			m_pHeadPtr=NULL;

		memset(pitem,0,sizeof(ST_PTRITEM));
		delete pitem;
		pitem=NULL;

		m_i_Count--;

		if(m_i_Count==0)
		{
			m_pHeadPtr=NULL;
			m_pTailPtr=NULL;
		}
	}
}
void CPointerList::RemoveTail()
{
	ST_PTRITEM *pitem = m_pTailPtr;

	if(m_pTailPtr)
	{
		if(m_pTailPtr->pprev)
		{
			m_pTailPtr = (ST_PTRITEM *)m_pTailPtr->pprev;
			m_pTailPtr->pnext = NULL;
		}
		else
			m_pTailPtr=NULL;

		memset(pitem,0,sizeof(ST_PTRITEM));
		delete pitem;
		pitem=NULL;

		m_i_Count--;

		if(m_i_Count==0)
		{
			m_pHeadPtr=NULL;
			m_pTailPtr=NULL;
		}
	}
}
void CPointerList::RemoveAll()
{
	while(m_i_Count)
	{
		RemoveTail();
	}
}



void CPointerList::Swap(int i_index1,int i_index2)
{
	if(i_index1==i_index2)return;
	if(i_index1<0)return;
	if(i_index2<0)return;

	ST_PTRITEM *p1 = GetPtrItemAt(i_index1);	
	ST_PTRITEM *p2 = GetPtrItemAt(i_index2);	

	void  *ptemp1 = 	(ST_PTRITEM *)p1->ptr;
	void  *ptemp2 = 	(ST_PTRITEM *)p2->ptr;

	p1->ptr = ptemp2;
	p2->ptr = ptemp1;
}



void *CPointerList::Get_NextPtrItem(void *pt)
{
	ST_PTRITEM *pitem = (ST_PTRITEM *)pt;

	if(pitem && pitem->pnext)
		return pitem->pnext;

	return NULL;
}

void *CPointerList::Get_PrevPtrItem(void *pt)
{
	ST_PTRITEM *pitem = (ST_PTRITEM *)pt;

	if(pitem && pitem->pprev)
		return pitem->pprev;

	return NULL;
}

void *CPointerList::Get_ItemFromPtrItem(void *pt)
{
	ST_PTRITEM *pitem = (ST_PTRITEM *)pt;

	if(pitem && pitem)
		return pitem->ptr;

	return NULL;
}

