#ifndef _AAC_H
#define _AAC_H

#include "rmbasic.h"
#include "aacdec.h"

class AACDecoder
{
	AAC_GlobalData	*m_AACdata;
	RMuint32		m_Initialized;	

public:
	AACDecoder ();
	~AACDecoder ();

	RMuint32		m_sampleRate;
	RMuint32		m_channelCount;

	RMuint32 Init (void);
	RMuint32 dsiInit (RMuint8 *dsi, RMuint32 dsiLength);
	RMuint32 aacInit (RMint32 AudioObjectType, RMint32 NumberOfChannels, RMint32 Sampling_Frequency_Index, RMint32 FrameSize);
	RMint32  DecodeFrame (RMuint8 *dataIn, RMuint32 InLength, RMint16 *dataOut, RMuint32 OutLength);
	RMuint32 SetProperty (RMint32 propertyID, void *buffer, RMuint32 length);
};

#endif
