//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Simple String class definition
// 
// $Source: /home/cvsroot/TAVI020/tavi/Component/String.cpp,v $
// $Revision: 1.2 $
// $Date: 2006/03/09 03:04:53 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: String.cpp,v $
// Revision 1.2  2006/03/09 03:04:53  zstein
// *** empty log message ***
//
// Revision 1.29  2005/12/16 04:29:02  zstein
// *** empty log message ***
//
// Revision 1.28  2005/12/08 06:40:45  zstein
// - deleted debug messages
//
// Revision 1.27  2005/11/16 06:42:39  zstein
// *** empty log message ***
//
// Revision 1.26  2005/11/08 07:28:18  zstein
// *** empty log message ***
//
// Revision 1.25  2005/11/07 06:25:14  yong
// printHex()
//
// Revision 1.24  2005/11/07 05:22:36  zstein
// *** empty log message ***
//
// Revision 1.23  2005/11/07 00:54:23  zstein
// *** empty log message ***
//
// Revision 1.22  2005/10/20 03:04:51  zstein
// - added Shear(...).
//
//*******************************************************************************

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include "String.h"

///////////////////////////////////////////////////////////////////////////////
// Implementation of String methods
///////////////////////////////////////////////////////////////////////////////
  
// constructors
CString::CString(const char* s ) 
{
	if( !s ) {
		m_nSize = 0;
		m_szString = NULL;
		return;
	}

	m_fUnicode = false;
	m_nSize = strlen( s );
	m_szString = new char[m_nSize+1];
	strcpy( m_szString, s );
	m_szString[m_nSize] = 0;
}

CString::CString( const uchar_t* s )
{
	if( !s ) {
		m_nSize = 0;
		m_szString = NULL;
		return;
	}
	
	uchar_t* punisz;
	m_fUnicode = true;
	m_nSize = ustrlen( (uchar_t*)s );
	punisz = new uchar_t[m_nSize+1];
	ustrcpy( punisz, (uchar_t*)s );
	punisz[m_nSize] = 0;
	m_szString = ( char* )punisz;
}

CString::CString( const CString& s )
{
	if( this == &s ) return;
	m_nSize = s.m_nSize;
	m_fUnicode = s.m_fUnicode;
	if( s.m_fUnicode ) {
		if( s.m_szString ) {
			(uchar_t*)m_szString = new uchar_t[m_nSize+1];
			ustrcpy( (uchar_t*)m_szString, (uchar_t*)s.m_szString );
			uchar_t* wcString = ( uchar_t* )m_szString;
			wcString[m_nSize] = 0;
		}
		else {
			m_szString = 0;
		}
	}
	else {
		if( s.m_szString ) {
			m_szString = new char[m_nSize+1];
			strcpy( m_szString, s.m_szString );
			m_szString[m_nSize] = 0;
		}
		else {
			m_szString = 0;
		}
	}
};

// destructor
CString::~CString()
{
	if( m_szString ) {
		delete[] m_szString;
	}
};

// operators

CString& CString::operator= ( const CString& s )
{
	if (this != &s)
	{
		if( m_szString ) delete[] m_szString;
		const char* p = s.GetString();
		m_nSize = s.m_nSize;
		m_fUnicode = s.m_fUnicode;
		if( s.m_fUnicode ) {
			if( s.m_szString ) { 
				(uchar_t*)m_szString = new uchar_t[m_nSize+1];
				ustrcpy( (uchar_t*)m_szString, (uchar_t*)s.m_szString );
				uchar_t* wc = ( uchar_t* )m_szString;
				wc[m_nSize] = 0;
			}
			else {
				m_szString = 0;
			}
		}
		else {
			if( s.m_szString ) {
				m_szString = new char[m_nSize+1];
				strcpy( m_szString, p );
				m_szString[m_nSize] = 0;
			}
			else {
				m_szString = 0;
			}
		}
		
	};
	return *this;
};

