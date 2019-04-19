/*
 * object.h
 * 
 * Copyright(C) 2004 New Media Life Inc.
 * 
 * Base control class - CObject
 *
 * Author: Lee Seok<zstein@newmedialife.com>
 * Date: 12/11/2004
 * Update:
 * History
 * $Log: object.h,v $
 * Revision 1.2  2006/06/22 01:48:34  zstein
 * - added CList<T>::insert(...).
 *
 * Revision 1.1.1.1  2006/02/22 12:33:31  zstein
 * TAVI020 REBUILD
 *
 * Revision 1.21  2006/01/19 05:26:14  zstein
 * *** empty log message ***
 *
 * Revision 1.20  2005/11/26 03:18:51  zstein
 * *** empty log message ***
 *
 * Revision 1.19  2005/11/16 06:44:22  zstein
 * *** empty log message ***
 *
 * Revision 1.18  2005/10/17 02:41:36  yong
 * refactoring
 *
 * Revision 1.17  2005/10/07 06:32:40  zstein
 * CList::Delete()Ãß°¡
 *
 * Revision 1.16  2005/08/03 03:23:20  zstein
 * - updated.
 *
 * Revision 1.15  2005/07/16 04:13:52  zstein
 * - fixed bugs & added functions.
 *
 * Revision 1.14  2005/06/16 23:49:49  zstein
 * - bug fixed
 *
 */

 #ifndef __BASE_OBJECT_H__
 #define __BASE_OBJECT_H__

#ifndef NULL
#define NULL 0
#endif

#include <stdlib.h>

//typedef struct WideChar_t {
//	char LowByte;
//	char HighByte;
//} WideChar_t;

/* Class CPoint
 */
class CPoint {
private:
protected:
public:
	int px, py;
	// Constructor and Destructor
	CPoint( int x=0, int y=0 ) : px( x ), py( y ) {}
	~CPoint() {}

	// Access Methods
	int GetX( void ) const { return px; }
	int GetY( void ) const { return py; }
	void SetX( int x ) { px = x; }
	void SetY( int y ) { py = y; }
	void SetXY( int x, int y ) {px = x, py = y; }

	// operators
	CPoint operator+( const CPoint& rhs ) {
		return CPoint(this->px + rhs.px, this->py + rhs.py);
	}

	CPoint operator-( const CPoint rhs ) {
		return CPoint(this->px - rhs.px, this->py - rhs.py);
	}

	CPoint& operator+=( const CPoint& rhs ) {
		this->px += rhs.px;
		this->py += rhs.py;
		return *this;
	}

	CPoint& operator-=( const CPoint& rhs ) {
		this->px -= rhs.px;
		this->py -= rhs.py;
		return *this;
	}

	CPoint operator=( const CPoint rhs ) {
		return CPoint(rhs.px, rhs.py);
	}
};
 
/* Class CSize
 */
class CSize 
{
private:
protected:
public:
	int width;
	int height;
	
	CSize( int w=0, int h=0 ) : width( w ), height( h ) {}
    CSize( const CSize& sz ) { width = sz.width; height = sz.height; }
	~CSize() {}

	// Access Methods
	int GetWidth( void ) const { return width; }
	int GetHeight( void ) const { return height; }
	void SetSize( int w, int h ) { width = w, height = h; }
	void SetSize( const CSize size ) { width = size.width, height = size.height; }
	void GetSize( int& w, int&h ) { w=width; h=height; }
	
	// Operators
	CSize operator+( const CSize& rhs ) {
		return CSize(this->width + rhs.width, this->height + rhs.height);
	}

	CSize operator-( const CSize& rhs ) {
		return CSize(this->width - rhs.width, this->height - rhs.height);
	}

	CSize& operator+=( const CSize& rhs ) {
		width += rhs.width;
		height += rhs.height;
		return *this;
	}

	CSize& operator-=( const CSize& rhs ) {
		width -= rhs.width;
		height -= rhs.height;
		return *this;
	}

	//CSize operator= (const CSize& rhs ) {
	//	return CSize(rhs.width, rhs.height);
	//}
 };

class CRect {
private:
protected:
public:
	int x1, y1, x2, y2;
	CRect() : x1( 0 ), y1( 0 ), x2( 0 ), y2( 0 ) {}
	CRect( int ltx, int lty, int rbx, int rby ) : x1( ltx ), y1( lty ), x2( rbx ), y2( rby ) {}
	CRect( const CPoint& lefttop, const CPoint& rightbottom ) {
		x1 = lefttop.px;
		y1 = lefttop.py;
		x2 = rightbottom.px;
		y2 = rightbottom.py;
	}
	CRect( const CRect& rhs ) {
		x1 = rhs.x1;
		y1 = rhs.y1;
		x2 = rhs.x2;
		y2 = rhs.y2; 
	}
	~CRect() {}

