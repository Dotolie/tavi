#ifndef __TAVI_SOUND_H__
#define __TAVI_SOUND_H__

#include <asm/ioctl.h>
#include "../misc_group.h"

struct output_ctrl_t 
{
	char vol_left; // 0 .. 79
	char vol_right; // 0 .. 79
	char mute;
	char range;
}; 

struct input_ctrl_t
{
	char vol_left; // 0 .. 79
	char vol_right; // 0..79
	#define LINE_SEL_LINE1		0
	#define LINE_SEL_LINE2		1
	#define LINE_SEL_LINE3		2
	char line_sel;
	char mute;
	char micon;
	char bypass;
	#define LEFT_ADC_ON				1
	#define RIGHT_ADC_ON			2
	#define BOTH_ADC_ON			3
	char adc_onoff;	
	char onoff;
	#define RIGHT_JUSTIFIED		0x00
	#define LEFT_JUSTIFIED		0x01
	#define I2S_MODE			0x02
	#define DSP_MODE			0x03
	char audio_format;
	char range;
	#define MIC_BOOST_OFF		0
	#define MIC_BOOST_13dB		1
	#define MIC_BOOST_20dB		2
	#define MIC_BOOST_29dB		3
	char boost;
};

union sndreg_t {
	short regdata;
	struct {
		char data;
		char reg;
	} __attribute__ ((packed)) r;
};

struct snd_dev_t
{
	struct output_ctrl_t lineout1;
	struct output_ctrl_t lineout2;
	struct output_ctrl_t mono;
	struct input_ctrl_t  linein;
	char micon;
	#define EQ_NORMAL			0
	#define EQ_ROCK				1
	#define EQ_POP				2
	#define EQ_JAZZ				3
	#define EQ_CLASSIC			4
	#define EQ_DANCE			5
	#define EQ_HEAVY			6
	#define EQ_DISCO			7
	#define EQ_SOFT				8
	#define EQ_3DSTEREO			9
	#define EQ_HALL				10
	int preset_eq;
	char threed;
	char treble;
	char bass;
	char speaker;
	char dac_mute;
};

#define LINEIN_VOL_LEFT			0x00	// reg0
#define LINEIN_VOL_RIGHT		0x01
#define LINEOUT1_VOL_LEFT		0x02	// reg2
#define LINEOUT1_VOL_RIGHT		0x03	// reg3
#define RESERVED1				0x04
#define ADC_DAC_CTRL			0x05
#define RESERVED2				0x06
#define AUDIO_INF				0x07
#define SAMPLE_RATE				0x08
#define RESERVED3				0x09
#define LEFT_DAC_VOL			0x0a
#define RIGHT_DAC_VOL			0x0b
#define BASS_CTRL				0x0c
#define TREBLE_CTRL				0x0d
#define RESERVED4				0x0e
#define RESET					0x0f
#define S3D_CTRL				0x10
#define ALC1						0x11
#define ALC2						0x12
#define ALC3						0x13
#define NOISE_GATE				0x14
#define LEFT_ADC_VOL			0x15
#define RIGHT_ADC_VOL			0x16
#define ADD_CTRL1				0x17
#define ADD_CTRL2				0x18
#define POWER_MGT1				0x19
#define POWER_MGT2				0x1a
#define ADD_CTRL3				0x1b
#define RESERVED6				0x1c
#define RESERVED7				0x1d
#define RESERVED8				0x1e
#define ADC_INPUT_MODE			0x1f
#define ADCL_SIG_PATH			0x20
#define ADCR_SIG_PATH			0x21
#define LEFTOUT_MIX1			0x22
#define LEFTOUT_MIX2			0x23
#define RIGHTOUT_MIX1			0x24
#define RIGHTOUT_MIX2			0x25
#define MONOOUT_MIX1			0x26
#define MONOOUT_MIX2			0x27
#define LINEOUT2_VOL_LEFT		0x28
#define LINEOUT2_VOL_RIGHT		0x29
#define MONOOUT_VOL			0x2a

#define TAVI_SND_MAGIC					's'
#define TAVI_IOCG_SND_REGISTERS			_IOR( TAVI_SND_MAGIC, 0, struct snd_dev_t )
#define TAVI_IOCS_SND_LINEOUT1_VOL		_IOW( TAVI_SND_MAGIC, 1, struct snd_dev_t )
#define TAVI_IOCS_SND_LINEOUT2_VOL		_IOW( TAVI_SND_MAGIC, 2, struct snd_dev_t )
#define TAVI_IOCS_SND_LINEOUT1_MUTE		_IOW( TAVI_SND_MAGIC, 3, struct snd_dev_t )
#define TAVI_IOCS_SND_LINEOUT2_MUTE		_IOW( TAVI_SND_MAGIC, 4, struct snd_dev_t )
#define TAVI_IOCS_SND_LINEIN_VOL			_IOW( TAVI_SND_MAGIC, 5, struct snd_dev_t )
#define TAVI_IOCS_SND_MIC_ON				_IOW( TAVI_SND_MAGIC, 6, struct snd_dev_t )
#define TAVI_IOCS_SND_MONOOUT_VOL		_IOW( TAVI_SND_MAGIC, 7, struct snd_dev_t )
#define TAVI_IOCS_SND_LINEIN_ON			_IOW( TAVI_SND_MAGIC, 8, struct snd_dev_t )
#define TAVI_IOCS_SND_INPUT_BYPASS		_IOW( TAVI_SND_MAGIC, 9, struct snd_dev_t )
#define TAVI_IOCS_SND_AUDIO_FORMAT		_IOW( TAVI_SND_MAGIC, 10, struct snd_dev_t )
#define TAVI_IOCS_SND_EQUALIZER			_IOW( TAVI_SND_MAGIC, 11, struct snd_dev_t )
#define TAVI_IOCS_SND_SPEAKERAMP_ONOFF	_IOW( TAVI_SND_MAGIC, 12, struct snd_dev_t )
#define TAVI_IOCS_SND_3D					_IOW( TAVI_SND_MAGIC, 13, struct snd_dev_t )
#define TAVI_IOCS_SND_TREBLE				_IOW( TAVI_SND_MAGIC, 14, struct snd_dev_t )
#define TAVI_IOCS_SND_BASS					_IOW( TAVI_SND_MAGIC, 15, struct snd_dev_t )
#define TAVI_IOCS_SND_DAC_MUTE			_IOW( TAVI_SND_MAGIC, 16, struct snd_dev_t )
#define TAVI_IOCS_SND_POWER_OFF			_IOW( TAVI_SND_MAGIC, 17, struct snd_dev_t )
#define TAVI_IOCS_SND_INPUT_SEL			_IOW( TAVI_SND_MAGIC, 18, struct snd_dev_t )
#define TAVI_IOCS_SND_LINEIN_MUTE			_IOW( TAVI_SND_MAGIC, 19, struct snd_dev_t )
#define TAVI_IOCS_SND_ADC_ON				_IOW( TAVI_SND_MAGIC, 20, struct snd_dev_t )
#define TAVI_IOCG_SND_REGISTER			_IOW( TAVI_SND_MAGIC, 21, union sndreg_t )
#define TAVI_IOCS_SND_REGISTER				_IOW( TAVI_SND_MAGIC, 22, union sndreg_t )
#define TAVI_IOCS_SND_MICBOOST			_IOW( TAVI_SND_MAGIC, 23, struct snd_dev_t )

#endif /* __TAVI_SOUND_H__ */
