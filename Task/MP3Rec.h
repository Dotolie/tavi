//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : MP3 Record Interface
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/MP3Rec.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: MP3Rec.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.3  2006/01/16 02:27:13  zstein
// *** empty log message ***
//
// Revision 1.2  2005/11/08 01:17:02  zstein
// *** empty log message ***
//
// Revision 1.1  2005/07/16 04:31:45  zstein
// - MP3 encoder
//
//
//*******************************************************************************

#ifndef __I2S_INTERFACE_H__
#define __I2S_INTERFACE_H__

extern int MP3Rec_Init( int nBitsPerSample, int nSamplePerSec, int nChannels );
extern void MP3Rec_Exit( void );
extern int MP3Rec_Recording( void  );
extern void MP3Rec_Start( void );
extern void MP3Rec_Stop( void );
extern int MP3Rec_Encoding( bool fFlush=false );
extern bool MP3Rec_SetEncodingPath( const char* path );
extern int MP3Rec_Load( void );
extern void MP3Rec_Unload( void );


#endif /* __I2S_INTERFACE_H__ */