	// Operators
	CRect& operator=( const CRect& rhs ) {
		x1 = rhs.x1, y1 = rhs.y1;
		x2 = rhs.x2, y2 = rhs.y2;
		return *this;
	}

	// Methods
	CSize GetSize( void ) const {
		int w = abs( x2-x1 )+1;
		int h = abs( y2-y1 )+1;
		return CSize( w, h );
	}
	int GetWidth( void ) const {
		return (abs( x2-x1 )+1);
	}
	int GetHeight( void ) const {
		return (abs( y2-y1 )+1);
	}
	void SetRect( int x1, int y1, int x2, int y2 ) {
		*this = CRect( x1, y1, x2, y2 );
	}
	void SetRect( const CRect& rc ) {
		*this = rc;
	}
};

#define MAX_NAME_LEN	30

class CObject
{
private:
protected:	
	char m_name[MAX_NAME_LEN];
	CSize m_size;
	CPoint m_coord;
public:
	CObject( CSize size, CPoint pt, const char* obj_name=NULL ) : m_size( size ), m_coord( pt ) {
		if( obj_name ) {
			int i;
			for ( i=0; obj_name[i] && i < sizeof(m_name)-1; i++ ) {
				m_name[i] = obj_name[i];
			}
			m_name[i] = 0;
		}
	}
	CObject(  ) {}
	~CObject() {}
};

#undef MAX_NAME_LEN

/*
 * CList : Doubly Link list
 */
template< class T > class CList;
template< class T >
class CNode
{
public:
	CNode< T > *m_pPrev, *m_pNext;
	T* m_Data;
	CNode();
	CNode( T* newObj );
	~CNode();
	
	T* GetData() const;
	void SetData( T* nObj );
	void SetPrev( CNode *pPrev );
	void SetNext( CNode *pNext );
};
template< class T >
CNode< T >::CNode() {
	m_pPrev = m_pNext = NULL;
}
template< class T >
CNode< T >::CNode( T* newObj ) {
	m_pPrev = m_pNext = NULL;
	m_Data = newObj;
}
template< class T >
CNode< T >::~CNode() {
	if( m_Data ) delete m_Data;
	m_pPrev = NULL;
	m_pNext = NULL;
}
template< class T >
T* CNode< T >::GetData() const { return m_Data; }
template< class T >
void CNode< T >::SetData( T* obj ) { m_Data = obj; }
template< class T >
void CNode< T >::SetPrev( CNode* prev ) { m_pPrev = prev; }
template< class T >
void CNode< T >::SetNext( CNode* next ) { m_pNext = next; }


#define MAX_BUF		100

template< class T >
class CList 
{
public:
	typedef bool( *PFN_Compare )( T* pNew, T* pPivot );
	typedef int( *PFN_QuickCmp )( T* pNew, T* pPivot );
private:
protected:
	CNode< T >* m_pHead;
	CNode< T >* m_pTail;
	CNode< T >* m_pCur;
	int m_count;
	void QuickSort( int left, int right, PFN_QuickCmp pFN, int dir );
public:
	CList();
	CList( CList& list, int start=0 ) {
		m_pHead = list.GetNode( start );
		m_pTail = list.m_pTail;
		m_pCur = m_pHead;
		m_count = list.m_count-start;
	}
	~CList();
	void Concat( CList<T>& list );
	void DeConcat( CList< T >& list );
  int Insert( CNode<T>* n, T* obj );
	int Add( T* obj );
	int AddTail( T* obj ) { Add( obj ); }
	int AddHead( T* obj );
	int AddWithSort( T* obj, PFN_Compare pFN );
	CNode< T >* NextNode( void );
	CNode< T >* PrevNode( void );
	bool Delete( int index );
	void DeleteNode( CNode< T >* pNode, bool fDelete=true );
	T* Next();
	T* Prev();
	T* Head();
	T* Tail() { return m_pTail ? m_pTail->m_Data : NULL; }
	void PrintAll() { }// For Debuggin 
	void FreeAll();
	void Rewind() { m_pCur = m_pHead; }
	void Last() { m_pCur = m_pTail; }
	int Count() const { return m_count; }
	// operators
	T* operator[]( int idx );
	void Swap( CNode< T >* a, CNode< T >* b );
	void Sort( PFN_QuickCmp pFN, int dir );
	CNode< T >* GetNode( int idx );
	T* GetData( int idx );
	CNode< T >* Detach( T* obj );
	CNode< T >* DetachHead( void );
	CNode< T >* DetachTail( void );
	CNode< T >* Search( T* obj );
	int AddNodeToTail( CNode< T >* obj );
	int AddNodeToHead( CNode< T >* obj );
};

