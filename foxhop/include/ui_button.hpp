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

/**
    @brief ��ư Ŭ����
    @remark ������ ���丮���� ���ش�.
*/
class UI_Button : public UI {
public :
    eButtonMotionPattern ButtonInitMotion      = eButtonMotionPattern::eInit_Default;      /**< ���۸��*/
    eButtonMotionPattern ButtonPauseMotion     = eButtonMotionPattern::ePause_Default;     /**< �Ҹ���*/
    eButtonMotionPattern ButtonMouseOverMotion = eButtonMotionPattern::eMouseover_Default; /**< ���콺���� ���*/
    eButtonMotionPattern ButtonClickMotion     = eButtonMotionPattern::eText_Default;     /**< Ŭ�� ���*/
    eButtonMotionPattern ButtonColorMotion     = eButtonMotionPattern::eColor_Default;     /**< ���� ���� ���*/
    eButtonMotionPattern ButtonTextMotion      = eButtonMotionPattern::eText_Default;      /**< �ؽ�Ʈ ���� ���*/
    unsigned int         ButtonInitPitch       = 0; /**< ���۸�� ��ġ*/
    unsigned int         ButtonPausePitch      = 0; /**< �Ҹ��� ��ġ*/
    unsigned int         ButtonMouseOverPitch  = 0; /**< ���콺������� ��ġ*/
    unsigned int         ButtonClickPitch      = 0; /**< Ŭ�� ��ġ*/
    unsigned int         ButtonColorPitch      = 0; /**< ���� ���� ��ġ*/
    unsigned int         ButtonTextPitch       = 0; /**< �ؽ�Ʈ ���� ��ġ*/
    D2D1_COLOR_F         ButtonFrameColor      = { 0.f, 0.f, 0.f, 0.f };     /**< �ʱ� �׵θ� ��*/
    D2D1_COLOR_F         ButtonFaceColor       = { 0.8f, 0.8f, 0.8f, 1.f };  /**< �ʱ� ��ư �����*/
    D2D1_COLOR_F         ButtonMouseoverColor  = { 1.f,  1.f,  1.f,  0.5f }; /**< ���콺 ���� ��*/
    D2D1_COLOR_F         ButtonHighlightColor  = { 1.f,  1.f,  1.f,  1.f };  /**< ��ư ���̶���Ʈ ��*/
    D2D1_COLOR_F         ButtonFontColor       = { 0.f, 0.f, 0.f, 1.f };     /**< �ʱ� �ؽ�Ʈ ��*/

private:
    PropBox*  MBoxFace;      /**< ���� ��� �ڽ�*/
    PropBox*  MBoxHighlight; /**< ���̶���Ʈ ��� �ڽ�*/
    PropText* MText;         /**< ��� �ڽ� �⺻ �ؽ�Ʈ*/
    int       nTextLen;
    wchar_t   szText[MAX_BUTTONNAME];
    
private: /*2022.04.30 : Devadversary - InputMotion �� �⺻������ Private �Լ����������� �׷��� �Ǹ�, DefaultButtonProc �Լ����� ������ ����.*/
    void InputMotion(eButtonAction Action, unsigned int nDelay, void* param);
    static void DefaultButtonProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam);

public:
    UI_Button(UISystem* pUISys, pfnUIHandler pfnCallback, POSITION Pos, wchar_t* pText, int nDelay);
    ~UI_Button() {};

    void pause(int nDelay);
    void resume(int nDelay);
    void Destroy();
    BOOL update(unsigned long time);
    void render();

public: /*UI�� �ɼ� �ż���*/
    void setText(wchar_t* pText, int nDelay);
    void setFaceColor(D2D1_COLOR_F Color, MOTION_INFO miColor);
    void setHighlightColor(D2D1_COLOR_F Color, MOTION_INFO miColor);
};
