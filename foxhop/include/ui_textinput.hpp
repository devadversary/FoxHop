#pragma once

#include "ui_base.hpp"
#include "ui_system.hpp"
#include <vector>

/**
    @brief 스태틱 모션 타입
*/
enum class eTextinputMotionPattern {
    eInitFrame_Default,
    eInitFrame_ExpendLR,
    ePauseFrame_Default,
    ePauseFrame_CollapseLR,

    eInitBg_Default,
    ePauseBg_Default,

    eInitText_Default,
    eInitText_SlideIn,
    ePauseText_Default,
    ePauseText_SlideOut,

    eInitCaret_Default,
    ePauseCaret_Default,

    eMoveCaret_Default,
    eMoveCaret_Decel,
};

class UI_Textinput_MotionParam {
public:
    eTextinputMotionPattern MotionInitBg;
    unsigned long PitchInitBg;
    unsigned long DelayInitBg;
    eTextinputMotionPattern MotionPauseBg;
    unsigned long PitchPauseBg;
    unsigned long DelayPauseBg;

    eTextinputMotionPattern MotionInitFrame;
    unsigned long PitchInitFrame;
    unsigned long DelayInitFrame;
    eTextinputMotionPattern MotionPauseFrame;
    unsigned long PitchPauseFrame;
    unsigned long DelayPauseFrame;

    eTextinputMotionPattern MotionInitText;
    unsigned long PitchInitText;
    unsigned long DelayInitText;
    eTextinputMotionPattern MotionPauseText;
    unsigned long PitchPauseText;
    unsigned long DelayPauseText;

    eTextinputMotionPattern MotionInitCaret;
    unsigned long PitchInitCaret;
    unsigned long DelayInitCaret;
    eTextinputMotionPattern MotionPauseCaret;
    unsigned long PitchPauseCaret;
    unsigned long DelayPauseCaret;
    eTextinputMotionPattern MotionMoveCaret;
    unsigned long PitchMoveCaret;
    float CaretWidth;

    D2D1_COLOR_F ColorFrame;
    D2D1_COLOR_F ColorBg;
    D2D1_COLOR_F ColorText;
    D2D1_COLOR_F ColorCaret;
    D2D1_COLOR_F ColorSelect;

public:
    UI_Textinput_MotionParam() {
        MotionInitBg = eTextinputMotionPattern::eInitBg_Default;
        PitchInitBg = 0;
        DelayInitBg = 0;
        MotionPauseBg = eTextinputMotionPattern::ePauseBg_Default;
        PitchPauseBg = 0;
        DelayPauseBg = 0;

        MotionInitFrame = eTextinputMotionPattern::eInitFrame_Default;
        PitchInitFrame = 0;
        DelayInitFrame = 0;
        MotionPauseFrame = eTextinputMotionPattern::ePauseFrame_Default;
        PitchPauseFrame = 0;
        DelayPauseFrame = 0;

        MotionInitText = eTextinputMotionPattern::eInitText_Default;
        PitchInitText = 0;
        DelayInitText = 0;
        MotionPauseText = eTextinputMotionPattern::ePauseText_Default;
        PitchPauseText = 0;
        DelayPauseText = 0;

        MotionInitCaret = eTextinputMotionPattern::eInitCaret_Default;
        PitchInitCaret = 0;
        DelayInitCaret = 0;
        MotionPauseCaret = eTextinputMotionPattern::ePauseCaret_Default;
        PitchPauseCaret = 0;
        DelayPauseCaret = 0;

        ColorFrame = { 0.f,0.f,0.f,1.f };
        ColorBg = { 1.f, 1.f, 1.f, 1.f };
        ColorText = { 0,0,0,1 };
        ColorCaret = { 0,0,0,1 };
        ColorSelect = { 0.2f, 0.56f, 1.f, 1.f };
        CaretWidth = 2;
    }
};

class UI_Textinput : public UI {
public:
    UI_Textinput_MotionParam Motion;

private:
    PropBox* pBoxBg;
    PropBox* pBoxFrame;
    PropBox* pBoxCaret;
    PropTextLayout* pTextLayout;
    ID2D1SolidColorBrush* pSelectBrush;
    IDWriteTextFormat* pTextFormat;
    BOOL DragState;
    int CaretIdx;
    BOOL CaretTrail;
    float CaretX, CaretY;
    float CaretH;
    int StartSelectIdx;
    std::vector<DWRITE_LINE_METRICS> LineMetric;
    DWRITE_TEXT_METRICS TextMet;
    BOOL ImeCompBoot;

private:
    static void DefaultTextinputProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam);

    void ResumeFrame(unsigned long Delay);
    void PauseFrame(unsigned long Delay);
    void ResumeBg(unsigned long Delay);
    void PauseBg(unsigned long Delay);
    void ResumeCaret(unsigned long Delay);
    void PauseCaret(unsigned long Delay);
    void MoveCaret(float CaretHeight, BOOL bMotion);
    void ResumeTextlayout(unsigned long Delay);
    void PauseTextlayout(unsigned long Delay);

    void DrawSelectArea();
    BOOL DeleteSelectArea();
    void OnKeyInput(UINT Message, WPARAM wParam, LPARAM lParam);

public: /*반드시 있어야 되는 매서드*/
    UI_Textinput(UISystem* pUISys, pfnUIHandler pfnCallback, POSITION Pos, IDWriteTextFormat* pTextFmt, UI_Textinput_MotionParam MotionParam = UI_Textinput_MotionParam());
    ~UI_Textinput();    
    void pause(int nDelay);
    void resume(int nDelay);
    BOOL update(unsigned long time);
    void render();

public: /*UI별 옵션 매서드*/
    void SetText(wchar_t* pStr, int nDelay = 0);
    void GetText(wchar_t* pBuffer, size_t BufferLen);
};