template< class T >
CList< T >::CList() : 
	m_pHead( NULL ), m_pTail( NULL ), m_pCur( NULL )
{
	m_count = 0;
}

template< class T >
CList< T >::~CList() 
{
	if( m_pHead ) FreeAll();
	m_pHead = m_pTail = m_pCur = NULL;
}	

template< class T >
void CList< T >::Concat( CList<T>& list ) 
{
	if( m_pHead == NULL ) {
		m_pHead = list.m_pHead;
	}
	else {
		m_pTail->m_pNext = list.m_pHead;
		list.m_pHead->m_pPrev = m_pTail;
	}
	m_count += list.Count();
}

template< class T >
void CList< T >::DeConcat( CList< T >& list ) 
{
	if( m_pTail == NULL && m_pHead ) return;
	else if( m_pTail == NULL ) {
		if( m_pHead == list.m_pHead ) 
			m_pHead = NULL;
		else return;
	}
	else{
		if( m_pTail->m_pNext == list.m_pHead )
			m_pTail->m_pNext = NULL;
		else return;
	}
	m_count -= list.Count();
}

template< class T >
int CList< T >::Insert( CNode<T>* n, T* obj )
{
  if( !n ) {
    n = m_pHead;
  }

  CNode< T >* node = new CNode< T >( obj );
  m_count++;
  if( !n ) {
    m_pHead = m_pTail = node;
    return m_count;
  }

  CNode< T >* next = n->m_pNext;
  if( !next ) {
    n->m_pNext = node;
    node->m_pPrev = n;
    m_pTail = node;
  }
  else {
    n->m_pNext = node;
    node->m_pPrev = n;
    next->m_pPrev = node;
    node->m_pNext = next;
  }

  return m_count;
}

template< class T >
int CList< T >::Add( T* obj ) 
{
	CNode< T >* node = new CNode< T >( obj );
	if( node == NULL ) {
		return -1;
	}
	if( m_pHead == NULL ) { 
		m_pCur = m_pHead = m_pTail = node; 
	}
       else {
            node->m_pPrev = m_pTail;
            node->m_pNext = NULL;
            m_pTail->m_pNext = node;
            m_pTail = node;
        }
        m_count++;

        return m_count;
}

template< class T >
int CList< T >::AddHead( T* obj )
{
	CNode< T >*node = new CNode< T >( obj );
	if( node == NULL ) return -1;
	if( m_pHead == NULL ) {
		m_pHead = m_pTail = node;
	}
	else {
		m_pHead->m_pPrev = node;
		node->m_pNext = m_pHead;
		m_pHead = node;
	}
	m_count++;

	return m_count;
}


template< class T >
T* CList< T >::Head( void )
{
	return m_pHead ? m_pHead->m_Data : NULL;
}

template < class T >
CNode< T >* CList< T >::NextNode( void )
{
	CNode< T >* pNode;
	if( !m_pCur ) return NULL;

	m_pCur = m_pCur->m_pNext;
	pNode = m_pCur;

	return pNode;
}

template < class T >
CNode< T >* CList< T >::PrevNode( void )
{
	CNode< T >* pNode;

	if( !m_pCur ) return NULL;

	m_pCur = m_pCur->m_pPrev;
	pNode = m_pCur;

	return pNode;
}

template< class T >
void CList< T >::DeleteNode( CNode< T >* pNode, bool fDelete )
{
	CNode< T >* pTemp1;
	CNode< T >* pTemp2;
	
	if( !pNode ) return;
	
	m_count--;
	pTemp1 =pNode->m_pPrev;
	pTemp2 = pNode->m_pNext;
	if( pTemp1 ) {
		pTemp1->m_pNext = pNode->m_pNext;
	}
	else { // this node is head
		m_pHead = pTemp2;
	}
	
	if( pTemp2 ) {
		pTemp2->m_pPrev = pNode->m_pPrev;
	}
	else { // this node is tail
		m_pTail = pTemp1;
	}

	if( fDelete ) delete pNode;
}

