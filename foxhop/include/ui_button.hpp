#pragma once

#include "ui_base.hpp"

class UISystem; /*UI�ý��� Ŭ������ ���漱��*/

#define MAX_BUTTONNAME 32 /*��ư�� �ִ� �ؽ�Ʈ ����*/

/**
    @brief ��ư ��� Ÿ�� ���
*/
enum class eButtonMotionPattern {
    eInit_Default = 0,      /**< ����: ��� ����*/
    eInit_Reload,           /**< ����: ���� ���*/
    eInit_Flick,            /**< ����: �ø�Ŀ�� ���*/

    ePause_Default = 0,     /**< �Ҹ�: ��� ����*/
    ePause_Flick,           /**< �Ҹ�: �ø�Ŀ�� ���*/

    eMouseover_Default = 0, /**< ���콺����: ��� ����*/
    eMouseover_Flick,       /**< ���콺����: �ø�Ŀ�� ���*/

    eClick_Default = 0,     /**< Ŭ��: ��� ����*/
    eClick_Flash,           /**< Ŭ��: ���� ���*/

    eColor_Default = 0,     /**< ���󺯰�: ��� ����*/
    eColor_Flash,           /**< ���󺯰�: ���� ���*/

    eText_Default = 0       /**< �ؽ�Ʈ: ��� ����*/
};

/**
    @brief ��ư ��� Ÿ��
*/
enum class eButtonAction {
    eAction_Init = 0,  /**< �ʱ�ȭ ���*/
    eAction_Pause,     /**< �Ҹ� ���*/
    eAction_Mouseover, /**< ���콺 ���� ��� (Ŀ���� ���������� ����)*/
    eAction_Click,     /**< Ŭ�� ���*/
    eAction_Color,     /**< ���� ���� ���*/
    eAction_Text       /**< �ؽ�Ʈ ���*/
};

class UI_Button_MotionParam {
public:
    eButtonMotionPattern InitMotion;
    eButtonMotionPattern PauseMotion;
    eButtonMotionPattern MouseoverMotion;
    eButtonMotionPattern ClickMotion;
    eButtonMotionPattern ColorMotion;
    eButtonMotionPattern TextMotion;
    unsigned int         InitPitch;
    unsigned int         PausePitch;
    unsigned int         MouseoverPitch;
    unsigned int         ClickPitch;
    unsigned int         ColorPitch;
    unsigned int         TextPitch;
    D2D1_COLOR_F         FrameColor;
    D2D1_COLOR_F         FaceColor;
    D2D1_COLOR_F         MouseoverColor;
    D2D1_COLOR_F         HighlightColor;
    D2D1_COLOR_F         FontColor;

public:
    UI_Button_MotionParam() {
        InitMotion = eButtonMotionPattern::eInit_Default;      /**< ���۸��*/
        PauseMotion = eButtonMotionPattern::ePause_Default;     /**< �Ҹ���*/
        MouseoverMotion = eButtonMotionPattern::eMouseover_Default; /**< ���콺���� ���*/
        ClickMotion = eButtonMotionPattern::eText_Default;     /**< Ŭ�� ���*/
        ColorMotion = eButtonMotionPattern::eColor_Default;     /**< ���� ���� ���*/
        TextMotion = eButtonMotionPattern::eText_Default;      /**< �ؽ�Ʈ ���� ���*/
        InitPitch = 0; /**< ���۸�� ��ġ*/
        PausePitch = 0; /**< �Ҹ��� ��ġ*/
        MouseoverPitch = 0; /**< ���콺������� ��ġ*/
        ClickPitch = 0; /**< Ŭ�� ��ġ*/
        ColorPitch = 0; /**< ���� ���� ��ġ*/
        TextPitch = 0; /**< �ؽ�Ʈ ���� ��ġ*/
        FrameColor = { 0.f, 0.f, 0.f, 0.f };     /**< �ʱ� �׵θ� ��*/
        FaceColor = { 0.9f, 0.9f, 0.9f, 1.f };  /**< �ʱ� ��ư �����*/
        MouseoverColor = { 1.f,  1.f,  1.f,  0.5f }; /**< ���콺 ���� ��*/
        HighlightColor = { 1.f,  1.f,  1.f,  1.f };  /**< ��ư ���̶���Ʈ ��*/
        FontColor = { 0.f, 0.f, 0.f, 1.f };     /**< �ʱ� �ؽ�Ʈ ��*/
    }
};
/**
    @brief ��ư Ŭ����
    @remark ������ ���丮���� ���ش�.
*/
class UI_Button : public UI {
public :
    UI_Button_MotionParam Motion;
private:
    PropBox*  MBoxFace;      /**< ���� ��� �ڽ�*/
    PropBox*  MBoxHighlight; /**< ���̶���Ʈ ��� �ڽ�*/
    PropText* MText;         /**< ��� �ڽ� �⺻ �ؽ�Ʈ*/
    int       nTextLen;
    wchar_t   szText[MAX_BUTTONNAME];
    
private:
    void InputMotion(eButtonAction Action, unsigned int nDelay, void* param);
    static void DefaultButtonProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam);

public:
    UI_Button(UISystem* pUISys, pfnUIHandler pfnCallback, POSITION Pos, wchar_t* pText, int nDelay, UI_Button_MotionParam MotionParam = UI_Button_MotionParam());
    ~UI_Button();

    void pause(int nDelay);
    void resume(int nDelay);
    BOOL update(unsigned long time);
    void render();

public: /*UI�� �ɼ� �ż���*/
    void setText(wchar_t* pText, int nDelay);
    void setFaceColor(D2D1_COLOR_F Color, MOTION_INFO miColor);
    void setHighlightColor(D2D1_COLOR_F Color, MOTION_INFO miColor);
};
