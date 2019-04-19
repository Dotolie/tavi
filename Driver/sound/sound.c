/* 
 * 
 * Copyright(C) 2004 New Media Life Inc.
 *
 * Author: Lee Seok<zstein@newmedialife.com>
 *
 * $Log: sound.c,v $
 * Revision 1.3  2006/06/07 08:34:31  zstein
 * - improved volume control and removed unused codes.
 *
 * Revision 1.2  2006/03/09 02:11:21  zstein
 * - Make sound mute when the volume level is 0.
 *
 * Revision 1.1.1.1  2006/02/22 12:33:31  zstein
 * TAVI020 REBUILD
 *
 * Revision 1.33  2006/02/15 07:42:09  ywkim
 * Change macro of EM85XX_I2C_WRITE to function
 * Change actual parameter of speakeramp_onoff() to 1 for AMP off in dsp_init()
 *
 * Revision 1.32  2006/02/14 09:55:00  ywkim
 * Add macro of EM85XX_I2C_WRITE for rewriting data at fail time
 * Change all em85xx_i2c_write() to EM85XX_I2C_WRITE
 *
 * Revision 1.31  2006/02/14 06:32:25  ywkim
 * Change funciton of dsp_init() for rewriting with fail
 *
 * Revision 1.30  2006/02/09 05:22:31  zstein
 * - added Log cvs macro.
 *
 *
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/miscdevice.h>

#include <asm/uaccess.h>
#include <asm/arch/hardware.h>
#include <asm/delay.h>
#include <asm/smplock.h>

#include "sound.h"
#include "../i2cm/i2cm.h"
#include "../misc/misc_ctrl.h"

//#define DEBUG
#ifdef DEBUG
#define DPRINTK			printk
#else
#define DPRINTK
#endif

#define I2C_ADDR				0x34

#define VOLUME_LEVEL_BASE		0x60
#define VOLUME_LEVEL_STEP		80

// register set
#define REG_NUM					43
#define UPDATE					0x01

#define SPEAKERAMP_ONOFF		14

static unsigned short __reg_set[REG_NUM] = {
	0x0097, 0x0297, 0x04ff, 0x06ff, 0x0800, 
	0x0a08, 0x0c00, 0x0e01, 0x1000, 0x1200,
	0x15ff, 0x17ff, 0x180f, 0x1a0f, 0x1c00,
	0x1e00, 0x2000, 0x227b, 0x2400, 0x2632,
	0x2800, 0x2ac3, 0x2cc3, 0x2ec0, 0x3000,
	0x3200, 0x3400, 0x3600, 0x3800, 0x3a00,
	0x3c00, 0x3e00, 0x4000, 0x4200, 0x4450,
	0x4650, 0x4850, 0x4a50, 0x4c50, 0x4e50,
	0x5079, 0x5279, 0x5479 };

/*
 * 0x0a08: dac mute for removing pop noise
 * 0x0397 -> 0x0297, 0x0579 -> 0x0479(OUT1 L/R)
 * 0x3580 -> 0x3400 ( disable DACL/R )
 * 0x35f8  -> 0x3478
 */
static unsigned short __snd_init_data[] = {
	0x32c0, /* POWER MGT1 */
	0x3580, 
	0x35f8, 
	0x0097,
	0x0297, 
	0x0580/*0x0579*/, // lineout1 L mute
	0x0780/*0x0779*/, // lineout1 R mute
	0x0800,
	0x0a00, 0x0c00, 
	0x0e0a, /* AUDIO INTERFACE */
	0x1000,
	0x1200, 
	0x15ff, /* 0x15ff, */ //0dB LDAC
	//0x15ff,
	0x17ff, /* 0x17ff, */ // 0dB RDAC
	//0x17ff,
	0x180f,
	0x1a0f, 0x2000, 0x227b, 0x2400,
	0x2632, 
	0x2888, /* NOISE GATE: -65dBfs, PGA gain held constant, DISABLE */
	/*0x2bc9*/0x2ac3, /*0x2dc9*/0x2cc3,
	0x2ec0, /* ADDITIONAL CONTROL1 */
	0x3000, 0x3e00, 0x4000,
	0x4200, 
	0x4403, /* LD2LO: LMIX: -3dB */
	0x4650, 
	0x4850, /* RMIX: -9dB */
	0x4a03, /* RD2RO: RMIX: -3dB */
	0x4c50, 
	0x4e50, 
	0x5180, // lineout2 L mute & zc
	0x5380, // lineout2 R mute & zc
	0x5479
};

static struct snd_dev_t snddev;

