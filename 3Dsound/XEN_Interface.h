/*********************************************************************************/
/* date : 2005/06/01    By : Emersys Ltd.   Koo Jung Mo                          */
/* 05-06-01 : EMX Brand 변경 --> XEN (Xound Exciting & Natural)                  */
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
	Int32  AUDIO_ESA_SPECTRUM[10]; 	// 0 ~ 16 ( 17 Level : 0 - 표시하지 않음, 즉 표현할 threshold 이하의 값, 1~16 표현할 각 단계 )
	Uint32 AUDIO_ESA_BAND;	 		// The Number of Spectrum Bar (Max: 10)
	Uint32 AUDIO_ESA_FLAG;			// 0: Off, 1: On
	Uint32 DISPLAY_STEP;
} _XEN_ESA;

typedef struct _AXENPROP {      // Instance를 의미하는 구조체
	int PROC_MODE;	// 3D, EQ, bypass 등 효과의 모드를 의미
	int _3DPARAM0;	// 3D 효과의 확산 정도를 의미
	int _3DPARAM1;	// 3D 효과의 저음 보강 정도를 의미
	int _3DPARAM2;	// 3D 효과의 고음 보강 정도를 의미
	int _EQPARAM0;	// EQ 효과의 Band_0 영역을 보강하는 정도를 의미 (Band_0 -> 125 Hz)
	int _EQPARAM1;	// EQ 효과의 Band_1 영역을 보강하는 정도를 의미 (Band_1 -> 250 Hz)
	int _EQPARAM2;	// EQ 효과의 Band_2 영역을 보강하는 정도를 의미 (Band_2 -> 1 kHz)
	int _EQPARAM3;	// EQ 효과의 Band_3 영역을 보강하는 정도를 의미 (Band_3 -> 4 kHz)
	int _EQPARAM4;	// EQ 효과의 Band_4 영역을 보강하는 정도를 의미 (Band_4 -> 8 kHz)
} AEFFECT_EMERSYS_INSTANCE;

#define XEN_MODE_COUNT					32

#define XEN_EXTX_PARAM_VALUE_RANGE		1
#define XEN_MEX_PARAM_VALUE_RANGE		7
#define XEN_REX_WIDE_PARAM_VALUE_RANGE	7
#define XEN_REX_LIVE_PARAM_VALUE_RANGE	7

#define EQ_BAND_COUNT					5
#define EQ_PARAM_VALUE_RANGE			19

///////////////// AEffectSetParamValue의 param_type에 사용하는 값 /////////////////
// 모드 변경시 XEN_PTYPE_PROC_MODE
// 3D의 값을 변경하고 싶을 경우 XEN_PTYPE_SURR~XEN_PTYPE_TREB
// EQ의 값을 변경하고 싶을 경우 EQ_PTYPE_BAND_0~EQ_PTYPE_BAND_4
typedef enum {
	XEN_PTYPE_PROC_MODE = 0,	// 적용 3D 모드 또는 EQ 등을 결정하고 싶을 경우
	XEN_PTYPE_SURR,		// 3D 효과의 확산 정도를 의미하는 파라미터를 조절하고 싶을 경우
	XEN_PTYPE_BASS,		// 3D 효과의 저음 보강 정도를 의미하는 파라미터를 조절하고 싶을 경우
	XEN_PTYPE_TREB,		// 3D 효과의 고음 보강 정도를 의미하는 파라미터를 조절하고 싶을 경우 (MEX only)
	EQ_PTYPE_BAND_0,	// EQ 효과의 Band_0 영역을 보강 또는 감쇄하고 싶을 경우 (Band_0 -> 125 Hz)
	EQ_PTYPE_BAND_1,	// EQ 효과의 Band_1 영역을 보강 또는 감쇄하고 싶을 경우 (Band_1 -> 250 Hz)
	EQ_PTYPE_BAND_2,	// EQ 효과의 Band_2 영역을 보강 또는 감쇄하고 싶을 경우 (Band_2 -> 1 kHz)
	EQ_PTYPE_BAND_3,	// EQ 효과의 Band_3 영역을 보강 또는 감쇄하고 싶을 경우 (Band_3 -> 4 kHz)
	EQ_PTYPE_BAND_4		// EQ 효과의 Band_4 영역을 보강 또는 감쇄하고 싶을 경우 (Band_4 -> 8 kHz)
} ePARAM_TYPE;


