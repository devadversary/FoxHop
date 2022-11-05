#pragma once

#include "ui_base.hpp"
#include "ui_theme.hpp"

class UISystem; /*UI시스템 클래스의 전방선언*/

#define LISTVIEW_ALLOC_ROW_UNIT 100000 /* 메모리 할당시, 추가할당될 행 갯수단위*/

/**
    @brief 버튼 클래스
    @remark 생성은 팩토리에서 해준다.
*/
class UI_Listview : public UI {
private:
    int Count; /**< 현재 리스트 요소 갯수*/
    int MaxRow; /**< 최대 리스트 요소 갯수 (늘리기 가능)*/
    int ScrollPixel; /**< 현재 스크롤된 픽셀*/
    BOOL IsMultiSelMode; /**< 여러개의 행을 선택 할 수 있는 모드인가?*/
    int* SelectedIndex; /**< 선택된 인덱스들. IsMultiSelMode가 FALSE이면 항상 원소는 1개뿐이다.*/

private:
    void InputMotion(eListviewAction Action, UITheme* Theme, unsigned int nDelay, void* param);
    static void DefaultListviewProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam);

public:
    UI_Listview(UISystem* pUISys, pfnUIHandler pfnCallback, POSITION Pos, wchar_t* ColList, int* AdjustColWidth, int RowHeight, int nDelay);
    ~UI_Listview() {};

    void pause(int nDelay);
    void resume(int nDelay);
    void Destroy();
    BOOL update(unsigned long time);
    void render();

public: /*UI별 옵션 매서드*/
    void AddRow(wchar_t** ppText);
    void DelRow(int index);
    void MoveLine(int index);
    void ClickLine(int index);
};
