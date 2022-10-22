#include "./include/ui_panel.hpp"
#include "./include/ui_system.hpp"

#include "./include/ui_button.hpp"
#include "./include/ui_static.hpp"
#include "./include/ui_fraggedline.hpp"
#include "./include/ui_listview.hpp"

static void DefaultPanelHandler(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam);

#if 1
UI_Panel::UI_Panel(UISystem* pUISys, ID2D1RenderTarget* pRT, pfnUIHandler pfnCallback, POSITION Pos, int nDelay)
{
    uiSys          = pUISys;
    pRenderTarget  = pRT;
    Focusable      = TRUE;
    MessageHandler = pfnCallback;
    DefaultHandler = DefaultPanelHandler;
    uiPos          = Pos;
    PanelDelay     = nDelay;
    //transform = D2D1::Matrix3x2F::Translation(PanelPos.x + 0.5f, PanelPos.y + 0.5f);
    if (MessageHandler) MessageHandler(this, UIM_CREATE, NULL, NULL); /*UI���� �޼��� ����*/

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
    return FALSE;
}

/**
    @brief �г��� ���� ��ü UI ������ (���� �г� ����)
*/
void UI_Panel::render()
{
    pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(0.5f + uiPos.x, 0.5f + uiPos.y));
}

/**
    @brief �ش� �г��� ���� UI�� �����ϰ� ���� ����Ʈ�� ����Ѵ�
    @return ������ UI ��ü
*/
UI* UI_Panel::CreateUI(UIType type, POSITION pos, wchar_t* pText, int nDelay, pfnUIHandler callback)
{
    return NULL;
}

/**
    @brief �⺻ ���콺 �̺�Ʈ �ڵ鷯
    @remark ���UI�� ���콺 ���� / ���� �̺�Ʈ�� �����Ѵ�.
*/
void UI_Panel::DefaultMouseHandler(POINT pt, UINT Message, WPARAM wParam, LPARAM lParam)
{
    int i;
    UI* pTargetUI = NULL;

    /*������ �����ȿ� �ִ� ��Ʈ���� �־��ٸ� �� ���� �˻�*/
    if (pMouseOverUI) {
        /*���� ��Ʈ�Ѻ��� �����˻�*/
        if (IsInRect(pMouseOverUI->uiPos, pt)) {
            uiSys->SendUIMessage(pMouseOverUI, Message, wParam, lParam);
            return;
        }
        else {
            uiSys->SendUIMessage(pMouseOverUI, UIM_MOUSELEAVE, wParam, lParam);
            pMouseOverUI = NULL;
        }
    }
    /*���� ��Ʈ���� �ƴ� �ٸ� �����϶�*/
    /*�г� UI���� üũ*/
    for (UI_Panel* pPanel : PanelList) {
        if (pPanel->uiMotionState == eUIMotionState::eUMS_Hide) continue;
        if (IsInRect(pPanel->uiPos, pt)) {
            pMouseOverUI = pPanel;
            uiSys->SendUIMessage(pPanel, UIM_MOUSEON, wParam, lParam);
            uiSys->SendUIMessage(pPanel, Message, wParam, lParam);
            return;
        }
    }
    /*UI ����Ʈ üũ*/
    for (UI* pUI : UIList) {
        if (pUI->uiMotionState == eUIMotionState::eUMS_Hide) continue;
        /* ������-ũ�� �� ��� ������-���� ���� ����ϴ� UI�� �����˻� X*/
        if (pUI->uiType == UIType::eUI_FragLine) continue;
        if (IsInRect(pUI->uiPos, pt)) {
            pMouseOverUI = pUI;
            uiSys->SendUIMessage(pMouseOverUI, UIM_MOUSEON, wParam, lParam);
            uiSys->SendUIMessage(pMouseOverUI, Message, wParam, lParam);
            return;
        }
    }
}

/**
    @brief �г��� �⺻ �ڵ鷯.
    @remark ���콺 �������� �⺻ �ڵ鷯���� ó���ǰ�,
    @n      �� ���� ����� ���� �ڵ鷯�� ����ȴ�.
*/
static void DefaultPanelHandler(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam)
{
    UI_Panel* pPanel = (UI_Panel*)pUI;
    pfnUIHandler UserHandler = pUI->MessageHandler;
    POINT pt;

    switch (Message) {
    case UIM_MOUSEMOVE:
        pt = *(POINT*)wParam;
        //pPanel->DefaultMouseHandler(pt,  );
    }
    if (UserHandler) UserHandler(pUI, Message, wParam, lParam);
}

#endif