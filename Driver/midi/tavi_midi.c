//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Hwa sound source driver
// 
// $Source: /home/cvsroot/TAVI020/tavi/Driver/midi/tavi_midi.c,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $
//
//******************************************************************************
// $Log: tavi_midi.c,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.3  2005/09/14 05:16:00  ywkim
// change PowerDown_Q64(), remove CMD_FADE_OUT
//
// Revision 1.2  2005/08/25 04:55:56  ywkim
// initial release
//
// Revision 1.1.2.1  2005/08/25 04:38:03  ywkim
// initial release for reducing power consumtion
//
//******************************************************************************

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/miscdevice.h>
#include <linux/wait.h>
#include <linux/completion.h>
#include <linux/delay.h>
#include <asm/semaphore.h>
#include <asm/arch/hardware.h>
#include <asm/delay.h>
#include <asm/uaccess.h>
#include <linux/pbi.h>

#include "tavi_midi.h"
#include "qs64_def.h"
#include "../misc/misc_ctrl.h"


// Macros
#define TAVI_MIDI_VER_MAJOR		1
#define TAVI_MIDI_VER_MINOR		0

#define MIDI_IRQ						0	// PIo_0	QS_#INT
#define DEVICE_NO					2	// use IDE_#CS2

#define ADDR_DATAREG				0	//	data register	pbi_addr5=0, pbi_addr0=0;
#define ADDR_INDEXREG				1	//	INDX register	pbi_addr5=0, pbi_addr0=1;
#define MIDI_TIMEOUT				1024

UINT8 *pFIFO;
static struct tavi_midi_private {
	struct midi_struct midistruct;
	int busy;
} tavi_midi_priv;

UINT8 PreFIFO[128];   // temporary buffer to convert from GEMF to SMF0


void init_irq(void)
{
	//XXX
	*pPIO0_DIR = MK_REG( MIDI_IRQ, 0 );
	*pPIO0_POL = MK_REG( MIDI_IRQ, 0 );
	*pPIO0_INTEN = MK_REG( MIDI_IRQ, 1 );
}

void pbi_init(int mode)
{
	unsigned long m, n, d, Dram_PllReg, sysclk, cpuclk, Ta, Tb, Tc, Td, timing;
	*PBI_SL_CTRL_REG = mode;
	*PBI_USE_TIMING1_REG = 0x3f4;
	Dram_PllReg = *((volatile unsigned long *)(0x607010));
	m = (Dram_PllReg >> 2) & 0x3f;
	n = (Dram_PllReg >> 8) & 0x0f;
	d = (Dram_PllReg >> 1) & 0x01;
	if (d)
		cpuclk = (135 * (m+2) / (n+2));
	else
		cpuclk = (270 * (m+2) / (n+2));
	sysclk = cpuclk / 2;
	// program timing registers
	// hopefully we are no overwriting another device's timing ...
	Ta = cpuclk * 5 / 10000 + 1;
	Tb = cpuclk * 0 / 10000;
	Tc = cpuclk * 22 / 10000 + 1;
	Td = cpuclk * 84 / 10000 + 1;
	Ta = 1;
	Tb = 2;
	Tc = 3;
	Td = 0;
	timing = (Ta << 0) | (Tb << 8) | (Tc << 16) | (Td << 24);
	*PBI_TIMING1_REG = timing;
/*	
	printk( "cpu clk: %ld, timing: %ld, %ld, %ld, %ld\n", cpuclk, Ta, Tb, Tc, Td );
	printk("PBI_USE_TIMING0_REG = %08lx\n", *PBI_USE_TIMING0_REG );
	printk("PBI_USE_TIMING1_REG = %08lx\n", *PBI_USE_TIMING1_REG );
	printk("PBI_USE_TIMING2_REG = %08lx\n", *PBI_USE_TIMING2_REG );
	printk("PBI_USE_TIMING3_REG = %08lx\n", *PBI_USE_TIMING3_REG );
	printk("PBI_USE_TIMING4_REG = %08lx\n", *PBI_USE_TIMING4_REG );

	printk("PBI_CS_CONFIG_REG = %08lx\n", *PBI_CS_CONFIG_REG );
	printk("PBI_SL_CTRL_REG = %08lx\n", *PBI_SL_CTRL_REG );
*/
	return;
}

