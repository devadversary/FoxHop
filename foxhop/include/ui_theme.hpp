#pragma once

#include "ui_base.hpp"

/**
    @brief 버튼 모션 타입 상수
*/
enum class eButtonMotionPattern {
    eInit_Default = 0,      /**< 생성: 모션 없음*/
    eInit_Reload,           /**< 생성: 장전 모션*/
    eInit_Flick,            /**< 생성: 플리커링 모션*/

    ePause_Default = 0,     /**< 소멸: 모션 없음*/
    ePause_Flick,           /**< 소멸: 플리커링 모션*/

    eMouseover_Default = 0, /**< 마우스오버: 모션 없음*/
    eMouseover_Flick,       /**< 마우스오버: 플리커링 모션*/

    eClick_Default = 0,     /**< 클릭: 모션 없음*/
    eClick_Flash,           /**< 클릭: 점멸 모션*/

    eColor_Default = 0,     /**< 색상변경: 모션 없음*/
    eColor_Flash,           /**< 색상변경: 점멸 모션*/

    eText_Default = 0       /**< 텍스트: 모션 없음*/
};

/**
    @brief 버튼 모션 타입
*/
enum class eButtonAction {
    eAction_Init = 0,  /**< 초기화 모션*/
    eAction_Pause,     /**< 소멸 모션*/
    eAction_Mouseover, /**< 마우스 오버 모션 (커서가 떠났을때도 포함)*/
    eAction_Click,     /**< 클릭 모션*/
    eAction_Color,     /**< 색상 변경 모션*/
    eAction_Text       /**< 텍스트 모션*/
};

#define MAX_BUTTONNAME 32 /*버튼의 최대 텍스트 길이*/

/**
    @brief UI들의 테마에 대한 정보
*/
class UITheme {
public:
    /*UI_Button 의 속성*/
    eButtonMotionPattern ButtonInitMotion;      /**< 시작모션*/
    eButtonMotionPattern ButtonPauseMotion;     /**< 소멸모션*/
    eButtonMotionPattern ButtonMouseOverMotion; /**< 마우스오버 모션*/
    eButtonMotionPattern ButtonClickMotion;     /**< 클릭 모션*/
    eButtonMotionPattern ButtonColorMotion;     /**< 색상 변경 모션*/
    eButtonMotionPattern ButtonTextMotion;      /**< 텍스트 변경 모션*/
    unsigned int         ButtonInitPitch;       /**< 시작모션 피치*/
    unsigned int         ButtonPausePitch;      /**< 소멸모션 피치*/
    unsigned int         ButtonMouseOverPitch;  /**< 마우스오버모션 피치*/
    unsigned int         ButtonClickPitch;      /**< 클릭 피치*/
    unsigned int         ButtonColorPitch;      /**< 색상 변경 피치*/
    unsigned int         ButtonTextPitch;       /**< 텍스트 변경 피치*/
    D2D1_COLOR_F         ButtonFrameColor;      /**< 초기 테두리 색*/
    D2D1_COLOR_F         ButtonFaceColor;       /**< 초기 버튼 전경색*/
    D2D1_COLOR_F         ButtonMouseoverColor;  /**< 마우스 오버 색*/
    D2D1_COLOR_F         ButtonHighlightColor;  /**< 버튼 하이라이트 색*/
    D2D1_COLOR_F         ButtonFontColor;       /**< 초기 텍스트 색*/

public:
    UITheme() {
        ButtonInitMotion = eButtonMotionPattern::eInit_Default;
        ButtonPauseMotion = eButtonMotionPattern::ePause_Default;
        ButtonMouseOverMotion = eButtonMotionPattern::eMouseover_Default;
        ButtonClickMotion = eButtonMotionPattern::eText_Default;
        ButtonColorMotion = eButtonMotionPattern::eColor_Default;
        ButtonTextMotion = eButtonMotionPattern::eText_Default;
        ButtonInitPitch = 0;
        ButtonPausePitch = 0;
        ButtonMouseOverPitch = 0;
        ButtonClickPitch = 0;
        ButtonColorPitch = 0;
        ButtonTextPitch = 0;
        ButtonFrameColor = ALL_ZERO;
        ButtonFaceColor = { 0.8f, 0.8f, 0.8f, 1.f };  /*밝은 회색*/
        ButtonMouseoverColor = { 1.f,  1.f,  1.f,  0.5f }; /*반투명 흰색*/;
        ButtonHighlightColor = { 1.f,  1.f,  1.f,  1.f }; /*흰색*/
        ButtonFontColor = { 0.f, 0.f, 0.f, 1.f};
    }
    ~UITheme() {}
};