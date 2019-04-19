//************************ Copyright(C) New Media Life Inc. *******************
//
// Description : Authentification functions header for melon
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/AudioPlayer/Melon/Melon.h,v $
// $Revision: 1.1 $
// $Date: 2006/06/12 04:33:50 $
// $Author: ywkim $
//
//*****************************************************************************
// $Log: Melon.h,v $
// Revision 1.1  2006/06/12 04:33:50  ywkim
// Initial release for melon
//
//*****************************************************************************
#ifndef __MELON_H__
#define __MELON_H__


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define MELON_VALIDTIME	'V'

typedef struct  {
	char scSerialNo[16];
	char scNonce[44];
	unsigned long uGap;
	char cSign;
	char cTimeValid;
}	Melon_Security_t;


int MELON_Init( char *pId );
void MELON_Destroy( void );
int MELON_CreateDID( void );
int MELON_DeleteDID( void );
int MELON_MoveCluster( void );
int MELON_TimeAdjust( void );
int MELON_SetDrmInfo( char Data );
int MELON_DrmTimeInfo( int flag );
int MELON_SaveNonce( void );



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __MELON_H__ */


