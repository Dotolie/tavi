//************************ Copyright(C) New Media Life Inc. ************************

//

// Description : Audio Interface declaration

// 

// $Source: /home/cvsroot/TAVI020/tavi/Task/tavi_mq.h,v $

// $Revision: 1.1.1.1 $

// $Date: 2006/02/22 12:33:31 $

// $Author: zstein $ Lee Seok<zstein@newmedialife.com>

//

//******************************************************************************

// $Log: tavi_mq.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.4  2005/08/19 11:20:04  etnlwind
// Update for __ATTRIBUTE_PACKED__ and FileBrowser Delete Func.
//
// Revision 1.3  2005/06/15 11:51:30  etnlwind
// Update for Video Player Touch Control And Background Savefile Process
//
// Revision 1.2  2005/06/14 05:48:42  zstein
// - added tavi_mq
//
// Revision 1.1  2005/06/14 05:40:05  zstein
// *** empty log message ***
//
// Revision 1.3  2005/03/07 00:50:37  zstein
// Cebit Version
//
// Revision 1.2  2005/02/21 02:11:39  zstein
// BUG fixed & update
//
// Revision 1.1  2005/02/16 16:17:37  zstein
// *** empty log message ***
//

//*******************************************************************************
#ifndef __TAVI_MESSAGE_QUEUE_H__
#define __TAVI_MESSAGE_QUEUE_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>

typedef int QID_t;
typedef int QKey_t;
typedef int QFLAG_t;
typedef int QCMD_t;
typedef struct msqid_ds QDESC_t;

typedef struct {
	long lMsgID;
	long lLength;
	char szMsg[1];
} QBUF_t;

typedef enum {
	QError_NOERROR,
	QError_OPEN,
	QError_CLOSE,
	QError_SEND,
	QError_RECV,
	QError_CTRL
} QError_t;

#define Q_STAT		IPC_STAT
#define Q_SET		IPC_SET
#define Q_REMOVE	IPC_RMID

extern QID_t SysMSGInit( QKey_t qKey );
extern QError_t SysMSGClose( QID_t qid );
extern QError_t SysMSGSend( QID_t qid, const QBUF_t* buf, int size, QFLAG_t flags );
extern QError_t SysMSGRecv( QID_t qid, QBUF_t* buf, long type, QFLAG_t flags );
extern QError_t SysMSGCtrl( QID_t qid, QCMD_t cmd, QDESC_t* desc );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TAVI_MESSAGE_QUEUE_H__ */

