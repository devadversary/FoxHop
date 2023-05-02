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
};

class UI_Static : public UI {
public:
    eStaticMotionPattern MotionInit  = eStaticMotionPattern::eInit_Default;
    eStaticMotionPattern MotionPause = eStaticMotionPattern::ePause_Default;
    eStaticMotionPattern MotionText  = eStaticMotionPattern::eText_Default;
    D2D1_COLOR_F ColorFrame = { 0,0,0,1}; /**< �׵θ� ��*/
    D2D1_COLOR_F ColorBg  = { 0.94f, 0.94f, 0.94f, 1.f }; /**< �����*/
    D2D1_COLOR_F ColorFont  = { 0,0,0,1 }; /**< �ؽ�Ʈ ��*/
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

public: /*�ݵ�� �־�� �Ǵ� �ż���*/
    UI_Static(UISystem* pUISys, pfnUIHandler pfnCallback, POSITION Pos, wchar_t* Text);
    ~UI_Static();
    void pause(int nDelay);
    void resume(int nDelay);
    BOOL update(unsigned long time);
    void render();

public: /*UI�� �ɼ� �ż���*/
    void SetText(wchar_t* pStr, int nDelay = 0);
};
