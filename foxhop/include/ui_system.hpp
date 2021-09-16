#pragma once

#include "d2dassist.hpp"
#include "ui_base.hpp"
#include "ui_panel.hpp"
#include "ui_button.hpp"
#include "ui_divisionline.hpp"
#include "ui_static.hpp"

/*TODO : 추후에 폰트를 쉽게 변경 가능하도록 하드코딩을 제거할 수 있는 구조로 변경할것.*/
#define UISYSTEM_FONTNAME_DEFAULT (L"monoMMM_5") /*UI시스템이 기본적으로 사용하는 폰트 이름*/

/**
    @brief ID별로 UI를 관리할 테이블.
    @remark UI의 ID를 배열의 인덱스로 사용한다
*/
typedef struct _st_UI_TABLE
{
    UI* pUI;
}UI_TABLE;

/**
    @brief UI 종류 열거자
*/
enum class UIType {
    eUI_Panel = 1,    /**< 패널 컨트롤. 이 안에 다른 UI를 배치하여 한번에 관리할때 사용. (패널 안에 패널 가능)*/
    eUI_Button = 100, /**< winAPI의 버튼 컨트롤과 같은 용도*/
    eUI_Static,       /**< 스태틱 컨트롤*/
    eUI_DivLine,      /**< 분리선*/
    eUI_List,         /**< 리스트뷰 컨트롤*/
    eUI_Edit,         /**< 에디트 컨트롤*/
    eUI_Scroll,       /**< 스크롤 바 (가로 세로)*/
    eUI_Progress,     /**< 프로그래스 바*/
    eUI_Graph         /**< 그래프 뷰(가로 세로 원형 등등..)*/
};

/**
    @brief 모션 UI를 생성하고 관리하는 전역개체
    @remark 전역에 하나만 두고 쓰는것을 의도 했으나
    @n      상황에 따라 여러개 두고 써도 된다.
    @n      폰트 초기화 작업도 진행하므로, 필요한 폰트가 있다면
    @n      사전에 미리 로드 해두어야한다.
*/
class UISystem {
public:
    D2DA_CONTEXT                     D2DA;            /**< D2D 개체들의 컨텍스트 (이 안에 다 있다)*/
    IDWriteTextFormat*               TinyTextForm;    /**< 작은 텍스트 폼*/
    IDWriteTextFormat*               SmallTextForm;   /**< 작은 텍스트 폼*/
    IDWriteTextFormat*               ButtonTextForm;  /**< 버튼 컨트롤들이 전역적으로 사용할 텍스트 폼. (winAPI 버튼도 텍스트 폼은 무조건 고정임)*/
    IDWriteTextFormat*               MediumTextForm;  /**< 중간크기 텍스트 폼*/
    ObjectPoolList<ObjectMotionBox>  BoxPoolStorage;  /**< 모션박스풀의 스토리지*/
    ObjectPoolList<ObjectMotionLine> LinePoolStorage;
    ObjectPoolList<ObjectMotionText> TextPoolStorage;

private:
    UI**                             pUITbl;          /**< UI 인스턴스를 모아둘 배열*/
    int                              nUITblCnt;          /**< 배열의 길이 (UI의 ID를 배열의 인덱스로 사용)*/

public:
    UISystem();
    ~UISystem();
    void Init(HWND hWnd, unsigned int nMaxUI);
    UI* CreateUI(UIType type, unsigned int nID, pfnUIHandler callback);
    void SendUIMessage(UI* pUI, UINT Message, void* parm);
    void SendUIMessageByID(unsigned int nID, UINT Message, void* parm);
    void ReleaseUI(unsigned int nID);
};
