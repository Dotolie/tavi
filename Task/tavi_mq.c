//************************ Copyright(C) New Media Life Inc. ************************

//

// Description : Message Queue definition

// 

// $Source: /home/cvsroot/TAVI020/tavi/Task/tavi_mq.c,v $

// $Revision: 1.1.1.1 $

// $Date: 2006/02/22 12:33:31 $

// $Author: zstein $ Lee Seok<zstein@newmedialife.com>

//

//******************************************************************************

// $Log: tavi_mq.c,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.1  2005/06/14 05:40:05  zstein
// *** empty log message ***
//
// Revision 1.2  2005/02/21 02:11:39  zstein
// BUG fixed & update
//
// Revision 1.1  2005/02/17 02:24:24  zstein
// *** empty log message ***
//

//*******************************************************************************

#include <stdio.h>
#include "tavi_mq.h"

QID_t SysMSGInit( QKey_t key )
{
	QID_t id;

	id = msgget( key, IPC_CREAT|0666 );

	return id;
}

QError_t SysMSGClose( QID_t qid )
{
	QError_t ret = QError_NOERROR;
	
	if( msgctl( (int)qid, IPC_RMID, NULL ) < 0 ) {
		fprintf( stderr, "failed to msgctl\n" );
		ret = QError_CLOSE;
	}

	return ret;
}

QError_t SysMSGSend( QID_t qid, const QBUF_t* buf, int size, QFLAG_t flags )
{
	QError_t ret = QError_NOERROR;

	if( msgsnd( (int)qid, (struct msgbuf*)buf, size, flags ) < 0 ) {
		ret = QError_SEND;
	}
	
	return ret;
}

QError_t SysMSGRecv( QID_t qid, QBUF_t* buf, long type, QFLAG_t flags )
{
	QError_t ret = QError_NOERROR;

	if( msgrcv( (int)qid, (struct msgbuf*)buf, buf->lLength, type, flags ) < 0 ) {
		ret = QError_RECV;
	}
	
	return ret;
}

QError_t SysMSGCtrl( QID_t qid, QCMD_t cmd, QDESC_t* desc )
{
	QError_t ret = QError_NOERROR;

	if( msgctl( (int)qid, (int)cmd, (struct msqid_ds*)desc ) < 0 ) {
		ret = QError_CTRL;
	}
	
	return ret;

}
