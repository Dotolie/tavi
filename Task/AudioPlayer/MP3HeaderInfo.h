//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : MP3 Header Information class declaration
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/AudioPlayer/MP3HeaderInfo.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: MP3HeaderInfo.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.6  2005/12/19 08:08:42  zstein
// *** empty log message ***
//
// Revision 1.5  2005/11/08 01:20:34  zstein
// *** empty log message ***
//
//******************************************************************************
#ifndef __MP3_HEADER_INFORMATION_H__
#define __MP3_HEADER_INFORMATION_H__

class MP3HdrInfo
{
private:
protected:
	int DecodeHeader( const char* header );
	int CalcFrameSize( void );
public:
	enum {
		MPEG_VERSION_2_5,
		MPEG_VERSION_RESERVED,
		MPEG_VERSION_2,
		MPEG_VERSION_1
	};
	int m_nVersion;
	enum {
		MPEG_LAYER_RESERVED,
		MPEG_LAYER_3,
		MPEG_LAYER_2,
		MPEG_LAYER_1
	};
	int m_nLayer;
	bool m_fProtected;
	int m_nBitrate;
	int m_nSamplingrate;
	int m_nPadding;
	bool m_fPrivate;
	enum {
		MPEG_CH_STEREO,
		MPEG_CH_JOINTSTEREO,
		MPEG_CH_DUAL,
		MPEG_CH_SINGLE
	};
	int m_nChannel;
	int m_nModeExt;
	bool m_fCopyright;
	bool m_fOriginal;
	enum {
		EMP_NONE,
		EMP_50_15MS,
		EMP_RESERVED,
		EMP_CCITJ17
	};	
	int m_nEmphasis;
	int m_nFrameCount;
	int m_nFrameSize;
	int m_nTotalTime;
	bool m_fVBR;

	MP3HdrInfo( void );
	~MP3HdrInfo( void );

	bool Decode( const char* szPath, int unicode=1 );

};

#endif /* __MP3_HEADER_INFORMATION_H__ */

