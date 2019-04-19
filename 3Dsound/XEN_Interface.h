/*********************************************************************************/
/* date : 2005/06/01    By : Emersys Ltd.   Koo Jung Mo                          */
/* 05-06-01 : EMX Brand ���� --> XEN (Xound Exciting & Natural)                  */
/*********************************************************************************/
#ifndef __XEN_INTERFACE_H__
#define __XEN_INTERFACE_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef short          Int16;
typedef unsigned short Uint16;
typedef long           Int32;
typedef unsigned long  Uint32;

typedef struct {
	Int32  AUDIO_ESA_SPECTRUM[10]; 	// 0 ~ 16 ( 17 Level : 0 - ǥ������ ����, �� ǥ���� threshold ������ ��, 1~16 ǥ���� �� �ܰ� )
	Uint32 AUDIO_ESA_BAND;	 		// The Number of Spectrum Bar (Max: 10)
	Uint32 AUDIO_ESA_FLAG;			// 0: Off, 1: On
	Uint32 DISPLAY_STEP;
} _XEN_ESA;

typedef struct _AXENPROP {      // Instance�� �ǹ��ϴ� ����ü
	int PROC_MODE;	// 3D, EQ, bypass �� ȿ���� ��带 �ǹ�
	int _3DPARAM0;	// 3D ȿ���� Ȯ�� ������ �ǹ�
	int _3DPARAM1;	// 3D ȿ���� ���� ���� ������ �ǹ�
	int _3DPARAM2;	// 3D ȿ���� ���� ���� ������ �ǹ�
	int _EQPARAM0;	// EQ ȿ���� Band_0 ������ �����ϴ� ������ �ǹ� (Band_0 -> 125 Hz)
	int _EQPARAM1;	// EQ ȿ���� Band_1 ������ �����ϴ� ������ �ǹ� (Band_1 -> 250 Hz)
	int _EQPARAM2;	// EQ ȿ���� Band_2 ������ �����ϴ� ������ �ǹ� (Band_2 -> 1 kHz)
	int _EQPARAM3;	// EQ ȿ���� Band_3 ������ �����ϴ� ������ �ǹ� (Band_3 -> 4 kHz)
	int _EQPARAM4;	// EQ ȿ���� Band_4 ������ �����ϴ� ������ �ǹ� (Band_4 -> 8 kHz)
} AEFFECT_EMERSYS_INSTANCE;

#define XEN_MODE_COUNT					32

#define XEN_EXTX_PARAM_VALUE_RANGE		1
#define XEN_MEX_PARAM_VALUE_RANGE		7
#define XEN_REX_WIDE_PARAM_VALUE_RANGE	7
#define XEN_REX_LIVE_PARAM_VALUE_RANGE	7

#define EQ_BAND_COUNT					5
#define EQ_PARAM_VALUE_RANGE			19

///////////////// AEffectSetParamValue�� param_type�� ����ϴ� �� /////////////////
// ��� ����� XEN_PTYPE_PROC_MODE
// 3D�� ���� �����ϰ� ���� ��� XEN_PTYPE_SURR~XEN_PTYPE_TREB
// EQ�� ���� �����ϰ� ���� ��� EQ_PTYPE_BAND_0~EQ_PTYPE_BAND_4
typedef enum {
	XEN_PTYPE_PROC_MODE = 0,	// ���� 3D ��� �Ǵ� EQ ���� �����ϰ� ���� ���
	XEN_PTYPE_SURR,		// 3D ȿ���� Ȯ�� ������ �ǹ��ϴ� �Ķ���͸� �����ϰ� ���� ���
	XEN_PTYPE_BASS,		// 3D ȿ���� ���� ���� ������ �ǹ��ϴ� �Ķ���͸� �����ϰ� ���� ���
	XEN_PTYPE_TREB,		// 3D ȿ���� ���� ���� ������ �ǹ��ϴ� �Ķ���͸� �����ϰ� ���� ��� (MEX only)
	EQ_PTYPE_BAND_0,	// EQ ȿ���� Band_0 ������ ���� �Ǵ� �����ϰ� ���� ��� (Band_0 -> 125 Hz)
	EQ_PTYPE_BAND_1,	// EQ ȿ���� Band_1 ������ ���� �Ǵ� �����ϰ� ���� ��� (Band_1 -> 250 Hz)
	EQ_PTYPE_BAND_2,	// EQ ȿ���� Band_2 ������ ���� �Ǵ� �����ϰ� ���� ��� (Band_2 -> 1 kHz)
	EQ_PTYPE_BAND_3,	// EQ ȿ���� Band_3 ������ ���� �Ǵ� �����ϰ� ���� ��� (Band_3 -> 4 kHz)
	EQ_PTYPE_BAND_4		// EQ ȿ���� Band_4 ������ ���� �Ǵ� �����ϰ� ���� ��� (Band_4 -> 8 kHz)
} ePARAM_TYPE;


