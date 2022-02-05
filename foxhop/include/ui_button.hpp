#pragma once

#include "ui_base.hpp"

class UISystem; /*UI�ý��� Ŭ������ ���漱��*/

/**
    @brief ��ư ��� Ÿ�� ���
*/
enum class eButtonMotionPattern {
    eMotionInit_Default = 0,      /**< ����: ��� ����*/
    eMotionInit_Reload,           /**< ����: ���� ���*/
    eMotionInit_Flick,            /**< ����: �ø�Ŀ�� ���*/

    eMotionPause_Default = 0,     /**< �Ҹ�: ��� ����*/
    eMotionPause_Flick,           /**< �Ҹ�: �ø�Ŀ�� ���*/

    eMotionMouseover_Default = 0, /**< ���콺����: ��� ����*/
    eMotionMouseover_Flick,       /**< ���콺����: �ø�Ŀ�� ���*/

    eMotionClick_Default = 0,     /**< Ŭ��: ��� ����*/
    eMotionClick_Flash,           /**< Ŭ��: ���� ���*/

    eMotionColor_Default = 0,     /**< ���󺯰�: ��� ����*/
    eMotionColor_Flash,           /**< ���󺯰�: ���� ���*/

    eMotionText_Default = 0       /**< �ؽ�Ʈ: ��� ����*/
};

/**
    @brief ��ư ��� Ÿ��
*/
enum class eButtonMotionType {
    eMotionType_Init = 0,   /**< �ʱ�ȭ ���*/
    eMotionType_Pause,      /**< �Ҹ� ���*/
    eMotionType_Mouseover,  /**< ���콺 ���� ���*/
    eMotionType_Click,      /**< Ŭ�� ���*/
    eMotionType_Color,      /**< ���� ���� ���*/
    eMotionType_Text        /**< �ؽ�Ʈ ���*/
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
    void InputMotion(eButtonMotionType MotionType, eButtonMotionPattern Pattern, unsigned int nDelay, unsigned int nPitch);

    BUTTON_MOTION_SET             MotionSet;      /**< ��� ����*/
    BUTTON_COLOR_SET              ColorSet;       /**< ���� ����*/

public:
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
