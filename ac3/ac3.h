#ifndef _AC3_H
#define _AC3_H

#include "rmbasic.h"
#define __ATTRIBUTE_PACKED__	__attribute__((packed))

typedef enum
{
	AC3_DECODER_ERROR_NO_ERROR,
	AC3_DECODER_ERROR_NOT_IMPLEMENTED,
	AC3_DECODER_ERROR_NOT_AN_AC3_FILE,
	AC3_DECODER_ERROR_FILE_DONE

} AC3_DECODER_ERROR;

typedef struct tagAC3_DECODE_INFO
{
	RMuint32 SamplesPerSecond;

} __ATTRIBUTE_PACKED__ AC3_DECODE_INFO;
#define AC3_MSG_AC3_DECODE_INFO		0

typedef RMuint32 (*AC3_fopen) (char *filename, void *context);
typedef RMuint32 (*AC3_fread) (RMuint32 handle, void *buf, RMuint32 length, void *context);
typedef RMuint32 (*AC3_fseek) (RMuint32 handle, RMuint32 pos, RMuint32 whence, void *context);
typedef RMuint32 (*AC3_fclose)(RMuint32 handle, void *context);
typedef RMuint32 (*AC3_addref) (RMuint8 *pBuffer, void *context);
typedef RMuint32 (*AC3_release) (RMuint8 *pBuffer, void *context);
typedef RMuint32 (*AC3_info) (RMint32 msg, void *info, void *context);
typedef RMuint32 (*AC3_getAC3) (RMuint8 **pBuffer, RMuint32 *plength, void *context);
typedef RMuint32 (*AC3_putAC3) (RMuint8 *pBuffer, RMuint32 length, void *context);

typedef struct tagAC3_CALLBACK_TABLE
{
	void		*context;
	AC3_fopen	fopen;
	AC3_fread	fread;
	AC3_fseek	fseek;
	AC3_fclose	fclose;
	AC3_addref	addref;
	AC3_release	release;
	AC3_info	info;
	AC3_getAC3	getAC3;
	AC3_putAC3	putAC3;

} __ATTRIBUTE_PACKED__ AC3_CALLBACK_TABLE;

class AC3Demux
{
	AC3_CALLBACK_TABLE	m_CallbackTable;
	RMuint32			m_State;
	RMuint32			m_SamplesPerSecond;
	RMuint32			m_Handle;
	RMuint8				*m_pAC3buffer;
	RMuint32			m_AC3length;

public:
	AC3Demux ();
	~AC3Demux ();

	AC3_DECODER_ERROR Init ();
	AC3_DECODER_ERROR InitCallbackTable (AC3_CALLBACK_TABLE *pCallbackTable);
	AC3_DECODER_ERROR DemuxFile (RMint8 *filename);
	AC3_DECODER_ERROR Schedule ();
};

#endif