int EM85XX_I2C_WRITE( char addr, char reg, const char* data, int size)
{	
	int ret=0;
	int count=0;

  lock_kernel();
	while ( count < 3 ) {
		ret =em85xx_i2c_write( addr,reg,data,size);
		if ( ret < 0 ) {
			printk("[%d] error i2c write reg=0x%x, retern=%d\n",count, reg, ret );
			}
		else {
			break;
			}
		count++;
		}
  unlock_kernel();

	return ret;
}

static void set_dac_mute( struct snd_dev_t* pdev )
{
	char reg, data;
  
	if( pdev->dac_mute ) __reg_set[POWER_MGT2] &= ~0x78;
	else __reg_set[POWER_MGT2] |= 0x78;
	reg = (__reg_set[POWER_MGT2]&0xff00)>>8;
	data = (__reg_set[POWER_MGT2]&0xff);
	EM85XX_I2C_WRITE( I2C_ADDR, reg, &data, 1 );
}
// Minimizing pop-noise when power off.
static void power_off( void )
{
	char reg, data;

	// 1. Mute DAC
	snddev.dac_mute = 1;
	set_dac_mute( &snddev );
	// 2. Disable All output
	reg = 0x34;
	data = 0x00;
	EM85XX_I2C_WRITE( I2C_ADDR, reg, &data, 1 );
}

// Max¿¡¼­ 6dBÁ¤µµ ³·Ãß±â À§ÇØ »ç¿ë
static int convert( int line, int vol ) 
{
	int limit;
	int c_vol;
	limit = ( line ) ? 80 : 80;
	c_vol = (int)(vol*limit/80) + 48;
	return c_vol;
}

static void set_vol( int line, struct snd_dev_t* pdev )
{
	char regl, regr, datal, datar;
	int r, l, vl, vr;
	int mute=0;

	if( pdev->lineout1.vol_left >= pdev->lineout1.range ) 
		pdev->lineout1.vol_left = 79;
	if( pdev->lineout1.vol_right >= pdev->lineout1.range )
		pdev->lineout1.vol_right = 79;

	if( line == 0 ) {
		mute = pdev->lineout1.mute;
		l = LINEOUT1_VOL_LEFT;
		r = LINEOUT1_VOL_RIGHT;
	}
	else if( line == 1 ) {
		mute= pdev->lineout2.mute;
		l = LINEOUT2_VOL_LEFT;
		r = LINEOUT2_VOL_RIGHT;
	}
	else return;

	vl = pdev->lineout1.vol_left;
	vr = pdev->lineout1.vol_right;
	
	__reg_set[l] &= 0xff00;
	if( line == 0 ) __reg_set[l] |= pdev->lineout1.vol_left + 48;
	else __reg_set[l] |= pdev->lineout2.vol_left + 48;
	
	regl = (__reg_set[l]&0xff00) >> 8;
	datal = (__reg_set[l]&0xff);
    if( datal == 48 ) {
        datal--;
    }
	// 6dB Á¤µµ ³·Ãã. 
	datal = convert( line, vl );
	if( !mute ) EM85XX_I2C_WRITE( I2C_ADDR, regl, &datal, 1 );
	
	__reg_set[r] &= 0xff00;
	if( line == 0 ) __reg_set[r] |= pdev->lineout1.vol_right + 48;
	else __reg_set[r] |= pdev->lineout2.vol_right + 48;
	regr = (__reg_set[r]&0xff00) >> 8;
	datar = (__reg_set[r]&0xff);
	// 6dB Á¤µµ ³·Ãã. 
	datar = convert( line, vr );
    if( datar == 48 ) {
        datar--;
    }
	if( !mute ) {
    EM85XX_I2C_WRITE( I2C_ADDR, regr, &datar, 1 );
  }
}

