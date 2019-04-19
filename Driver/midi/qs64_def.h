/********************************************************************/
/*                                                                  */
/*   Prj. : QS6400 Application for Demoboard                        */
/*   File : Q64_DEF.H                                               */
/*   Programed by Jeong K. H.   2003 / 06                           */
/*   [ CPU : 8051 / CLOCK : 24Mhz / ROM : 64Kbyte / RAM : 64KByte ] */
/*                                                                  */
/********************************************************************/

// ====================================================================================
//  type define range
// ====================================================================================

// define declare type
/* ************************************* */
/* SINT8  : signed 8bit type             */
/* SINT16 : signed 16bit type            */
/* SINT32 : signed 32bit type            */
/* UINT8  : unsigned 8bit type           */
/* UINT16 : unsigned 16bit type          */
/* UINT32 : unsigned 32bit type          */
/* ************************************* */
typedef signed char    SINT8;       
typedef signed short   SINT16;
typedef signed long    SINT32;
typedef unsigned char  UINT8; 
typedef unsigned short UINT16;
typedef unsigned long  UINT32;


// define _midi_ctrl_flags union data
/* ************************************************************************ */
/* .bits.play         : on = play state, off = play stop state              */
/* .bits.pause        : on = pause state, off = pause release state         */
/* .bits.make_fifo    : on = prepared fifo data, off = unprepared fifo data */
/* .bits.request      : on = request next data packet                       */
/* .bits.end_of_track : on = marking end of track                           */
/* ************************************************************************ */
typedef union _midi_ctrl_flags    // control flags for midi data packet 
{
   struct _midi_flags {
      int play           : 1;
      int pause          : 1;
      int rev1           : 1;   // reserved bit
      int make_fifo      : 1;
      int request        : 1;
      int rev2           : 1;   // reserved bit
      int ing_midi_fifo  : 1;   // reserved bit
      int end_of_track   : 1;
      } bits;
   int value;
} MIDI_CTRL_FLAGS;

// define _wave_ctrl_flags union data
/* ************************************************************************ */
/* .bits.ch1_play    : on = play state, off = play stop state               */
/* .bits.ch2_play    : on = play state, off = play stop state               */
/* ************************************************************************ */
typedef union _wave_ctrl_flags    // control flags for wave data packet 
{
   struct _wave_flags{
      int ch1_play       : 1;
      int ch2_play       : 1;
      int ch1_sync_play  : 1;
      int ch2_sync_play  : 1;
      int ch1_req        : 1;
      int ch2_req        : 1;
      int ch1_sync_stop  : 1;
      int ch2_sync_stop  : 1;
      } bits;
   int value;
} WAVE_CTRL_FLAGS;



// define _control_xfer struct data
typedef struct _control_xfer
{
   UINT8  bWaveAssign; // specify using wave channel (only 0,1,2)
   UINT32 dwPtrMIDI;   // specify offset address of the midi song
   UINT32 dwPtrWAV1;   // specify offset address of the wave song for wave channel 1
   UINT32 dwPtrWAV2;   // specify offset address of the wave song for wave channel 2
   UINT32 dwSizeMIDI;  // specify length of the midi song
   UINT32 dwSizeWAV1;  // specify length the wave song for wave channel 1
   UINT32 dwSizeWAV2;  // specify length the wave song for wave channel 2
   UINT8  * pMIDI;     // specify song pointer of the midi
   UINT8  * pWAV1;     // specify song pointer of the wave channel 1
   UINT8  * pWAV2;     // specify song pointer of the wave channel 
} CONTROL_XFER;


// define _gate_info struct data
typedef struct _gate_info
{
   UINT16 wTime;   // specify remaining gate off time for any poly
   UINT8  bCh;     // specify midi channel for gate off
   UINT8  bKey;    // specify midi note number for gate off
} GATE_INFO;






// ====================================================================================
//  constant define range
// ====================================================================================
#define ON                1
#define OFF               0
#define TRUE              1
#define FALSE             0

#define MAX_TEMP_SMF0_BUF 10
#define MAX_GATE          64 
#define MAX_TRACK         32

#define SMF_TYPE          1   // identify standard midi format 

#define GEMF_TYPE         3   // identify hwa midi bell format
#define SMAF_TYPE         4   // identify yamaha bell format
#define IMELODY_TYPE      5   // identify imelody bell format
#define RTTL_TYPE         6   // identify rttl bell format
#define CMX_TYPE          7   // identify compact midi bell format





// ====================================================================================
//  define qs6400 register and command 
//   [X:0x??] =>  X : W = write register, R = read register
//               ?? : register index value
// ====================================================================================

