#include "./include/ui_divisionline.hpp"
#include "./include/ui_system.hpp"

UI_DivisionLine::UI_DivisionLine() {}

UI_DivisionLine::UI_DivisionLine(UISystem* pUISys, ID2D1RenderTarget* pRT, int nID, pfnUIHandler pfnCallback)
{
    preInit(pUISys, pRT, nID, pfnCallback);
}

UI_DivisionLine::~UI_DivisionLine() {}

/**
    @brief 생성자를 사용 할 수 없을때 직접 미리초기화를 할 수 있다.
*/
void UI_DivisionLine::preInit(UISystem* pUISys, ID2D1RenderTarget* pRT, int nID, pfnUIHandler pfnCallback)
{
    uiSys = pUISys;
    pRenderTarget = pRT;
    MessageHandler = pfnCallback;
    ID = nID;
    PoolLine = pUISys->LinePoolStorage.getPool();
}

/**
    @brief 구분선 UI를 초기화한다.
    @param nFragmentCnt 구분선을 몇등분에 나눠서 렌더링 할것인지 결정
*/
void UI_DivisionLine::Init(int Motion, POSITION Pos, D2D1_COLOR_F Color, int nFragmentCnt, int nDelay)
{
    int i;
    MOTION_INFO miColor;

    if (nFragmentCnt < 0) nFragmentCnt = 1;
    uiPos = Pos;
    uiMotion = Motion;
    FrameColor = Color;

    nFragLineCnt = min(MAX_FRAGMENTCNT, nFragmentCnt);
    for (i = 0; i < nFragLineCnt; i++)
        MLine[i] = PoolLine->activateObject();

    InputMotion(Motion, nDelay);
    if (MessageHandler) MessageHandler(this, UIM_CREATE, NULL); /*UI생성 메세지 전송*/
}

void UI_DivisionLine::InputMotion(int Motion, int nDelay)
{
    MOTION_INFO miMove, miColor;
    POSITION TmpStartPos, TmpEndPos;
    POSITION FraggedPos;
    D2D1_COLOR_F TmpStartColor;
    D2D1_COLOR_F TmpColor;
    float tx, ty;
    int i;

    tx = (uiPos.x, uiPos.x2 - uiPos.x) / nFragLineCnt;
    ty = (uiPos.y, uiPos.y2 - uiPos.y) / nFragLineCnt;

    for (i = 0; i < nFragLineCnt; i++) {
        /*선 쪼개기*/
        FraggedPos.x = uiPos.x + tx * i;
        FraggedPos.y = uiPos.y + ty * i;
        FraggedPos.x2 = uiPos.x + tx * (i + 1);
        FraggedPos.y2 = uiPos.y + ty * (i + 1);
        MLine[i]->Init(pRenderTarget, FraggedPos, { 0.f,0.f,0.f,0.f });
        miColor.formular = eMotionForm::eMotion_Pulse1;
        miColor.nDelay = nDelay + i * 150;
        miColor.nPitch = 250;
        TmpColor = FrameColor;
        TmpColor.a = 0;
        //MLine[i]->addMovementMotion();
        MLine[i]->addColorMotion(miColor, FALSE, TmpColor, FrameColor);
    }
}

/**
    @brief UI 사용을 끝내고 초기화 후 반환한다.
*/
void UI_DivisionLine::Destroy()
{
    int i;

    for (i = 0; i < nFragLineCnt; i++) PoolLine->deactivateObject(MLine[i]);
    uiSys->LinePoolStorage.returnPool(PoolLine);
    PoolLine = 0;
    uiSys->ReleaseUI(ID); /*스스로를 UI 시스템에 반환*/
}

void UI_DivisionLine::pause(int nDelay)
{
    MOTION_INFO miColor;
    int i;

    /*TODO : 소멸 모션도 지정할 수 있도록 할 예정*/
    switch (0) {
    default:
        miColor.formular = eMotionForm::eMotion_Pulse1;
        miColor.nPitch = 200;
        for (i = 0; i < nFragLineCnt; i++) {
            miColor.nDelay = nDelay + (rand() % 300);
            MLine[i]->SetColor(miColor, TRUE, { 0.f,0.f,0.f,0.f }, { 0.f,0.f,0.f,0.f });
        }
    }
}

void UI_DivisionLine::resume(int nDelay)
{
    InputMotion(uiMotion, nDelay);
}

void UI_DivisionLine::update(unsigned long time)
{
    PoolLine->getActiveHead()->allUpdate(time);
}

void UI_DivisionLine::render()
{
    PoolLine->getActiveHead()->allRender(pRenderTarget);
}
