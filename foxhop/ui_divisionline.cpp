#include "./include/ui_divisionline.hpp"
#include "./include/ui_system.hpp"

UI_DivisionLine::UI_DivisionLine() {}

UI_DivisionLine::UI_DivisionLine(UISystem* pUISys, ID2D1RenderTarget* pRT, int nID, pfnUIHandler pfnCallback)
{
    preInit(pUISys, pRT, nID, pfnCallback);
}

UI_DivisionLine::~UI_DivisionLine() {}

/**
    @brief �����ڸ� ��� �� �� ������ ���� �̸��ʱ�ȭ�� �� �� �ִ�.
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
    @brief ���м� UI�� �ʱ�ȭ�Ѵ�.
    @param nFragmentCnt ���м��� ���п� ������ ������ �Ұ����� ����
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
    if (MessageHandler) MessageHandler(this, UIM_CREATE, NULL); /*UI���� �޼��� ����*/
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
        /*�� �ɰ���*/
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
    @brief UI ����� ������ �ʱ�ȭ �� ��ȯ�Ѵ�.
*/
void UI_DivisionLine::Destroy()
{
    int i;

    for (i = 0; i < nFragLineCnt; i++) PoolLine->deactivateObject(MLine[i]);
    uiSys->LinePoolStorage.returnPool(PoolLine);
    PoolLine = 0;
    uiSys->ReleaseUI(ID); /*�����θ� UI �ý��ۿ� ��ȯ*/
}

void UI_DivisionLine::pause(int nDelay)
{
    MOTION_INFO miColor;
    int i;

    /*TODO : �Ҹ� ��ǵ� ������ �� �ֵ��� �� ����*/
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