/* ************************************************************************* */
/* [W:0x03] midi file control register                                       */
/* 0x11 : smf play start                                                     */
/* 0x12 : smf play stop                                                      */
/* 0x13 : smf play pause                                                     */
/* 0x14 : smf pause release                                                  */
/* 0x18 : smf repeat play start (reserved)                                   */
/* 0x21 : smf tempo up                                                       */
/* 0x22 : smf tempo down                                                     */
/* 0x28 : smf key up (reserved)                                              */
/* 0x29 : smf key down (reserved)                                            */
/* 0x31 : internal fg_single_midi_mode active on and midi pipe initial       */
/* 0x32 : internal fg_single_midi_mode active off                            */
/* 0x80 : direct tempo value control 0 ~ 50 (reserved)                       */
/* ************************************************************************* */
#define REG_SMF_CTRL   0x03
 
#define CMD_SMF_PLAY   0x11
#define CMD_SMF_STOP   0x12
#define CMD_SMF_PAUSE  0x13 
#define CMD_SMF_PPLAY  0x14
#define CMD_INC_TEMPO  0x21 // increment tempo rate
#define CMD_DEC_TEMPO  0x22 // decrement tempo rate
#define CMD_SINGLE_MIDI_ON  0x31 // FG_single_midi_mode Active On 
#define CMD_SINGLE_MIDI_OFF 0x32 // FG_single_midi_mode Active Off
               

   
/* ************************************************************************* */
/* [W:0x04] master volume control register                                   */
/*       => adjust volume range : 0x00 ~ 0xFF                                */
/* ************************************************************************* */
#define REG_VOLUME     0x04 

/* ************************************************************************* */
/* [W:0x05] end of packet register for midi file                             */
/*       => write any value after writing a packet(128byte)                  */
/* ************************************************************************* */
#define REG_SMF_EOP    0x05 

/* ************************************************************************* */
/* [W:0x06] parallel midi data register                                      */
/*       => write any midi data                                              */
/*          reference function : UINT8 Txd_Parallel_MIDI(UINT8 bMidi)        */
/* ************************************************************************* */
#define REG_PIO_MIDI   0x06

/* ************************************************************************* */
/* [W:0x07] wave channel number register                                     */
/*       => write wave channel number to play : 0x00,0x01,0x02               */
/* ************************************************************************* */
#define REG_WAVE_CHAN  0x07 

#define NON_WAVE_CHAN  0x00 // not using wave channel
#define ONE_WAVE_CHAN  0x01 // only using a wave channel
#define TWO_WAVE_CHAN  0x02 // using two wave channels



/* ************************************************************************* */
/* [W:0x08] wave file control register for wave channel 1                    */
/* 0x11 : wave 1 play start                                                  */
/* 0x12 : wave 1 play stop                                                   */
/* ************************************************************************* */
#define REG_WAV1_CTRL  0x08 

#define CMD_WAV1_PLAY  0x11
#define CMD_WAV1_STOP  0x12

/* ************************************************************************* */
/* [W:0x09] wave data volume control register                                */
/*       => adjust volume range : 0x00 ~ 0xFF                                */
/* ************************************************************************* */
#define REG_WAV_VOL    0x09 


/* ************************************************************************* */
/* [W:0x0A] pointer register of the midi fifo for single midi mode           */
/* ************************************************************************* */
#define REG_PTR_FIFO   0x0A 

/* ************************************************************************* */
/* [W:0x0B] wave channel 1 samplerate control register                       */
/*       => write wave file's 4th a byte data                                */
/* ************************************************************************* */
#define REG_WAV1_SR    0x0B 

/* ************************************************************************* */
/* [W:0x0C] wave file control register for wave channel 2                    */
/* 0x11 : wave 2 play start                                                  */
/* 0x12 : wave 2 play stop                                                   */
/* ************************************************************************* */
#define REG_WAV2_CTRL  0x0C
 
#define CMD_WAV2_PLAY  0x11
#define CMD_WAV2_STOP  0x12

/* ************************************************************************* */
/* [W:0x0F] wave channel 2 samplerate control register                       */
/*       => write wave file's 4th a byte data                                */
/* ************************************************************************* */
#define REG_WAV2_SR    0x0F 

/* ************************************************************************* */
/* [W:0x11] midi fifo register                                               */
/* ************************************************************************* */
#define REG_SMF_BUF    0x11

/* ************************************************************************* */
/* [W:0x12] wave 1 fifo register                                             */
/* ************************************************************************* */
#define REG_WAV1_BUF   0x12

/* ************************************************************************* */
/* [W:0x13] wave 2 fifo register                                             */
/* ************************************************************************* */
#define REG_WAV2_BUF   0x13