CString& CString::operator=( const char* szText )
{
	if( m_szString ) delete[] m_szString;
	
	if( szText == NULL ) {
		m_szString = NULL;
	}
	else {
		m_fUnicode = false;
		m_nSize = strlen( szText );
		m_szString = new char[m_nSize+1];
		strcpy( m_szString, szText );
		m_szString[m_nSize] = 0;
	}

	return *this;
}

CString& CString::operator=( const uchar_t* text )
{
	if( m_szString ) delete[] m_szString;
	if( text == NULL ) {
		m_szString = NULL;
	}
	else {
		m_fUnicode = true;
		m_nSize = ustrlen( text );
		(uchar_t*)m_szString = new uchar_t[m_nSize+1];
		ustrcpy( (uchar_t*)m_szString, (uchar_t*)text );
		uchar_t* p = ( uchar_t* )m_szString;
		p[m_nSize] = 0;
	}

	return *this;
}

bool CString::operator<( const CString& rhs ) const
{
	if( this == &rhs ) return true;

	char* pSrc = this->m_szString;
	char* pDst = rhs.m_szString;

	if( m_fUnicode ) {
		if( ustrcmp((uchar_t*)pSrc, (uchar_t*)pDst) < 0 ) return true;
	}
	else {
		if( strcmp( pSrc, pDst ) < 0 ) return true;
	}
	
	return false;
}

bool CString::operator<=( const CString& rhs ) const
{
	if( this == &rhs ) return true;
	char* pSrc = this->m_szString;
	char* pDst = rhs.m_szString;

	if( strcmp( pSrc, pDst ) <= 0 ) return true;
	else return false;
}

bool CString::operator>( const CString& rhs ) const
{
	bool ret = false;
	if( this == &rhs ) return true;
	char* pSrc = this->m_szString;
	char* pDst = rhs.m_szString;

	if( m_fUnicode ) {
		if( ustrcmp((uchar_t*)pSrc, (uchar_t*)pDst) > 0 ) ret = true;
	}
	else {
		if( strcmp( pSrc, pDst ) > 0 ) ret = true;
	}

	return ret;
}

bool CString::operator>=( const CString& rhs ) const
{
	if( this == &rhs ) return true;
	char* pSrc = this->m_szString;
	char* pDst = rhs.m_szString;

	if( strcmp( pSrc, pDst ) >= 0 ) return true;
	else return false;
}

bool CString::operator==( const CString& rhs ) const
{
	if( this == &rhs ) return true;
	char* pSrc = this->m_szString;
	char* pDst = rhs.m_szString;

	if( m_fUnicode ) {
		if( ustrcmp((uchar_t*)pSrc, (uchar_t*)pDst) == 0 ) return true;
	}
	else {
		if( strcmp( pSrc, pDst ) == 0 ) return true;
	}
	return false;
}

bool CString::operator==( const char* sz ) const
{
	if( !sz ) return false;

	if( m_fUnicode ) {
		if( ustrcmp((uchar_t*)m_szString, (uchar_t*)sz) == 0 ) return true;
	}
	else {
		if( strcmp( m_szString, sz ) == 0 ) return true;
	}
	return false;
}

bool CString::operator!=( const CString& rhs ) const
{
	if( this == &rhs ) return false;
	char* pSrc = this->m_szString;
	char* pDst = rhs.m_szString;

	if( m_fUnicode ) {
		if( ustrcmp( (uchar_t*)pSrc,  (uchar_t*)pDst) ) return true;
		else return false;
	}
	else {
		if( strcmp(pSrc, pDst) ) return true;
		else return false;
	}
}

CString& CString::operator+=( const CString& rhs )
{
	int len = m_nSize+rhs.m_nSize+1;
	uchar_t* pwszTemp = NULL;
	char* pszTemp = NULL;
	char* pbuf = NULL;

	pszTemp = new char[len];
	strcpy( pszTemp, m_szString );
	strcat( pszTemp, rhs.m_szString );
	pszTemp[len-1] = 0;
	delete[] m_szString;
	m_szString = pszTemp;
	m_nSize = len;

	return *this;
}

