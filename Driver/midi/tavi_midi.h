//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Hwa sound source driver
// 
// $Source: /home/cvsroot/TAVI020/tavi/Driver/midi/tavi_midi.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $
//
//******************************************************************************
// $Log: tavi_midi.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.3  2005/09/14 05:16:01  ywkim
// change PowerDown_Q64(), remove CMD_FADE_OUT
//
// Revision 1.2  2005/08/25 04:55:56  ywkim
// initial release
//
// Revision 1.1.2.1  2005/08/25 04:38:03  ywkim
// initial release for reducing power consumtion
//
//******************************************************************************

#ifndef __TAVI_MIDI_H__
#define __TAVI_MIDI_H__

#include <asm/ioctl.h>
#include "../misc_group.h"

#define	MIDICMD_SMF_START			0
#define	MIDICMD_SMF_STOP				1
#define	MIDICMD_SMF_PAUSE			2
#define	MIDICMD_SMF_RESUME			3
#define	MIDICMD_SMF_TEMPOINC			4
#define	MIDICMD_SMF_TEMPODEC		5
#define	MIDICMD_SMF_TEMPOBASE		6
#define	MIDICMD_SMF_KEYINC			7
#define	MIDICMD_SMF_KEYDEC			8
#define	MIDICMD_SMF_KEYBASE			9
#define 	MIDICMD_CTR_MASTERVOLUME	10
#define 	MIDICMD_CTR_TONEGENERATER	11

struct midi_struct {
	char	cCMD;
	char cParam1;
	char cParam2;
	char cParam3;
};

/* ioctl command */
#define TAVI_MIDI_MAGIC				'W'
#define TAVI_MIDI_IOCS_CMD			_IOW( TAVI_MIDI_MAGIC, 1, struct midi_struct )
#define TAVI_MIDI_IOCG_REG			_IOW( TAVI_MIDI_MAGIC, 2, struct midi_struct )



#endif /* __TAVI_MIDI_H__ */