///////////////// AEffectSetParamValue�� param_value�� ����ϴ� �� /////////////////

// AEffectSetParamValue�� param_type�� XEN_PTYPE_PROC_MODE �� ���Ǵ� ��� ����ϴ� ��
typedef enum {
	XEN_MODE_SP_EXTX = 0,
	XEN_MODE_HP_MEX,
	XEN_MODE_HP_REX_MILD,
	XEN_MODE_HP_REX_WIDE,
	XEN_MODE_HP_REX_LIVE,
	XEN_MODE_HP_EQ,
	XEN_MODE_NONE
} ePROC_MODE;

// AEffectSetParamValue�� param_type�� XEN_PTYPE_SURR~EQ_PTYPE_BAND_4 �� ���Ǵ� ��� ����ϴ� ��
// 3D�� ��� XEN_VALUE_0~XEN_VALUE_6
// EQ�� ��� EQ_VALUE_N9DB~EQ_VALUE_P9DB ��� ����
typedef enum {
	XEN_VALUE_0 = 0,
	XEN_VALUE_1,
	XEN_VALUE_2,
	XEN_VALUE_3,
	XEN_VALUE_4,
	XEN_VALUE_5,
	XEN_VALUE_6,
	EQ_VALUE_N9DB = 0,
	EQ_VALUE_N8DB,
	EQ_VALUE_N7DB,
	EQ_VALUE_N6DB,
	EQ_VALUE_N5DB,
	EQ_VALUE_N4DB,
	EQ_VALUE_N3DB,
	EQ_VALUE_N2DB,
	EQ_VALUE_N1DB,
	EQ_VALUE_0DB,
	EQ_VALUE_P1DB,
	EQ_VALUE_P2DB,
	EQ_VALUE_P3DB,
	EQ_VALUE_P4DB,
	EQ_VALUE_P5DB,
	EQ_VALUE_P6DB,
	EQ_VALUE_P7DB,
	EQ_VALUE_P8DB,
	EQ_VALUE_P9DB
} ePARAM_VALUE;


///////////////// AEffectSetPreset�� preset�� ����ϴ� �� /////////////////

// 3D ������
typedef enum {
	XEN_eXTX_PRESET = 0,
	XEN_MEX_BASS_PRESET,
	XEN_MEX_LITE_PRESET,
	XEN_MEX_PRO_PRESET,
	XEN_REX_MILD_PRESET,
	XEN_REX_WIDE_PRESET,
	XEN_REX_LIVE_PRESET
} eXEN_PRESET;

// EQ ������
typedef enum {
	SEQ_FLAT = 7,
	SEQ_VOICE,
	SEQ_POWERBASS,
	SEQ_BRIGHT,
	SEQ_DARKWARM,
	SEQ_CLASSIC,
	SEQ_HARDROCK,
	SEQ_SOFTROCK,
	SEQ_JAZZ,
	SEQ_DANCE,
	SEQ_SLOW,
	SEQ_POP,
	SEQ_BLUES,
	SEQ_INSTRUMENTAL,
	SEQ_FUNK,
	SEQ_ACOUSTIC,
	SEQ_ELECTRONIC,
	SEQ_HIPHOP,
	SEQ_LATIN,
	SEQ_LECTURE,
	XEN_SEQ_PRESET_COUNTS
} eEQ_PRESET;