CString CString::operator+( const CString& rhs )
{
	CString sz;

	sz.m_nSize = m_nSize + rhs.m_nSize;
	char* pTemp = NULL;

	sz.m_szString = new char[sz.m_nSize+1];
	strcpy( sz.m_szString, m_szString );
	strcat( sz.m_szString, rhs.m_szString );
	sz.m_szString[sz.m_nSize] = 0;

	return sz;
}

uchar_t CString::operator[]( int idx ) const
{
	return GetChar( idx );
}

uchar_t CString::GetChar( int idx ) const
{
	if( idx < 0 || idx > m_nSize ) {
		printf( "CString::operator[] -> out of bound( total size: %d, index: %d )\n",
			m_nSize, idx );
		return (uchar_t)-1;
	}
	if( m_fUnicode ) {
		uchar_t* pwsz = ( uchar_t* )m_szString;
		return pwsz[idx];
	}
	else {
		return (uchar_t)m_szString[idx];
	}
}

bool CString::IsSpace( const CString& sz )
{
	int i;
	if( sz.m_fUnicode ) {
		uchar_t* pwc = ( uchar_t* )sz.m_szString;
		for( i=0; i<sz.m_nSize; i++ ) {
			if( pwc[i] != ' ' && pwc[i] != '\t' ) break;
		}
	}
	else {
		for( i=0; i<sz.m_nSize; i++ ) {
			if( sz.m_szString[i] != ' ' && sz.m_szString[i] != '\t' ) break;
		}
	}

	if( i == sz.m_nSize ) return true;
	else return false;
}

void CString::Print( void ) const
{
	unsigned short* pwch = ( unsigned short* )m_szString;
	for( int i=0; i<m_nSize; i++ ) {
		if( m_fUnicode ) {
			printf( "%c", pwch[i] );
		}
		else {
			printf( "%c", m_szString[i] );
		}
	}
}


void CString::Print( const char* szText, int nSize )
{
}

void CString::Print( const uchar_t* sz )
{
	uchar_t* pwch = ( uchar_t* )sz;
	for( int i=0; pwch[i]; i++ ) {
		printf( "%c", pwch[i] );
	}
}

void CString::PrintHex( void ) const
{
	int size = m_nSize;
	if( m_fUnicode ) size = m_nSize*2;
	printf( "0x" );
	for( int i=0; i<m_nSize; i++ ) {
		printf( "%02x", m_szString[i] );
	}
}


void CString::SetString( const char* sz, bool fUnicode )
{
	uchar_t* p = (  uchar_t* )sz;
	m_fUnicode = fUnicode;
	if( m_szString ) delete[] m_szString;

	if( sz == NULL ) {
		m_szString = NULL;
		m_nSize = 0;
		return;
	}

	int len;
	if( fUnicode ) {
		len = ustrlen( p );
		( uchar_t* )m_szString = new uchar_t[len+2];
		ustrcpy( (uchar_t*)m_szString, (uchar_t*)sz );
		p = ( uchar_t* )m_szString;
		p[len] = 0;
	}
	else {
		len = strlen( sz );
		m_szString = new char[len+1];
		strcpy( m_szString, sz );
		m_szString[len] = 0;
	}
	m_nSize = len;
}

void CString::Add( const CString& sz )
{
	if( m_fUnicode != sz.m_fUnicode ) return;

	if( m_fUnicode ) {
		uchar_t* wsz;
		wsz = new uchar_t[m_nSize+1];
		ustrcpy( wsz, (uchar_t*)m_szString );
		m_nSize += sz.m_nSize;
		delete[] m_szString;
		( uchar_t* )m_szString = new uchar_t[m_nSize+1];
		ustrcpy( (uchar_t*)m_szString, wsz );
		ustrcat( (uchar_t*)m_szString, (uchar_t*)sz.m_szString );
		((uchar_t*)m_szString)[m_nSize] = 0;
		delete[] wsz;
	}
	else {
		char* szTmp;
		szTmp = new char[m_nSize+1];
		strcpy( szTmp, m_szString );
		m_nSize += sz.m_nSize;
		delete[] m_szString;
		m_szString = new char[m_nSize+1];
		strcpy( m_szString, szTmp );
		strcat( m_szString, sz.m_szString );
		m_szString[m_nSize] = 0;
		delete[] szTmp;
	}
}

