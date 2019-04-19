//************************ Copyright(C) New Media Life Inc. ************************

//

// Description : Stack class declaration

// 

// $Source: /home/cvsroot/TAVI020/tavi/Component/Stack.h,v $

// $Revision: 1.1.1.1 $

// $Date: 2006/02/22 12:33:31 $

// $Author: zstein $ Lee Seok<zstein@newmedialife.com>

//

//******************************************************************************

// $Log: Stack.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.7  2005/11/24 03:10:08  zstein
// *** empty log message ***
//
// Revision 1.6  2005/11/22 09:09:57  zstein
// *** empty log message ***
//
// Revision 1.5  2005/07/16 04:13:51  zstein
// - fixed bugs & added functions.
//
// Revision 1.4  2005/05/03 02:14:13  zstein
// *** empty log message ***
//
// Revision 1.3  2005/04/05 08:02:36  zstein
// *** empty log message ***
//
// Revision 1.2  2005/02/25 13:39:01  etnlwind
// FileManager Add
//
// Revision 1.1  2005/02/16 16:03:25  zstein
// *** empty log message ***
//

//*******************************************************************************


#ifndef __STACK_H__
#define __STACK_H__

#define DEFAULT_STACK_SIZE		100


template <class T>
class CStack
{
private:
	void Init( int size );
protected:
	T* m_pVect;
	long m_nStackSize;
	int m_nStackTop;
public:
	CStack( void );
	CStack( int len );
	virtual ~CStack( void );

	void Reset( int size );
	bool Push( T data );
	const T& Pop( void );
	const T& Peek( void );
	bool IsEmpty( void ) { return (m_nStackTop==-1) ? true : false; }
	bool IsFull( void ) { return (m_nStackTop == (m_nStackSize-1) ); }
	int Count( void ) { return m_nStackTop+1; }
};


template< class T >
CStack< T >::CStack( void )
{
	Init( DEFAULT_STACK_SIZE );
}

template< class T >
CStack< T >::CStack( int nSize )
{
	Init( nSize );
}

template< class T >
CStack< T >::~CStack( void )
{
	if( m_pVect ) delete[] m_pVect;
}

template< class T >
void CStack< T >::Init( int nSize )
{
	m_pVect = new T[nSize];
	m_nStackTop = -1;
	m_nStackSize = nSize;
}

template< class T >
void CStack< T >::Reset( int nSize )
{
	delete m_pVect;

	m_pVect = new T[nSize];

	m_nStackTop = -1;
	m_nStackSize = nSize;
}


template< class T >
bool CStack< T >::Push( T data )
{
	if( m_nStackTop == m_nStackSize-1 ) {
		return false;		
	}
	else {
		m_pVect[++m_nStackTop] = data;
	}

	return true;
}

template< class T >
const T& CStack< T >::Pop( void )
{
	if( m_nStackTop != -1 ) {
		return m_pVect[m_nStackTop--];
	}

	return m_pVect[0];
}

template< class T >
const T& CStack< T >::Peek( void )
{
	if( m_nStackTop == -1 ) return m_pVect[0];

	return m_pVect[m_nStackTop];
}

#endif /* __STACK_H__ */
