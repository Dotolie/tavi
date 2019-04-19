//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : FM Radio(GNT-702S) Driver header for TAVI
// 
// $Source: /home/cvsroot/TAVI020/tavi/Driver/fmradio/tavi_fm.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: tavi_fm.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.1  2005/02/07 07:15:49  ywkim
// FMRadio driver file
//
//*****************************************************************************
#ifndef __TAVI_FM_H__
#define __TAVI_FM_H__

#include <asm/ioctl.h>
/*
struct fm_tx_t 
{
	char mute;
	char search_mode;
	unsigned short pll;
	char search_dir;
	char search_level;
	char hilo_injection;
	char mono;
	char mute_l;
	char mute_r;
	char swp1; 		// software programmable port1
	char swp2; 		// software programmable port2
	char standby;
	char band_limit;
	char xtal;
	char soft_mute;
	char hcc; 		// high cut control
	char snc; 		// stereo noise cancelling
	char si; 			// serach indicator
	char pllref;
	char dtc; 			// de-emphasis time constant
};

struct fm_rx_t
{
	char ready;
	char band_limit;
	unsigned short pll;
	char stereo;
	char IF;
	char level;
};
*/

/* ioctl command set */
#define TAVI_FM_MAGIC			'f'
//#define TAVI_IOCG_PARAM			_IOR( TAVI_FM_MAGIC, 0, struct fm_tx_t )
//#define TAVI_IOCS_PARAM			_IOW( TAVI_FM_MAGIC, 1, struct fm_rx_t )

#endif /* __TAVI_FM_H__ */
