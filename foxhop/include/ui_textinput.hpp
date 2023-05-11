#pragma once

#include "ui_base.hpp"
#include "ui_system.hpp"
#include <vector>

/**
    @brief 스태틱 모션 타입
*/
enum class eTextinputMotionPattern {
    eBgInit_Default = 0,   /**< 생성 : 모션 없음*/
    eBgPause_Default = 100,

    eFrameInit_Default = 200,  /**< 소멸 : 모션 없음*/
    eFramePause_Default = 300,  /**< 소멸 : 모션 없음*/

    eTextInit_Default = 400,   /**< 텍스트 : 모션 없음*/
    eTextPause_Default = 500,

    eCaretInit_Default = 600,

    eCaretMove_Default = 700,
    eCaretMove_Decel,

    eCaretPause_Default = 800,
};

class UI_Textinput_MotionParam {
public:
    eTextinputMotionPattern MotionBgInit;
    eTextinputMotionPattern MotionBgPause;
    eTextinputMotionPattern MotionFrameInit;
    eTextinputMotionPattern MotionFramePause;
    eTextinputMotionPattern MotionTextInit;
    eTextinputMotionPattern MotionTextPause;
    eTextinputMotionPattern MotionCaretInit;
    eTextinputMotionPattern MotionCaretMove;
    eTextinputMotionPattern MotionCaretPause;
    D2D1_COLOR_F ColorFrame;
    D2D1_COLOR_F ColorBg;
    D2D1_COLOR_F ColorFont;
    D2D1_COLOR_F ColorCaret;
    D2D1_COLOR_F ColorSelect;
    unsigned long PitchInit;
    unsigned long PitchPause;
    unsigned long PitchText;
    unsigned long PitchCaretMove;
    float CaretWidth;

public:
    UI_Textinput_MotionParam() {
        MotionBgInit = eTextinputMotionPattern::eBgInit_Default;
        MotionBgPause = eTextinputMotionPattern::eBgPause_Default;
        MotionFrameInit = eTextinputMotionPattern::eFrameInit_Default;
        MotionFramePause = eTextinputMotionPattern::eFramePause_Default;
        MotionTextInit = eTextinputMotionPattern::eTextInit_Default;
        MotionTextPause = eTextinputMotionPattern::eTextPause_Default;
        MotionCaretInit = eTextinputMotionPattern::eCaretInit_Default;
        MotionCaretMove = eTextinputMotionPattern::eCaretMove_Default;
        MotionCaretPause = eTextinputMotionPattern::eCaretPause_Default;
        ColorFrame = { 0.f,0.f,0.f,1.f };
        ColorBg = { 1.f, 1.f, 1.f, 1.f };
        ColorFont = { 0,0,0,1 };
        ColorCaret = { 0,0,0,1 };
        ColorSelect = { 0.2f, 0.56f, 1.f, 1.f };
        PitchInit = 0;
        PitchPause = 0;
        PitchText = 0;
        PitchCaretMove = 0;
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
    std::wstring Str;
    ID2D1SolidColorBrush* pTextBrush;
    ID2D1SolidColorBrush* pSelectBrush;
    IDWriteTextFormat* pTextFormat;
    IDWriteTextLayout* pLayout;
    BOOL DragState;
    int CaretIdx;
    BOOL CaretTrail;
    float CaretX, CaretY;
    int StartSelectIdx;
    std::vector<DWRITE_LINE_METRICS> LineMetric;
    DWRITE_TEXT_METRICS TextMet;
    BOOL ImeCompBoot;

private:
    static void DefaultTextinputProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam);
    void SetFrame(D2D1_COLOR_F Color, BOOL bMotion);
    void PauseFrame(unsigned long Delay);
    void SetBg(D2D1_COLOR_F Color, BOOL bMotion);
    void PauseBg(unsigned long Delay);
    void SetCaret(float CaretHeight, BOOL bMotion);
    void PauseCaret(unsigned long Delay);
    void SetTextlayout(D2D1_COLOR_F Color, BOOL bMotion);
    void PauseTextlayout(D2D1_COLOR_F Color, BOOL bMotion);
    void UpdateTextLayout();
    void DrawSelectArea();
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