template< class T >
bool CList< T >::Delete( int idx )
{
	if( idx >= m_count ) return false;

	CNode< T >* p = m_pHead;
	for( int i=0; i<idx; i++ ) p = p->m_pNext;
	DeleteNode( p, true );
	return true;
}

template< class T >
int CList< T >::AddWithSort( T* obj, PFN_Compare pFN )
{
	CNode< T >* pNode = m_pTail;
	CNode< T >* pNewNode = new CNode< T >( obj );

	if( m_pHead == NULL ) {
		m_pCur = m_pHead = m_pTail = pNewNode;
		m_count = 1;
		return m_count;
	}

	if( pFN == NULL ) {
		return Add( obj );	
	}
	
	while( pNode ) {
		if( pFN( obj, pNode->m_Data )  ) 
			{
			break;
			}
		pNode = pNode->m_pPrev;
	}

	if( pNode ) {
		CNode< T >* pTemp = pNode->m_pNext;
		pNode->m_pNext = pNewNode;
		pNewNode->m_pPrev = pNode;
		if( pTemp ) {
			pTemp->m_pPrev = pNewNode;
			pNewNode->m_pNext = pTemp;
		}
		else {
			m_pTail = pNewNode;
		}
	}
	else {
		pNewNode->m_pNext = m_pHead;
		m_pHead->m_pPrev = pNewNode;
		pNewNode->m_pPrev = NULL;
		m_pHead = pNewNode;
	}

	m_count++;
	return m_count;
}


template< class T >
T* CList< T >::operator[]( int idx ) 
{
	if( !m_pHead ) return NULL;
	m_pCur = m_pHead;
	int i;
	for( i=0; m_pCur && i<idx; i++ ) m_pCur = m_pCur->m_pNext;
	if( i == idx ) return m_pCur->m_Data;
	else return NULL;
}

#include <sys/time.h>
#include <unistd.h>
template< class T >
void CList< T >::FreeAll( void ) 
{
	CNode< T >* p;
	m_pCur = m_pHead;
	while( m_pCur ) {
		p = m_pCur;
		m_pCur = m_pCur->m_pNext;
		delete p;
	}
	m_count = 0;
	m_pHead = m_pTail = m_pCur = NULL;
}

template< class T >
T* CList< T >::Next() 
{ 
	if( m_pCur == NULL ) return NULL;
	m_pCur = m_pCur->m_pNext;
	if( !m_pCur ) return NULL;
	return m_pCur->m_Data;
}

template< class T >
T* CList< T >::Prev() 
{
	if( m_pCur == NULL ) return NULL;
	m_pCur = m_pCur->m_pPrev;
	if( !m_pCur ) return NULL;

	return m_pCur->m_Data;
}

template< class T >
CNode< T >* CList< T >::GetNode( int idx )
{
	m_pCur = m_pHead;
	int i;
	for( i=0; m_pCur && i<idx; i++ ) m_pCur = m_pCur->m_pNext;
	if( i == idx ) return m_pCur;
	else return NULL;
}

template< class T >
T* CList< T >::GetData( int idx )
{
	m_pCur = m_pHead;
	int i;
	if( idx >= m_count ) return NULL;
	if( m_count == 0 ) return NULL;
	for( i=0; m_pCur && i<idx; i++ ) m_pCur = m_pCur->m_pNext;
	if( i == idx ) return m_pCur->m_Data;
	else return NULL;
}

template< class T >
void CList< T >::Swap( CNode< T >* a, CNode< T >* b )
{
	if( !a || !b ) return;

	T* tmp;

	tmp = a->m_Data;
	a->m_Data = b->m_Data;
	b->m_Data = tmp;

	return;
}

template< class T >
void CList< T >::Sort( PFN_QuickCmp pFN, int dir )
{
	if( m_count == 0 ) return;
	QuickSort( 0, m_count-1, pFN, dir );

	return;
}

