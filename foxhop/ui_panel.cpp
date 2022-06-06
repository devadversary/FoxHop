#include "./include/ui_panel.hpp"
#include "./include/ui_system.hpp"

#include "./include/ui_button.hpp"
#include "./include/ui_static.hpp"
#include "./include/ui_fraggedline.hpp"
#include "./include/ui_listview.hpp"

#if 0
UI_Panel::UI_Panel(UISystem* pUISys, ID2D1RenderTarget* pRT, pfnUIHandler pfnCallback, POSITION Pos, int nDelay = 0)
{
    uiSys = pUISys;
    pRenderTarget = pRT;
    MessageHandler = pfnCallback;
    DefaultHandler = DefaultPanelHandler;
    PanelPos = Pos;
    PanelDelay = nDelay;
    //transform = D2D1::Matrix3x2F::Translation(PanelPos.x + 0.5f, PanelPos.y + 0.5f);
    if (MessageHandler) MessageHandler(this, UIM_CREATE, NULL); /*UI���� �޼��� ����*/

}

UI_Panel::~UI_Panel() {}

static void DefaultPanelHandler(UI* pUI, UINT Message, void* param);


#if 0 /*�ʿ� ���� �� ��*/
void UI_Panel::InputMotion(int Motion, int nDelay)
{

}
#endif

/**
    @brief �ش� �г��� ��� ��Ȱ��ȭ�ϰ� UI�Ҹ�. ������Ʈ, ������ ����Ʈ���� ����
*/
void UI_Panel::pause(int nDelay)
{
    uiSys->SendUIMessage(this, UIM_PAUSE, &nDelay);
}

void UI_Panel::resume(int nDelay)
{
    uiSys->SendUIMessage(this, UIM_RESUME, &nDelay);
}

void UI_Panel::Destroy()
{
    uiSys->SendUIMessage(this, UIM_DELETE, NULL);
}

/**
    @brief �г��� ���� ��ü UI ������Ʈ (���� �г� ����)
*/
BOOL UI_Panel::update(unsigned long time)
{

}

/**
    @brief �г��� ���� ��ü UI ������ (���� �г� ����)
*/
void UI_Panel::render()
{

}

/**
    @brief �ش� �г��� ���� UI�� �����ϰ� ���� ����Ʈ�� ����Ѵ�
    @return ������ UI ��ü
*/
UI* UI_Panel::CreateUI(UIType type, unsigned int nID, POSITION pos, wchar_t* pText, int nDelay, pfnUIHandler callback)
{

}

/**
    @brief �⺻ ���콺 �̺�Ʈ �ڵ鷯
    @remark ���UI�� ���콺 ���� / ���� �̺�Ʈ�� �����Ѵ�.
*/
void UI_Panel::DefaultMouseHandler(POINT pt, UINT Message, void* param)
{
    int i;
    UI* pTargetUI = NULL;

    /*������ �����ȿ� �ִ� ��Ʈ���� �־��ٸ� �� ���� �˻�*/
    if (pTmpUI) {
        /*���� ��Ʈ�Ѻ��� �����˻�*/
        if (IsInRect(pTmpUI->uiPos, pt)) {
            uiSys->SendUIMessage(pTmpUI, Message, param);
            return;
        }
        else {
            uiSys->SendUIMessage(pTmpUI, UIM_MOUSELEAVE, param);
            pTmpUI = NULL;
        }
    }
    /*���� ��Ʈ���� �ƴ� �ٸ� �����϶�*/
    /*�г� UI���� üũ*/
    for (i = 0; i < nCntPanel; i++) {
        if (PanelList[i]->uiMotionState == eUIMotionState::eUMS_Hide) continue;
        if (IsInRect(PanelList[i]->uiPos, pt)) {
            pTmpUI = PanelList[i];
            uiSys->SendUIMessage(pTmpUI, UIM_MOUSEON, param);
            uiSys->SendUIMessage(pTmpUI, Message, param);
            return;
        }
    }
    /*UI ����Ʈ üũ*/
    for (i = 0; i < nCntUI; i++) {
        if (UIList[i].pUI->uiMotionState == eUIMotionState::eUMS_Hide) continue;
        /* ������-ũ�� �� ��� ������-���� ���� ����ϴ� UI�� �����˻� X*/
        if (UIList[i].pUI->uiType == UIType::eUI_FragLine) continue;
        if (IsInRect(UIList[i].pUI->uiPos, pt)) {
            pTmpUI = UIList[i].pUI;
            uiSys->SendUIMessage(pTmpUI, UIM_MOUSEON, param);
            uiSys->SendUIMessage(pTmpUI, Message, param);
            return;
        }
    }
}

/**
    @brief �г��� �⺻ �ڵ鷯.
    @remark ���콺 �������� �⺻ �ڵ鷯���� ó���ǰ�,
    @n      �� ���� ����� ���� �ڵ鷯�� ����ȴ�.
*/
static void DefaultPanelHandler(UI* pUI, UINT Message, void* param)
{
    UI_Panel* pPanel = (UI_Panel*)pUI;
    pfnUIHandler UserHandler = pUI->MessageHandler;
    POINT pt;

    switch (Message) {
    case UIM_MOUSEMOVE:
        pt = *(POINT*)param;
        //pPanel->DefaultMouseHandler(pt,  );
    }
    if (UserHandler) UserHandler(pUI, Message, param);
}

#endif