void pbi_device_config( int device, int config )
{
	uint32_t tmp;

	tmp=*PBI_CS_CONFIG_REG;
	tmp&=~(0x1111<< device);
	tmp|=(config <<device);
	*PBI_CS_CONFIG_REG=tmp;

	return;
}

unsigned char pbi_readb( unsigned short addr )
{
	uint32_t flags;
	uint16_t tmp;

	__save_flags (flags);
	__clf ();
	__cli ();

//	*PBI_SL_CTRL_REG = PBI_MODE_IDE_PADS;		// select pbi
	tmp = pbi_read_reg16(DEVICE_NO, (uint32_t)addr) & 0x000000ff;
//	*PBI_SL_CTRL_REG = PBI_MODE_LBC_PADS;	// return ide
//	*PBI_SL_CTRL_REG = 0x0d;	// return ide
	__restore_flags (flags);

	return (unsigned char)tmp;
}

void pbi_writeb( unsigned char data, unsigned short addr )
{
	uint32_t flags;
	uint16_t value;

	value = ((uint16_t)data);
	__save_flags (flags);
	__clf ();
	__cli ();

//	*PBI_SL_CTRL_REG = PBI_MODE_IDE_PADS;		// select pbi
	pbi_write_reg16(DEVICE_NO, (uint32_t)addr, value & 0x000000ff);
//	*PBI_SL_CTRL_REG = PBI_MODE_LBC_PADS;		// return ide
//	*PBI_SL_CTRL_REG = 0x0d;	// return ide

	__restore_flags (flags);

}


/********************************************************************/
/*                                                                  */
/*  Func.  : ReadStatus                                             */
/*  input  : bIndex(register address)                               */
/*  return : bData(read data)                                       */
/*                                                                  */
/********************************************************************/
UINT8 ReadStatus( UINT8 bIndex)
{
	pbi_writeb( bIndex, ADDR_INDEXREG );
	bIndex = pbi_readb(ADDR_DATAREG);
   
	return bIndex;
}



/********************************************************************/
/*                                                                  */
/*  Func.  : WriteCommand                                           */
/*  input  : bIndex(register address),bCmd(control command)         */
/*  return : no value                                               */
/*                                                                  */
/********************************************************************/
void WriteCommand( UINT8 bIndex, UINT8 bCmd)
{
	pbi_writeb( bIndex, ADDR_INDEXREG );
	pbi_writeb( bCmd, ADDR_DATAREG );
}



/********************************************************************/
/*                                                                  */
/*  Func.  : CommandOut                                             */
/*  input  : reg(register address),val(control command)             */
/*  return : true or false                                          */
/*                                                                  */
/********************************************************************/
UINT8 CommandOut(UINT8 reg, UINT8 val)
{
   WriteCommand(reg,val);

   return TRUE;   
}


/********************************************************************/
/*                                                                  */
/*  Func.  : SetIndexAddress                                        */
/*  input  : bIndex(register address)                               */
/*  return : no value                                               */
/*                                                                  */
/********************************************************************/
void SetIndexAddress(UINT8 bIndex)
{
	pbi_writeb( bIndex, ADDR_INDEXREG );
}

/********************************************************************/
/*                                                                  */
/*  Func.  : TxdCommand                                             */
/*  input  : bCmd(command)                                          */
/*  return : no value                                               */
/*                                                                  */
/********************************************************************/
void TxdCommand( UINT8 bCmd)
{
	pbi_writeb( bCmd, ADDR_DATAREG );
}


