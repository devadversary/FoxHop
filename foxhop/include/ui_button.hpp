#pragma once

#include "ui_base.hpp"

class UISystem; /*UI�ý��� Ŭ������ ���漱��*/

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
enum class eButtonMotionType {
    eType_Init = 0,  /**< �ʱ�ȭ ���*/
    eType_Pause,     /**< �Ҹ� ���*/
    eType_Mouseover, /**< ���콺 ���� ��� (Ŀ���� ���������� ����)*/
    eType_Click,     /**< Ŭ�� ���*/
    eType_Color,     /**< ���� ���� ���*/
    eType_Text       /**< �ؽ�Ʈ ���*/
};

/**
    @brief ��� ����
    @remark ��ü�� �ż��带 ���� ���� �� �� ����
*/
typedef struct _stButtonMotionSet {
    eButtonMotionPattern Init;           /**< ���۸��*/
    unsigned int         InitPitch;      /**< ���۸�� ��ġ*/

    eButtonMotionPattern Pause;          /**< �Ҹ���*/
    unsigned int         PausePitch;     /**< �Ҹ��� ��ġ*/

    eButtonMotionPattern MouseOver;      /**< ���콺���� ���*/
    unsigned int         MouseOverPitch; /**< ���콺������� ��ġ*/

    eButtonMotionPattern Click;          /**< Ŭ�� ���*/
    unsigned int         ClickPitch;     /**< Ŭ�� ��ġ*/

    eButtonMotionPattern Color;          /**< ���� ���� ���*/
    unsigned int         ColorPitch;     /**< ���� ���� ��ġ*/

    eButtonMotionPattern Text;           /**< �ؽ�Ʈ ���� ���*/
    unsigned int         TextPitch;      /**< �ؽ�Ʈ ���� ��ġ*/
}BUTTON_MOTION_SET;

/**
    @brief  ��ư ���� ����
    @remark ��ü�� �ż��带 ���� ���� �� �� ����
*/
typedef struct _stColorSet {
    D2D1_COLOR_F Frame;     /**< �ʱ� �׵θ� ��*/
    D2D1_COLOR_F Face;      /**< �ʱ� ��ư �����*/
    D2D1_COLOR_F Mouseover; /**< ���콺 ���� ��*/
    D2D1_COLOR_F Highlight; /**< ��ư ���̶���Ʈ ��*/
    D2D1_COLOR_F Font;      /**< �ʱ� �ؽ�Ʈ ��*/
}BUTTON_COLOR_SET;

#define MAX_BUTTONNAME 32 /*��ư�� �ִ� �ؽ�Ʈ ����*/

/**
    @brief ��ư Ŭ����
    @remark ������ ���丮���� ���ش�.
*/
class UI_Button : public UI {
private:
    ObjectMotionBox*              MBoxFace;       /**< ���� ��� �ڽ�*/
    ObjectMotionBox*              MBoxHighlight;  /**< ���̶���Ʈ ��� �ڽ�*/
    ObjectMotionText*             MText;          /**< ��� �ڽ� �⺻ �ؽ�Ʈ*/
    int                           nTextLen;
    wchar_t                       szText[MAX_BUTTONNAME];
    BUTTON_MOTION_SET             MotionSet;      /**< ��� ����*/
    BUTTON_COLOR_SET              ColorSet;       /**< ���� ����*/
    
private: /*2022.04.30 : Devadversary - InputMotion �� �⺻������ Private �Լ����������� �׷��� �Ǹ�, DefaultButtonProc �Լ����� ������ ����.*/
    void InputMotion(eButtonMotionType MotionType, eButtonMotionPattern Pattern, unsigned int nDelay, unsigned int nPitch, void* param);
    static void DefaultButtonProc(UI* pUI, UINT Message, void* param);

public: /*�ݵ�� �־�� �Ǵ� �ż���*/
    void CreateUI(UISystem* pUISys, int nID, ID2D1RenderTarget* pRT,
                  BUTTON_MOTION_SET UIMotionSet, BUTTON_COLOR_SET UIColorSet,
                  pfnUIHandler pfnCallback, POSITION Pos, wchar_t* pText, int nDelay);
    
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

/**
    @brief ��ư ���� ���丮
*/
class UI_ButtonFactory {
private:
    BUTTON_MOTION_SET  MotionSet;     /**< ��� ����*/
    BUTTON_COLOR_SET   ColorSet;      /**< ���� ����*/
    UISystem*          pUISys;        /**< UI�ý��� �ν��Ͻ�*/
    ID2D1RenderTarget* pRenderTarget; /**< ������ Ÿ��*/

public:
    void Init(UISystem* pUISystem, ID2D1RenderTarget* pRT);
    void GetCurrentMotionSet(BUTTON_MOTION_SET* pMotionSet);
    void SetCurrentMotionSet(BUTTON_MOTION_SET* pMotionSet);
    void GetCurrentColorSet(BUTTON_COLOR_SET* pColorSet);
    void SetCurrentColorSet(BUTTON_COLOR_SET* pColorSet);
    UI*  CreateUI(int nID, POSITION Pos, wchar_t* pText, int nDelay, pfnUIHandler pfnCallback);
};
