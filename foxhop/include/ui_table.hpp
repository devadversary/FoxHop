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
    unsigned int nDelay;    /**< 해당 행의 딜레이*/
    PropText**   ppText; /**< 해당 행의 모션텍스트객체들 (UI_Table의 열 갯수만큼)*/
    PropLine**   ppColLine; /**< 열 구분선*/
    PropBox*     pBackgroundBox; /**< 해당 행의 배경색 (행 선택 색상 또한 겸용)*/
    PropBox*     pHighlightBox; /**< 해당 행의 하이라이트 색 (내용변경 등의 모션에 응용 가능)*/

} VIEWTABLE_ROW;

#if 0
/**
    @brief 테이블 행을 나타낼 하위 클래스.
    @remark 유저 커스텀 리스트뷰를 만들때 이 클래스를 상속받아 구현한다.
    @n      다양한 모습의 리스트뷰를 렌더링 하기 위한 매개체.
    @n      UI_ListviewBase에 의해 유한개로 관리된다. (한 화면에 보이는 갯수 + 1개)
*/
class UI_TextField {
public:
    int Height; /**< 행 높이*/
    int* width;
public:
    UI_LvField(int nColCnt);
    ~UI_LvField();
    virtual BOOL update(unsigned long time) = 0;
    virtual void render() = 0;
    virtual void pause(int nDelay) = 0;
    virtual void resume(int nDelay) = 0;
};

/**
    @brief 기본 제공되는 문자열 위주의 클래식한 리스트뷰 행
*/
class UI_LvFieldString : public UI_LvField {
public:
};
#endif

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
    std::vector<TABLE_ROW> MainDataPool;    /**< 갖고있는 모든 데이터*/
    BOOL                   MultiSelectMode; /**< FALSE:단일 선택 / TRUE:여러줄 선택 모드*/
    unsigned long long     DataCount;       /**< 현재 데이터 갯수*/
    unsigned long long     ScrollPixel;     /**< 스크롤된 픽셀. 최대 스크롤 길이는, 데이터 갯수 x 행 높이*/
    unsigned long long     MaxScrollPixel;  /**< 데이터 갯수 x 행 높이 = 최대 스크롤가능 픽셀*/
    int       ColCnt;   /**< 열 갯수*/
    wchar_t** ColName;  /**< 열 이름*/
    int*      ColWidth; /**< 열 가로폭 픽셀*/
    int       RowWidth; /**< 행 높이 픽셀*/
    std::vector<VIEWTABLE_ROW> ViewData; /**< 화면에 보일 행 데이터 (화면 크기만큼만 생성)*/

    /*
    ViewStartPtr;
    ViewDataPool;
    */

private:
    //void InputMotion(eListviewAction Action, UITheme* Theme, unsigned int nDelay, void* param);
    static void DefaultTableProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam);

public:
    UI_Table(UISystem* pUISys, pfnUIHandler pfnCallback, POSITION Pos, unsigned int ColumnCount, wchar_t** ColumnNameList, unsigned int* ColumnWidth, unsigned int RowHeight, BOOL MultiSelect);
    ~UI_Table() {};

    void pause(int nDelay);
    void resume(int nDelay);
    void Destroy();
    BOOL update(unsigned long time);
    void render();

public: /*UI별 옵션 매서드*/

    void AddData(wchar_t* Data[]);
};
