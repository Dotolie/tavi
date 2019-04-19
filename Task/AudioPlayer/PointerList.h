#ifndef __POINTER_LIST_H__
#define __POINTER_LIST_H__
#ifdef _WIN32
#pragma once
#else
#define FALSE		0
#define TRUE			1
typedef bool BOOL;
#endif


typedef struct 
{
	void *ptr;
	void *pnext;
	void *pprev;
}ST_PTRITEM;

class CFirmPointerList
{
public:
	CFirmPointerList(void);
	~CFirmPointerList(void);

	int m_i_Count;
	int GetCount()
	{
		return m_i_Count;
	};

	void AddHead(void *ptr);
	void AddTail(void *ptr);
	void  *GetAt(int i_index);
	void  *GetHead();
	void  *GetTail();

	void *GetHeadPtr();
	void *GetTailPtr();

	ST_PTRITEM *GetPtrItemAt(int i_index);

	void *Get_NextPtrItem(void *pitem);
	void *Get_PrevPtrItem(void *pitem);

	void *Get_ItemFromPtrItem(void *pitem);

	BOOL IsEmpty();
	int Find(void *ptr);

	ST_PTRITEM *m_pHeadPtr;
	ST_PTRITEM *m_pTailPtr;

};



class CPointerList
{
public:
	CPointerList(void);
	~CPointerList(void);

	int m_i_Count;
	int GetCount()
	{
		return m_i_Count;
	}

	void AddHead(void *ptr);
	void AddTail(void *ptr);
	void  *GetAt(int i_index);
	void  *GetHead();
	void  *GetTail();

	void *GetHeadPtr();
	void *GetTailPtr();

	ST_PTRITEM *GetPtrItemAt(int i_index);

	void *Get_NextPtrItem(void *pitem);
	void *Get_PrevPtrItem(void *pitem);

	void *Get_ItemFromPtrItem(void *pitem);

	BOOL IsEmpty();
	void RemoveAt(int i_index);
	void RemoveHead();
	void RemoveTail();
	void RemoveAll();

	int Find(void *ptr);

	void Swap(int i_index1,int i_index2);

	ST_PTRITEM *m_pHeadPtr;
	ST_PTRITEM *m_pTailPtr;

};

#endif
