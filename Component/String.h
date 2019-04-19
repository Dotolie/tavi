//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Simple String class declaration
// 
// $Source: /home/cvsroot/TAVI020/tavi/Component/String.h,v $
// $Revision: 1.2 $
// $Date: 2006/03/09 03:04:57 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: String.h,v $
// Revision 1.2  2006/03/09 03:04:57  zstein
// *** empty log message ***
//
// Revision 1.21  2005/11/08 07:28:19  zstein
// *** empty log message ***
//
//*******************************************************************************

#ifndef __STRING_H__
#define __STRING_H__

#ifndef NULL
#define NULL		0
#endif /* NULL */

#include <Lib/LibWideString.h>

class CString
{
protected:
	// member variables
	long m_nSize;
	char* m_szString;
	bool m_fUnicode;

public:
	enum {
		HEAD,
		TAIL
	};
	enum CharFormat {
		ASCII,
		UTF8,
		UTF16
	};
	
	// constructors
	CString() {
		m_szString = NULL;
		m_nSize = 0;
		m_fUnicode = false;
	}
	CString( const char* szText );
	CString( const uchar_t* text );
	CString(const CString& rhs);

	// destructor
	virtual ~CString();

	const char* GetString( void ) const { return m_szString; }
	long GetSize( void ) const { return m_nSize; }
	
	// operators
	CString& operator=( const CString& rhs );
	CString& operator=( const char* szText );
	CString& operator=( const uchar_t* text );
	uchar_t operator[]( int idx ) const;
	uchar_t GetChar( int idx ) const;
	operator const char* ()  const { return m_szString; };
	operator char* () { return m_szString; }
	operator uchar_t* () { return (uchar_t*)m_szString; }
	operator const uchar_t* () const { return (uchar_t*)m_szString; }
	bool operator > ( const CString& rhs ) const;
	bool operator >= ( const CString& rhs ) const;
	bool operator < ( const CString& rhs ) const;
	bool operator <= ( const CString& rhs ) const;
	bool operator == ( const CString& rhs ) const;
	bool operator ==  ( const char* sz ) const;
	bool operator!= ( const CString& rhs ) const;
	CString operator+ ( const CString& rhs );
	CString& operator+= ( const CString& rhs );

	static bool IsSpace( const CString& sz );
	bool IsUnicode( void ) const { return m_fUnicode; }
	void SetUnicode( bool unicode ) { m_fUnicode = unicode; }
	void SetString( const char* sz, bool unicode );
	void Print( void ) const;
	static void Print( const char* szText, int nSize );
	static void Print( const uchar_t* sz );
	void PrintHex( void ) const;
	void Add( const CString& sz );
	void Add( const uchar_t* sz );
	void toUTF16( void );
	static int ClearWhite( char* szText, bool unicode, int len );
	CString EndWith( uchar_t ch ) const;
	int CString::ChrChr( uchar_t s, uchar_t d );
	int CString::Shear( uchar_t ch, int dir );
};
  
#endif /* __STRING_H__ */



