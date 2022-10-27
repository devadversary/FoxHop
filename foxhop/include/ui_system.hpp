#pragma once

/*순환 참조를 방지하기 위해, UISystem 헤더에서 모든 하위 UI 을 포함 하도록 한다.*/

#include "d2dassist.hpp"
#include "ui_base.hpp"
#include "ui_panel.hpp"
#include "ui_button.hpp"
#include "ui_listview.hpp"
#include "ui_static.hpp"
#include "ui_fraggedline.hpp"
#include <list>

/*TODO : 추후에 폰트를 쉽게 변경 가능하도록 하드코딩을 제거할 수 있는 구조로 변경할것.*/
#define UISYSTEM_FONTNAME_DEFAULT (L"monoMMM_5") /*UI시스템이 기본적으로 사용하는 폰트 이름*/
#define DEFAULT_MAXUI (256) /*UISystem 초기화시, nMaxUI 를 0으로 설정시 적용할 기본 UI 갯수*/

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
    eUI_Undefined = 0, /**< 아직 정의되지 않은 UI 개체 (일종의NULL)*/
    eUI_Custom,        /**< 유저 커스텀 UI*/
    eUI_Panel,         /**< 패널 컨트롤. 이 안에 다른 UI를 배치하여 한번에 관리할때 사용. (패널 안에 패널 가능)*/
    eUI_Button,        /**< winAPI의 버튼 컨트롤과 같은 용도*/
    eUI_Static,        /**< 스태틱 컨트롤*/
    eUI_FragLine,      /**< 분리선*/
    eUI_List,          /**< 리스트뷰 컨트롤*/
    eUI_Edit,          /**< 에디트 컨트롤*/
    eUI_Scroll,        /**< 스크롤 바 (가로 세로)*/
    eUI_Progress,      /**< 프로그래스 바*/
    eUI_Graph          /**< 그래프 뷰(가로 세로 원형 등등..)*/
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
    HWND                         hBindWnd;             /**< UISystem이 바인딩된 윈도우 핸들*/
    D2DA_CONTEXT                 D2DA;             /**< D2D 개체들의 컨텍스트 (이 안에 다 있다)*/
    IDWriteTextFormat*           TinyTextForm;     /**< 작은 텍스트 폼*/
    IDWriteTextFormat*           SmallTextForm;    /**< 작은 텍스트 폼*/
    IDWriteTextFormat*           ButtonTextForm;   /**< 버튼 컨트롤들이 전역적으로 사용할 텍스트 폼. (winAPI 버튼도 텍스트 폼은 무조건 고정임)*/
    IDWriteTextFormat*           MediumTextForm;   /**< 중간크기 텍스트 폼*/

    ObjectPool<ObjectMotionBox>  ObjPoolBox;
    ObjectPool<ObjectMotionLine> ObjPoolLine;
    ObjectPool<ObjectMotionText> ObjPoolText;

    UI_ButtonFactory*            pUIButtonFactory; /**< 버튼UI 팩토리*/
    UI_Panel*                    pMainPanel;       /**< 앞으로 모든 UI가 그려질 메인 패널 (이 패널로 UI를 생성한다)*/

public:
    UISystem(HWND hWnd);
    ~UISystem();

    UI_Panel* InitMainPanel(HWND hWnd, pfnUIHandler MainPanelProc);
    BOOL SendUIMessage(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam);
    void ReleaseUI();
};