static void set_input_bypass( struct snd_dev_t* dev )
{
	unsigned short left_mixer1 = __reg_set[LEFTOUT_MIX1];
	unsigned short left_mixer2 = __reg_set[LEFTOUT_MIX2];
	unsigned short right_mixer1 = __reg_set[RIGHTOUT_MIX1];
	unsigned short right_mixer2 = __reg_set[RIGHTOUT_MIX2];
	char reg, data;

	if( dev->linein.bypass ) {
		// clear left mixer
		left_mixer1 &= ~0x87;
		left_mixer2 &= ~0x87;
		// set  left mixer
		left_mixer1 |= 0x80 | dev->linein.line_sel;		// LI2LO on(line-in left)
		// clear right mixer
		right_mixer1 &= ~0x87;
		right_mixer2 &= ~0x80;
		// set right mixer
		right_mixer1 |= dev->linein.line_sel;		// LI2RO off
		right_mixer2 |= 0x80;					// RI2RO on
	}
	else {
		left_mixer1 &= ~0x0080;
		left_mixer2 &= ~0x0080;
		right_mixer1 &= ~0x0080;
		right_mixer2 &= ~0x0080;
		left_mixer1 |= 0x0100;
		right_mixer2 |= 0x0100;
	}

	reg = (left_mixer1&0xff00) >> 8;
	data = (left_mixer1&0xff);
	if( EM85XX_I2C_WRITE( I2C_ADDR, reg, &data, 1 ) ) __reg_set[LEFTOUT_MIX1] = left_mixer1;
	reg = (left_mixer2&0xff00) >> 8;
	data = (left_mixer2&0xff);
	if( EM85XX_I2C_WRITE( I2C_ADDR, reg, &data, 1 ) ) __reg_set[LEFTOUT_MIX2] = left_mixer2;
	reg = (right_mixer1&0xff00) >> 8;
	data = (right_mixer1&0xff);
	if( EM85XX_I2C_WRITE( I2C_ADDR, reg, &data, 1 ) ) __reg_set[RIGHTOUT_MIX1] = right_mixer1;
	reg = (right_mixer2&0xff00) >> 8;
	data = (right_mixer2&0xff);
	if( EM85XX_I2C_WRITE( I2C_ADDR, reg, &data, 1 ) ) __reg_set[RIGHTOUT_MIX2] = right_mixer2;
}

static void set_linein( struct snd_dev_t* dev )
{
	char reg, data;
	unsigned short power_mgt1 = __reg_set[POWER_MGT1];

	if( dev->linein.onoff ) {
		power_mgt1 |= 0x0f<<2;
	}
	else {
		power_mgt1 &= ~(0x0f<<2);
	}

	__reg_set[POWER_MGT1] = power_mgt1;
	reg = (power_mgt1&0xff00)>>8;
	data = (power_mgt1&0xff);
	EM85XX_I2C_WRITE( I2C_ADDR, reg, &data, 1 );
}

static void set_audio_format( struct snd_dev_t* dev )
{
	char reg, data;
	unsigned short audio_inf = __reg_set[AUDIO_INF];

	audio_inf &= ~0x03; // clear
	audio_inf |= dev->linein.audio_format;

	__reg_set[AUDIO_INF] = audio_inf;
	reg = (__reg_set[AUDIO_INF]&0xff00) >> 8;
	data = (__reg_set[AUDIO_INF]&0xff);
	EM85XX_I2C_WRITE( I2C_ADDR, reg, &data, 1 );
}

#if 0
static void set_3d( char value )
{
	char reg, data;
	short tmp1, tmp2;

	tmp1 = __reg_set[POWER_MGT1];
	tmp2 = __reg_set[POWER_MGT2];

#if 0
	__reg_set[POWER_MGT1] &= ~0x0c;
	reg = (__reg_set[POWER_MGT1]&0xff00) >> 8;
	data = (__reg_set[POWER_MGT1]&0xff);
	EM85XX_I2C_WRITE( I2C_ADDR, reg, &data, 1 );
	__reg_set[POWER_MGT2] &= ~0x180;
	reg = (__reg_set[POWER_MGT2]&0xff00) >> 8;
	data = (__reg_set[POWER_MGT2]&0xff);
	EM85XX_I2C_WRITE( I2C_ADDR, reg, &data, 1 );
#endif
	__reg_set[S3D_CTRL] &= 0xff80;
	__reg_set[S3D_CTRL] |= value;
	DPRINTK( "3D: 0x%04lx\n", __reg_set[S3D_CTRL] );
	reg = (__reg_set[S3D_CTRL]&0xff00) >> 8;
	data = (__reg_set[S3D_CTRL]&0xff);
	EM85XX_I2C_WRITE( I2C_ADDR, reg, &data, 1 );
	DPRINTK( "3D -> reg: 0x%02lx, data: 0x%02lx\n", reg, data );

#if 0
	//restore power management register 1/2
	__reg_set[POWER_MGT1] = tmp1;
	__reg_set[POWER_MGT2] = tmp2;

	reg = (__reg_set[POWER_MGT1]&0xff00) >> 8;
	data = (__reg_set[POWER_MGT1]&0xff);
	EM85XX_I2C_WRITE( I2C_ADDR, reg, &data, 1 );
	reg = (__reg_set[POWER_MGT2]&0xff00) >> 8;
	data = (__reg_set[POWER_MGT2]&0xff);
	EM85XX_I2C_WRITE( I2C_ADDR, reg, &data, 1 );
#endif
}

