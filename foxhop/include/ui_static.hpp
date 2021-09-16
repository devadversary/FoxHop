#pragma once

#include "ui_base.hpp"
#include "ui_system.hpp"

#define MAX_STATIC_TEXTLEN 256

class UISystem; /*UI�ý��� Ŭ������ ���漱��*/

class UI_Static : public UI {
private:
    ObjectPool<ObjectMotionBox>*  PoolBox;
    ObjectPool<ObjectMotionText>* PoolText;
    ObjectMotionBox*              MBox;
    ObjectMotionText*             MText;
    D2D1_COLOR_F                  FrameColor;     /**< �ʱ� �׵θ� ��*/
    D2D1_COLOR_F                  FaceColor;      /**< �ʱ� ��ư �����*/
    D2D1_COLOR_F                  FontColor;      /**< �ʱ� �ؽ�Ʈ ��*/
    wchar_t                       szText[MAX_STATIC_TEXTLEN];
    int                           nTextLen;

public: /*�ݵ�� �־�� �Ǵ� �ż���*/
    UI_Static();
    UI_Static(UISystem* pUISys, ID2D1RenderTarget* pRT, int nID, pfnUIHandler pfnCallback);
    ~UI_Static();
    void preInit(UISystem* pUISys, ID2D1RenderTarget* pRT, int nID, pfnUIHandler pfnCallback);
    void Init(int Motion, POSITION Pos, D2D1_COLOR_F Color, wchar_t* pText, D2D1_COLOR_F TextColor, int nDelay = 0);
    void InputMotion(int Motion, int nDelay);
    void pause(int nDelay);
    void resume(int nDelay);
    void Destroy();
    void update(unsigned long time);
    void render();

public: /*UI�� �ɼ� �ż���*/
    void setText(wchar_t* pText, int nDelay);
};
