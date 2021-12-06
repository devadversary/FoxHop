#pragma once

#include "ui_base.hpp"

class UISystem; /*UI�ý��� Ŭ������ ���漱��*/

/**
    @brief ��ư ���� ��� Ÿ��
*/
enum class eButtonMotionInit {
    eMotionDefault = 0, /**< ��� ����*/
    eMotionReload,      /**< ���� ���*/
    eMotionFlick        /**< �ø�Ŀ�� ���*/
};

/**
    @brief ��ư �Ҹ� ��� Ÿ��
*/
enum class eButtonMotionPause {
    eMotionDefault = 0, /**< ��� ����*/
    eMotionFlick        /**< �ø�Ŀ�� ���*/
};

/**
    @brief ��ư ���콺 ���� ��� Ÿ��
*/
enum class eButtonMotionMouseover {
    eMotionDefault = 0, /**< ��� ����*/
    eMotionFlick        /**< �ø�Ŀ�� ���*/
};

/**
    @brief ��ư Ŭ�� ��� Ÿ��
*/
enum class eButtonMotionClick {
    eMotionDefault = 0, /**< ��� ����*/
    eMotionFlash        /**< ���� ���*/
};

/**
    @brief ��ư ���� ���� ��� Ÿ��
*/
enum class eButtonMotionColor {
    eMotionDefault = 0, /**< ��� ����*/
    eMotionFlash        /**< ���� ���*/
};

/**
    @brief ��ư �ؽ�Ʈ ��� Ÿ��
*/
enum class eButtonMotionText {
    eMotionDefault = 0 /**< ��� ����*/
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
    eButtonMotionInit      Init;           /**< ���۸��*/
    unsigned int           InitPitch;      /**< ���۸�� ��ġ*/

    eButtonMotionPause     Pause;          /**< �Ҹ���*/
    unsigned int           PausePitch;     /**< �Ҹ��� ��ġ*/

    eButtonMotionMouseover MouseOver;      /**< ���콺���� ���*/
    unsigned int           MouseOverPitch; /**< ���콺������� ��ġ*/

    eButtonMotionClick     Click;          /**< Ŭ�� ���*/
    unsigned int           ClickPitch;     /**< Ŭ�� ��ġ*/

    eButtonMotionColor     Color;          /**< ���� ���� ���*/
    unsigned int           ColorPitch;     /**< ���� ���� ��ġ*/

    eButtonMotionText      Text;           /**< �ؽ�Ʈ ���� ���*/
    unsigned int           TextPitch;      /**< �ؽ�Ʈ ���� ��ġ*/
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
    @remark ������ �ظ��ϸ� ���丮���� ���ش�.
*/
class UI_Button : public UI {
private:
    ObjectMotionBox*              MBoxFace;       /**< ���� ��� �ڽ�*/
    ObjectMotionBox*              MBoxHighlight;  /**< ���̶���Ʈ ��� �ڽ�*/
    ObjectMotionText*             MText;          /**< ��� �ڽ� �⺻ �ؽ�Ʈ*/
    int                           nTextLen;
    D2D1_COLOR_F                  FrameColor;     /**< �ʱ� �׵θ� ��*/
    D2D1_COLOR_F                  FaceColor;      /**< �ʱ� ��ư �����*/
    D2D1_COLOR_F                  HighlightColor; /**< ��ư ���̶���Ʈ ��*/
    D2D1_COLOR_F                  FontColor;      /**< �ʱ� �ؽ�Ʈ ��*/
    wchar_t                       szText[MAX_BUTTONNAME];
    void InputMotion(eButtonMotionInit Motion, int nDelay);

public:
    BUTTON_MOTION_SET             MotionSet;      /**< ��� ����*/
    BUTTON_COLOR_SET              ColorSet;       /**< ���� ����*/

public: /*�ݵ�� �־�� �Ǵ� �ż���*/
    void CreateUI(UISystem* pUISys, ID2D1RenderTarget* pRT,
                  BUTTON_MOTION_SET UIMotionSet, BUTTON_COLOR_SET UIColorSet,
                  pfnUIHandler pfnCallback, POSITION Pos, int nID, wchar_t* pText, int nDelay);
    
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
