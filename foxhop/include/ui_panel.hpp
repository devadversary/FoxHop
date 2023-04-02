#pragma once

#include "ui_base.hpp"
#include <list>

class UISystem; /*UI시스템 클래스의 전방선언*/

/**
    @brief 패널 UI 클래스
    @remark 패널 UI는 타 컨트롤 클래스와는 다르게
    @n      자신이 가지고있는 하위 패널의 리스트와
    @n      컨트롤, 모션오브젝트들의 리스트를 갖고 있어야 한다.
    @n      마우스 오버, 포커스, 키 입력 등 사용자 입력을 처리하기 위해서는
    @n      이 리스트가 필요하기 때문이다.
    @n      버튼, 체크박스, 리스트, 스태틱 등의 컨트롤들은 스스로가
    @n      마우스 입력, 클릭에 대해 반응할 수 없다.
*/
class UI_Panel : public UI {
private:
    int                  PanelDelay;   /**< 패널의 업데이트 딜레이*/
    std::list<UI_Panel*> PanelList;    /**< 패널이 가진 하위 패널의 리스트*/
    std::list<UI*>       UIList;       /**< 패널이 가진 하위 UI의 리스트*/
    D2D1::Matrix3x2F     TransMat;     /**< 해당 패널의 변환행렬*/
    BOOL                 IsMainPanel;  /**< 메인 패널 여부 (메인패널은 최상위 패널 하나여야함)*/
    UI*                  pMouseOverUI; /**< 마우스 이벤트 핸들링을 위한 임시변수*/
    UI*                  pFocusedUI;   /**< 포커싱된 UI*/

private:
    static void DefaultPanelHandler(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam);
    void        DefaultMouseHandler(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam);

public:
    UI_Panel(UISystem* pUISys, ID2D1RenderTarget* pRT, pfnUIHandler pfnCallback, POSITION Pos, BOOL IsMain = FALSE, int nDelay = 0);
    ~UI_Panel();
    void pause(int nDelay);
    void resume(int nDelay);
    void Destroy();
    BOOL update(unsigned long time);
    void render();

public: /*UI별 옵션 매서드*/
    void RegisterPanel(UI_Panel* pPanel);
    void RegisterUI(UI* pUI);
};