///////////////// AEffectSetParamValue의 param_value에 사용하는 값 /////////////////

// AEffectSetParamValue의 param_type에 XEN_PTYPE_PROC_MODE 가 사용되는 경우 사용하는 값
typedef enum {
	XEN_MODE_SP_EXTX = 0,
	XEN_MODE_HP_MEX,
	XEN_MODE_HP_REX_MILD,
	XEN_MODE_HP_REX_WIDE,
	XEN_MODE_HP_REX_LIVE,
	XEN_MODE_HP_EQ,
	XEN_MODE_NONE
} ePROC_MODE;

// AEffectSetParamValue의 param_type에 XEN_PTYPE_SURR~EQ_PTYPE_BAND_4 가 사용되는 경우 사용하는 값
// 3D의 경우 XEN_VALUE_0~XEN_VALUE_6
// EQ의 경우 EQ_VALUE_N9DB~EQ_VALUE_P9DB 사용 가능
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


///////////////// AEffectSetPreset의 preset에 사용하는 값 /////////////////

// 3D 프리셋
typedef enum {
	XEN_eXTX_PRESET = 0,
	XEN_MEX_BASS_PRESET,
	XEN_MEX_LITE_PRESET,
	XEN_MEX_PRO_PRESET,
	XEN_REX_MILD_PRESET,
	XEN_REX_WIDE_PRESET,
	XEN_REX_LIVE_PRESET
} eXEN_PRESET;

// EQ 프리셋
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

// 재생 컨텐츠의 샘플링 레이트를 전달하는 함수
// 효과에 사용되는 대부분의 coefficient의 경우 샘플링 레이트에 절대적인 영향을 받으므로
// 샘플링 레이트가 변경될 때 이 함수를 반드시 호출해주어야 함
// nFS의 값에 대한 합의가 아직 이루어지지 않아 이 함수는 현재 동작하지 않음
// nFS : 샘플링 레이트를 의미하는 값. 아직 정의되지 않았음
// ret : 정상 동작 시 0
int AEffectSetContentsProp(int SampleRate, int channel);

// 실질적인 데이터 프로세싱을 담당하는 함수
// pInst : AEffectInitXEN() 함수를 호출하여 얻은 포인터를 넘겨줌
// pIOL, pIOR : 실제 데이터가 담겨있는 버퍼의 포인터, 차후 진행에 따라 interleaved 형식으로 변경하는 것도 가능함
// length : 프로세싱을 수행해야 할 샘플의 수 (L/R을 묶어 1 sample로 간주)
//          이 값은 약속한 최대값을 넘지 않도록 해야 하며, 최대값은 현재 1152 sample로 정의되어 있음 (현재는 정적 메모리 할당을 사용중)
// ret : 정상 동작 시 0
int AEffectSoundProcess(AEFFECT_EMERSYS_INSTANCE *pInst, short *pIO, int length);


// XEN module을 초기화하고 활성화시키는 함수
// ret : instance, 직접 access하지 않고 AEffectSetParamValue() 함수를 사용하기를 권장함
//		초기화 여부를 host가 쉽게 확인하도록 하기 위한 장치임.
AEFFECT_EMERSYS_INSTANCE* AEffectInitXEN(void);

// 모듈을 비활성화시키고 동적 메모리 할당이 있을 경우 메모리 반환 등을 수행
// pInst : AEffectInitXEN() 함수를 호출하여 얻은 포인터를 넘겨줌
// ret : 정상 동작 시 0
int AEffectRelease(AEFFECT_EMERSYS_INSTANCE *pInst);


