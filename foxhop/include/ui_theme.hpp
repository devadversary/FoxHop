#pragma once

#include "ui_base.hpp"

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

#define MAX_BUTTONNAME 32 /*��ư�� �ִ� �ؽ�Ʈ ����*/

/**
    @brief UI���� �׸��� ���� ����
*/
class UITheme {
public:
    /*UI_Button �� �Ӽ�*/
    eButtonMotionPattern ButtonInitMotion;      /**< ���۸��*/
    eButtonMotionPattern ButtonPauseMotion;     /**< �Ҹ���*/
    eButtonMotionPattern ButtonMouseOverMotion; /**< ���콺���� ���*/
    eButtonMotionPattern ButtonClickMotion;     /**< Ŭ�� ���*/
    eButtonMotionPattern ButtonColorMotion;     /**< ���� ���� ���*/
    eButtonMotionPattern ButtonTextMotion;      /**< �ؽ�Ʈ ���� ���*/
    unsigned int         ButtonInitPitch;       /**< ���۸�� ��ġ*/
    unsigned int         ButtonPausePitch;      /**< �Ҹ��� ��ġ*/
    unsigned int         ButtonMouseOverPitch;  /**< ���콺������� ��ġ*/
    unsigned int         ButtonClickPitch;      /**< Ŭ�� ��ġ*/
    unsigned int         ButtonColorPitch;      /**< ���� ���� ��ġ*/
    unsigned int         ButtonTextPitch;       /**< �ؽ�Ʈ ���� ��ġ*/
    D2D1_COLOR_F         ButtonFrameColor;      /**< �ʱ� �׵θ� ��*/
    D2D1_COLOR_F         ButtonFaceColor;       /**< �ʱ� ��ư �����*/
    D2D1_COLOR_F         ButtonMouseoverColor;  /**< ���콺 ���� ��*/
    D2D1_COLOR_F         ButtonHighlightColor;  /**< ��ư ���̶���Ʈ ��*/
    D2D1_COLOR_F         ButtonFontColor;       /**< �ʱ� �ؽ�Ʈ ��*/

public:
    UITheme() {
        ButtonInitMotion = eButtonMotionPattern::eInit_Default;
        ButtonPauseMotion = eButtonMotionPattern::ePause_Default;
        ButtonMouseOverMotion = eButtonMotionPattern::eMouseover_Default;
        ButtonClickMotion = eButtonMotionPattern::eText_Default;
        ButtonColorMotion = eButtonMotionPattern::eColor_Default;
        ButtonTextMotion = eButtonMotionPattern::eText_Default;
        ButtonInitPitch = 0;
        ButtonPausePitch = 0;
        ButtonMouseOverPitch = 0;
        ButtonClickPitch = 0;
        ButtonColorPitch = 0;
        ButtonTextPitch = 0;
        ButtonFrameColor = ALL_ZERO;
        ButtonFaceColor = { 0.8f, 0.8f, 0.8f, 1.f };  /*���� ȸ��*/
        ButtonMouseoverColor = { 1.f,  1.f,  1.f,  0.5f }; /*������ ���*/;
        ButtonHighlightColor = { 1.f,  1.f,  1.f,  1.f }; /*���*/
        ButtonFontColor = { 0.f, 0.f, 0.f, 1.f};
    }
    ~UITheme() {}
};