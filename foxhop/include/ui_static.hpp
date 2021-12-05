#pragma once

#include "ui_base.hpp"

#define MAX_STATIC_TEXTLEN 256

class UISystem; /*UI�ý��� Ŭ������ ���漱��*/

/**
    @brief ����ƽ ���� ��� Ÿ��
*/
enum class eStaticMotionInit {
    eMotionDefault = 0,    /**< ��� ����*/
    eMotionSlideConst,     /**< ��� �����̵�*/
    eMotionSlideAcc,       /**< ���� �����̵�*/
    eMotionSlideDec,       /**< ���� �����̵�*/
    eMotionFlick           /**< �ø�Ŀ�� ���*/
};

/**
    @brief ��ư �Ҹ� ��� Ÿ��
*/
enum class eStaticMotionPause {
    eMotionDefault = 0, /**< ��� ����*/
    eMotionFlick        /**< �ø�Ŀ�� ���*/
};

class UI_Static : public UI {
private:
    ObjectMotionBox*              MBox;
    ObjectMotionText*             MText;
    D2D1_COLOR_F                  FrameColor;     /**< �ʱ� �׵θ� ��*/
    D2D1_COLOR_F                  FaceColor;      /**< �ʱ� ��ư �����*/
    D2D1_COLOR_F                  FontColor;      /**< �ʱ� �ؽ�Ʈ ��*/
    wchar_t                       szText[MAX_STATIC_TEXTLEN];
    int                           nTextLen;
    void InputMotion(int Motion, int nDelay);

public:
    void CreateUI(UISystem* pUISys, ID2D1RenderTarget* pRT,
                  BUTTON_MOTION_SET UIMotionSet, BUTTON_COLOR_SET UIColorSet,
                  pfnUIHandler pfnCallback, POSITION Pos, int nID, wchar_t* pText, int nDelay);
    void pause(int nDelay);
    void resume(int nDelay);
    void Destroy();
    void update(unsigned long time);
    void render();
    
public: /*UI�� �ɼ� �ż���*/
    void setText(wchar_t* pText, int nDelay);
};
