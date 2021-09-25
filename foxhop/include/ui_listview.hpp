#pragma once

#include "ui_base.hpp"
#include "ui_system.hpp"

/*UI시스템 클래스의 전방선언*/
class UISystem;

class UI_Listview : public UI {
private:
    ObjectPool<ObjectMotionBox>* PoolBox;
    ObjectPool<ObjectMotionText>* PoolText;
    ObjectPool<ObjectMotionLine>* PoolLine;
    D2D1_COLOR_F                  FrameColor;     /**< 초기 테두리 색*/
    D2D1_COLOR_F                  FaceColor;      /**< 초기 버튼 전경색*/
    D2D1_COLOR_F                  HighlightColor; /**< 버튼 하이라이트 색*/
    D2D1_COLOR_F                  FontColor;      /**< 초기 텍스트 색*/
    //wchar_t                       szText[MAX_STATIC_TEXTLEN];
    int                           nTextLen;

public: /*반드시 있어야 되는 매서드*/
    UI_Listview();
    UI_Listview(UISystem* pUISys, ID2D1RenderTarget* pRT, int nID, pfnUIHandler pfnCallback);
    ~UI_Listview();
    void preInit(UISystem* pUISys, ID2D1RenderTarget* pRT, int nID, pfnUIHandler pfnCallback);
    void Init(int Motion, POSITION Pos, D2D1_COLOR_F FaceColor, wchar_t* pText, D2D1_COLOR_F TextColor, int nDelay = 0);
    void InputMotion(int Motion, int nDelay);
    void pause(int nDelay);
    void resume(int nDelay);
    void Destroy();
    void update(unsigned long time);
    void render();

public: /*UI별 옵션 매서드*/
    void addColumn();
    void delColumn(int nColIndex);
    void addData(void* pData);
    void setItemCount(int nCnt);
    void selectRow();
    void unselectRow();
};