// ��� �������� ���ø� ����Ʈ�� �����ϴ� �Լ�
// ȿ���� ���Ǵ� ��κ��� coefficient�� ��� ���ø� ����Ʈ�� �������� ������ �����Ƿ�
// ���ø� ����Ʈ�� ����� �� �� �Լ��� �ݵ�� ȣ�����־�� ��
// nFS�� ���� ���� ���ǰ� ���� �̷������ �ʾ� �� �Լ��� ���� �������� ����
// nFS : ���ø� ����Ʈ�� �ǹ��ϴ� ��. ���� ���ǵ��� �ʾ���
// ret : ���� ���� �� 0
int AEffectSetContentsProp(int SampleRate, int channel);

// �������� ������ ���μ����� ����ϴ� �Լ�
// pInst : AEffectInitXEN() �Լ��� ȣ���Ͽ� ���� �����͸� �Ѱ���
// pIOL, pIOR : ���� �����Ͱ� ����ִ� ������ ������, ���� ���࿡ ���� interleaved �������� �����ϴ� �͵� ������
// length : ���μ����� �����ؾ� �� ������ �� (L/R�� ���� 1 sample�� ����)
//          �� ���� ����� �ִ밪�� ���� �ʵ��� �ؾ� �ϸ�, �ִ밪�� ���� 1152 sample�� ���ǵǾ� ���� (����� ���� �޸� �Ҵ��� �����)
// ret : ���� ���� �� 0
int AEffectSoundProcess(AEFFECT_EMERSYS_INSTANCE *pInst, short *pIO, int length);


// XEN module�� �ʱ�ȭ�ϰ� Ȱ��ȭ��Ű�� �Լ�
// ret : instance, ���� access���� �ʰ� AEffectSetParamValue() �Լ��� ����ϱ⸦ ������
//		�ʱ�ȭ ���θ� host�� ���� Ȯ���ϵ��� �ϱ� ���� ��ġ��.
AEFFECT_EMERSYS_INSTANCE* AEffectInitXEN(void);

// ����� ��Ȱ��ȭ��Ű�� ���� �޸� �Ҵ��� ���� ��� �޸� ��ȯ ���� ����
// pInst : AEffectInitXEN() �Լ��� ȣ���Ͽ� ���� �����͸� �Ѱ���
// ret : ���� ���� �� 0
int AEffectRelease(AEFFECT_EMERSYS_INSTANCE *pInst);