/********************************************************************/
/*                                                                  */
/*  Func.  : TxdData                                                */
/*  input  : bData(transmit data)                                   */
/*  return : no value                                               */
/*                                                                  */
/********************************************************************/
void TxdData(UINT8 bData)
{
	pbi_writeb( bData, ADDR_DATAREG );
}


/********************************************************************/
/*                                                                  */
/*  Func.  : RxdData                                                */
/*  input  : no input                                               */
/*  return : bData(receive data)                                    */
/*                                                                  */
/********************************************************************/
UINT8 RxdData(void)
{
	UINT8 bData;

	bData = (char)pbi_readb(ADDR_DATAREG);
    
   	return bData;
}


/********************************************************************/
/*                                                                  */
/*  Func.  : Read_IREQ_Type                                         */
/*  input  : no data                                                */
/*  return : bData(interrupt type)                                  */
/*                                                                  */
/********************************************************************/
UINT8 Read_IREQ_Type(void)
{
	 UINT8 bData;

	pbi_writeb( REG_IREQ_TYPE, ADDR_INDEXREG );
	bData = (char)pbi_readb(ADDR_DATAREG);
   
   	return bData;
}


/*
void Write_REG_SMF_CTRL(UINT8 bCmd)
{
   nADD16 = ON;

   HPPI_ADDR = REG_SMF_CTRL;
   HPPI_DATA = bCmd;
   
   for(;;) {
      HPPI_ADDR = REG_IF_FLAG1;
      bCmd = HPPI_DATA;
      if(bCmd & 0x80) ;  // busy state
      else break;       
      }

   nADD16 = OFF;
}
*/
/********************************************************************/
/*                                                                  */
/*  Func.  : WaitAcknowledge                                        */
/*  input  : no data                                                */
/*  return : 1 = normal ready ok                                    */
/*           0 = ready failure                                      */
/*                                                                  */
/********************************************************************/
UINT8 WaitAcknowledge(void)
{
   
   UINT16 val,i;
  
   for(i=0;i<MIDI_TIMEOUT;i++) {
      // 0.1mSec waiting insert
      val = ReadStatus(REG_QDSP_MODE);
      if(val == 0x83) {
         WriteCommand(REG_READY_QS,0x4D);
         printk("Initialize OK QS6400 [0x%x]\n", val);
         return TRUE;
         }
      }
   printk("Initialize Fail QS6400 [0x%x]\n", val);
   return FALSE;
}

void PowerDown_QS64(void)
{
	UINT8 val;

	// fade out active polys
/*	// for chip bug
	WriteCommand(REG_POWER_DOWN,CMD_FADE_OUT);
	for(;;) {
		val = ReadStatus(REG_IF_FLAG3);
		if(val & 0x08) ;  // busy state
		else break;       
	}
*/
	printk("tavi_midi.o : PowerDown\n");
	// go to the power down mode
	WriteCommand(REG_POWER_DOWN,CMD_POWER_DOWN);
}

char WakeUp_QS64(void)
{

	*pPIO0_DATA = MK_REG( 5, 1 );
	udelay(10000);
	*pPIO0_DATA = MK_REG( 5, 0 );
	udelay(10000);
	*pPIO0_DATA = MK_REG( 5, 1 );
	udelay(10000);
	
	return WaitAcknowledge();
}



/********************************************************************/
/*                                                                  */
/*  Func.  : Set_WaveChannel_to_Use                                 */
/*  input  : bCh(select wave channels to use )                      */
/*           bVol(adjust wave volume to use )                       */
/*  return : no value                                               */
/*                                                                  */
/********************************************************************/
void Set_WaveChannel_to_Use(UINT8 bCh,UINT8 bVol)
{
   WriteCommand(REG_WAVE_CHAN,bCh);
   WriteCommand(REG_WAV_VOL,bVol);
}


