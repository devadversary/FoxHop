#include "./include/ui_panel.hpp"
#include "./include/ui_system.hpp"

#include "./include/ui_button.hpp"
#include "./include/ui_static.hpp"
#include "./include/ui_fraggedline.hpp"
#include "./include/ui_listview.hpp"

UI_Panel::UI_Panel(UISystem* pUISys, ID2D1RenderTarget* pRT, int nID, pfnUIHandler pfnCallback)
{
    void* pTmp;
    char p[32];
    uiSys = pUISys;
    pRenderTarget = pRT;
    MessageHandler = pfnCallback;
    DefaultHandler = DefaultPanelHandler;
}

UI_Panel::~UI_Panel() {}

static void DefaultPanelHandler(UI* pUI, UINT Message, void* param);

void UI_Panel::Init(POSITION Pos, int nDelay)
{
    memset(PanelList, 0, sizeof(PanelList)); /*패널 하나당 가질 수 있는 최대 하위패널 갯수 셋팅*/
    memset(UIList, 0, sizeof(UIList)); /*패널 하나당 가질 수 있는 최대 하위UI 갯수 셋팅*/
    PanelPos = Pos;
    PanelDelay = nDelay;
    nTimeSum = 0;
    nCntPanel = 0;
    nCntUI = 0;
    transform = D2D1::Matrix3x2F::Translation(PanelPos.x+0.5f, PanelPos.y+0.5f);
    if (MessageHandler) MessageHandler(this, UIM_CREATE, NULL); /*UI생성 메세지 전송*/
}

#if 0 /*필요 없을 듯 함*/
void UI_Panel::InputMotion(int Motion, int nDelay)
{

}
#endif

/**
    @brief 해당 패널을 잠시 비활성화하고 UI소멸. 업데이트, 렌더링 리스트에서 제외
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
    @brief 패널이 가진 전체 UI 업데이트 (하위 패널 포함)
*/
void UI_Panel::update(unsigned long time)
{
    int i;

    for (i = 0; i < nCntPanel; i++) PanelList[i]->update(time);

    /*UI 타입별로 렌더링*/
    for (i = 0; i < nCntUI; i++) {
        switch (UIList[i].type) {
        case UIType::eUI_Button:
            break;

        case UIType::eUI_Static:
            break;

        case UIType::eUI_FragLine:
            break;

        case UIType::eUI_List:
            break;

        case UIType::eUI_Edit:
            break;

        case UIType::eUI_Graph:
            break;

        case UIType::eUI_Progress:
            break;

        case UIType::eUI_Scroll:
            break;

        default:break;
        }
    }
}

/**
    @brief 패널이 가진 전체 UI 렌더링 (하위 패널 포함)
*/
void UI_Panel::render()
{
    int i;
    pRenderTarget->SetTransform(transform);
    /*패널은 패널대로 렌더링*/
    for (i = 0; i < nCntPanel; i++) PanelList[i]->render();

    /*UI 타입별로 렌더링*/
    for (i = 0; i < nCntUI; i++) {
        switch (UIList[i].type) {
        case UIType::eUI_Button:
            ((UI_Button*)UIList[i].pUI)->render();
            break;

        case UIType::eUI_Static:
            break;

        case UIType::eUI_FragLine:
            break;

        case UIType::eUI_List:
            break;

        case UIType::eUI_Edit:
            break;

        case UIType::eUI_Graph:
            break;

        case UIType::eUI_Progress:
            break;

        case UIType::eUI_Scroll:
            break;

        default:break;
        }
    }
}

/**
    @brief 해당 패널의 하위 UI를 생성하고 내부 리스트에 등록한다
    @return 생성된 UI 객체
*/
UI* UI_Panel::CreateUI(UIType type, unsigned int nID, POSITION pos, wchar_t* pText, int nDelay, pfnUIHandler callback)
{

}

/**
    @brief 기본 마우스 이벤트 핸들러
*/
void UI_Panel::DefaultMouseHandler(POINT pt, UINT Message, void* param)
{
    int i;
    UI* pTargetUI = NULL;

    /*이전에 영역안에 있던 컨트롤이 있었다면 걔 먼저 검사*/
    if (pTmpUI) {
        /*이전 컨트롤부터 영역검사*/
        if (IsInRect(pTmpUI->uiPos, pt)) {
            uiSys->SendUIMessage(pTmpUI, Message, param);
            return;
        }
        else {
            uiSys->SendUIMessage(pTmpUI, UIM_MOUSELEAVE, param);
            pTmpUI = NULL;
        }
    }
    /*이전 컨트롤이 아닌 다른 영역일때*/
    /*패널 UI부터 체크*/
    for (i = 0; i < nCntPanel; i++) {
        if (PanelList[i]->uiMotionState == eUIMotionState::eUMS_Hide) continue;
        if (IsInRect(PanelList[i]->uiPos, pt)) {
            pTmpUI = PanelList[i];
            uiSys->SendUIMessage(pTmpUI, UIM_MOUSEON, param);
            uiSys->SendUIMessage(pTmpUI, Message, param);
            return;
        }
    }
    /*UI 리스트 체크*/
    for (i = 0; i < nCntUI; i++) {
        if (UIList[i].pUI->uiMotionState == eUIMotionState::eUMS_Hide) continue;
        /* 시작점-크기 쌍 대신 시작점-끝점 쌍을 사용하는 UI는 영역검사 X*/
        if (UIList[i].pUI->uiType == UIType::eUI_FragLine) continue;
        if (IsInRect(UIList[i].pUI->uiPos, pt)) {
            pTmpUI = UIList[i].pUI;
            uiSys->SendUIMessage(pTmpUI, UIM_MOUSEON, param);
            uiSys->SendUIMessage(pTmpUI, Message, param);
            return;
        }
    }
}

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