// XEN ȿ���� �Ӽ� �Ǵ� ���� ���θ� �����ϴ� �Լ�
// ������ ������ �� �Լ��� ���� �ϱ⸦ ������
// 4���� 3D ȿ���� EQ �� ��� ���� ����� ��, �Ǵ� ȿ���� ���� (EQ�� ��� band�� level) ���� �����ϱ� ���� �Ķ���͸� �Ѱ��ִ� �Լ�
// pInst : AEffectInitXEN() �Լ��� ȣ���Ͽ� ���� �����͸� �Ѱ���
// param_type : � �Ķ������ ���� ���������� �����ϴ� ����
//				enum ePARAM_TYPE ���� ���� ����� �� ����
//				����� ������ �ϰ� ���� ����				XEN_PTYPE_PROC_MODE
//				3D ȿ���� ���ڸ� �����ϰ� ���� ����			XEN_PTYPE_SURR, XEN_PTYPE_BASS, XEN_PTYPE_TREB �� �ϳ�
//				EQ ȿ���� Band�� gain�� �����ϰ� ���� ����	EQ_PTYPE_BAND_0 ~ EQ_PTYPE_BAND_4 �� �ϳ��� ���ڷ� ���
// param_value : �����Ϸ��� ���� �ǹ��ϴ� ����
//				����� ������ ���� ����						enum ePROC_MODE �� �ϳ��� ���
//				3D ȿ���� ���ڸ� �����ϰ� ���� ����			enum ePARAM_VALUE�� ���뿡 �ִ� XEN_VALUE_0 ~ XEN_VALUE_0 �� �ϳ�
//				EQ ȿ���� Band�� gain�� �����ϰ� ���� ����	enum ePARAM_VALUE�� ���뿡 �ִ� EQ_VALUE_N9DB ~ EQ_VALUE_P9DB�� �ϳ��� ���ڷ� ���
//
// param_type�� XEN_PTYPE_PROC_MODE�� ��� ������ ���� ���� param_value�� ����� �� ����
// 	XEN_MODE_SP_EXTX		�� ����Ŀ ����� 3D ȿ���� �����ϸ�
//	XEN_MODE_HP_MEX			�� ����� ����� 3D ȿ�� �� MEX �� �����ϸ�
//	XEN_MODE_HP_REX_MILD	�� ����� ����� 3D ȿ�� �� REX MILD �� �����ϸ�
//	XEN_MODE_HP_REX_WIDE	�� ����� ����� 3D ȿ�� �� REX WIDE �� �����ϸ�
//	XEN_MODE_HP_REX_LIVE	�� ����� ����� 3D ȿ�� �� REX LIVE �� �����ϸ�
//	XEN_MODE_HP_EQ			�� EQ ȿ���� �����ϸ�
//	XEN_MODE_NONE			�� �ƹ� ȿ���� ������� �ʴ� ���� �ǹ���
//
// param_type�� XEN_PTYPE_SURR�� ��� param_value��
//			XEN_VALUE_0 ~ XEN_VALUE_6�� ���� ����� �� ������ ���ڰ� Ŀ������ ���� 3D ȿ���� ������
//
// param_type�� XEN_PTYPE_BASS�� ��� param_value��
//			XEN_VALUE_0 ~ XEN_VALUE_6�� ���� ����� �� ������ ���ڰ� Ŀ������ ���� BASS ȿ���� ������
//
// param_type�� XEN_PTYPE_TREB�� ��� param_value��
//			XEN_VALUE_0 ~ XEN_VALUE_6�� ���� ����� �� ������ ���ڰ� Ŀ������ ������ ������ ������
//
// param_type�� EQ_PTYPE_BAND_0�� ��� param_value��
//			EQ_VALUE_N9DB ~ EQ_VALUE_P9DB�� ���� ����� �� ������ �� ���� -9dB ~ +9dB ��ŭ ���� �Ǵ� ������ �ǹ���
//
// param_type�� EQ_PTYPE_BAND_1 ~ EQ_PTYPE_BAND_4 �� ��� param_value��
//			EQ_PTYPE_BAND_0�� ���� �����̸� ���� ���ļ� ������ �޶���
//
// ret : ���� ����� preset�� enum ��, �������� ������ ���� ���� ��� -1 �Ǵ� -2 ���� ��ȯ
int AEffectSetParamValue(AEFFECT_EMERSYS_INSTANCE *pInst, int param_type, int param_value);


