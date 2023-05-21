#pragma once

#include "ui_base.hpp"
#include "ui_system.hpp"

/**
    @brief 스태틱 모션 타입
*/
enum class eLineChartMotionPattern {
    eInitFrame_Default,
    ePauseFrame_Default,

    eInitBg_Default,
    ePauseBg_Default,
};

class UI_LineChart_MotionParam {
public:
    eLineChartMotionPattern MotionInitBg;
    unsigned long PitchInitBg;
    unsigned long DelayInitBg;
    eLineChartMotionPattern MotionPauseBg;
    unsigned long PitchPauseBg;
    unsigned long DelayPauseBg;

    eLineChartMotionPattern MotionInitFrame;
    unsigned long PitchInitFrame;
    unsigned long DelayInitFrame;
    eLineChartMotionPattern MotionPauseFrame;
    unsigned long PitchPauseFrame;
    unsigned long DelayPauseFrame;

    D2D1_COLOR_F ColorFrame;
    D2D1_COLOR_F ColorBg;

public:
    UI_LineChart_MotionParam() {
        MotionInitBg = eLineChartMotionPattern::eInitBg_Default;
        PitchInitBg = 0;
        DelayInitBg = 0;
        MotionPauseBg = eLineChartMotionPattern::ePauseBg_Default;
        PitchPauseBg = 0;
        DelayPauseBg = 0;

        MotionInitFrame = eLineChartMotionPattern::eInitFrame_Default;
        PitchInitFrame = 0;
        DelayInitFrame = 0;
        MotionPauseFrame = eLineChartMotionPattern::ePauseFrame_Default;
        PitchPauseFrame = 0;
        DelayPauseFrame = 0;

        ColorFrame = { 0.f,0.f,0.f,1.f };
        ColorBg = { 1.f, 1.f, 1.f, 1.f };
    }
};

class UI_LineChart : public UI {
public:
    UI_LineChart_MotionParam Motion;

private:
    SRWLOCK lock;
    PropBox* pBoxBg;
    PropBox* pBoxFrame;

private:
    static void DefaultLineChartProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam);

    void ResumeFrame(unsigned long Delay);
    void PauseFrame(unsigned long Delay);
    void ResumeBg(unsigned long Delay);
    void PauseBg(unsigned long Delay);

public: /*반드시 있어야 되는 매서드*/
    UI_LineChart(UISystem* pUISys, pfnUIHandler pfnCallback, POSITION Pos, IDWriteTextFormat* pTextFmt, UI_LineChart_MotionParam MotionParam = UI_LineChart_MotionParam());
    ~UI_LineChart();
    void pause(int nDelay);
    void resume(int nDelay);
    BOOL update(unsigned long time);
    void render();

public: /*UI별 옵션 매서드*/
    //AddData 같은거
};
