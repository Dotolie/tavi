//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : character set
// 
// $Source: /home/cvsroot/TAVI020/tavi/Component/CharSet.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: CharSet.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.1  2005/05/15 11:10:07  zstein
// - [START] CharSet converter
//
//
//*******************************************************************************

#ifndef __CHARACTER_SET_H__
#define __CHARACTER_SET_H__

extern int ConvertUnicode( const char* sz, int slen, uchar_t* unicode, int dlen, int charset );

#endif /* __CHARACTER_SET_H__ */