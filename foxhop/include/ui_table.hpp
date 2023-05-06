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
    bool      bTextMotionPlayed; /**< 모션이 진행 되었는지 여부*/
    wchar_t** ppData;    /**< 데이터 */
} TABLE_ROW;

/**
    @brief 테이블 모션 타입 상수
*/
enum class eTableMotionPattern {
    eInit_Default = 0,       /**< 생성: 모션 없음*/

    ePause_Default = 0,      /**< 소멸: 모션 없음*/

    eMouseover_Default = 0,  /**< 마우스오버 : 모션 없음*/

    eMouseleave_Default = 0, /**< 마우스이탈 : 모션 없음*/

    eSelect_Default = 0,     /**< 선택 : 모션 없음*/
    eSelect_Linear,          /**< 선택 : 선형 모션*/
    eSelect_Decel,           /**< 선택 : 감속 모션*/

    eUnselect_Default = 0,   /**< 선택 해제 : 모션 없음*/

    eHighlight_Default = 0,  /**< 하이라이팅 : 모션 없음*/

    eText_Default = 0,       /**< 텍스트 : 모션 없음*/
    eText_Typing,            /**< 텍스트 : 타이핑*/
};

class RowObject;

class UI_Table_MotionParam {
public:
    eTableMotionPattern MotionInit;
    eTableMotionPattern MotionPause;
    eTableMotionPattern MotionRowText;
    eTableMotionPattern MotionMouseover;
    eTableMotionPattern MotionMouseleave;
    eTableMotionPattern MotionSelect;
    eTableMotionPattern MotionUnselect;
    unsigned long PitchInit;
    unsigned long PitchPause;
    unsigned long PitchMouseover;
    unsigned long PitchSelect;
    unsigned long PitchUnselect;
    unsigned long PitchScroll;
    unsigned long PitchRowOneText;
    unsigned long PitchRowAllText;
    unsigned long PitchRowBg;
    D2D1_COLOR_F  ColorFrame;
    D2D1_COLOR_F  ColorHeaderBg;
    D2D1_COLOR_F  ColorHeaderText;
    D2D1_COLOR_F  ColorRowBg1;
    D2D1_COLOR_F  ColorRowBg2;
    D2D1_COLOR_F  ColorRowText;
    D2D1_COLOR_F  ColorRowLine;
    D2D1_COLOR_F  ColorRowBgMouseover;
    D2D1_COLOR_F  ColorRowBgHighlight;
    D2D1_COLOR_F  ColorRowBgSelect;
    D2D1_COLOR_F  ColorRowTextSelect;

public:
    UI_Table_MotionParam() {
        MotionInit = eTableMotionPattern::eInit_Default;
        MotionPause = eTableMotionPattern::ePause_Default;
        MotionRowText = eTableMotionPattern::eText_Default;
        MotionMouseover = eTableMotionPattern::eMouseover_Default;
        MotionMouseleave = eTableMotionPattern::eMouseover_Default;
        MotionSelect = eTableMotionPattern::eSelect_Default;
        MotionUnselect = eTableMotionPattern::eUnselect_Default;
        PitchInit = 0;
        PitchPause = 0;
        PitchMouseover = 0;
        PitchSelect = 0;
        PitchUnselect = 0;
        PitchScroll = 200;
        PitchRowOneText = 0;
        PitchRowAllText = 0;
        PitchRowBg = 0;
        ColorFrame = { 0.f ,0.f, 0.f, 1.f };
        ColorHeaderBg = { 0.9f, 0.9f, 0.9f, 1.f };
        ColorHeaderText = { 0.f, 0.f, 0.f, 1.f };
        ColorRowBg1 = { 1.f, 1.f, 1.f, 1.f };
        ColorRowBg2 = { 0.96f, 0.96f, 0.96f, 1.f };
        ColorRowText = { 0.f, 0.f, 0.f, 1.f };
        ColorRowLine = { 0.f, 0.f, 0.f, 1.f };
        ColorRowBgMouseover = { 0.f, 0.f, 0.f, 0.1f };
        ColorRowBgHighlight = { 0.f, 0.f, 0.f, 1.f };
        ColorRowBgSelect = { 0.2f, 0.56f, 1.f, 1.f };
        ColorRowTextSelect = { 1.f, 1.f, 1.f, 1.f };
    }
};
/**
    @brief 테이블 클래스
*/
class UI_Table : public UI {
public:
    std::vector<TABLE_ROW>  MainDataPool; /**< 갖고있는 모든 데이터*/
    UI_Table_MotionParam Motion;

private:
    ComponentMotion*        ScrollComp;   /**< 스크롤 모션 진행을 위한 컴포넌트*/
    PropBox*  pBoxHeader;
    PropText** ppTextHdr;
    PropBox*  pBoxFrame;
    std::vector<RowObject*> ViewData;     /**< 화면에 보일 행 데이터 (화면 크기만큼만 생성)*/
    POINT     MousePt;         /**< 마우스 위치*/
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
    long long PrevSelMainIdx;  /**< 이전에 선택했던 실 데이터 인덱스 (음수이면 미선택)*/

private:
    static void DefaultTableProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam);

public:
    UI_Table(UISystem* pUISys, pfnUIHandler pfnCallback, POSITION Pos, unsigned int ColumnCount, wchar_t** ColumnNameList, unsigned int* ColumnWidth, unsigned int HeaderHeight, unsigned int RowHeight, BOOL MultiSelect, UI_Table_MotionParam MotionParam = UI_Table_MotionParam());
    ~UI_Table() {};

    void pause(int nDelay);
    void resume(int nDelay);
    BOOL update(unsigned long time);
    void render();

public: /*UI별 옵션 매서드*/
    void Resize(POSITION Pos);
    void AddData(wchar_t* Data[], BOOL bAutoScroll);
    void SetScroll(long long TargetScrollPx);
};

/**
    @brief 테이블이 화면에 렌더링할 행 데이터
*/
class RowObject : public UI {
public: 
    unsigned long long MainDataIdx = -1;   /**< 이 뷰행이 가리키는 MainDataPool 데이터의 인덱스*/

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
    void SetData(wchar_t** ppData, int* pWidth, int nCnt, BOOL bMotion);
    void SetSelectBox(BOOL bSel, D2D1_COLOR_F Color, BOOL bMotion);
    void SetHighlight(D2D1_COLOR_F Color);
    void SetBgColor(D2D1_COLOR_F Color, BOOL bMotion);
    void SetFontColor(D2D1_COLOR_F Color, BOOL bMotion);
    void OnBind(unsigned long long TargetDataIdx, int* pColWidth, BOOL bNeedUpdate = FALSE);
    void OnSelectEvent();
    void pause(int nDelay);
    void resume(int nDelay);
    BOOL update(unsigned long time);
    void render();
};