static void set_treble( char value )
{
	char reg, data;

	DPRINTK( "pdev->treble: 0x%02lx\n", value );
	__reg_set[TREBLE_CTRL] &= 0xff00;
	__reg_set[TREBLE_CTRL] |= value;
	reg = (__reg_set[TREBLE_CTRL]&0xff00) >> 8;
	data = (__reg_set[TREBLE_CTRL]&0xff);
	EM85XX_I2C_WRITE( I2C_ADDR, reg, &data, 1 );
	DPRINTK( "treble -> reg: 0x%02lx, data: 0x%02lx\n", reg, data );
}

static void set_bass( char value )
{
	char reg, data;

	DPRINTK( "pdev->bass: 0x%02lx\n", value );
	__reg_set[BASS_CTRL] &= 0xff00;
	__reg_set[BASS_CTRL] |= value;
	reg = (__reg_set[BASS_CTRL]&0xff00) >> 8;
	data = (__reg_set[BASS_CTRL]&0xff);
	EM85XX_I2C_WRITE( I2C_ADDR, reg, &data, 1 );
	DPRINTK( "bass -> reg: 0x%02lx, data: 0x%02lx\n", reg, data );
}

static int set_equalizer( struct snd_dev_t* dev )
{
	char reg, data;
	
	DPRINTK( "eq: %d\n", dev->preset_eq );
	switch( dev->preset_eq ) {
	case EQ_NORMAL:
		/* Bass: Cut-off 130Hz, Type->Linear, Level->Bypass
		  * Treble: Cut-off->8KHz, Level->Bypass
		  * 3D: Cut-off low->200Hz, Cut-off High->2.2KHz, Level->Disable
		  */
		set_bass( 0x0f );
		set_treble( 0x00 );
		set_3d( 0x0f );
		break;
	case EQ_ROCK:
		/* Bass: Cut-off->130Hz, Type->Adapt, Level->11
		 * Treble: Cut-off->8KHz, Level->+1.5dB
		 * 3D: Cut-off low->200Hz, Cut-off High->2.2KHz, Level->33%
		 */
		set_bass( 0x84 );
		set_treble( 0x06 );
		set_3d( 0x1b );
		break;
	case EQ_POP:
		/* Bass: Cut-off->130Hz, Type->Linear, Level: -3dB
		 * Treble: Cut-off->8KHz, Level->+1.5dB
		 * 3D: Cut-off Low->200Hz, Cut-off High->2.2KHz, Level->46%
		 */
		set_bass( 0x09 );
		set_treble( 0x06 );
		set_3d( 0x8f );
		break;
	case EQ_JAZZ:
		/* Bass: Cut-off->200Hz, Type->Linear, Level->+6dB
		 * Treble: Cut-off->4KHz, Level->+4.5dB
		 * 3D: Cut-off Low->500Hz, Cut-off High->2.2KHz, Level->33%
		 */
		set_bass( 0x43 );
		set_treble( 0x44 );
		set_3d( 0xab );
		break;
	case EQ_CLASSIC:
		/* Bass: Cut-off->130Hz, Type->Linear, Level->Bypass
		 * Treble: Cut-off->4KHz, Level->+6dB
		 * 3D: Cut-off Low->200Hz, Cut-off High->2.2KHz, Level->20%
		 */
		set_bass( 0x0f );
		set_treble( 0x43 );
		set_3d( 0x87 );
		break;
	case EQ_DANCE:
		/* Bass: Cut-off->130Hz, Type->Adapt, Level->13
		 * Treble: Cut-off->8KHz, Level->+6dB
		 * 3D: Cut-off Low->500Hz, Cut-off High->2.2KHz, Level->80%
		 */
		set_bass( 0x82 );
		set_treble( 0x03 );
		set_3d( 0xb9 );
		break;
	case EQ_HEAVY:
		/* Bass: Cut-off->130Hz, Type->Linear, Level->+6dB
		 * Treble: Cut-off->8KHz, Level->+6dB
		 * 3D: Cut-off Low->200Hz, Cut-off High->2.2KHz, Level->Disable
		 */
		set_bass( 0x03 );
		set_treble( 0x03 );
		set_3d( 0x80 );
		break;
	case EQ_DISCO:
		/* Bass: Cut-off->200Hz, Type->Adapt, Level->11
		 * Treble: Cut-off->8KHz, Level->+4.5dB
		 * 3D: Cut-off Low->200Hz, Cut-off High->2.2KHz, Level->Disable
		 */
		set_bass( 0x03 );
		set_treble( 0xc4 );
		set_3d( 0x80 );
		break;
	case EQ_SOFT:
		/* Bass: Cut-off->200Hz, Type->Adapt, Level->11
		 * Treble: Cut-off->4KHz, Level->+4.5dB
		 * 3D: Cut-off Low->200Hz, Cut-off High->2.2KHz, Level->Disable
		 */
		set_bass( 0x03 );
		set_treble( 0x44 );
		set_3d( 0x80 );
		break;
	case EQ_3DSTEREO:
		/* Bass: Cut-off->130Hz, Type->Linear, Level->Bypass
		 * Treble: Cut-off->8Hz, Level->Bypass
		 * 3D: Cut-off Low->200Hz, Cut-off High->2.2KHz, Level->73%
		 */
		set_bass( 0x0f );
		set_treble( 0x0f );
		set_3d( 0x97 );
		break;
	case EQ_HALL:
		/* Bass: Cut-off->130Hz, Type->Linear, Level->+4.5dB
		 * Treble: Cut-off->8KHz, Level->+7.5dB
		 * 3D: Cut-off Low->200Hz, Cut-off High->2.2KHz, Level->76%
		 */
		set_bass( 0x04 );
		set_treble( 0x02 );
		set_3d( 0x99 );
		break;
	default:
		return -1;
	}

	reg = (__reg_set[BASS_CTRL]&0xff00) >> 8;
	data = (__reg_set[BASS_CTRL]&0xff);
	EM85XX_I2C_WRITE( I2C_ADDR, reg, &data, 1 );
	reg = (__reg_set[TREBLE_CTRL]&0xff00) >> 8;
	data = (__reg_set[TREBLE_CTRL]&0xff);
	EM85XX_I2C_WRITE( I2C_ADDR, reg, &data, 1 );
	reg = (__reg_set[S3D_CTRL]&0xff00) >> 8;
	data = (__reg_set[S3D_CTRL]&0xff);
	EM85XX_I2C_WRITE( I2C_ADDR, reg, &data, 1 );
	
	return 0;
}
#endif