// XEN 효과의 속성 또는 적용 여부를 결정하는 함수
// 인자의 변경은 이 함수를 통해 하기를 권장함
// 4가지 3D 효과와 EQ 중 어느 것을 사용할 지, 또는 효과의 정도 (EQ의 경우 band의 level) 등을 결정하기 위한 파라미터를 넘겨주는 함수
// pInst : AEffectInitXEN() 함수를 호출하여 얻은 포인터를 넘겨줌
// param_type : 어떤 파라미터의 값을 변경할지를 전달하는 인자
//				enum ePARAM_TYPE 안의 값을 사용할 수 있음
//				모드의 변경을 하고 싶을 때는				XEN_PTYPE_PROC_MODE
//				3D 효과의 인자를 변경하고 싶을 때는			XEN_PTYPE_SURR, XEN_PTYPE_BASS, XEN_PTYPE_TREB 중 하나
//				EQ 효과의 Band별 gain을 변경하고 싶을 때는	EQ_PTYPE_BAND_0 ~ EQ_PTYPE_BAND_4 중 하나를 인자로 사용
// param_value : 변경하려는 값을 의미하는 인자
//				모드의 변경을 원할 때는						enum ePROC_MODE 중 하나를 사용
//				3D 효과의 인자를 변경하고 싶을 때는			enum ePARAM_VALUE의 내용에 있는 XEN_VALUE_0 ~ XEN_VALUE_0 중 하나
//				EQ 효과의 Band별 gain을 변경하고 싶을 때는	enum ePARAM_VALUE의 내용에 있는 EQ_VALUE_N9DB ~ EQ_VALUE_P9DB중 하나를 인자로 사용
//
// param_type이 XEN_PTYPE_PROC_MODE일 경우 다음과 같은 값을 param_value로 사용할 수 있음
// 	XEN_MODE_SP_EXTX		는 스피커 모드의 3D 효과를 적용하며
//	XEN_MODE_HP_MEX			는 헤드폰 모드의 3D 효과 중 MEX 를 적용하며
//	XEN_MODE_HP_REX_MILD	는 헤드폰 모드의 3D 효과 중 REX MILD 를 적용하며
//	XEN_MODE_HP_REX_WIDE	는 헤드폰 모드의 3D 효과 중 REX WIDE 를 적용하며
//	XEN_MODE_HP_REX_LIVE	는 헤드폰 모드의 3D 효과 중 REX LIVE 를 적용하며
//	XEN_MODE_HP_EQ			는 EQ 효과를 적용하며
//	XEN_MODE_NONE			는 아무 효과도 사용하지 않는 것을 의미함
//
// param_type이 XEN_PTYPE_SURR일 경우 param_value는
//			XEN_VALUE_0 ~ XEN_VALUE_6의 값을 사용할 수 있으며 숫자가 커질수록 강한 3D 효과를 적용함
//
// param_type이 XEN_PTYPE_BASS일 경우 param_value는
//			XEN_VALUE_0 ~ XEN_VALUE_6의 값을 사용할 수 있으며 숫자가 커질수록 강한 BASS 효과를 적용함
//
// param_type이 XEN_PTYPE_TREB일 경우 param_value는
//			XEN_VALUE_0 ~ XEN_VALUE_6의 값을 사용할 수 있으며 숫자가 커질수록 고음의 보강이 강해짐
//
// param_type이 EQ_PTYPE_BAND_0일 경우 param_value는
//			EQ_VALUE_N9DB ~ EQ_VALUE_P9DB의 값을 사용할 수 있으며 각 값은 -9dB ~ +9dB 만큼 감쇄 또는 보강을 의미함
//
// param_type이 EQ_PTYPE_BAND_1 ~ EQ_PTYPE_BAND_4 일 경우 param_value는
//			EQ_PTYPE_BAND_0과 같은 개념이며 적용 주파수 영역만 달라짐
//
// ret : 실제 적용된 preset의 enum 값, 정상적인 동작을 하지 않을 경우 -1 또는 -2 값이 반환
int AEffectSetParamValue(AEFFECT_EMERSYS_INSTANCE *pInst, int param_type, int param_value);


