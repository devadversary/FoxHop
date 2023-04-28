#pragma once

#include "ui_base.hpp"
#include <vector>

class UISystem; /*UI시스템 클래스의 전방선언*/

/**
    @brief 테이블이 관리할 행 데이터
*/
typedef struct _st_TableRow
{
    bool      bSelected; /**< 행 선택 여부*/
    wchar_t** ppData;    /**< 데이터 */
} TABLE_ROW;

/**
    @brief 테이블 모션 타입 상수
*/
enum class eTableMotionPattern {
    eInit_Default = 0, /**< 생성: 모션 없음*/

    ePause_Default = 0, /**< 소멸: 모션 없음*/

    eMouseover_Default = 0, /**< 마우스오버 : 모션 없음*/

    eMouseleave_Default = 0, /**< 마우스이탈 : 모션 없음*/

    eSelect_Default = 0, /**< 선택 : 모션 없음*/

    eUnselect_Default = 0, /**< 선택 해제 : 모션 없음*/

    eHighlight_Default = 0 /**< 하이라이팅 : 모션 없음*/
};

/**
    @brief 테이블 모션 타입
*/
enum class eTableAction {
    eAction_Init = 0,   /**< 초기화 모션*/
    eAction_Pause,      /**< 소멸 모션*/
    eAction_Mouseover,  /**< 마우스 오버 모션*/
    eAction_Mouseleave, /**< 마우스 이탈 모션*/
    eAction_Select,     /**< 행 선택 모션*/
    eAction_Unselect,   /**< 행 선택 모션*/
    eAction_Highlight,  /**< 하이라이팅*/
};

class RowObject;

/**
    @brief 테이블 클래스
*/
class UI_Table : public UI {
public:
    eTableMotionPattern MotionInit      = eTableMotionPattern::eInit_Default;
    eTableMotionPattern MotionPause     = eTableMotionPattern::ePause_Default;
    eTableMotionPattern MotionMouseover = eTableMotionPattern::eMouseover_Default;
    eTableMotionPattern MotionMouseleave = eTableMotionPattern::eMouseover_Default;
    eTableMotionPattern MotionSelect    = eTableMotionPattern::eSelect_Default;
    eTableMotionPattern MotionUnselect  = eTableMotionPattern::eUnselect_Default;
    unsigned long PitchInit      = 0;
    unsigned long PitchPause     = 0;
    unsigned long PitchMouseover = 0;
    unsigned long PitchSelect    = 0;
    unsigned long PitchUnselect  = 0;
    unsigned long PitchScroll    = 300; /**< 스크롤 모션 속도는 0.2초 기본값.*/
    D2D1_COLOR_F  ColorFrame          = { 0.f ,0.f, 0.f, 1.f };     /**< 검은색*/
    D2D1_COLOR_F  ColorHeaderBg       = { 0.9f, 0.9f, 0.9f, 1.f };  /**< 밝은 회색*/
    D2D1_COLOR_F  ColorHeaderText     = { 0.f, 0.f, 0.f, 1.f };     /**< 검은색*/
    D2D1_COLOR_F  ColorRowBg1         = { 1.f, 1.f, 1.f, 1.f };     /**< 흰색*/
    D2D1_COLOR_F  ColorRowBg2         = { 0.92f, 0.92f, 0.92f, 1.f }; /**< 밝은 회색*/
    D2D1_COLOR_F  ColorRowText        = { 0.f, 0.f, 0.f, 1.f };     /**< 검은색*/
    D2D1_COLOR_F  ColorRowLine        = { 0.f, 0.f, 0.f, 1.f };     /**< 검은색*/
    D2D1_COLOR_F  ColorRowBgMouseover = { 0.f, 0.f, 0.f, 0.1f };    /**< 투명도 0.1 검은색*/
    D2D1_COLOR_F  ColorRowBgHighlight = { 0.f, 0.f, 0.f, 1.f };     /**< 검은색*/
    D2D1_COLOR_F  ColorRowBgSelect    = { 0.2f, 0.56f, 0.1f, 1.f }; /**< 연한 파란색*/

private:
    ComponentMotion*           ScrollComp;   /**< 스크롤 모션 진행을 위한 컴포넌트*/
    std::vector<TABLE_ROW>     MainDataPool; /**< 갖고있는 모든 데이터*/
    std::vector<RowObject*>     ViewData;     /**< 화면에 보일 행 데이터 (화면 크기만큼만 생성)*/
    RowObject*                  pHeader;
    PropBox* pBox;
    BOOL      MultiSelectMode; /**< FALSE:단일 선택 / TRUE:여러줄 선택 모드*/
    long long DataCount;       /**< 현재 데이터 갯수*/
    float     CurrScrollPixel; /**< 현재 모션 진행중인 스크롤 픽셀 (연속적으로 변함, 렌더링시 사용)*/
    long long CurrMainIndex;   /**< 현재 렌더링 될 데이터의 시작 인덱스 (4이면, 4,5,6,7...n 이 렌더링된다.)*/
    long long ScrollPixel;     /**< 스크롤된 픽셀. 최대 스크롤 길이는, 데이터 갯수 x 행 높이*/
    long long MaxScrollPixel;  /**< 데이터 갯수 x 행 높이 = 최대 스크롤가능 픽셀*/
    int       ColCnt;          /**< 열 갯수*/
    wchar_t** ColName;         /**< 열 이름*/
    int*      ColWidth;        /**< 열 가로폭 픽셀*/
    int       HeaderHgt;       /**< 헤더 높이 픽셀*/
    int       RowHgt;          /**< 행 높이 픽셀*/
    int       ClientHeight;    /**< 헤더 높이를 제외한 순수 테이블 영역 높이*/
    int       ViewRowCnt;      /**< 화면에 보여지는 행 갯수*/
    /*
    ViewStartPtr;
    ViewDataPool;
    */

private:
    //void InputMotion(eListviewAction Action, UITheme* Theme, unsigned int nDelay, void* param);
    static void DefaultTableProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam);

