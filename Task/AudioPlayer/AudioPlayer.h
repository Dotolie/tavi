//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Music Player 
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/AudioPlayer/AudioPlayer.h,v $
// $Revision: 1.2 $
// $Date: 2006/04/17 04:27:19 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: AudioPlayer.h,v $
// Revision 1.2  2006/04/17 04:27:19  zstein
// - added AP_HDD_Wakeup().
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.40  2005/12/27 09:44:29  zstein
// *** empty log message ***
//
// Revision 1.39  2005/12/23 04:05:15  yong
// add AP_GetPlaystate()
// use different peek flags for MSG_Peek_Ex() based on return value of AP_GetPlaystate()
//
// Revision 1.38  2005/12/22 04:27:54  zstein
// - removed AP_GetPlaystate()
//
// Revision 1.37  2005/12/16 09:56:58  zstein
// *** empty log message ***
//
// Revision 1.36  2005/12/15 01:48:12  zstein
// *** empty log message ***
//
// Revision 1.35  2005/12/11 13:02:10  zstein
// *** empty log message ***
//
// Revision 1.34  2005/12/07 07:38:08  zstein
// *** empty log message ***
//
// Revision 1.33  2005/12/01 01:04:08  zstein
// *** empty log message ***
//
// Revision 1.32  2005/11/24 02:34:57  zstein
// *** empty log message ***
//
// Revision 1.31  2005/11/22 09:06:54  zstein
// *** empty log message ***
//
// Revision 1.30  2005/11/17 04:24:50  zstein
// *** empty log message ***
//
// Revision 1.29  2005/11/16 07:13:57  zstein
// *** empty log message ***
//
// Revision 1.28  2005/11/08 01:20:34  zstein
// *** empty log message ***
//
//******************************************************************************

#ifndef __AUDIO_PLAYER_H__
#define __AUDIO_PLAYER_H__

#include <tavi_lib.h>
#include <Task/tavi_global.h>
#include <Component/ListView.h>
#include <Lib/LibSoundPaletteDbx.h>
#include "MusicDB.h"
#include "FolderPL.h"
#include "MusicDBPL.h"
#include "PlaylistPL.h"

extern CSoundPaletteDbx* g_pSndPalette;
extern CFolderPL* g_FolderPL;
extern CPlaylistPL* g_PlaylistPL;
extern CMusicDBPL* g_MusicDBPL;
extern CArray<CListItem*> g_AllTrackList;
extern int g_PlayKey;

extern int AP_MsgProc( TMsgEntry* pMsg );
extern int AP_Play( int type, const CString& path, CDeviceMusicDB* handle, const CString& filename );
extern int AP_Stop( void );
extern int AP_Pause( void );
extern int AP_Resume( void );
extern int AP_Seek( int sec, int dir ); // 0: rewind, 1: FF
extern int AP_Volume( int dir );
extern int AP_Next( void );
extern int AP_Prev( void );
extern int AP_GetPlayState();
extern int AP_GetQueue( void );
extern int AP_Kill( void );
extern bool AP_SetPlaystate( int state );
extern int AP_GetPlaystate( void );
extern int AudioPlayer_Task( TTaskInitParam* tParam );
extern int AP_MsgHandler( int id, U32 msgid, int param1, int param2 );
extern void AP_DoIdle( void* arg );
extern void AP_Wait( int, void* );
extern CWindow* AP_HasPlayer( void );
extern CList< CWindow >* AP_GetWndList( void );
extern void AP_Remount( void );
extern void AP_BackToPlayer( void );
extern void AP_HDD_Wakeup( void );

#endif

