#pragma once

#include "ui_base.hpp"

#define UISTATIC_MAX_TEXTLEN 1024

/**
    @brief 스태틱 모션 타입
*/
enum class eStaticMotionPattern {
    eInitFrame_Default,
    eInitFrame_ExpendCenter,
    ePauseFrame_Default,
    ePauseFrame_CollapseCenter,
    
    eInitBg_Default,
    eInitBg_ExpendRight,
    ePauseBg_Default,
    ePauseBg_CollapseLeft,

    eInitText_Default,
    eInitText_Flick,
    eInitText_Typing,
    ePauseText_Default,
    ePauseText_Flick,
};

class UI_Static_MotionParam {
public:
    eStaticMotionPattern MotionInitFrame;
    unsigned long        PitchInitFrame;
    unsigned long        DelayInitFrame;
    eStaticMotionPattern MotionPauseFrame;
    unsigned long        PitchPauseFrame;
    unsigned long        DelayPauseFrame;

    eStaticMotionPattern MotionInitBg;
    unsigned long        PitchInitBg;
    unsigned long        DelayInitBg;
    eStaticMotionPattern MotionPauseBg;
    unsigned long        PitchPauseBg;
    unsigned long        DelayPauseBg;
    
    eStaticMotionPattern MotionInitText;
    unsigned long        PitchInitText;
    unsigned long        DelayInitText;
    eStaticMotionPattern MotionPauseText;
    unsigned long        PitchPauseText;
    unsigned long        DelayPauseText;

    D2D1_COLOR_F ColorFrame;
    D2D1_COLOR_F ColorBg;
    D2D1_COLOR_F ColorText;

public:
    UI_Static_MotionParam() {
        MotionInitFrame = eStaticMotionPattern::eInitFrame_Default;
        PitchInitFrame = 0;
        DelayInitFrame = 0;
        MotionPauseFrame = eStaticMotionPattern::ePauseFrame_Default;
        PitchPauseFrame = 0;
        DelayPauseFrame = 0;
        
        MotionInitBg = eStaticMotionPattern::eInitBg_Default;
        PitchInitBg = 0;
        DelayInitBg = 0;
        MotionPauseBg = eStaticMotionPattern::ePauseBg_Default;
        PitchPauseBg = 0;
        DelayPauseBg = 0;

        MotionInitText = eStaticMotionPattern::eInitText_Default;
        PitchInitText = 0;
        DelayInitText = 0;
        MotionPauseText = eStaticMotionPattern::ePauseText_Default;
        PitchPauseText = 0;
        DelayPauseText = 0;

        ColorFrame = { 0,0,0,1 };
        ColorBg = { 0.94f, 0.94f, 0.94f, 1.f };
        ColorText = { 0,0,0,1 };
    }
};

class UI_Static : public UI {
public: 
    UI_Static_MotionParam Motion;

private:
    SRWLOCK lock;
    wchar_t szText[UISTATIC_MAX_TEXTLEN];
    PropText* pText;
    PropBox*  pBoxBg;
    PropBox*  pBoxFrame;
    IDWriteTextFormat* pTextFmt;

private:
    static void DefaultStaticProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam);
    void ResumeFrame(unsigned long Delay);
    void PauseFrame(unsigned long Delay);
    void ResumeBg(unsigned long Delay);
    void PauseBg(unsigned long Delay);
    void ResumeText(unsigned long Delay);
    void PauseText(unsigned long Delay);

public: /*반드시 있어야 되는 매서드*/
    UI_Static(UISystem* pUISys, pfnUIHandler pfnCallback, POSITION Pos, IDWriteTextFormat* pFormat, wchar_t* Text, UI_Static_MotionParam MotionParam = UI_Static_MotionParam());
    ~UI_Static();
    void pause(int nDelay);
    void resume(int nDelay);
    BOOL update(unsigned long time);
    void render();

public: /*UI별 옵션 매서드*/
    void SetText(wchar_t* pStr, int nDelay = 0);
};