// XEN 효과 중 이머시스에서 제안하는 프리셋을 한번에 설정할 수 있도록 하는 함수
// pInst : AEffectInitXEN() 함수를 호출하여 얻은 포인터를 넘겨줌
// preset : 적용하고 싶은 preset의 종류를 의미
//			eXEN_PRESET, eEQ_PRESET 중 하나의 값을 사용할 수 있음
//	XEN_eXTX_PRESET			는 스피커 모드의 프리셋
//	XEN_MEX_BASS_PRESET		3D 모드 중 MEX의 저음 보강 효과를 가장 잘 나타내는 프리셋, MEX BASS
//	XEN_MEX_LITE_PRESET		3D 모드 중 MEX의 3D 확산 효과를 가장 잘 나타내는 프리셋, MEX LITE
//	XEN_MEX_PRO_PRESET		3D 모드 중 MEX의 3D 확산과 저음 보강 효과를 적절히 조합한 프리셋 MEX PRO
//	XEN_REX_MILD_PRESET		3D 모드 중 REX MILD의 3D 확산 효과를 가장 잘 나타내는 프리셋, REX MILD
//	XEN_REX_WIDE_PRESET		3D 모드 중 REX WIDE의 3D 확산 효과를 가장 잘 나타내는 프리셋, REX WIDE
//	XEN_REX_LIVE_PRESET		3D 모드 중 REX LIVE의 3D 확산 효과를 가장 잘 나타내는 프리셋, REX LIVE
//	SEQ_FLAT				EQ 모드 중 FLAT
//	SEQ_VOICE				EQ 모드 중 VOICE
//	SEQ_POWERBASS			EQ 모드 중 POWERBASS
//	SEQ_BRIGHT				EQ 모드 중 BRIGHT
//	SEQ_DARKWARM			EQ 모드 중 DARKWARM
//	SEQ_CLASSIC				EQ 모드 중 CLASSIC
//	SEQ_HARDROCK			EQ 모드 중 HARDROCK
//	SEQ_SOFTROCK			EQ 모드 중 SOFTROCK
//	SEQ_JAZZ				EQ 모드 중 JAZZ
//	SEQ_DANCE				EQ 모드 중 DANCE
//	SEQ_SLOW				EQ 모드 중 SLOW
//	SEQ_POP					EQ 모드 중 POP
//	SEQ_BLUES				EQ 모드 중 BLUES
//	SEQ_INSTRUMENTAL		EQ 모드 중 INSTRUMENTAL
//	SEQ_FUNK				EQ 모드 중 FUNK
//	SEQ_ACOUSTIC			EQ 모드 중 ACOUSTIC
//	SEQ_ELECTRONIC			EQ 모드 중 ELECTRONIC
//	SEQ_HIPHOP				EQ 모드 중 HIPHOP
//	SEQ_LATIN				EQ 모드 중 LATIN
//	SEQ_LECTURE				EQ 모드 중 LECTURE
// ret : 실제 적용된 preset의 enum 값, 정상적인 동작을 하지 않을 경우 -1 또는 -2 값이 반환
int AEffectSetPreset(AEFFECT_EMERSYS_INSTANCE *pInst, int preset);


// 현재 설정되어있는 XEN 효과의 인자를 불러올 수 있는 함수
// pInst : AEffectInitXEN() 함수를 호출하여 얻은 포인터를 넘겨줌
// param_type : AEffectSetParamValue()의 param_type 과 같은 값을 사용할 수 있음
// ret : param_type에 해당하는 인자를 반환
int AEffectGetParamValue(AEFFECT_EMERSYS_INSTANCE *pInst, int param_type);


// 사용 중인 메모리를 초기화하는 함수
// 빨리감기, 되감기 등으로 인해 연속적이지 않은 데이터가 넘어올 때 잡음을 방지하기 위해 호출
// pInst : AEffectInitXEN() 함수를 호출하여 얻은 포인터를 넘겨줌
// ret : 정상 동작 시 0
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


