#pragma once

#include <windows.h>
#include <stdlib.h>
#include <math.h>

#define MAX_CHANNEL_CNT (3) /*채널 최대 갯수*/
#define MAX_CHAIN_CNT (4)   /*각 채널에 할당될 최대 체인 갯수*/

/**
    @brief 오실레이터 함수포인터 규격 지정
*/
typedef float(*pfnFormula)(float);

/**
    @brief 모션 적용시킬 포인터 리스트
*/
typedef struct _st_ValueChain {
    float* pVal;   /**< 모션을 적용할 포인터 (어떤 값을 제어할것인지)*/
    float  nStart; /**< 시작값*/
    float  nEnd;   /**< 끝 값*/
}VALUE_CHAIN;

/**
    @brief 모션 함수 열거자
*/
enum class eMotionForm {
    eMotion_None = 0,    /**< 모션 없음*/
    eMotion_Pulse1,      /**< 펄스 함수 (Preset_Pulse1)*/
    eMotion_Pulse2,      /**< 펄스 함수 (Preset_Pulse2)*/
    eMotion_Linear1,     /**< 선형 함수 (Preset_Linear1)*/
    eMotion_Linear2,     /**< 선형 함수 (Preset_Linear2)*/
    eMotion_x3_1,        /**< x 3제곱   (Preset_x3_0to1_1)*/
    eMotion_x3_2,        /**< x 3제곱   (Preset_x3_0to1_2)*/
    eMotion_x3_1to0_1,   /**< x 3제곱   (Preset_x3_1to0_1)*/
    eMotion_x3_1to0_2,   /**< x 3제곱   (Preset_x3_1to0_2)*/
    eMotion_Custom = 255 /**< 커스텀 모션 (모션함수 직접정의)*/
};

/**
    @brief 모션 정보 구조체
*/
typedef struct _st_MotionInfo
{
    eMotionForm formular; /**< 모션 함수*/
    int         nDelay;   /**< 딜레이*/
    int         nPitch;   /**< 모션 재생 시간*/
}MOTION_INFO;

/**
    @brief 애니메이션 컴포넌트에서 사용할 채널 표현 구조체
*/
typedef struct _st_MotionChannel {
    MOTION_INFO      MotionInfo;              /**< 모션정보*/
    pfnFormula       pfnForm;                 /**< 기본 제공 모션ID가 Custom일 경우 이 함수 호출, 평소엔 NULL*/
    float            fCurrent;                /**< 현재 출력값 (0.f ~ 1.f)*/
    VALUE_CHAIN      arrChain[MAX_CHAIN_CNT]; /**< 제어될 포인터 리스트 (이 모션으로 어떤 값들을 제어할것인지?)*/
    unsigned __int8  nChain;                  /**< 리스트에 들어간 갯수*/
}MOTION_PATTERN;

/**
    @brief  개체의 애니메이션을 제어하는 컴포넌트
    @remark 이 컴포넌트를 포함하는 개체에서 매 프레임마다
    @n      update 매서드로 애니메이션을 진행하면 된다.
*/
class ComponentMotion {
private:
    MOTION_PATTERN ChannelList[MAX_CHANNEL_CNT];
    int            nCntUsedChannel; /**< 현재 사용중인 채널 갯수*/
    unsigned int   nTotalPlaytime;  /**< 채널 전체 재생시간*/
    unsigned int   nRunTime;        /**< 총 진행시간*/

public:
    ComponentMotion();
    void         addChannel(MOTION_PATTERN patt);
    void         appendChannel(MOTION_PATTERN patt);
    int          getChannelCnt();
    float        getChannelVal(int idx);
    unsigned int getPlayTime();
    void         setRuntime(unsigned long time);
    void         clearChannel();
    BOOL         update(unsigned long ElapsedTime);
};

/**
    @brief 모션 패턴 구조체 셋팅
*/
inline MOTION_PATTERN SetMotionPattern(MOTION_INFO MotionInfo, pfnFormula pfnForm)
{
    MOTION_PATTERN patt;

    if (MotionInfo.nPitch <= 0) MotionInfo.nPitch = 1;
    patt.MotionInfo = MotionInfo;
    patt.pfnForm = pfnForm;
    patt.fCurrent = 0;
    patt.nChain = 0;
    return patt;
}

/**
    @brief 모션제어값 체인 추가
*/
void AddChain(MOTION_PATTERN* pPatt, float* pVal, float nStart, float nEnd);

/**
    @brief 변환함수 프리셋 : 펄스1 ( 0 1 0 1 0 1 )
*/
inline float Preset_Pulse1(float x)
{
#if 0 /*스위칭 주기가 너무 짧아 프레임 스킵되는 현상이 있음*/
    if (x > 0.9) return 1.f;
    if (x > 0.8) return 0.f;
    if (x > 0.7) return 1.f;
    if (x > 0.6) return 0.f;
    if (x > 0.5) return 1.f;
    if (x > 0.4) return 0.f;
    if (x > 0.3) return 1.f;
    if (x > 0.2) return 0.f;
    if (x > 0.1) return 1.f;
    return 0.f;
#else
    if (x > 0.8333) return 1.f;
    if (x > 0.6666) return 0.f;
    if (x > 0.5) return 1.f;
    if (x > 0.3333) return 0.f;
    if (x > 0.1666) return 1.f;
    return 0.f;
#endif
}

/**
    @brief 변환함수 프리셋 : 펄스2 ( 1 0 1 0 1 0 )
*/
inline float Preset_Pulse2(float x)
{
#if 0 /*스위칭 주기가 너무 짧아 프레임 스킵되는 현상이 있음*/
    if (x > 0.9) return 0.f;
    if (x > 0.8) return 1.f;
    if (x > 0.7) return 0.f;
    if (x > 0.6) return 1.f;
    if (x > 0.5) return 0.f;
    if (x > 0.4) return 1.f;
    if (x > 0.3) return 0.f;
    if (x > 0.2) return 1.f;
    if (x > 0.1) return 0.f;
    return 1.f;
#else
    if (x > 0.8333) return 0.f;
    if (x > 0.6666) return 1.f;
    if (x > 0.5) return 0.f;
    if (x > 0.3333) return 1.f;
    if (x > 0.1666) return 0.f;
    return 1.f;

#endif
}

inline float Preset_None(float x) { return 1; }
inline float Preset_Osc1(float x) { return (float)(-0.5 * cos(6.283184 * x) + 0.5); }                      /* y = -1/2 cos(2pi) + 1/2 */
inline float Preset_Linear1(float x) { if (x > 1) return 1.f; return x; }                                  /* y = x */
inline float Preset_Linear2(float x) { if (x > 1) return 0.f; return -x + 1.f; }                           /* y = -x + 1 */
inline float Preset_x3_0to1_1(float x) { if (x > 1) return 1.f; return x * x * x; }                        /* ||| |  |    |      | */
inline float Preset_x3_0to1_2(float x) { float z = x - 1.f; if (x > 1) return 1.f; return z * z * z + 1; } /* |      |    |  | ||| */
inline float Preset_x3_1to0_1(float x) { if (x > 1) return 0.f; return -(x * x * x) + 1; }                 /* ||| |  |    |      | */
inline float Preset_x3_1to0_2(float x) { float z = x - 1.f; if (x > 1) return 0.f; return -(z * z * z); }  /* |      |    |  | ||| */