static void speakeramp_onoff( int onoff )
{
	speaker_amp_onoff( (onoff ? 1:0) );
	return;
}

static void lineout1_mute( struct snd_dev_t* pdev )
{
	char reg, data;

	unsigned short line1_L = __reg_set[LINEOUT1_VOL_LEFT];
	unsigned short line1_R = __reg_set[LINEOUT1_VOL_RIGHT];
	if( pdev->lineout1.mute ) {
		line1_L &= 0xff00;
		line1_R &= 0xff00;
		reg = (line1_L&0xff00) >> 8;
		data = (line1_L&0xff);
		EM85XX_I2C_WRITE( I2C_ADDR, reg, &data, 1 );
		reg = (line1_R&0xff00) >> 8;
		data = (line1_R&0xff);
		EM85XX_I2C_WRITE( I2C_ADDR, reg, &data, 1 );
	}
	else {
		line1_L = __reg_set[LINEOUT1_VOL_LEFT];
		line1_R = __reg_set[LINEOUT1_VOL_RIGHT];
		reg = (line1_L&0xff00) >> 8;
		data = (line1_L&0xff) - 48;
		data = convert( 0, data );
		EM85XX_I2C_WRITE( I2C_ADDR, reg, &data, 1 );
		reg = (line1_R&0xff00) >> 8;
		data = (line1_R&0xff) - 48;
		data = convert( 0, data );
		EM85XX_I2C_WRITE( I2C_ADDR, reg, &data, 1 );
	}
}

static void lineout2_mute( struct snd_dev_t* pdev )
{
	char reg, data;

	unsigned short line_L = __reg_set[LINEOUT2_VOL_LEFT];
	unsigned short line_R = __reg_set[LINEOUT2_VOL_RIGHT];
	if( pdev->lineout2.mute ) {
		line_L &= 0xff00;
		line_R &= 0xff00;
		reg = (line_L&0xff00) >> 8;
		data = (line_L&0xff);
		EM85XX_I2C_WRITE( I2C_ADDR, reg, &data, 1 );
		reg = (line_R&0xff00) >> 8;
		data = (line_R&0xff);
		EM85XX_I2C_WRITE( I2C_ADDR, reg, &data, 1 );
	}
	else {
		line_L = __reg_set[LINEOUT2_VOL_LEFT];
		line_R = __reg_set[LINEOUT2_VOL_RIGHT];
		reg = (line_L&0xff00) >> 8;
		data = (line_L&0xff) - 48;
		data = convert( 1, data );
		EM85XX_I2C_WRITE( I2C_ADDR, reg, &data, 1 );
		reg = (line_R&0xff00) >> 8;
		data = (line_R&0xff) - 48;
		data = convert( 1, data );
		EM85XX_I2C_WRITE( I2C_ADDR, reg, &data, 1 );
	}
}

