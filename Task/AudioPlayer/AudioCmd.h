#ifndef __AUDIO_COMMAND_H__
#define __AUDIO_COMMAND_H__

#define AUDIO_MSG_KEY				0xA0D0

#include "../tavi_global.h"
#include "TypeDef.h"
#include "tavi_structures.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


struct AudioCmd {
	long lMsgID;
	long lLength;
	#define AUDIO_CMD_INFO					0
	#define AUDIO_CMD_PLAY					1
	#define AUDIO_CMD_PAUSE				2
	#define AUDIO_CMD_STOP					3
	#define AUDIO_CMD_SEEK					4
	#define AUDIO_CMD_PLAYBACK			5
	#define AUDIO_CMD_ACK					7
	#define AUDIO_CMD_NOINFO				8
	#define AUDIO_CMD_ERROR				13
	#define AUDIO_CMD_AB_START			16
	#define AUDIO_CMD_AB_STOP				17
	#define AUDIO_CMD_INIT					18
	#define AUDIO_CMD_RESUME				20

	#define AUDIO_CMD_SET_EOF				24
	#define AUDIO_CMD_CLEAR_EOF			25
	#define AUDIO_CMD_HDD_WAKEUP			26

	#define AUDIO_CMD_SCANFOLDER			29
	#define AUDIO_CMD_REMOUNT				30
	// for melon
	#define AUDIO_CMD_MELONPLAYINFO		31
	#define AUDIO_CMD_MELONUPDATEINFO	32
	#define AUDIO_CMD_MELONCREATEDID		33

	long lCmd;
	#define AUDIO_STATE_READY				0
	#define AUDIO_STATE_BUSY				1
	#define AUDIO_STATE_PLAYING			2
	#define AUDIO_STATE_PAUSE				3
	#define AUDIO_STATE_SEEKING			4
	#define AUDIO_STATE_STOP				5
	long lState;
	#define AUDIO_ERR_NOERROR				0x00000
	#define AUDIO_ERR_INVALID_CMD			0xE5001
	#define AUDIO_ERR_FILEOPEN				0xE5002
	#define AUDIO_ERR_NOTSUPPORT			0xE5003
	// command error
	#define AUDIO_ERR_PLAY					0xE5010
	#define AUDIO_ERR_PAUSE				0xE5011
	#define AUDIO_ERR_RESUME				0xE5012
	#define AUDIO_ERR_STOP					0xE5013
	#define AUDIO_ERR_SEEK					0xE5014
	#define AUDIO_ERR_AB					0xE5015
	// for melon
	#define AUDIO_ERR_MELON				0xE5100
	long lError;
	char szArg[512];
};

#define ERR_MSG_NOERROR					"NO ERROR"
#define ERR_MSG_INVALID_CMD				"INVALID COMMAND"
#define ERR_MSG_FILEOPEN					"FAILED TO OPEN FILE"
#define ERR_MSG_NOTSUPPORT				"NOT SUPPORTED FILE"


struct VideoCmd {
	long lMsgID;
	long lLength;
	
	#define VIDEO_CMD_SAVE_LPP 			100	// LPP: Last Play Position
	#define VIDEO_CMD_CAPTION_ALLOC		101
	#define VIDEO_CMD_CAPTION_DEALLOC	102
	long lCmd;
	
	long lState;
	long lError;
	char szArg[512];
};


enum {
	TYPE_FILE = 0x0010,
	TYPE_PLAYLIST = 0x0020,
	TYPE_FOLDER = 0x0040,
	TYPE_MUSICDB = 0x0080
};

enum {
	IDDB_TYPE_ARTIST,
	IDDB_TYPE_ALBUM,
	IDDB_TYPE_GENRE,
	IDDB_TYPE_ALLTRACKS
};

struct PlayInfo {
	long lStartTime;
	char szFilename[1];
};

#define BYTE_LENGTH		80 * 2

typedef struct StringInfo {
	char bytes[BYTE_LENGTH];
	bool unicode;
};

typedef struct PlaybackInfo {
	long lTime;
	long lBitrate;
	long lSamplingrate;
	long lVBR;
};

typedef struct AckInfo {
	int cmd;
	int suberr;
	int media;
	bool done;
	union {
		struct {
			StringInfo title;
			StringInfo artist;
			StringInfo album;
			int totaltime;
		} play;
		struct {
			int dummy;
		} pause;
		struct {
			int dummy;
		} resume;
		struct {
			int dummy;
		} seek;
		struct {
			int dummy;
		} ab;
	} u;
};

struct ABInfo {
	int lStartTime;
	int lEndTime;
};

struct SavePositionInfo {
	uchar_t* sPathname;
	uchar_t* sFilename;
	U16 usFilepositionInSeconds;
};

struct CaptionInfo {
	//CSubtitle* pSubtitle;
	VP_CLOSE_SMI CloseSMI;
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __AUDIO_COMMAND_H__ */
