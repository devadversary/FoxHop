#pragma once

#include "ui_base.hpp"
#include "ui_system.hpp"

/*UI�ý��� Ŭ������ ���漱��*/
class UISystem;

class UI_Listview : public UI {
private:
    ObjectPool<ObjectMotionBox>* PoolBox;
    ObjectPool<ObjectMotionText>* PoolText;
    ObjectPool<ObjectMotionLine>* PoolLine;
    D2D1_COLOR_F                  FrameColor;     /**< �ʱ� �׵θ� ��*/
    D2D1_COLOR_F                  FaceColor;      /**< �ʱ� ��ư �����*/
    D2D1_COLOR_F                  HighlightColor; /**< ��ư ���̶���Ʈ ��*/
    D2D1_COLOR_F                  FontColor;      /**< �ʱ� �ؽ�Ʈ ��*/
    //wchar_t                       szText[MAX_STATIC_TEXTLEN];
    int                           nTextLen;

public: /*�ݵ�� �־�� �Ǵ� �ż���*/
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

public: /*UI�� �ɼ� �ż���*/
    void addColumn();
    void delColumn(int nColIndex);
    void addData(void* pData);
    void setItemCount(int nCnt);
    void selectRow();
    void unselectRow();
};
