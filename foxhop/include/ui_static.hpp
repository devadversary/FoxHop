#pragma once

#include "ui_base.hpp"

#define UISTATIC_MAX_TEXTLEN 1024

/**
    @brief ����ƽ ��� Ÿ��
*/
enum class eStaticMotionPattern {
    eInit_Default = 0,   /**< ���� : ��� ����*/

    ePause_Default = 0,  /**< �Ҹ� : ��� ����*/

    eText_Default = 0,   /**< �ؽ�Ʈ : ��� ����*/
    eText_Flick,   /**< �ؽ�Ʈ : ��� ����*/
};

class UI_Static_MotionParam {
public:
    eStaticMotionPattern MotionInit;
    eStaticMotionPattern MotionPause;
    eStaticMotionPattern MotionText;
    D2D1_COLOR_F ColorFrame;
    D2D1_COLOR_F ColorBg;
    D2D1_COLOR_F ColorFont;
    unsigned long PitchInit;
    unsigned long PitchPause;
    unsigned long PitchText;

public:
    UI_Static_MotionParam() {
        MotionInit = eStaticMotionPattern::eInit_Default;
        MotionPause = eStaticMotionPattern::ePause_Default;
        MotionText = eStaticMotionPattern::eText_Default;
        ColorFrame = { 0,0,0,1 };
        ColorBg = { 0.94f, 0.94f, 0.94f, 1.f };
        ColorFont = { 0,0,0,1 };
        PitchInit = 0;
        PitchPause = 0;
        PitchText = 0;
    }
};

class UI_Static : public UI {
public: 
    UI_Static_MotionParam Motion;

private:
    wchar_t szText[UISTATIC_MAX_TEXTLEN];
    PropText* pText;
    PropBox*  pBoxBg;
    PropBox*  pBoxFrame;

private:
    static void DefaultStaticProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam);

public: /*�ݵ�� �־�� �Ǵ� �ż���*/
    UI_Static(UISystem* pUISys, pfnUIHandler pfnCallback, POSITION Pos, wchar_t* Text, UI_Static_MotionParam MotionParam = UI_Static_MotionParam());
    ~UI_Static();
    void pause(int nDelay);
    void resume(int nDelay);
    BOOL update(unsigned long time);
    void render();

public: /*UI�� �ɼ� �ż���*/
    void SetText(wchar_t* pStr, int nDelay = 0);
};
