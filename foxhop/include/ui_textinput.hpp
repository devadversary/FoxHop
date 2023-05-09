#pragma once

#include "ui_base.hpp"
#include "ui_system.hpp"

/**
    @brief ����ƽ ��� Ÿ��
*/
enum class eTextinputMotionPattern {
    eBgInit_Default = 0,   /**< ���� : ��� ����*/
    eBgPause_Default = 100,

    eFrameInit_Default = 200,  /**< �Ҹ� : ��� ����*/
    eFramePause_Default = 300,  /**< �Ҹ� : ��� ����*/

    eTextInit_Default = 400,   /**< �ؽ�Ʈ : ��� ����*/
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
    unsigned long PitchInit;
    unsigned long PitchPause;
    unsigned long PitchText;
    unsigned long PitchCaretMove;

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
        PitchInit = 0;
        PitchPause = 0;
        PitchText = 0;
        PitchCaretMove = 0;
    }
};

class UI_Textinput : public UI {
public:
    UI_Textinput_MotionParam Motion;

private:
    //wchar_t szText[UISTATIC_MAX_TEXTLEN];
    //PropText* pText;
    PropBox* pBoxBg;
    PropBox* pBoxFrame;
    PropBox* pBoxCaret;
    std::wstring Str;
    ID2D1SolidColorBrush* pTextBrush;
    IDWriteTextFormat* pTextFormat;
    IDWriteTextLayout* pLayout;
    BOOL DragState;
    long long CaretIdx;

private:
    static void DefaultTextinputProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam);
    void SetFrame(D2D1_COLOR_F Color, BOOL bMotion);
    void PauseFrame(unsigned long Delay);
    void SetBg(D2D1_COLOR_F Color, BOOL bMotion);
    void PauseBg(unsigned long Delay);
    void SetCaret(POSITION CaretPos, BOOL bMotion);
    void PauseCaret(unsigned long Delay);
    void SetTextlayout(D2D1_COLOR_F Color, BOOL bMotion);
    void UpdateTextLayout();
public: /*�ݵ�� �־�� �Ǵ� �ż���*/
    UI_Textinput(UISystem* pUISys, pfnUIHandler pfnCallback, POSITION Pos, IDWriteTextFormat* pTextFmt, UI_Textinput_MotionParam MotionParam = UI_Textinput_MotionParam());
    ~UI_Textinput();    
    void pause(int nDelay);
    void resume(int nDelay);
    BOOL update(unsigned long time);
    void render();

public: /*UI�� �ɼ� �ż���*/
    void SetText(wchar_t* pStr, int nDelay = 0);
    void GetText(wchar_t* pBuffer, size_t BufferLen);
};