static void set_input_sel( struct snd_dev_t* pdev )
{
	char reg, data;
	
	__reg_set[ADCL_SIG_PATH] &= 0xff00;
	__reg_set[ADCR_SIG_PATH] &= 0xff00;
	switch( pdev->linein.line_sel ) {
	case LINE_SEL_LINE1: // fm
		__reg_set[ADCL_SIG_PATH] |= 0x00;
		__reg_set[ADCR_SIG_PATH] |= 0x00;
		break;
	case LINE_SEL_LINE2: // line-in
		__reg_set[ADCL_SIG_PATH] |= 0x10;
		__reg_set[ADCR_SIG_PATH] |= 0x10;
		break;
	case LINE_SEL_LINE3: // mic
		__reg_set[ADCL_SIG_PATH] |= 0xa0;
		__reg_set[ADCR_SIG_PATH] |= 0x20;
		break;
	}

	reg = (__reg_set[ADCL_SIG_PATH]&0xff00) >> 8;
	data = (__reg_set[ADCL_SIG_PATH]&0xff);
	EM85XX_I2C_WRITE( I2C_ADDR, reg, &data, 1 );

	reg = (__reg_set[ADCR_SIG_PATH]&0xff00) >> 8;
	data = (__reg_set[ADCR_SIG_PATH]&0xff);
	EM85XX_I2C_WRITE( I2C_ADDR, reg, &data, 1 );
}

static void set_linein_mute( struct snd_dev_t* pdev )
{
	char reg, data;

	if( pdev->linein.mute ) {
		__reg_set[LINEIN_VOL_LEFT] |= 0x80;
		__reg_set[LINEIN_VOL_RIGHT] |= 0x80;
	}
	else {
		__reg_set[LINEIN_VOL_LEFT] &= ~0x80;
		__reg_set[LINEIN_VOL_RIGHT] &= ~0x80;
	}

	reg = (__reg_set[LINEIN_VOL_LEFT]&0xff00) >> 8;
	data = (__reg_set[LINEIN_VOL_LEFT]&0xff);
	EM85XX_I2C_WRITE( I2C_ADDR, reg, &data, 1 );

	reg = (__reg_set[LINEIN_VOL_RIGHT]&0xff00) >> 8;
	data = (__reg_set[LINEIN_VOL_RIGHT]&0xff);
	EM85XX_I2C_WRITE( I2C_ADDR, reg, &data, 1 );
}

static void set_adc_onoff( struct snd_dev_t* dev )
{
	char reg, data;
	unsigned short power_mgt1 = __reg_set[POWER_MGT1];

	if( dev->linein.adc_onoff&(1<<LEFT_ADC_ON) ) 
		power_mgt1 |= 0x80;
	else
		power_mgt1 &= ~0x80;
	if( dev->linein.adc_onoff&(1<<RIGHT_ADC_ON) )
		power_mgt1 |= 0x40;
	else
		power_mgt1 &= ~0x40;

	reg = (power_mgt1&0xff00)>>8;
	data = (power_mgt1&0xff);
	EM85XX_I2C_WRITE( I2C_ADDR, reg, &data, 1 );

	__reg_set[POWER_MGT1] = power_mgt1;
}

static void set_mic_boost( struct snd_dev_t* pdev )
{
	char reg, data;
	
	__reg_set[ADCL_SIG_PATH] &= ~0x3f; // RINPUT3 AND 13dB
	__reg_set[ADCL_SIG_PATH] |= pdev->linein.boost<<4;
	reg = ( __reg_set[ADCL_SIG_PATH]&0xff00 ) >> 8;
	data = __reg_set[ADCL_SIG_PATH]&0xff;
	EM85XX_I2C_WRITE( I2C_ADDR, reg, &data, 1 );
}