/* ************************************************************************* */
/* [W:0x14] ready sign transfer register                                     */
/*       => write only 0x4D                                                  */
/* ************************************************************************* */
#define REG_READY_QS   0x14

#define CMD_READY_OK   0x4D 

/* ************************************************************************* */
/* [W:0x15] operation clock select register                                  */
/* 0x01 : 19.2Mhz operation clock                                            */
/* 0x08 : 13Mhz or 26Mhz operation clock                                     */
/* 0x09 : 19.68Mhz operation clock                                           */
/* ************************************************************************* */
#define REG_REF_TUNE   0x15 

#define CLK_CDMA_1     0x01
#define CLK_CDMA_2     0x09
#define CLK_GSM        0x08

/* ************************************************************************* */
/* [W:0x16] audio output mode select register                                */
/* bit7 : 0 = PWM mode , 1 = DAC mode                                        */
/* bit6 : 0 = speaker output on(mute off) , 1 = speaker output off(mute on)  */
/* bit5 : 0 = earphone output on(mute off) , 1 = earphone output off(mute on)*/
/* bit4 : 0 = stereo output mode , 1 = mono output mode                      */
/* bit3 : earphone output mode select(reserved)                              */
/* bit2 : earphone right channel mute(reserved)                              */
/* bit1 : earphone left channel mute(reserved)                               */
/* bit0 : reserved                                                           */
/* ************************************************************************* */
#define REG_OUT_MODE   0x16

#define DAC_OUT_MODE   0x80
#define PWM_MUTE_ON    0x40
// #define EARP_MUTE_ON   0x20
#define MONO_BELL_MODE 0x10
#define EARP_L_MUTE    0x02
#define EARP_R_MUTE    0x04

/* ************************************************************************* */
/* [W:0x17] powerdown mode select register                                   */
/* ************************************************************************* */
#define REG_POWER_DOWN 0x17 

#define CMD_POWER_DOWN 0xAA
#define CMD_FADE_OUT   0x55

/* ************************************************************************* */
/* [W:0x18] earphone volume control register(reserved)                       */
/* bit7 : 0 = right , 1 = left                                               */
/* bit6~bit0 : volume level                                                  */
/* ************************************************************************* */
#define REG_EARP_VOL   0x18 // not used(reserved)

/* ************************************************************************* */
/* [W:0x19] pwm drive clock mode select                                      */
/* 0x00 : normal operation clock                                             */
/* 0x01 : double operation clock                                             */
/* ************************************************************************* */
#define REG_PWM_CLOCK  0x19 

#define CLK_PWM_NORMAL 0x00
#define CLK_PWM_DOUBLE 0x01

/* ************************************************************************* */
/* [R:0x1A] data request type register                                       */
/* bit7 : weve 2 start or stop request                                       */
/* bit6 : wave 1 start or stop request                                       */
/* bit5 : end midi data request                                              */
/* bit4 : end wave 1 data request                                            */
/* bit3 : end wave 2 data request                                            */
/* bit2 : next midi data request                                             */
/* bit1 : next wave 1 data request                                           */
/* bit0 : next wave 2 data request                                           */
/* ************************************************************************* */
#define REG_IREQ_TYPE  0x1A // check interrupt type


/* ************************************************************************* */
/* [R:0x1C] qs6400 internal status register                                  */
/* bit7 : 0 = abnormal wakeup state , 1 = normal wakeup state                */
/* bit6 : 0 = normal wakeup state , 1 = abnoraml wakeup state                */
/* bit5 : 0 = normal mode , 1 = using midi fifo for single midi operation    */
/* bit4 : reserved                                                           */
/* bit3 : 0 = midi stop state , 1 = midi play state                          */
/* bit2 : 0 = normal mode , 1 = success fade-out by power down               */
/* bit1 : 0 = wave 1 play state , 1 = wave 1 stop state                      */
/* bit0 : 0 = wave 2 play state , 1 = wave 2 stop state                      */
/* ************************************************************************* */
#define REG_QDSP_MODE  0x1C 


/* ************************************************************************* */
/* [R:0x1D] index return register                                            */
/* ************************************************************************* */
#define REG_WRITE_BACK 0x1D // write back register



// 031102 : add internal flag register
#define REG_IF_FLAG1   0x22
#define REG_IF_FLAG2   0x23
#define REG_IF_FLAG3   0x24
#define REG_IF_FLAG4   0x25


// 030812 : add new control register
#define REG_EARP_L_VOL 0x28 // not used
#define REG_EARP_R_VOL 0x29 // not used
#define RD_EARP_L_VOL  0x20 // not used
#define RD_EARP_R_VOL  0x21 // not used

