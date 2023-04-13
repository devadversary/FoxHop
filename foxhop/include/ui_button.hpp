#pragma once

#include "ui_base.hpp"

class UISystem; /*UI시스템 클래스의 전방선언*/

#define MAX_BUTTONNAME 32 /*버튼의 최대 텍스트 길이*/

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

/**
    @brief 버튼 클래스
    @remark 생성은 팩토리에서 해준다.
*/
class UI_Button : public UI {
public :
    eButtonMotionPattern ButtonInitMotion      = eButtonMotionPattern::eInit_Default;      /**< 시작모션*/
    eButtonMotionPattern ButtonPauseMotion     = eButtonMotionPattern::ePause_Default;     /**< 소멸모션*/
    eButtonMotionPattern ButtonMouseOverMotion = eButtonMotionPattern::eMouseover_Default; /**< 마우스오버 모션*/
    eButtonMotionPattern ButtonClickMotion     = eButtonMotionPattern::eText_Default;     /**< 클릭 모션*/
    eButtonMotionPattern ButtonColorMotion     = eButtonMotionPattern::eColor_Default;     /**< 색상 변경 모션*/
    eButtonMotionPattern ButtonTextMotion      = eButtonMotionPattern::eText_Default;      /**< 텍스트 변경 모션*/
    unsigned int         ButtonInitPitch       = 0; /**< 시작모션 피치*/
    unsigned int         ButtonPausePitch      = 0; /**< 소멸모션 피치*/
    unsigned int         ButtonMouseOverPitch  = 0; /**< 마우스오버모션 피치*/
    unsigned int         ButtonClickPitch      = 0; /**< 클릭 피치*/
    unsigned int         ButtonColorPitch      = 0; /**< 색상 변경 피치*/
    unsigned int         ButtonTextPitch       = 0; /**< 텍스트 변경 피치*/
    D2D1_COLOR_F         ButtonFrameColor      = { 0.f, 0.f, 0.f, 0.f };     /**< 초기 테두리 색*/
    D2D1_COLOR_F         ButtonFaceColor       = { 0.8f, 0.8f, 0.8f, 1.f };  /**< 초기 버튼 전경색*/
    D2D1_COLOR_F         ButtonMouseoverColor  = { 1.f,  1.f,  1.f,  0.5f }; /**< 마우스 오버 색*/
    D2D1_COLOR_F         ButtonHighlightColor  = { 1.f,  1.f,  1.f,  1.f };  /**< 버튼 하이라이트 색*/
    D2D1_COLOR_F         ButtonFontColor       = { 0.f, 0.f, 0.f, 1.f };     /**< 초기 텍스트 색*/

private:
    PropBox*  MBoxFace;      /**< 전경 모션 박스*/
    PropBox*  MBoxHighlight; /**< 하이라이트 모션 박스*/
    PropText* MText;         /**< 모션 박스 기본 텍스트*/
    int       nTextLen;
    wchar_t   szText[MAX_BUTTONNAME];
    
private: /*2022.04.30 : Devadversary - InputMotion 은 기본적으로 Private 함수여야하지만 그렇게 되면, DefaultButtonProc 함수에서 접근을 못함.*/
    void InputMotion(eButtonAction Action, unsigned int nDelay, void* param);
    static void DefaultButtonProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam);

public:
    UI_Button(UISystem* pUISys, pfnUIHandler pfnCallback, POSITION Pos, wchar_t* pText, int nDelay);
    ~UI_Button() {};

    void pause(int nDelay);
    void resume(int nDelay);
    void Destroy();
    BOOL update(unsigned long time);
    void render();

public: /*UI별 옵션 매서드*/
    void setText(wchar_t* pText, int nDelay);
    void setFaceColor(D2D1_COLOR_F Color, MOTION_INFO miColor);
    void setHighlightColor(D2D1_COLOR_F Color, MOTION_INFO miColor);
};
