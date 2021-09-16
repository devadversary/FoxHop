#pragma once

#include "ui_base.hpp"
#include "ui_system.hpp"

#define MAX_BUTTONNAME 32 /*��ư�� �ִ� �ؽ�Ʈ ����*/

class UISystem; /*UI�ý��� Ŭ������ ���漱��*/

/**
    @brief ��ư ���� ��� Ÿ��
*/
enum class eButtonMotionInit {
    eMotionReload = 0, /**< ���� ���*/
    eMotionFlick       /**< �ø�Ŀ�� ���*/
};

/**
    @brief ��ư �Ҹ� ��� Ÿ��
*/
enum class eButtonMotionPause {
    eMotionFlick = 0       /**< �ø�Ŀ�� ���*/
};

/**
    @brief ��ư ���콺 ���� ��� Ÿ��
*/
enum class eButtonMotionMouseover {
    eMotionFlick = 0       /**< �ø�Ŀ�� ���*/
};

/**
    @brief ��ư Ŭ�� ��� Ÿ��
*/
enum class eButtonMotionClick {
    eMotionFlash = 0       /**< ���� ���*/
};

/**
    @brief ��ư ���� ���� ��� Ÿ��
*/
enum class eButtonMotionColor {
    eMotionFlash = 0       /**< ���� ���*/
};

/**
    @brief ��ư �ؽ�Ʈ ��� Ÿ��
*/
enum class eButtonMotionText {
    eMotionDefault = 0       /**< ��� ����*/
};


/**
    @brief ��� ����
    @remark ��ü�� �ż��带 ���� ���� �� �� ����
*/
typedef struct _stButtonMotionSet {
    eButtonMotionInit      Init;      /**< ���۸��*/
    eButtonMotionPause     Pause;     /**< �Ҹ���*/
    eButtonMotionMouseover MouseOver; /**< ���콺���� ���*/
    eButtonMotionClick     Click;     /**< Ŭ�� ���*/
    eButtonMotionColor     Color;     /**< ���� ���� ���*/
    eButtonMotionText      Text;      /**< �ؽ�Ʈ ���� ���*/
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

/**
    @brief ��ư ���� ���丮
*/
class UI_ButtonFactory {
private:
    BUTTON_MOTION_SET MotionSet; /**< ��� ����*/
    BUTTON_COLOR_SET  ColorSet;  /**< ���� ����*/
};

class UI_Button : public UI {
private:
    ObjectPool<ObjectMotionBox>*  PoolBox;
    ObjectPool<ObjectMotionText>* PoolText;
    BUTTON_MOTION_SET             MotionSet;      /**< ��� ����*/
    BUTTON_COLOR_SET              ColorSet;       /**< ���� ����*/
    ObjectMotionBox*              MBoxFace;       /**< ���� ��� �ڽ�*/
    ObjectMotionBox*              MBoxHighlight;  /**< ���̶���Ʈ ��� �ڽ�*/
    ObjectMotionText*             MText;          /**< ��� �ڽ� �⺻ �ؽ�Ʈ*/
    int                           nTextLen;
    D2D1_COLOR_F                  FrameColor;     /**< �ʱ� �׵θ� ��*/
    D2D1_COLOR_F                  FaceColor;      /**< �ʱ� ��ư �����*/
    D2D1_COLOR_F                  HighlightColor; /**< ��ư ���̶���Ʈ ��*/
    D2D1_COLOR_F                  FontColor;      /**< �ʱ� �ؽ�Ʈ ��*/
    wchar_t                       szText[MAX_BUTTONNAME];

public: /*�ݵ�� �־�� �Ǵ� �ż���*/
    UI_Button();
    UI_Button(UISystem* pUISys, ID2D1RenderTarget* pRT, int nID, pfnUIHandler pfnCallback);
    ~UI_Button();
    void preInit(UISystem* pUISys, ID2D1RenderTarget* pRT, int nID, pfnUIHandler pfnCallback);
    void CreateUI(UISystem* pUISys, ID2D1RenderTarget* pRT,
                  BUTTON_MOTION_SET UIMotionSet, BUTTON_COLOR_SET UIColorSet,
                  pfnUIHandler pfnCallback, POSITION Pos, int nID, wchar_t* pText, int nDelay);
    void Init(int Motion, POSITION Pos, D2D1_COLOR_F Color, wchar_t* pText, D2D1_COLOR_F TextColor, int nDelay = 0);
    void InputMotion(int Motion, int nDelay);
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
