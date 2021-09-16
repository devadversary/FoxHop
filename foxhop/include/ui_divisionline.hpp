#pragma once

#include "ui_base.hpp"
#include "ui_system.hpp"

class UISystem; /*UI�ý��� Ŭ������ ���漱��*/

#define MAX_FRAGMENTCNT 24

class UI_DivisionLine : public UI {
private:
    ObjectPool<ObjectMotionLine>* PoolLine;
    ObjectMotionLine*             MLine[MAX_FRAGMENTCNT];
    int                           nFragLineCnt; /**< ���м��� �����ؼ� �������� ����*/
    D2D1_COLOR_F                  FrameColor;     /**< �ʱ� �׵θ� ��*/

public:
    UI_DivisionLine();
    UI_DivisionLine(UISystem* pUISys, ID2D1RenderTarget* pRT, int nID, pfnUIHandler pfnCallback);
    ~UI_DivisionLine();
    void preInit(UISystem* pUISys, ID2D1RenderTarget* pRT, int nID, pfnUIHandler pfnCallback);
    void Init(int Motion, POSITION Pos, D2D1_COLOR_F Color, int nFragmentCnt=1, int nDelay=0);
    void InputMotion(int Motion, int nDelay);
    void pause(int nDelay);
    void resume(int nDelay);
    void Destroy();
    void update(unsigned long time);
    void render();
};
