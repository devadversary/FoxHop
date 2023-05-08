#pragma once

#include "ui_base.hpp"
#include "ui_system.hpp"

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
};

class UI_Textinput_MotionParam {
public:
    eTextinputMotionPattern MotionBgInit;
    eTextinputMotionPattern MotionBgPause;
    eTextinputMotionPattern MotionFrameInit;
    eTextinputMotionPattern MotionFramePause;
    eTextinputMotionPattern MotionTextInit;
    eTextinputMotionPattern MotionTextPause;
    D2D1_COLOR_F ColorFrame;
    D2D1_COLOR_F ColorBg;
    D2D1_COLOR_F ColorFont;
    D2D1_COLOR_F ColorCaret;
    unsigned long PitchInit;
    unsigned long PitchPause;
    unsigned long PitchText;

public:
    UI_Textinput_MotionParam() {
        MotionBgInit = eTextinputMotionPattern::eBgInit_Default;
        MotionBgPause = eTextinputMotionPattern::eBgPause_Default;
        MotionFrameInit = eTextinputMotionPattern::eFrameInit_Default;
        MotionFramePause = eTextinputMotionPattern::eFramePause_Default;
        MotionTextInit = eTextinputMotionPattern::eTextInit_Default;
        MotionTextPause = eTextinputMotionPattern::eTextPause_Default;
        ColorFrame = { 0.f,0.f,0.f,1.f };
        ColorBg = { 1.f, 1.f, 1.f, 1.f };
        ColorFont = { 0,0,0,1 };
        ColorCaret = { 0,0,0,1 };
        PitchInit = 0;
        PitchPause = 0;
        PitchText = 0;
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
    //std::wstring Str;

private:
    static void DefaultTextinputProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam)
    {
        UI_Textinput* pInput = static_cast<UI_Textinput*>(pUI);
    }

    void SetFrame(D2D1_COLOR_F Color, BOOL bMotion)
    {
        eTextinputMotionPattern Patt;
        if (bMotion) Patt = eTextinputMotionPattern::eFrameInit_Default;
        else Patt = Motion.MotionFrameInit;

        switch (Patt) {
        case eTextinputMotionPattern::eFrameInit_Default:
            pBoxFrame->Init(uiPos, Color, FALSE);
            break;
        }
    }

    void PauseFrame(unsigned long Delay)
    {
        switch (Motion.MotionFramePause) {
        case eTextinputMotionPattern::eFramePause_Default:
            pBoxFrame->Init(uiPos, ALL_ZERO, FALSE);
            break;
        }
    }

    void SetBg(D2D1_COLOR_F Color, BOOL bMotion)
    {
        eTextinputMotionPattern Patt;
        if (bMotion) Patt = eTextinputMotionPattern::eBgInit_Default;
        else Patt = Motion.MotionBgInit;

        switch (Patt) {
        case eTextinputMotionPattern::eBgInit_Default:
            pBoxBg->Init(uiPos, Color);
            break;
        }
    }

    void PauseBg(unsigned long Delay)
    {
        switch (Motion.MotionBgPause) {
        case eTextinputMotionPattern::eBgPause_Default:
            pBoxBg->Init(uiPos, ALL_ZERO);
            break;
        }
    }

public: /*반드시 있어야 되는 매서드*/
    UI_Textinput(UISystem* pUISys, pfnUIHandler pfnCallback, POSITION Pos, UI_Textinput_MotionParam MotionParam = UI_Textinput_MotionParam())
    {
        uiSys = pUISys;
        pRenderTarget = pUISys->D2DA.pRenTarget;
        Focusable = TRUE;
        uiMotionState = eUIMotionState::eUMS_PlayingVisible;

        DefaultHandler = DefaultTextinputProc;
        MessageHandler = pfnCallback;
        uiPos = Pos;

        Motion = MotionParam;
        pBoxBg = new PropBox(pRenderTarget);
        pBoxFrame = new PropBox(pRenderTarget);
        resume(0);
    }

    ~UI_Textinput()
    {

    }
    
    void pause(int nDelay)
    {
        
    }
    
    void resume(int nDelay)
    {
        SetBg(Motion.ColorBg, TRUE);
        SetFrame(Motion.ColorFrame, TRUE);
    }

    BOOL update(unsigned long time)
    {
        BOOL bUpdated = FALSE;

        if (uiMotionState == eUIMotionState::eUMS_Hide) return FALSE;

        bUpdated |= pBoxFrame->update(time);
        bUpdated |= pBoxBg->update(time);
        //bUpdated |= pText->update(time);

        if (!bUpdated) {
            if (uiMotionState == eUIMotionState::eUMS_PlayingHide)
                uiMotionState = eUIMotionState::eUMS_Hide;

            else if (uiMotionState == eUIMotionState::eUMS_PlayingVisible)
                uiMotionState = eUIMotionState::eUMS_Visible;
            return FALSE;
        }
        return TRUE;
    }

    void render()
    {
        if (uiMotionState == eUIMotionState::eUMS_Hide) return;
        pBoxBg->render(pRenderTarget);
        pBoxFrame->render(pRenderTarget);
    }

public: /*UI별 옵션 매서드*/
    void SetText(wchar_t* pStr, int nDelay = 0);
    void GetText(wchar_t* pBuffer, size_t BufferLen);
};