void CString::Add( const uchar_t* sz )
{
	if( m_fUnicode == false ) return;
	
	uchar_t* wsz;
	wsz = new uchar_t[m_nSize+1];
	ustrcpy( wsz, (uchar_t*)m_szString );
	int len = ustrlen( sz );
	m_nSize += len;
	delete[] m_szString;
	( uchar_t* )m_szString = new uchar_t[m_nSize+1];
	ustrcpy( (uchar_t*)m_szString, wsz );
	ustrcat( (uchar_t*)m_szString, sz );
	((uchar_t*)m_szString)[m_nSize] = 0;

	delete[] wsz;
}

void CString::toUTF16( void )
{
	if( m_fUnicode ) return;
	m_fUnicode = true;
	uchar_t* wsz = new uchar_t[m_nSize+1];
	str2ustr( wsz, m_szString, m_nSize );
	wsz[m_nSize] = 0;
	delete[] m_szString;
	m_szString = ( char* )wsz;
}

int CString::ClearWhite( char* szText, bool unicode, int len )
{
	int i;
	if( unicode ) {
		uchar_t* pwsz = ( uchar_t* )szText;
		for( i=len-1; i>-1; i-- ) {
			if( pwsz[i] == 0x20 || pwsz[i] == '\t' ) pwsz[i] = 0;
			else if( pwsz[i] ) break;
		}
	}
	else {
		for( i=len-1; i>-1; i-- ) {
			if( szText[i] == 0x20 || szText[i] == '\t' ) szText[i] = 0;
			else if( szText[i] ) break;
		}
	}

	return i+1;
}

CString CString::EndWith( uchar_t ch ) const
{
	CString sz;
	const uchar_t* pwsz = ( uchar_t* )m_szString;
	int i;

	for( i=m_nSize-1; i>=0; i-- ) {
		if( m_fUnicode ) {
			if( pwsz[i] == ch ) break;
		}
		else {
			if( m_szString[i] == (char)ch ) break;
		}
	}

	if( i >= 0 ) {
		i++;
		if( m_fUnicode ) {
			sz.SetString( (const char*)(pwsz+i), true );
		}
		else {
			sz.SetString( (const char*)m_szString+i, false );
		}
	}

	return sz;
}

int CString::ChrChr( uchar_t s, uchar_t d  )
{
	int count=0;
	uchar_t* pucs = ( uchar_t* )m_szString;
	char* psz = m_szString;
	
	for( int i=0; i<m_nSize; i++ ) {
		if( m_fUnicode ) {
			if( pucs[count] == s ) pucs[count++] = d;
		}
		else {
			if( psz[count] == s ) psz[count++] = d;
		}
	}
	
	return count;
}

int CString::Shear( uchar_t ch, int dir )
{
	if( m_nSize == 0 ) return 0;

	int count = 0;
	char* ps, *pe;
	if( dir == HEAD ) {
		for( int i=0; i>m_nSize; i++ ) {
			count++;
			if( GetChar(i) == ch ) break;
		}
		ps = m_szString + (m_fUnicode+1)*count;
		pe = m_szString + (m_fUnicode+1)*(m_nSize-1);
	}
	else if( dir == TAIL ) {
		for( int i=m_nSize-1; i>=0; i-- ) {
			count++;
			if( GetChar(i) == ch ) break;
		}
		ps = m_szString;
		pe = m_szString + (m_fUnicode+1)*(m_nSize-count-1);
	}
	else ; // so what??

	if( count == m_nSize ) count = 0;
	
	if( count ) {
		m_nSize -= count;
		int nbytes = pe - ps+(m_fUnicode+1);
		char* p = new char[nbytes+2];
		memcpy( p, ps, nbytes );
		p[nbytes] = 0;
		p[nbytes+1] = 0;
		delete[] m_szString;
		m_szString = p;
	}
	
	return count;
}