/********************************************************************/
/*                                                                  */
/*  Func.  : Set_PWM_Drive_Clock                                    */
/*  input  : bMode(select PWM clock mode)                           */
/*  return : no value                                               */
/*                                                                  */
/********************************************************************/
void Set_PWM_Drive_Clock(UINT8 bMode)
{
   
   WriteCommand(REG_PWM_CLOCK,bMode);
}


/********************************************************************/
/*                                                                  */
/*  Func.  : Set_AudioOutMode                                       */
/*  input  : bMode(select audio out mode)                           */
/*           Bit7 : 0=PWM, 1=DAC                                    */
/*           Bit6 : 0=PWM mute off, 1=PWM mute on                   */
/*           Bit5 : 0=EarP mute off, 1=EarP mute on                 */
/*           Bit4 : 0=Stereo, 1=Mono                                */
/*  return : no value                                               */
/*                                                                  */
/*                                                                  */
/********************************************************************/
void Set_AudioOutMode(UINT8 bMode)
{
   WriteCommand(REG_OUT_MODE,bMode);
}


/********************************************************************/
/*                                                                  */
/*  Func.  : Set_OperationClock                                     */
/*  input  : bClk(select operation clock)                           */
/*           0x01 : 19.2Mhz                                         */
/*           0x09 : 19.68Mhz                                        */
/*           0x08 : 13 or 26Mhz                                     */
/*  return : no value                                               */
/*                                                                  */
/********************************************************************/
void Set_OperationClock(UINT8 bClk)
{
   WriteCommand(REG_REF_TUNE,bClk);
}


/********************************************************************/
/*                                                                  */
/*  Func.  : Adjust_MasterVolume                                    */
/*  input  : bVol(volume value)                                     */
/*  return : no value                                               */
/*                                                                  */
/********************************************************************/
void Adjust_MasterVolume(UINT8 bVol)
{
   WriteCommand(REG_VOLUME,bVol);
}


/********************************************************************/
/*                                                                  */
/*  Func.  : Adjust_TempoRate                                       */
/*  input  : bUpDn(1:increment , 0:decrement)                       */
/*  return : no value                                               */
/*                                                                  */
/********************************************************************/
void Adjust_TempoRate(UINT8 bUpDn)
{
   if(bUpDn) { // increment tempo
      WriteCommand(REG_SMF_CTRL, CMD_INC_TEMPO);
      }
   else {      // decrement tempo 
      WriteCommand(REG_SMF_CTRL, CMD_DEC_TEMPO);
      }
}


/********************************************************************/
/*                                                                  */
/*  Func.  : Txd_Parallel_MIDI                                      */
/*  input  : bMidi(1 byte midi data)                                */
/*  return : true or false                                          */
/*                                                                  */
/********************************************************************/
UINT8 Txd_Parallel_MIDI(UINT8 bMidi)
{
   UINT8 val;

   for(;;) {
      val = ReadStatus(REG_IF_FLAG1);
      if(val & 0x10) ;  // busy state
      else break;       
      }
   
   WriteCommand(REG_PIO_MIDI,bMidi);

   return TRUE;   
}


/********************************************************************/
/*                                                                  */
/*  Func.  : Command_SMF_CTRL                                       */
/*  input  : bCmd                                                   */
/*  return : true or false                                          */
/*                                                                  */
/********************************************************************/
void Command_SMF_CTRL(UINT8 bCmd)
{
   WriteCommand(REG_SMF_CTRL,bCmd);
   
   for(;;) {
      bCmd = ReadStatus(REG_IF_FLAG1);
      if(bCmd & 0x80) ;  // busy state
      else break;       
      }
}


/********************************************************************/
/*                                                                  */
/*  Func.  : Update_PTR_FIFO                                        */
/*  input  : bCnt                                                   */
/*  return : true or false                                          */
/*                                                                  */
/********************************************************************/
void Update_SMF_CTRL(UINT8 bCnt)
{
   WriteCommand(REG_PTR_FIFO,bCnt);
   
   for(;;) {
      bCnt = ReadStatus(REG_IF_FLAG1);
      if(bCnt & 0x01) ; // busy state
      else break;       
      }
}

