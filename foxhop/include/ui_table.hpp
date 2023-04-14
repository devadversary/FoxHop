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
    @brief 테이블이 화면에 렌더링할 행 데이터
*/
typedef struct _st_viewrow
{
    unsigned int nDelay;         /**< 해당 행의 딜레이*/
    PropText**   ppText;         /**< 해당 행의 모션텍스트객체들 (UI_Table의 열 갯수만큼)*/
    PropLine**   ppColLine;      /**< 열 구분선*/
    PropBox*     pBackgroundBox; /**< 해당 행의 배경색 (행 선택 색상 또한 겸용)*/
    PropBox*     pHighlightBox;  /**< 해당 행의 하이라이트 색 (내용변경 등의 모션에 응용 가능)*/
} VIEWTABLE_ROW;

/**
    @brief 테이블 모션 타입 상수
*/
enum class eTableMotionPattern {
    eInit_Default = 0, /**< 생성: 모션 없음*/

    ePause_Default = 0, /**< 소멸: 모션 없음*/

    eMouseover_Default = 0, /**< 마우스오버 : 모션 없음*/

    eSelect_Default = 0, /**< 선택 : 모션 없음*/

    eUnselect_Default = 0, /**< 선택 해제 : 모션 없음*/
};

/**
    @brief 테이블 모션 타입
*/
enum class eTableAction {
    eAction_Init = 0,  /**< 초기화 모션*/
    eAction_Pause,     /**< 소멸 모션*/
    eAction_Mouseover, /**< 마우스 오버 모션 (커서가 떠났을때도 포함)*/
    eAction_Select,    /**< 행 선택 모션*/
    eAction_Unselect,  /**< 행 선택 모션*/
};

/**
    @brief 테이블 클래스
*/
class UI_Table : public UI {
public:
    eTableMotionPattern MotionInit      = eTableMotionPattern::eInit_Default;
    eTableMotionPattern MotionPause     = eTableMotionPattern::ePause_Default;
    eTableMotionPattern MotionMouseover = eTableMotionPattern::eMouseover_Default;
    eTableMotionPattern MotionSelect    = eTableMotionPattern::eSelect_Default;
    eTableMotionPattern MotionUnselect  = eTableMotionPattern::eUnselect_Default;
    unsigned long PitchInit      = 0;
    unsigned long PitchPause     = 0;
    unsigned long PitchMouseover = 0;
    unsigned long PitchSelect    = 0;
    unsigned long PitchUnselect  = 0;
    unsigned long PitchScroll    = 200; /**< 스크롤 모션 속도는 0.2초 기본값.*/
    D2D1_COLOR_F  ColorHeaderBg       = { 0.9f, 0.9f, 0.9f, 1.f };  /**< 밝은 회색*/
    D2D1_COLOR_F  ColorHeaderText     = { 0.f, 0.f, 0.f, 1.f };     /**< 검은색*/
    D2D1_COLOR_F  ColorRowBg1         = { 1.f, 1.f, 1.f, 1.f };     /**< 흰색*/
    D2D1_COLOR_F  ColorRowBg2         = { 1.f, 1.f, 1.f, 1.f };     /**< 흰색*/
    D2D1_COLOR_F  ColorRowText        = { 0.f, 0.f, 0.f, 1.f };     /**< 검은색*/
    D2D1_COLOR_F  ColorRowLine        = { 0.f, 0.f, 0.f, 1.f };     /**< 검은색*/
    D2D1_COLOR_F  ColorRowBgMouseover = { 0.f, 0.f, 0.f, 0.1f };    /**< 투명도 0.1 검은색*/
    D2D1_COLOR_F  ColorRowBgHighlight = { 0.f, 0.f, 0.f, 1.f };     /**< 검은색*/
    D2D1_COLOR_F  ColorRowBgSelect    = { 0.2f, 0.56f, 0.1f, 1.f }; /**< 연한 파란색*/

private:
    ComponentMotion*           ScrollComp;   /**< 스크롤 모션 진행을 위한 컴포넌트*/
    std::vector<TABLE_ROW>     MainDataPool; /**< 갖고있는 모든 데이터*/
    std::vector<VIEWTABLE_ROW> ViewData;     /**< 화면에 보일 행 데이터 (화면 크기만큼만 생성)*/
    
    BOOL      MultiSelectMode; /**< FALSE:단일 선택 / TRUE:여러줄 선택 모드*/
    long long DataCount;       /**< 현재 데이터 갯수*/
    float     CurrScrollPixel; /**< 현재 모션 진행중인 스크롤 픽셀 (연속적으로 변함)*/
    long long ScrollPixel;     /**< 스크롤된 픽셀. 최대 스크롤 길이는, 데이터 갯수 x 행 높이*/
    long long MaxScrollPixel;  /**< 데이터 갯수 x 행 높이 = 최대 스크롤가능 픽셀*/
    //int       ScrollModule;    /**< 스크롤 모듈러스 (0 ~ 행 높이 사이의 값)*/
    int       ColCnt;          /**< 열 갯수*/
    wchar_t** ColName;         /**< 열 이름*/
    int*      ColWidth;        /**< 열 가로폭 픽셀*/
    int       HeaderHgt;       /**< 헤더 높이 픽셀*/
    int       RowHgt;          /**< 행 높이 픽셀*/
    int       ClientHeight;    /**< 헤더 높이를 제외한 순수 테이블 높이*/
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

    void AddData(wchar_t* Data[]);
};