template< class T >
void CList< T >::QuickSort( int left, int right, PFN_QuickCmp pFN, int dir )
{
	int i, j;
	int pivot;
	int cmp;
	
	if( left >= right ) return;

	// Partitioning
	pivot = (left+right)/2;
	for( i=left; i<=right; i++ ) {
		if( i == pivot ) continue;
		
		if( dir == 0 ) { // ascent 
			cmp = pFN(GetData(i), GetData(pivot));
			if( cmp < 0 ) { // data is smaller than pivot
				if( i > pivot ) {
					Swap( GetNode(i), GetNode(pivot) );
					if( i > pivot+1 ) {
						Swap( GetNode(pivot+1), GetNode(i) );
						pivot++;
					}
					else {
						pivot = i;
					}
				}
			}
			else if( cmp > 0 ) {
				if( i < pivot ) {
					for( j=right; j>pivot; j-- ) {
						if( pFN(GetData(j), GetData(pivot)) < 0 ) {
							Swap( GetNode(i), GetNode(j) );
							break;
						}
					}
					if( j==pivot ) Swap( GetNode(i), GetNode(pivot) );
				}
			}
		}
		else { // descent
			cmp = pFN(GetData(i), GetData(pivot));
			if( cmp < 0 ) {
				if( i < pivot ) {
					for( j=right; j>pivot; j-- ) {
						if( pFN(GetData(j), GetData(pivot)) > 0 ) {
							Swap( GetNode(j), GetNode(i) );
							break;
						}
					}
					if( j == pivot ) Swap( GetNode(i), GetNode(pivot) );
				}
			}
			else if( cmp > 0 ) {
				if( i > pivot ) {
					Swap( GetNode(i), GetNode(pivot) );
					if( i > pivot+1 ) {
						Swap( GetNode(pivot+1), GetNode(i) );
						pivot++;
					}
					else {
						pivot = i;
					}
				}
			}
		}
	}
	QuickSort( left, pivot-1, pFN, dir );
	QuickSort( pivot+1, right, pFN, dir );

	return;
}

template< class T >
CNode< T >* CList< T >::Detach( T* obj )
{
	CNode< T >* pNode = Search( obj );
	if( pNode == NULL ) return NULL;

	CNode< T >* pPrev = pNode->m_pPrev;
	CNode< T >*pNext = pNode->m_pNext;

	if( pPrev ) {
		pPrev->m_pNext = pNext;
	}
	else { // Head Node
		m_pHead = pNext;
	}

	if( pNext ) {
		pNext->m_pPrev = pPrev;
	}
	else { // Tail Node
		m_pTail = pPrev;
	}

	m_count--;
	return pNode;		
}

template< class T >
CNode< T >* CList< T >::DetachHead( void )
{
	CNode< T >* pNode = m_pHead;
	if( !pNode ) return NULL;
	
	CNode< T >* pNext = m_pHead->m_pNext;
	if( pNext ) {
		pNext->m_pPrev = NULL;
	}
	m_pHead = pNext;
	if( m_pHead == NULL ) m_pTail = NULL;
	m_count--;

	pNode->m_pPrev = NULL;
	pNode->m_pNext = NULL;
	return pNode;
}

template< class T >
CNode< T >* CList< T >::DetachTail( void )
{
	CNode< T >* pNode = m_pTail;
	if( m_pTail == NULL ) return NULL;

	CNode< T >* pPrev = m_pTail->m_pPrev;
	if( pPrev ) pPrev->m_pNext = NULL;
	m_pTail = pPrev;
	if( pPrev == NULL ) m_pHead = NULL;
	m_count--;

	pNode->m_pNext = NULL;
	pNode->m_pPrev = NULL;
	return pNode;
}

template< class T >
CNode< T >* CList< T >::Search( T* obj )
{
	CNode< T >* pNode = m_pHead;

	for( ; pNode; pNode = pNode->m_pNext ) {
		if( pNode->m_Data == obj ) break;
	}

	return pNode;
}

template< class T >
int CList< T >::AddNodeToHead( CNode< T >* pNode )
{
	if( !pNode ) return -1;

	if( !m_pHead ) {
		m_pHead = m_pTail = pNode;
		m_count = 1;
	}
	else {
		pNode->m_pNext = m_pHead;
		pNode->m_pPrev = NULL;
		m_pHead->m_pPrev = pNode;
		m_pHead = pNode;
		m_count++;
	}

	return m_count;
}

template< class T >
int CList< T >::AddNodeToTail( CNode< T >* pNode )
{
	if( !pNode ) return -1;

	if( !m_pTail ) {
		m_pHead = m_pTail = pNode;
		m_count = 1;
	}
	else {
		m_pTail->m_pNext = pNode;
		pNode->m_pPrev = m_pTail;
		pNode->m_pNext = NULL;
		m_pTail = pNode;
		m_count++;
	}

	return m_count;
}

#endif /* __BASE_OBJECT_H__ */
