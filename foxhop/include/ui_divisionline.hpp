#pragma once

#include "ui_base.hpp"
#include "ui_system.hpp"

class UISystem; /*UI시스템 클래스의 전방선언*/

#define MAX_FRAGMENTCNT 24

class UI_DivisionLine : public UI {
private:
    ObjectPool<ObjectMotionLine>* PoolLine;
    ObjectMotionLine*             MLine[MAX_FRAGMENTCNT];
    int                           nFragLineCnt; /**< 구분선을 몇등분해서 연출할지 결정*/
    D2D1_COLOR_F                  FrameColor;     /**< 초기 테두리 색*/

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