static void set_linein_vol( struct snd_dev_t* pdev )
{
	char reg, data;

	__reg_set[LINEIN_VOL_LEFT] &= 0xfe00;
	__reg_set[LINEIN_VOL_LEFT] |= 0x100 | pdev->linein.vol_left;
	reg = ( __reg_set[LINEIN_VOL_LEFT]&0xff00 ) >> 8;
	data = __reg_set[LINEIN_VOL_LEFT]&0xff;
	EM85XX_I2C_WRITE( I2C_ADDR, reg, &data, 1 );

	__reg_set[LINEIN_VOL_RIGHT] &= 0xfe00;
	__reg_set[LINEIN_VOL_RIGHT] |= 0x100 | pdev->linein.vol_right;
	reg = ( __reg_set[LINEIN_VOL_RIGHT]&0xff00 ) >> 8;
	data = __reg_set[LINEIN_VOL_RIGHT]&0xff;
	EM85XX_I2C_WRITE( I2C_ADDR, reg, &data, 1 );
}

static void set_mic_on( struct snd_dev_t* pdev )
{
	char reg, data;
	if( pdev->micon ) __reg_set[POWER_MGT1] |= 0x02;
	else __reg_set[POWER_MGT1] &= ~0x02;
	reg = (__reg_set[POWER_MGT1]&0xff00)>>8;
	data = __reg_set[POWER_MGT1]&0xff;
	EM85XX_I2C_WRITE( I2C_ADDR, reg, &data, 1 );
}

static int dsp_open( struct inode* inode, struct file* filep )
{
	filep->private_data = ( void* )&snddev;

	return 0;
}

static int dsp_release( struct inode* inode, struct file* filep )
{
	return 0;
}

static int dsp_read( struct file* filep, char* buf, size_t size, loff_t* offset )
{
	return -EINVAL;
}

static int dsp_write( struct file* filep, const char* buf, size_t size, loff_t* offset )
{
	return -EINVAL;
}

static int dsp_ioctl( struct inode* inode, struct file* filep, unsigned int cmd, unsigned long arg )
{
	struct snd_dev_t* pdev = ( struct snd_dev_t* )arg;
	char regaddr, data;
	union sndreg_t* preg;
	int regNum;

	switch( cmd ) {
	case TAVI_IOCG_SND_REGISTERS:
		copy_to_user( (void*)pdev, (void*)&snddev, sizeof(snddev) );
		break;
	case TAVI_IOCS_SND_LINEOUT1_VOL:
		copy_from_user( (void*)&snddev, (void*)pdev, sizeof(snddev) );
		set_vol( 0, pdev );
		break;
	case TAVI_IOCS_SND_LINEOUT2_VOL:
		copy_from_user( (void*)&snddev, (void*)pdev, sizeof(snddev) );
		set_vol( 1, pdev );
		break;
	case TAVI_IOCS_SND_MONOOUT_VOL:
		copy_from_user( (void*)&snddev, (void*)pdev, sizeof(snddev) );
		break;
	case TAVI_IOCS_SND_LINEOUT1_MUTE:
		copy_from_user( (void*)&snddev, (void*)pdev, sizeof(snddev) );
		lineout1_mute( &snddev );
		break;
	case TAVI_IOCS_SND_LINEOUT2_MUTE:
		copy_from_user( (void*)&snddev, (void*)pdev, sizeof(snddev) );
		lineout2_mute( &snddev );
		break;
	case TAVI_IOCS_SND_LINEIN_VOL:
		set_linein_vol( pdev );
		break;
	case TAVI_IOCS_SND_MIC_ON:
		copy_from_user( (void*)&snddev, (void*)pdev, sizeof(snddev) );
		set_mic_on( &snddev );
		break;
	case TAVI_IOCS_SND_LINEIN_ON:
		set_linein( pdev );
		snddev.linein.onoff = pdev->linein.onoff;
		break;
	case TAVI_IOCS_SND_INPUT_BYPASS:
		set_input_bypass( pdev );
		snddev.linein.bypass = pdev->linein.bypass;
		break;
	case TAVI_IOCS_SND_AUDIO_FORMAT:
		set_audio_format( pdev );
		snddev.linein.audio_format = pdev->linein.audio_format;
		break;
	#if 0
	case TAVI_IOCS_SND_EQUALIZER:
		//copy_from_user( (void*)&snddev, (void*)pdev, sizeof(snddev) );
		//set_equalizer( pdev );
		break;
	#endif
	case TAVI_IOCS_SND_SPEAKERAMP_ONOFF:
		speakeramp_onoff( pdev->speaker );
		break;
	#if 0
	case TAVI_IOCS_SND_3D:
		//set_3d( pdev->threed );
		break;	
	case TAVI_IOCS_SND_TREBLE:
		//set_treble( pdev->treble );
		break;
	case TAVI_IOCS_SND_BASS:
		//set_bass( pdev->bass );
		break;
	#endif
	case TAVI_IOCS_SND_DAC_MUTE:
		set_dac_mute( pdev );
		break;
	case TAVI_IOCS_SND_POWER_OFF:
		power_off();
		break;
	case TAVI_IOCS_SND_INPUT_SEL:
		copy_from_user( (void*)&snddev, (void*)pdev, sizeof(snddev) );
		set_input_sel( &snddev );
		break;
	case TAVI_IOCS_SND_ADC_ON:
		copy_from_user( (void*)&snddev, (void*)pdev, sizeof(snddev) );
		set_adc_onoff( &snddev );
		break;
	case TAVI_IOCS_SND_LINEIN_MUTE:
		copy_from_user( (void*)&snddev, (void*)pdev, sizeof(snddev) );
		set_linein_mute( &snddev );
		break;
	case TAVI_IOCG_SND_REGISTER:
		preg = ( union sndreg_t* )arg;
		regNum = (preg->r.reg&0xfe) >> 1;
		copy_to_user( (void*)&preg->regdata, (void*)&__reg_set[regNum], sizeof(short) );
		break;
	case TAVI_IOCS_SND_REGISTER:
		preg = ( union sndreg_t* )arg;
		regNum = (preg->r.reg&0xfe) >> 1;
		copy_from_user( (void*)&__reg_set[regNum], (void*)&preg->regdata, sizeof(short) );
		regaddr = ( __reg_set[regNum]&0xff00 ) >> 8;
		data = ( __reg_set[regNum]&0xff );
		EM85XX_I2C_WRITE( I2C_ADDR, regaddr, &data, 1 );
		break;
	case TAVI_IOCS_SND_MICBOOST:
		copy_from_user( (void*)&snddev, (void*)pdev, sizeof(snddev) );
		set_mic_boost( &snddev );
		break;
	}

	return 0;
}

