#pragma once

#include "ui_base.hpp"

#define UISTATIC_MAX_TEXTLEN 1024

/**
    @brief 스태틱 모션 타입
*/
enum class eStaticMotionPattern {
    eInit_Default = 0,   /**< 생성 : 모션 없음*/
    ePause_Default = 0,  /**< 소멸 : 모션 없음*/
    eText_Default = 0,   /**< 텍스트 : 모션 없음*/
};

class UI_Static : public UI {
public:
    eStaticMotionPattern MotionInit  = eStaticMotionPattern::eInit_Default;
    eStaticMotionPattern MotionPause = eStaticMotionPattern::ePause_Default;
    eStaticMotionPattern MotionText  = eStaticMotionPattern::eText_Default;
    D2D1_COLOR_F ColorFrame = { 0,0,0,1}; /**< 테두리 색*/
    D2D1_COLOR_F ColorBg  = { 0.94f, 0.94f, 0.94f, 1.f }; /**< 전경색*/
    D2D1_COLOR_F ColorFont  = { 0,0,0,1 }; /**< 텍스트 색*/
    unsigned long PitchInit = 0;
    unsigned long PitchPause = 0;
    unsigned long PitchText = 0;

private:
    wchar_t szText[UISTATIC_MAX_TEXTLEN];
    PropText* pText;
    PropBox*  pBoxBg;
    PropBox*  pBoxFrame;

private:
    static void DefaultStaticProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam);

public: /*반드시 있어야 되는 매서드*/
    UI_Static(UISystem* pUISys, pfnUIHandler pfnCallback, POSITION Pos, wchar_t* Text);
    ~UI_Static();
    void pause(int nDelay);
    void resume(int nDelay);
    BOOL update(unsigned long time);
    void render();

public: /*UI별 옵션 매서드*/
    void SetText(wchar_t* pStr, int nDelay = 0);
};
