//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Recorder
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/Recorder/RecorderMain.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: RecorderMain.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.8  2005/12/05 02:44:05  ywkim
// Add function of REC_IsRecording() for idle shutdown
//
// Revision 1.7  2005/11/08 07:53:26  zstein
// *** empty log message ***
//
//*******************************************************************************

#ifndef __RECORDER_H__
#define __RECORDER_H__

#include <Task/Msg.h>

#define RECORD_DIR		"record"

enum {
	REC_PROP_M_RECORDING
};

extern int REC_MsgProc( TMsgEntry* msg );
extern int REC_Task( TTaskInitParam* tParam );
extern void REC_MP3Play( const CString& path );
extern void REC_Kill_Mp3Player( void );
extern CList< CWindow >* REC_GetWndList( void );
extern int REC_Kill( void );
extern void REC_SetProperty( int name, int value );
extern bool REC_IsRecording( void );


#endif /* __RECORDER_H__ */
