#pragma once

#include <windows.h>
#include <stdlib.h>
#include <math.h>

#define MAX_CHANNEL_CNT (3) /*ä�� �ִ� ����*/
#define MAX_CHAIN_CNT (4)   /*�� ä�ο� �Ҵ�� �ִ� ü�� ����*/

/**
    @brief ���Ƿ����� �Լ������� �԰� ����
*/
typedef float(*pfnFormula)(float);

/**
    @brief ��� �����ų ������ ����Ʈ
*/
typedef struct _st_ValueChain {
    float* pVal;   /**< ����� ������ ������ (� ���� �����Ұ�����)*/
    float  nStart; /**< ���۰�*/
    float  nEnd;   /**< �� ��*/
}VALUE_CHAIN;

/**
    @brief ��� �Լ� ������
*/
enum class eMotionForm {
    eMotion_None = 0,    /**< ��� ����*/
    eMotion_Pulse1,      /**< �޽� �Լ� (Preset_Pulse1)*/
    eMotion_Pulse2,      /**< �޽� �Լ� (Preset_Pulse2)*/
    eMotion_Linear1,     /**< ���� �Լ� (Preset_Linear1)*/
    eMotion_Linear2,     /**< ���� �Լ� (Preset_Linear2)*/
    eMotion_x3_1,        /**< x 3����   (Preset_x3_0to1_1)*/
    eMotion_x3_2,        /**< x 3����   (Preset_x3_0to1_2)*/
    eMotion_x3_1to0_1,   /**< x 3����   (Preset_x3_1to0_1)*/
    eMotion_x3_1to0_2,   /**< x 3����   (Preset_x3_1to0_2)*/
    eMotion_Custom = 255 /**< Ŀ���� ��� (����Լ� ��������)*/
};

/**
    @brief ��� ���� ����ü
*/
typedef struct _st_MotionInfo
{
    eMotionForm formular; /**< ��� �Լ�*/
    int         nDelay;   /**< ������*/
    int         nPitch;   /**< ��� ��� �ð�*/
}MOTION_INFO;

/**
    @brief �ִϸ��̼� ������Ʈ���� ����� ä�� ǥ�� ����ü
*/
typedef struct _st_MotionChannel {
    MOTION_INFO      MotionInfo;              /**< �������*/
    pfnFormula       pfnForm;                 /**< �⺻ ���� ���ID�� Custom�� ��� �� �Լ� ȣ��, ��ҿ� NULL*/
    float            fCurrent;                /**< ���� ��°� (0.f ~ 1.f)*/
    VALUE_CHAIN      arrChain[MAX_CHAIN_CNT]; /**< ����� ������ ����Ʈ (�� ������� � ������ �����Ұ�����?)*/
    unsigned __int8  nChain;                  /**< ����Ʈ�� �� ����*/
}MOTION_PATTERN;

/**
    @brief  ��ü�� �ִϸ��̼��� �����ϴ� ������Ʈ
    @remark �� ������Ʈ�� �����ϴ� ��ü���� �� �����Ӹ���
    @n      update �ż���� �ִϸ��̼��� �����ϸ� �ȴ�.
*/
class ComponentMotion {
private:
    MOTION_PATTERN ChannelList[MAX_CHANNEL_CNT];
    int            nCntUsedChannel; /**< ���� ������� ä�� ����*/
    unsigned int   nTotalPlaytime;  /**< ä�� ��ü ����ð�*/
    unsigned int   nRunTime;        /**< �� ����ð�*/

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
    @brief ��� ���� ����ü ����
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
    @brief ������ ü�� �߰�
*/
void AddChain(MOTION_PATTERN* pPatt, float* pVal, float nStart, float nEnd);

/**
    @brief ��ȯ�Լ� ������ : �޽�1 ( 0 1 0 1 0 1 )
*/
inline float Preset_Pulse1(float x)
{
#if 0 /*����Ī �ֱⰡ �ʹ� ª�� ������ ��ŵ�Ǵ� ������ ����*/
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
    @brief ��ȯ�Լ� ������ : �޽�2 ( 1 0 1 0 1 0 )
*/
inline float Preset_Pulse2(float x)
{
#if 0 /*����Ī �ֱⰡ �ʹ� ª�� ������ ��ŵ�Ǵ� ������ ����*/
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