// XEN ȿ�� �� �̸ӽý����� �����ϴ� �������� �ѹ��� ������ �� �ֵ��� �ϴ� �Լ�
// pInst : AEffectInitXEN() �Լ��� ȣ���Ͽ� ���� �����͸� �Ѱ���
// preset : �����ϰ� ���� preset�� ������ �ǹ�
//			eXEN_PRESET, eEQ_PRESET �� �ϳ��� ���� ����� �� ����
//	XEN_eXTX_PRESET			�� ����Ŀ ����� ������
//	XEN_MEX_BASS_PRESET		3D ��� �� MEX�� ���� ���� ȿ���� ���� �� ��Ÿ���� ������, MEX BASS
//	XEN_MEX_LITE_PRESET		3D ��� �� MEX�� 3D Ȯ�� ȿ���� ���� �� ��Ÿ���� ������, MEX LITE
//	XEN_MEX_PRO_PRESET		3D ��� �� MEX�� 3D Ȯ��� ���� ���� ȿ���� ������ ������ ������ MEX PRO
//	XEN_REX_MILD_PRESET		3D ��� �� REX MILD�� 3D Ȯ�� ȿ���� ���� �� ��Ÿ���� ������, REX MILD
//	XEN_REX_WIDE_PRESET		3D ��� �� REX WIDE�� 3D Ȯ�� ȿ���� ���� �� ��Ÿ���� ������, REX WIDE
//	XEN_REX_LIVE_PRESET		3D ��� �� REX LIVE�� 3D Ȯ�� ȿ���� ���� �� ��Ÿ���� ������, REX LIVE
//	SEQ_FLAT				EQ ��� �� FLAT
//	SEQ_VOICE				EQ ��� �� VOICE
//	SEQ_POWERBASS			EQ ��� �� POWERBASS
//	SEQ_BRIGHT				EQ ��� �� BRIGHT
//	SEQ_DARKWARM			EQ ��� �� DARKWARM
//	SEQ_CLASSIC				EQ ��� �� CLASSIC
//	SEQ_HARDROCK			EQ ��� �� HARDROCK
//	SEQ_SOFTROCK			EQ ��� �� SOFTROCK
//	SEQ_JAZZ				EQ ��� �� JAZZ
//	SEQ_DANCE				EQ ��� �� DANCE
//	SEQ_SLOW				EQ ��� �� SLOW
//	SEQ_POP					EQ ��� �� POP
//	SEQ_BLUES				EQ ��� �� BLUES
//	SEQ_INSTRUMENTAL		EQ ��� �� INSTRUMENTAL
//	SEQ_FUNK				EQ ��� �� FUNK
//	SEQ_ACOUSTIC			EQ ��� �� ACOUSTIC
//	SEQ_ELECTRONIC			EQ ��� �� ELECTRONIC
//	SEQ_HIPHOP				EQ ��� �� HIPHOP
//	SEQ_LATIN				EQ ��� �� LATIN
//	SEQ_LECTURE				EQ ��� �� LECTURE
// ret : ���� ����� preset�� enum ��, �������� ������ ���� ���� ��� -1 �Ǵ� -2 ���� ��ȯ
int AEffectSetPreset(AEFFECT_EMERSYS_INSTANCE *pInst, int preset);


// ���� �����Ǿ��ִ� XEN ȿ���� ���ڸ� �ҷ��� �� �ִ� �Լ�
// pInst : AEffectInitXEN() �Լ��� ȣ���Ͽ� ���� �����͸� �Ѱ���
// param_type : AEffectSetParamValue()�� param_type �� ���� ���� ����� �� ����
// ret : param_type�� �ش��ϴ� ���ڸ� ��ȯ
int AEffectGetParamValue(AEFFECT_EMERSYS_INSTANCE *pInst, int param_type);


// ��� ���� �޸𸮸� �ʱ�ȭ�ϴ� �Լ�
// ��������, �ǰ��� ������ ���� ���������� ���� �����Ͱ� �Ѿ�� �� ������ �����ϱ� ���� ȣ��
// pInst : AEffectInitXEN() �Լ��� ȣ���Ͽ� ���� �����͸� �Ѱ���
// ret : ���� ���� �� 0
//int AEffectClearBuffer(AEFFECT_EMERSYS_INSTANCE *pInst);
int AEffectClearBuffer(Int32 nMode);

//////////////////////////////////////////// ESA ////////////////////////////////////////////
int XEN_ESA_Init(void);
//int AEffectXEN_ESA(Int16 *psData, Int32 *pOutSpectrum, Uint32 length);
int AEffectXEN_ESA(Int16 *psData, Int32 *pOutSpectrum, Uint32 length, Uint32 call_frame);
//////////////////////////////////////////// ESA ////////////////////////////////////////////

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __XEN_INTERFACE_H__ */


