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
    POINT pt;

    switch (Message) {
    case WM_MOUSEMOVE:
        pPanel->DefaultMouseHandler(pUI, Message, wParam, lParam);
        break;

    case WM_KEYDOWN:
        break;
    }
    if (UserHandler) UserHandler(pUI, Message, wParam, lParam);
}

UI_Panel::UI_Panel(UISystem* pUISys, ID2D1RenderTarget* pRT, pfnUIHandler pfnCallback, POSITION Pos, int nDelay)
{
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
    @brief 해당 패널의 하위 UI를 생성하고 내부 리스트에 등록한다
    @return 생성된 UI 객체
*/
UI* UI_Panel::CreateUI(UIType type, POSITION pos, wchar_t* pText, int nDelay, pfnUIHandler callback)
{
    UI* pUI = NULL;

    switch (type) {
    case UIType::eUI_Button:
        pUI = uiSys->pUIButtonFactory->CreateUI(pos, pText, nDelay, callback);
        break;
    }
    UIList.push_back(pUI);
    return pUI;
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
    NewParam |= ((GET_Y_LPARAM(lParam) - UI_Y)<<16);
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
    int i;
    UI* pTargetUI = NULL;
    POINT pt;
    LPARAM NewLParam;

    pt.x = GET_X_LPARAM(lParam);
    pt.y = GET_Y_LPARAM(lParam);

    /*이전에 영역안에 있던 컨트롤이 있었다면 걔 먼저 검사*/
    if (pMouseOverUI) {
        /*이전 컨트롤부터 영역검사*/
        if (IsInRect(pMouseOverUI->uiPos, pt)) {
            NewLParam = RerouteMouseCursorPt(pMouseOverUI->uiPos.x, pMouseOverUI->uiPos.y, lParam);
            uiSys->SendUIMessage(pMouseOverUI, Message, wParam, NewLParam);
            return;
        }
        else {
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
            NewLParam = RerouteMouseCursorPt(pMouseOverUI->uiPos.x, pMouseOverUI->uiPos.y, lParam);
            uiSys->SendUIMessage(pPanel, UIM_MOUSEON, 0, 0);
            uiSys->SendUIMessage(pPanel, Message, wParam, NewLParam);
            return;
        }
    }
    /*UI 리스트 체크*/
    for (UI* pUI : UIList) {
        if (pUI->uiMotionState == eUIMotionState::eUMS_Hide) continue;
        /* 시작점-크기 쌍 대신 시작점-끝점 쌍을 사용하는 UI는 영역검사 X*/
        if (pUI->uiType == UIType::eUI_FragLine) continue;
        if (IsInRect(pUI->uiPos, pt)) {
            pMouseOverUI = pUI;
            NewLParam = RerouteMouseCursorPt(pMouseOverUI->uiPos.x, pMouseOverUI->uiPos.y, lParam);
            uiSys->SendUIMessage(pMouseOverUI, UIM_MOUSEON, 0, 0);
            uiSys->SendUIMessage(pMouseOverUI, Message, wParam, NewLParam);
            return;
        }
    }
}