// ************************************************************************************
// Key Tone generation by General MIDI sound
// ************************************************************************************

void KeyTonOn_usingFIFO(UINT8 bGM_Sound,UINT8 bKey,UINT8 bVolume)
{
   bGM_Sound &= 0x7F; bKey &= 0x7F; bVolume &= 0x7F;
   Command_SMF_CTRL(CMD_SMF_STOP);
   Command_SMF_CTRL(CMD_SINGLE_MIDI_ON); // using midi fifo buffer for a single midi protocol
   
   SetIndexAddress(REG_SMF_BUF);
   TxdData((UINT8)0xC0);
   TxdData(bGM_Sound);
   TxdData((UINT8)0x90);
   TxdData(bKey);
   TxdData(bVolume);
   Update_SMF_CTRL(5);

   Command_SMF_CTRL(CMD_SINGLE_MIDI_OFF);
}

void KeyTonOff(void)
{
   Command_SMF_CTRL(CMD_SMF_STOP);
   // Command_SMF_CTRL(CMD_SINGLE_MIDI_OFF);
}

/********************************************************************/
/*                                                                  */
/*  Func.  : Write_MID_to_FIFO                                      */
/*  input  :                                                        */
/*  return : no value                                               */
/*                                                                  */
/********************************************************************/
void Write_MID_to_FIFO(void)
{
   UINT8 cnt;

   SetIndexAddress(REG_SMF_BUF);
   for(cnt=0;cnt<128;cnt++) TxdData(PreFIFO[cnt]);
   WriteCommand(REG_SMF_EOP, REG_SMF_EOP);

}


/* File Operations
 */  
static int tavi_midi_open( struct inode* inode, struct file* filep )
{
	if( tavi_midi_priv.busy ) return -EBUSY;
	tavi_midi_priv.busy = 1;

	if ( WakeUp_QS64() )
	{
		//Initialize
		Set_WaveChannel_to_Use(NON_WAVE_CHAN, 0x0);
		Set_PWM_Drive_Clock(CLK_PWM_NORMAL);
		Set_AudioOutMode(PWM_MUTE_ON);		// dac & speaker off & earphone on & stereo
		Set_OperationClock(0x07);				// 24MHz
		Adjust_MasterVolume(0x7f);
	//	Adjust_TempoRate(UINT8 bUpDn);
		return 0;
	}
	else
	{
		tavi_midi_priv.busy = 0;
		return -1;
	}
}

static int tavi_midi_release( struct inode* inode, struct file* filep )
{
	tavi_midi_priv.busy = 0;
	PowerDown_QS64();
	return 0;
}

static int tavi_midi_read( struct file* filep, char* buf, size_t size, loff_t* offp )
{
	
	return sizeof( size );
}

static int tavi_midi_write( struct file* filep, char* buf, size_t size, loff_t* offp )
{
	pFIFO = (UINT8 *)buf;
	
	return sizeof( size );
}