public:
    UI_Table(UISystem* pUISys, pfnUIHandler pfnCallback, POSITION Pos, unsigned int ColumnCount, wchar_t** ColumnNameList, unsigned int* ColumnWidth, unsigned int HeaderHeight, unsigned int RowHeight, BOOL MultiSelect);
    ~UI_Table() {};

    void pause(int nDelay);
    void resume(int nDelay);
    BOOL update(unsigned long time);
    void render();

public: /*UI별 옵션 매서드*/
    void Resize(POSITION Pos);
    void AddData(wchar_t* Data[], BOOL bEnsure);
};

/**
    @brief 테이블이 화면에 렌더링할 행 데이터
*/
class RowObject : public UI {
public: 
    unsigned long long MainDataIdx = 0;   /**< 이 뷰행이 가리키는 MainDataPool 데이터의 인덱스*/
    BOOL               bBindComplete = FALSE; /**< 바인딩 완료 여부*/
private:
    UI_Table*  pParent;
    POSITION   Pos;
    int        nColumn;        /**< 열 갯수*/
    PropText** ppText;
    PropLine** ppColLine;      /**< 열 구분선*/
    PropBox*   pBackgroundBox; /**< 배경색*/
    PropBox*   pMouseoverBox;  /**< 마우스오버색*/
    PropBox*   pSelectBox;     /**< 선택 색상*/
    PropBox*   pHighlightBox;  /**< 하이라이트 색 (내용변경 등의 모션에 응용 가능)*/

public:
    RowObject(UISystem* pUISys, UI_Table* pParentTable, POSITION pos, unsigned int ColCnt);
    ~RowObject();
    void SetElapse(unsigned long value);
    void SetSelect(BOOL bSel, D2D1_COLOR_F Color);
    void SetHighlight(D2D1_COLOR_F Color);
    void SetBgColor(D2D1_COLOR_F Color);
    void SetData(wchar_t** ppData, int* pWidth, int nCnt, int Height);

    void pause(int nDelay);
    void resume(int nDelay);
    BOOL update(unsigned long time);
    void render();
};