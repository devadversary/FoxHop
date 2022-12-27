#pragma once

#include "ui_base.hpp"
#include "ui_theme.hpp"
#include <vector>

class UISystem; /*UI시스템 클래스의 전방선언*/

#define LISTVIEW_ALLOC_ROW_UNIT 100000 /* 메모리 할당시, 추가할당될 행 갯수단위*/
#define LISTVIEW_MAX_COLS 256

/**
    @brief 리스트뷰 칼럼 타입 열거자
*/
enum class ListviewColType {
    eType_Null = 0,
    eType_String,
    eType_CustomDraw, /**< 해당 칸은 사용자가 직접 그릴 수 있음.*/
    eType_UI,         /**< 버튼, 체크박스같은 UI를 넣을 수 있음.*/
    eType_Undef
};

typedef struct st_ListviewCol {
    ListviewColType ColType;
    wchar_t*        ColName;
    int             DataSize; /**< 데이터의 크기. (문자열일 경우 "글자수 x wchar_t의 크기" /
    /*커스텀 드로우일때의 유저 핸들러도 받아야 함.*/
}LISTVIEW_COL;

/**
    @brief 리스트뷰에 넣을 데이터 한 줄
*/
typedef struct st_ListViewDataLine {
    int    nDataCnt;
    void** Data; /**< 문자열이면 char*, 다른 데이터면 핸들링할 데이터의 포인터. */
};

/**
    @brief 리스트뷰의 칼럼 갯수와 타입을 지정할 구조체
*/
typedef struct st_ListviewColInfo {
    char            nColCnt;
    ListviewColType ColType[LISTVIEW_MAX_COLS];
}LISTVIEW_COL_INFO;

/**
    @brief 삽입 삭제를 지원하는 리스트뷰 클래스
    @remark 생성은 팩토리에서 해준다.
*/
class UI_Listview : public UI {
private:
    int  Count; /**< 현재 리스트 요소 갯수*/
    int  MaxRow; /**< 최대 리스트 요소 갯수 (늘리기 가능)*/
    int  ScrollPixel; /**< 현재 스크롤된 픽셀 (0부터 행 높이까지 순환)*/
    BOOL IsMultiSelMode; /**< 여러개의 행을 선택 할 수 있는 모드인가?*/
    int* SelectedIndex; /**< 선택된 인덱스들. IsMultiSelMode가 FALSE이면 항상 원소는 1개뿐이다.*/
    //std::vector<> MainDataPool;
    //unsigned int ViewStartIndex;
    //std::vector<> ViewDataPool;

    /*
    ViewStartPtr;
    ViewDataPool;
    */

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
    void SetColType(LISTVIEW_COL_INFO ColInfo);
    void AddRow(wchar_t** ppText);
    void DelRow(int index);
    //void MoveLine(int index);
    void SelectRow(int index);
};