static struct file_operations fops = {
	owner: THIS_MODULE,
	open: dsp_open,
	release: dsp_release,
	read: dsp_read,
	write: dsp_write,
	ioctl: dsp_ioctl
};

static struct miscdevice dsp_miscdev = {
	TAVI_AUDIO_MINOR,
	"sound",
	&fops
};

static int __init dsp_init( void )
{
	int count = sizeof( __snd_init_data ) / sizeof(short);
	int i, j;
	char reg, data;
	unsigned short p1, p2;

	speakeramp_onoff( 1 ); // Turn Off Speaker AMP 
	
	for( i=0; i<count; i++ ) {
		reg = ( __snd_init_data[i]&0xff00 ) >> 8;
		data = ( __snd_init_data[i]&0xff );
		__reg_set[reg>>1] = __snd_init_data[i];
		EM85XX_I2C_WRITE( I2C_ADDR, reg, &data, 1 );
//		if( i == 0 ) { // WHY??
//			for( j=0; j<1000; j++ ) udelay( 100 );
//		}
	}

	snddev.lineout1.vol_left = (__reg_set[LINEOUT1_VOL_LEFT]&0x7f) - 48;
	snddev.lineout1.vol_right = (__reg_set[LINEOUT1_VOL_RIGHT]&0x7f) - 48;
	snddev.lineout1.range = 80;
	snddev.lineout2.vol_left = (__reg_set[LINEOUT2_VOL_LEFT]&0x7f) - 48;
	snddev.lineout2.vol_right = (__reg_set[LINEOUT2_VOL_RIGHT]&0x7f) - 48;
	snddev.lineout2.range = 80;
	__reg_set[LINEIN_VOL_LEFT] = 0x0120;
	__reg_set[LINEIN_VOL_RIGHT] = 0x0320;
	snddev.linein.vol_left = __reg_set[LINEIN_VOL_LEFT]&0x3f;
	snddev.linein.vol_right = __reg_set[LINEIN_VOL_RIGHT]&0x3f;
	snddev.linein.range = 64;
	snddev.linein.boost = MIC_BOOST_OFF;
	snddev.micon = __reg_set[POWER_MGT1]&0x02 ? 1 : 0;
	snddev.dac_mute = 0;

	return misc_register( &dsp_miscdev );
}

static void __exit dsp_exit( void )
{
	misc_deregister( &dsp_miscdev );
}

module_init( dsp_init );
module_exit( dsp_exit );

MODULE_DESCRIPTION( "Sound Drvier for TAVI" );
MODULE_AUTHOR( "Lee Seok<zstein@newmedialife.com" );
MODULE_LICENSE( "GPL" );
