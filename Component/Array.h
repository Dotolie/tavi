//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Array template class
// 
// $Source: /home/cvsroot/TAVI020/tavi/Component/Array.h,v $
// $Revision: 1.2 $
// $Date: 2006/06/07 08:18:37 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: Array.h,v $
// Revision 1.2  2006/06/07 08:18:37  zstein
// *** empty log message ***
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.7  2006/01/09 02:32:09  zstein
// *** empty log message ***
//
// Revision 1.6  2005/12/27 09:57:59  zstein
// *** empty log message ***
//
// Revision 1.5  2005/12/21 03:17:08  zstein
// *** empty log message ***
//
// Revision 1.4  2005/12/20 11:54:27  zstein
// *** empty log message ***
//
// Revision 1.3  2005/12/01 01:02:31  zstein
// *** empty log message ***
//
// Revision 1.2  2005/11/24 03:09:32  zstein
// *** empty log message ***
//
// Revision 1.1  2005/11/16 06:44:49  zstein
// - Array
//
//*******************************************************************************

#include <stdio.h>
#include <stdlib.h>

#ifndef __ARRAY_H__
#define __ARRAY_H__

template< typename T >
class CArray {
protected:
	T* m_array;
	int m_count;
	int m_size;

	void _DeallocNodes( void ) {
		for( int i=0; i<m_count; i++ ) {
			if( m_array[i] ) delete m_array[i];
			m_array[i] = 0;
		}
	}
public:
	CArray( void ) {
		m_array = 0;
		m_size = 0;
		m_count = 0;
	}

	
	~CArray( void ) {
		_DeallocNodes();
		free( m_array );
	}

	int Copy( const CArray& rhs ) {
		if( m_size < rhs.m_size ) {
			if( m_array ) free( m_array );
			m_array = (T*)malloc(rhs.m_size*sizeof(T));
			m_size = rhs.m_size;
			memset( m_array, 0, sizeof(T)*m_size );
		}
		m_count = rhs.m_count;
		memcpy( m_array, rhs.m_array, m_size*sizeof(T) );
		return m_count;
	}
	
	bool Resize( int size ) {
		T* tmp;
		if( (tmp=(T*)realloc(m_array, size*sizeof(T*))) == NULL ) {
			return false;
		}
		m_array = tmp;
		m_size = size;
		return true;
	}
	
	int PushBack( const T& node ) {
		if( m_count == m_size ) {
			// expand array
			if( Resize( m_size + 100 ) == false ) return -1;
		}
		m_array[m_count++] = node;
		return 0;
	}

	int PushHead( const T& node ) {
		if( m_count == m_size ) {
			// expand array
			if( Resize( m_size + 100 ) == false ) return -1;
		}
		// shift right
		for( int i=m_count-1; i>=0; i-- ) {
			m_array[i+1] = m_array[i];
		}
		m_array[0] = node;
		m_count++;
		return 0;
	}

	int Add( const T& node ) {
		return PushBack( node );
	}

	
	int Count( void ) const { 
		return m_count; 
	}
	
	void FreeAll( void ) {
		// free memory
		_DeallocNodes();
		m_count = 0;
	}

	void FreeFake( void ) {
		memset( m_array, 0, m_count*sizeof(T) );
		m_count = 0;
	}
	
	T& operator[]( int index ) {
		if( index > m_count ) {
			printf( "failed to operator[%d]\n", index );
			return m_array[0];
		}
		return m_array[index];
	}

	void Sort( int(*pfn_sort)(const void*, const void*) ) {
		if( m_count == 0 ) return;
		qsort( (void*)m_array, m_count, sizeof(T), pfn_sort );
		return;
	}

	bool Delete( int idx ) {
		if( m_count <= idx ) return false;

		delete m_array[idx];
		for( int i=idx; i<m_count-1; i++ ) {
			m_array[i] = m_array[i+1];
		}
		m_count--;
		return true;
	}

	bool Swap( int idx, int didx ) {
		if( idx >= m_count || didx >= m_count ) return false;
		T tmp = m_array[idx];
		m_array[idx] = m_array[didx];
		m_array[didx] = tmp;
		return true;
	}
};

#endif
