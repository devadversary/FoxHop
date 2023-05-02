#include "./include/ui_panel.hpp"
#include "./include/ui_system.hpp"

#include "./include/ui_button.hpp"
#include "./include/ui_static.hpp"
#include "./include/ui_fraggedline.hpp"
#include "./include/ui_listview.hpp"

/**
    @brief �г��� �⺻ �ڵ鷯.
    @remark ���콺 �������� �⺻ �ڵ鷯���� ó���ǰ�,
    @n      �� ���� ����� ���� �ڵ鷯�� ����ȴ�.
*/
void UI_Panel::DefaultPanelHandler(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam)
{
    UI_Panel* pPanel = (UI_Panel*)pUI;
    pfnUIHandler UserHandler = pUI->MessageHandler;

    switch (Message) {
    /*���콺 ���� �޼��� ����*/
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

    /*Ű���� ���� �޼��� ����*/
    case WM_KEYDOWN:
        // DefaultKeyboardHandler �� �ʿ� �� ������ ����
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
    DefaultPanelHandler(this, UIM_CREATE, NULL, NULL); /*UI���� �޼��� ����*/
}

UI_Panel::~UI_Panel() {}



/**
    @brief �ش� �г��� ��� ��Ȱ��ȭ�ϰ� UI�Ҹ�. ������Ʈ, ������ ����Ʈ���� ����
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
    @brief �г��� ���� ��ü UI ������Ʈ (���� �г� ����)
*/
BOOL UI_Panel::update(unsigned long time)
{
    BOOL updated = FALSE;

    for (UI_Panel* pPanel : PanelList) updated |= pPanel->update(time);
    for (UI* pUI : UIList) updated |= pUI->update(time);
    return updated;
}

/**
    @brief �г��� ���� ��ü UI ������ (���� �г� ����)
*/
void UI_Panel::render()
{
    pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(0.5f + uiPos.x, 0.5f + uiPos.y));
    for (UI_Panel* pPanel : PanelList) pPanel->render();
    for (UI* pUI : UIList) pUI->render();
}

/**
    @brief �ش� �г��� ���� �г��� ����Ѵ�.
*/
void UI_Panel::RegisterPanel(UI_Panel* pPanel)
{
    PanelList.push_back(pPanel);
}

/**
    @brief �ش� �г��� ���� UI�� ����Ѵ�.
*/
void UI_Panel::RegisterUI(UI* pUI)
{
    UIList.push_back(pUI);
}

/**
    @brief ��� UI���� WM_MOUSEMOVE ��, �����ǥ���� ��ȯ�ؼ� ����
    @param UI_X �� �޼����� ���� UI�� X ������ǥ
    @param UI_Y �� �޼����� ���� UI�� Y ������ǥ
    @param lParam ���� ������ lParam
    @return ���� ���� lParam ��
*/
static LPARAM RerouteMouseCursorPt(int UI_X, int UI_Y, LPARAM lParam)
{
    LPARAM NewParam;

    NewParam = (GET_X_LPARAM(lParam) - UI_X);
    NewParam |= (((GET_Y_LPARAM(lParam) - UI_Y)<<16) & 0xffffffff);
    return NewParam;
}

/**
    @brief �⺻ ���콺 �̺�Ʈ �ڵ鷯
    @param pUI �޼����� ������ UI�� �ڵ�
    @param Message ���޵� �޼��� (�׻� WM_MOUSEMOVE ���� �Ѵ�)
    @remark ���UI�� ���콺 ���� / ���� �̺�Ʈ�� �����Ѵ�.
*/
void UI_Panel::DefaultMouseHandler(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam)
{
    UI* pTargetUI = NULL;
    POINT pt;
    LPARAM NewLParam;

    pt.x = GET_X_LPARAM(lParam);
    pt.y = GET_Y_LPARAM(lParam);

    /*�� ��ũ�� �޼����� ��ǥ ������ ����� ��ũ�� �����̴�. ���� �г��ϰ��
      ���� UI�� �޼����� �ѱ涧 lParam�� Ŭ���̾�Ʈ ��ǥ��� ��ȯ�Ͽ� �Ѱܾ� �Ѵ�.*/
    if (Message == WM_MOUSEWHEEL && this->IsMainPanel){
        ScreenToClient(pUI->uiSys->hBindWnd, &pt);
        lParam = pt.x;
        lParam |= ((pt.y << 16) & 0xffffffff);
    }

    /*������ �����ȿ� �ִ� ��Ʈ���� �־��ٸ� �� ���� �˻�*/
    if (pMouseOverUI) {
        /*���� ��Ʈ�Ѻ��� �����˻�*/
        if (IsInRect(pMouseOverUI->uiPos, pt)) {
            NewLParam = RerouteMouseCursorPt((int)pMouseOverUI->uiPos.x, (int)pMouseOverUI->uiPos.y, lParam);
            uiSys->SendUIMessage(pMouseOverUI, Message, wParam, NewLParam);
            return;
        }
        else {
            if (Message==WM_LBUTTONDOWN) pFocusedUI = NULL; /*Ŭ���޼������ ��Ŀ�� ����*/
            uiSys->SendUIMessage(pMouseOverUI, UIM_MOUSELEAVE, 0, 0);
            pMouseOverUI = NULL;
        }
    }
    /*���� ��Ʈ���� �ƴ� �ٸ� �����϶�*/
    /*�г� UI���� üũ*/
    for (UI_Panel* pPanel : PanelList) {
        if (pPanel->uiMotionState == eUIMotionState::eUMS_Hide) continue;
        if (IsInRect(pPanel->uiPos, pt)) {
            pMouseOverUI = pPanel;
            if (Message == WM_LBUTTONDOWN) pFocusedUI = pPanel; /*Ŭ�� �޼������ ��Ŀ�� ����*/
            NewLParam = RerouteMouseCursorPt((int)pMouseOverUI->uiPos.x, (int)pMouseOverUI->uiPos.y, lParam);
            uiSys->SendUIMessage(pPanel, UIM_MOUSEON, 0, 0);
            uiSys->SendUIMessage(pPanel, Message, wParam, NewLParam);
            return;
        }
    }
    /*UI ����Ʈ üũ*/
    for (UI* pUI : UIList) {
        if (pUI->uiMotionState != eUIMotionState::eUMS_Visible) continue;
        if (!pUI->MouseEventCheck) continue;
        if (IsInRect(pUI->uiPos, pt)) {
            pMouseOverUI = pUI;
            if (Message == WM_LBUTTONDOWN) pFocusedUI = pUI; /*Ŭ�� �޼������ ��Ŀ�� ����*/
            NewLParam = RerouteMouseCursorPt((int)pMouseOverUI->uiPos.x, (int)pMouseOverUI->uiPos.y, lParam);
            uiSys->SendUIMessage(pMouseOverUI, UIM_MOUSEON, 0, 0);
            uiSys->SendUIMessage(pMouseOverUI, Message, wParam, NewLParam);
            return;
        }
    }
}
