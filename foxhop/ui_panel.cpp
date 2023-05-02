#include "./include/ui_panel.hpp"
#include "./include/ui_system.hpp"

#include "./include/ui_button.hpp"
#include "./include/ui_static.hpp"
#include "./include/ui_fraggedline.hpp"
#include "./include/ui_listview.hpp"

/**
    @brief 패널의 기본 핸들러.
    @remark 마우스 움직임은 기본 핸들러에서 처리되고,
    @n      그 이후 사용자 지정 핸들러가 실행된다.
*/
void UI_Panel::DefaultPanelHandler(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam)
{
    UI_Panel* pPanel = (UI_Panel*)pUI;
    pfnUIHandler UserHandler = pUI->MessageHandler;

    switch (Message) {
    /*마우스 관련 메세지 전달*/
    case WM_MOUSEMOVE:
    case WM_MOUSEWHEEL:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDBLCLK:
    case WM_MBUTTONDBLCLK:
        pPanel->DefaultMouseHandler(pUI, Message, wParam, lParam);
        break;

    /*키보드 관련 메세지 전달*/
    case WM_KEYDOWN:
        // DefaultKeyboardHandler 도 필요 할 것으로 보임
        break;
    }
    if (UserHandler) UserHandler(pUI, Message, wParam, lParam);
}

UI_Panel::UI_Panel(UISystem* pUISys, ID2D1RenderTarget* pRT, pfnUIHandler pfnCallback, POSITION Pos, BOOL IsMain, int nDelay)
{
    IsMainPanel    = IsMain;
    pFocusedUI     = NULL;
    pMouseOverUI   = NULL;
    uiSys          = pUISys;
    pRenderTarget  = pRT;
    Focusable      = TRUE;
    MessageHandler = pfnCallback;
    DefaultHandler = DefaultPanelHandler;
    uiPos          = Pos;
    PanelDelay     = nDelay;
    //transform = D2D1::Matrix3x2F::Translation(PanelPos.x + 0.5f, PanelPos.y + 0.5f);
    DefaultPanelHandler(this, UIM_CREATE, NULL, NULL); /*UI생성 메세지 전송*/
}

UI_Panel::~UI_Panel() {}



/**
    @brief 해당 패널을 잠시 비활성화하고 UI소멸. 업데이트, 렌더링 리스트에서 제외
*/
void UI_Panel::pause(int nDelay)
{
    uiSys->SendUIMessage(this, UIM_PAUSE, nDelay, NULL);
}

void UI_Panel::resume(int nDelay)
{
    uiSys->SendUIMessage(this, UIM_RESUME, nDelay, NULL);
}

void UI_Panel::Destroy()
{
    uiSys->SendUIMessage(this, UIM_DELETE, NULL, NULL);
}

/**
    @brief 패널이 가진 전체 UI 업데이트 (하위 패널 포함)
*/
BOOL UI_Panel::update(unsigned long time)
{
    BOOL updated = FALSE;

    for (UI_Panel* pPanel : PanelList) updated |= pPanel->update(time);
    for (UI* pUI : UIList) updated |= pUI->update(time);
    return updated;
}

/**
    @brief 패널이 가진 전체 UI 렌더링 (하위 패널 포함)
*/
void UI_Panel::render()
{
    pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(0.5f + uiPos.x, 0.5f + uiPos.y));
    for (UI_Panel* pPanel : PanelList) pPanel->render();
    for (UI* pUI : UIList) pUI->render();
}

/**
    @brief 해당 패널의 하위 패널을 등록한다.
*/
void UI_Panel::RegisterPanel(UI_Panel* pPanel)
{
    PanelList.push_back(pPanel);
}

/**
    @brief 해당 패널의 하위 UI를 등록한다.
*/
void UI_Panel::RegisterUI(UI* pUI)
{
    UIList.push_back(pUI);
}