static int tavi_midi_ioctl( struct inode* inode, struct file* filep, unsigned int cmd, unsigned long arg )
{
	struct midi_struct MidiCmd;
	struct midi_struct *pMidiCmd;
	pMidiCmd = (struct midi_struct *)arg;

	copy_from_user( &MidiCmd, pMidiCmd, sizeof(struct midi_struct) );

	switch(cmd)
	{
		case TAVI_MIDI_IOCS_CMD:
			switch( MidiCmd.cCMD )
			{
				case MIDICMD_SMF_START:
					WriteCommand(REG_SMF_CTRL, CMD_SMF_PLAY);
					break;
				case MIDICMD_SMF_STOP:
					WriteCommand(REG_SMF_CTRL, CMD_SMF_STOP);
					break;
				case MIDICMD_SMF_PAUSE:
					WriteCommand(REG_SMF_CTRL, CMD_SMF_PAUSE);
					break;
				case MIDICMD_SMF_RESUME:
					WriteCommand(REG_SMF_CTRL, CMD_SMF_PPLAY);
					break;
				case MIDICMD_SMF_TEMPOINC:
					WriteCommand(REG_SMF_CTRL, CMD_INC_TEMPO);
					break;
				case MIDICMD_SMF_TEMPODEC:
					WriteCommand(REG_SMF_CTRL, CMD_DEC_TEMPO);
					break;
				case MIDICMD_CTR_TONEGENERATER:
					if ( MidiCmd.cParam1 >= 0x80 )
					{
						KeyTonOff();
					}
					else
					{
						KeyTonOn_usingFIFO( MidiCmd.cParam1, MidiCmd.cParam2, MidiCmd.cParam3);
					}
					break;
				case MIDICMD_SMF_TEMPOBASE:
//					WriteCommand(REG_SMF_CTRL, CMD_BASE_TEMPO);
					break;
				case MIDICMD_SMF_KEYINC:
//					WriteCommand(REG_SMF_CTRL, CMD_SMF_PPLAY);
					break;
				case MIDICMD_SMF_KEYDEC:
//					WriteCommand(REG_SMF_CTRL, CMD_SMF_PPLAY);					
					break;
				case MIDICMD_SMF_KEYBASE:
//					WriteCommand(REG_SMF_CTRL, CMD_SMF_PPLAY);					
					break;

			}
			break;
		case TAVI_MIDI_IOCG_REG:
			break;
			
	}
	return 0;
}

static struct file_operations tavi_midi_ops = {
	owner: THIS_MODULE,
	open: tavi_midi_open,
	read: tavi_midi_read,
	write: tavi_midi_write,
	release: tavi_midi_release,
	ioctl: tavi_midi_ioctl
};

static struct miscdevice tavi_midi_miscdev = {
	TAVI_MIDI_MINOR,
	"tmidi",
	&tavi_midi_ops
};
static int __init tavi_midi_init( void )
{
	int 	ret;
	int	i;
	int	j;
	int	key;
	
	pbi_init( PBI_MODE_IDE_PADS );
	pbi_device_config( DEVICE_NO, PBI_CS_CONFIG_D16 );

	printk( "TAVI Hwa Sound Source QS6400 driver Ver. %d.%d, %s\n", TAVI_MIDI_VER_MAJOR, TAVI_MIDI_VER_MINOR, __DATE__ );


	*pPIO0_DATA = MK_REG( 5, 1 );			// PIO0_5 is QS_#WAKE
	*pPIO0_DIR = MK_REG( 5, 1 ) | MK_REG (0, 0);

	//init_irq();

	if ( WaitAcknowledge() )
	{
		Set_WaveChannel_to_Use(NON_WAVE_CHAN, 0x0);
		Set_PWM_Drive_Clock(CLK_PWM_NORMAL);
		Set_AudioOutMode( PWM_MUTE_ON);		// dac & speaker off & earphone on & stereo
		Set_OperationClock(0x07);	// 24MHz
		Adjust_MasterVolume(0xff);
	//	Adjust_TempoRate(UINT8 bUpDn);
		PowerDown_QS64();

		ret = misc_register( &tavi_midi_miscdev );
	}
	else
		ret = -1;
	
	return ret;
}

static void __exit tavi_midi_exit( void )
{
	PowerDown_QS64();

//	*PBI_SL_CTRL_REG = PBI_MODE_LBC_PADS;	// return ide
//	*PBI_SL_CTRL_REG = 0x0d;	// return ide

	misc_deregister( &tavi_midi_miscdev );
	printk( "TAVI Midi driver Removed\n" );

}

MODULE_AUTHOR( "<ywkim@newmedialife.com>" );
MODULE_DESCRIPTION( "TAVI Midi Driver" );

module_init( tavi_midi_init );
module_exit( tavi_midi_exit );

