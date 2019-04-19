//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : MD5  for TAVI 1600
// 
// $Source: /home/cvsroot/TAVI020/tavi/md5/md5.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: md5.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.1  2005/11/22 05:54:01  ywkim
// Add new directory & source for MD5 key
//
//*****************************************************************************

#ifndef _MD5_H
#define _MD5_H

typedef struct
{
    unsigned long int total[2];
    unsigned long int state[4];
    unsigned char buffer[64];
}
md5_context;

class CMD5
{
private:
	md5_context m_Md5Context;

protected:

public:
	unsigned char m_digest[16];

	CMD5 ( void );
	~CMD5 ( void );
	void md5_starts( void );
	void md5_process( unsigned char data[64] );
	void md5_update( unsigned char *input, unsigned long int length );
	void md5_finish( void );
	unsigned char *md5_get( void );
};
#endif /* md5.h */