/**
    @brief 대상 UI에게 WM_MOUSEMOVE 시, 상대좌표정보 변환해서 전달
    @param UI_X 이 메세지를 받을 UI의 X 시작좌표
    @param UI_Y 이 메세지를 받을 UI의 Y 시작좌표
    @param lParam 전달 직전의 lParam
    @return 전달 해줄 lParam 값
*/
static LPARAM RerouteMouseCursorPt(int UI_X, int UI_Y, LPARAM lParam)
{
    LPARAM NewParam;

    NewParam = (GET_X_LPARAM(lParam) - UI_X);
    NewParam |= (((GET_Y_LPARAM(lParam) - UI_Y)<<16) & 0xffffffff);
    return NewParam;
}

/**
    @brief 기본 마우스 이벤트 핸들러
    @param pUI 메세지를 전송한 UI의 핸들
    @param Message 전달된 메세지 (항상 WM_MOUSEMOVE 여야 한다)
    @remark 대상UI에 마우스 진입 / 퇴장 이벤트만 전달한다.
*/
void UI_Panel::DefaultMouseHandler(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam)
{
    UI* pTargetUI = NULL;
    POINT pt;
    LPARAM NewLParam;

    pt.x = GET_X_LPARAM(lParam);
    pt.y = GET_Y_LPARAM(lParam);

    /*휠 스크롤 메세지는 좌표 정보가 모니터 스크린 기준이다. 메인 패널일경우
      하위 UI로 메세지를 넘길때 lParam을 클라이언트 좌표계로 변환하여 넘겨야 한다.*/
    if (Message == WM_MOUSEWHEEL && this->IsMainPanel){
        ScreenToClient(pUI->uiSys->hBindWnd, &pt);
        lParam = pt.x;
        lParam |= ((pt.y << 16) & 0xffffffff);
    }

    /*이전에 영역안에 있던 컨트롤이 있었다면 걔 먼저 검사*/
    if (pMouseOverUI) {
        /*이전 컨트롤부터 영역검사*/
        if (IsInRect(pMouseOverUI->uiPos, pt)) {
            NewLParam = RerouteMouseCursorPt((int)pMouseOverUI->uiPos.x, (int)pMouseOverUI->uiPos.y, lParam);
            uiSys->SendUIMessage(pMouseOverUI, Message, wParam, NewLParam);
            return;
        }
        else {
            if (Message==WM_LBUTTONDOWN) pFocusedUI = NULL; /*클릭메세지라면 포커스 해제*/
            uiSys->SendUIMessage(pMouseOverUI, UIM_MOUSELEAVE, 0, 0);
            pMouseOverUI = NULL;
        }
    }
    /*이전 컨트롤이 아닌 다른 영역일때*/
    /*패널 UI부터 체크*/
    for (UI_Panel* pPanel : PanelList) {
        if (pPanel->uiMotionState == eUIMotionState::eUMS_Hide) continue;
        if (IsInRect(pPanel->uiPos, pt)) {
            pMouseOverUI = pPanel;
            if (Message == WM_LBUTTONDOWN) pFocusedUI = pPanel; /*클릭 메세지라면 포커스 지정*/
            NewLParam = RerouteMouseCursorPt((int)pMouseOverUI->uiPos.x, (int)pMouseOverUI->uiPos.y, lParam);
            uiSys->SendUIMessage(pPanel, UIM_MOUSEON, 0, 0);
            uiSys->SendUIMessage(pPanel, Message, wParam, NewLParam);
            return;
        }
    }
    /*UI 리스트 체크*/
    for (UI* pUI : UIList) {
        if (pUI->uiMotionState != eUIMotionState::eUMS_Visible) continue;
        if (!pUI->MouseEventCheck) continue;
        if (IsInRect(pUI->uiPos, pt)) {
            pMouseOverUI = pUI;
            if (Message == WM_LBUTTONDOWN) pFocusedUI = pUI; /*클릭 메세지라면 포커스 지정*/
            NewLParam = RerouteMouseCursorPt((int)pMouseOverUI->uiPos.x, (int)pMouseOverUI->uiPos.y, lParam);
            uiSys->SendUIMessage(pMouseOverUI, UIM_MOUSEON, 0, 0);
            uiSys->SendUIMessage(pMouseOverUI, Message, wParam, NewLParam);
            return;
        }
    }
}
