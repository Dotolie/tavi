#include "aac.h"

#if 0
#include <stdio.h>
static void debug_break (void)
{
}
#define ASSERT(exp)					((void)((exp)?1:(printf ("***ASSERT failed: line %d, file %s\n", __LINE__,__FILE__), debug_break(), 0)))
#define DEBUGMSG(cond,printf_exp)	((void)((cond)?(printf printf_exp),1:0))
#else
#define ASSERT(exp)
#define DEBUGMSG(cond,printf_exp)
#endif

AACDecoder::AACDecoder ()
{
	m_Initialized = 0;
	m_AACdata = 0;
}

AACDecoder::~AACDecoder ()
{
	if (m_Initialized)
		aac_decoder_free (m_AACdata);
	if (m_AACdata)
		aac_dealloc_GlobalData (m_AACdata);
}

RMuint32 AACDecoder::Init (void)
{
	ASSERT (m_AACdata == 0);
	m_AACdata = (AAC_GlobalData*) aac_alloc_GlobalData ();
	ASSERT (m_AACdata);
	if (m_AACdata)
		aac_set_defaults (m_AACdata);
	return (RMuint32)m_AACdata;
}

RMint32 AACDecoder::DecodeFrame (RMuint8 *dataIn, RMuint32 InLength, RMint16 *dataOut, RMuint32 OutLength)
{
	ASSERT (m_Initialized);
	ASSERT (m_AACdata);

	int bits; 
	aacError err;
	err = aac_reset_input_databuffer (m_AACdata, dataIn, InLength);
	if (AAC_FAIL(err))
	{
		return (RMint32)err;
	}
	bits = aac_decode_raw_data_block (dataOut, OutLength, m_AACdata);
	return (RMint32)bits;
}

RMuint32 AACDecoder::dsiInit (RMuint8 *dsi, RMuint32 dsiLength)
{
	ASSERT (m_AACdata);

	// what we need for decoder initialization
	RMuint32 frameSize;
	RMuint32 channelCount;
	RMuint32 audioObjectType;
	RMuint32 samplingFreqIdx;

	// some helper variables
	RMuint32 sampleRate;
	RMuint8 frameLengthFlag;
	RMuint8 dependsOnCoreCoder;
	RMuint8 extensionFlag;
	RMuint16 coreDecoderDelay;

	if (AAC_FAIL (aac_reset_input_databuffer (m_AACdata, dsi, dsiLength)))
		return (RMuint32)AAC_ERROR;

	audioObjectType = (RMint32)aac_getbits (m_AACdata, 5);
	samplingFreqIdx = (RMint32)aac_getbits (m_AACdata, 4);

	if (samplingFreqIdx == 0xf) 
	{
		sampleRate = (RMuint32)aac_getbits (m_AACdata, 24);
		if (sampleRate > 92017) 
		{ 
			samplingFreqIdx = 0x0; //96000
		} 
		else if (sampleRate > 75132) 
		{ 
			samplingFreqIdx = 0x1; //88200
		}
		else if (sampleRate > 55426) 
		{ 
			samplingFreqIdx = 0x2; //64000
		}
		else if (sampleRate > 46009) 
		{ 
			samplingFreqIdx = 0x3; //48000
		}
		else if (sampleRate > 37566) 
		{ 
			samplingFreqIdx = 0x4; //44100
		}
		else if (sampleRate > 27713) 
		{ 
			samplingFreqIdx = 0x5; //32000
		}
		else if (sampleRate > 23004) 
		{ 
			samplingFreqIdx = 0x6; //24000
		}
		else if (sampleRate > 18783) 
		{ 
			samplingFreqIdx = 0x7; //22050
		}
		else if (sampleRate > 13856) 
		{ 
			samplingFreqIdx = 0x8; //16000
		}
		else if (sampleRate > 11502) 
		{ 
			samplingFreqIdx = 0x9; //12000
		}
		else if (sampleRate >  9391) 
		{ 
			samplingFreqIdx = 0xa; //11025
		}
		else 
		{
			samplingFreqIdx = 0xb; // 8000
		}
	}
	else 
	{
		switch (samplingFreqIdx) 
		{
		case 0x0 :		//  96000
			sampleRate = 96000;
			break;
		case 0x1 : 		//  88200
			sampleRate = 88200;
			break;
		case 0x2 : 		//  64000
			sampleRate = 64000;
			break;
		case 0x3 : 		// 48000
			sampleRate = 48000;
			break;
		case 0x4 : 		//  44100										
			sampleRate = 44100;
			break;
		case 0x5 : 		//  32000
			sampleRate = 32000;
			break;
		case 0x6 : 		//  24000
			sampleRate = 24000;
			break;
		case 0x7 : 		//  22050
			sampleRate = 22050;
			break;
		case 0x8 : 		//  16000
			sampleRate = 16000;
			break;
		case 0x9 : 		//  12000
			sampleRate = 12000;
			break;
		case 0xa : 		//  11025
			sampleRate = 11025;
			break;
		case 0xb : 		//  8000
			sampleRate = 8000;
			break;
		default :		//  44100
			sampleRate = 44100;
			break;
		}
	}

	channelCount = (RMuint32)aac_getbits (m_AACdata, 4);
	if (channelCount == 7)
		channelCount = 8;
	
	if ((audioObjectType == 1) || (audioObjectType == 2) ||
	    (audioObjectType == 3) || (audioObjectType == 4) ||
	    (audioObjectType == 6) || (audioObjectType == 7)) 
	{
		// GASpecificConfig - 14496-3 Subpart 4, page 13
		frameLengthFlag = (RMuint8)aac_getbits (m_AACdata, 1);
		dependsOnCoreCoder = (RMuint8)aac_getbits (m_AACdata, 1);
		if(dependsOnCoreCoder)
			coreDecoderDelay = (RMuint16)aac_getbits (m_AACdata, 14);
		extensionFlag = (RMuint8)aac_getbits (m_AACdata, 1);
		
		// number of channels is in program config element
		if (channelCount == 0) 
		{
			//really not the best solution...
			if (AAC_FAIL(aac_get_prog_config (m_AACdata)))
				return (RMuint32)AAC_ERROR;

			if (AAC_FAIL(aac_get_property (NUMBER_OF_SOUND_CHANNELS, &channelCount, sizeof(channelCount), m_AACdata)))
				return (RMuint32)AAC_ERROR;
		}
	
		if ((audioObjectType == 1) || (audioObjectType == 2) || (audioObjectType == 3) || (audioObjectType == 4)) 
		{
			m_Initialized = 1;
			frameSize = (frameLengthFlag) ? 960 : 1024;
			m_channelCount = channelCount;
			m_sampleRate = sampleRate;
			return aac_init (m_AACdata, audioObjectType-1, channelCount, samplingFreqIdx, frameSize);
		}
		else
			return (RMuint32)AAC_ERROR; //not yet implemented
	}
	else 
		return (RMuint32)AAC_ERROR;

	return 0;
}

RMuint32 AACDecoder::aacInit (RMint32 AudioObjectType, RMint32 NumberOfChannels, RMint32 Sampling_Frequency_Index, RMint32 FrameSize)
{
	ASSERT (m_AACdata);

	aacError err;
	m_Initialized = 1;
	err = aac_init (m_AACdata, AudioObjectType - 1, NumberOfChannels, Sampling_Frequency_Index, FrameSize);
	return (RMuint32)err;
}

RMuint32 AACDecoder::SetProperty (RMint32 propertyID, void *buffer, RMuint32 length)
{
	aacError err;
	err = aac_set_property (propertyID, buffer, length, m_AACdata);
	return (RMuint32)err;
}
