#include "./include/ui_listview.hpp"
#include "./include/ui_system.hpp"

#if 0
static void DefaultListProc(UI* pUI, UINT Message, void* parm);

UI_Listview::UI_Listview() {}

UI_Listview::UI_Listview(UISystem* pUISys, ID2D1RenderTarget* pRT, int nID, pfnUIHandler pfnCallback)
{
    preInit(pUISys, pRT, nID, pfnCallback);
}

UI_Listview::~UI_Listview() {}

/**
    @brief �����ڸ� ��� �� �� ������ ���� �̸��ʱ�ȭ�� �� �� �ִ�.
    @remark ��ü�� UI�ý��ۿ��� UI�� �����ҋ� ȣ�����ش�
*/
void UI_Listview::preInit(UISystem* pUISys, ID2D1RenderTarget* pRT, int nID, pfnUIHandler pfnCallback)
{
    uiSys = pUISys;
    ID = nID;
    pRenderTarget = pRT;
    DefaultHandler = DefaultListProc;
    MessageHandler = pfnCallback;
    PoolBox = pUISys->BoxPoolStorage.getPool();
    PoolText = pUISys->TextPoolStorage.getPool();
    szText[0] = 0;
    nTextLen = 0;
    MBox = NULL;
    MText = NULL;
}

/**
    @brief ��ưUI�� ����� �ڽ��� �ؽ�Ʈ������ Ǯ�� �ʱ�ȭ�Ѵ�.
    @remark TODO : ��ư ��Ÿ���� �����ؼ� �� �� �پ��� ����� ���������� �����ϵ��� ����.
*/
void UI_Listview::Init(int Motion, POSITION Pos, D2D1_COLOR_F Color, wchar_t* pText, D2D1_COLOR_F TextColor, int nDelay)
{
    if (!PoolBox || !PoolText) return;
    uiPos = Pos;
    uiMotion = Motion;
    FaceColor = Color;
    FontColor = TextColor;
    nTextLen = (int)wcslen(pText);
    wcscpy_s(szText, MAX_BUTTONNAME, pText);
    /*Init�� ���ֱ� ������ ������Ʈ, �������� �� �ʿ䰡 ���⋚���� ���⼭ ������Ʈ�� �����Ѵ�.*/
    MBox = PoolBox->activateObject();
    MText = PoolText->activateObject();
    InputMotion(Motion, nDelay);
    DefaultHandler(this, UIM_CREATE, NULL); /*UI���� �޼��� ����*/
}

/**
    @brief ��� ����
    @param Motion UI�� ���� ���
    @param nDelay UI�� ���� ������
*/
void UI_Listview::InputMotion(int Motion, int nDelay)
{
    MOTION_INFO miMove, miTextlen;
    POSITION TmpStartPos, TmpEndPos, TmpPos;
    D2D1_COLOR_F TmpStartColor;

    switch (uiMotion) {
#if 0 /*TODO : ����ƽ�� ��� ��Ÿ���� �����Ͽ� �پ��� ������ �����ϵ��� ���� ����*/
    case UIBS_RELOADMOTION:
        break;
#endif /*�켱�� �Ѱ��� ��Ǹ� ����*/
    default:
        /*��ư ������� �Է�*/
        miMove.formular = eMotionForm::eMotion_Linear1;
        miMove.nDelay = nDelay;
        miMove.nPitch = 200;
        TmpStartPos = uiPos;
        TmpStartPos.width = 0;
        MBox->Init(pRenderTarget, TmpStartPos, FaceColor);
        /*ù��° �̵�*/
        MBox->addMovementMotion(miMove, FALSE, TmpStartPos, uiPos);
        /*�ι�° �̵�*/
        TmpPos = uiPos;
        TmpPos.x += 10; /*�ؽ�Ʈ ��¦ �����*/
        TmpPos.width -= 10;
        MText->Init(pRenderTarget, uiSys->MediumTextForm, szText, nTextLen, TmpPos, FontColor, 0);
        miMove.nDelay += miMove.nPitch;
        miMove.nPitch = 300;
        MText->addLenMotion(miMove, FALSE, 0, nTextLen);
        //MText->addColorMotion(miColor, FALSE, { 1.f, 1.f, 1.f, 1.f }, FontColor);
        //MBox->addMovementMotion(miMove, TRUE, TmpEndPos, uiPos);
        //MBox->addColorMotion(miColor, TRUE, { 1.f,1.f,1.f,1.f }, FaceColor);
        break;
    }
}

/**
    @brief UI ����� ������ �ʱ�ȭ �� ��ȯ�Ѵ�.
*/
void UI_Listview::Destroy()
{
    PoolBox->deactivateObject(MBox);
    PoolText->deactivateObject(MText);
    uiSys->BoxPoolStorage.returnPool(PoolBox);
    uiSys->TextPoolStorage.returnPool(PoolText);
    PoolBox = 0;
    PoolText = 0;
    uiSys->ReleaseUI(ID); /*�����θ� UI �ý��ۿ� ��ȯ*/
}

/**
    @brief �� ��ü�� ������Ʈ, �������� �Ͻ������Ѵ� (������ �ӽ� ��Ȱ��ȭ)
*/
void UI_Listview::pause(int nDelay)
{
    MOTION_INFO miColor;

    /*TODO : �Ҹ� ��ǵ� ������ �� �ֵ��� �� ����*/
    switch (0) {
    default:
        miColor.formular = eMotionForm::eMotion_Pulse1;
        miColor.nDelay = nDelay;
        miColor.nPitch = 200;
        MText->SetColor(miColor, TRUE, { 0.f,0.f,0.f,0.f }, { 0.f,0.f,0.f,0.f });
        miColor.nDelay += 400;
        MBox->SetColor(miColor, TRUE, { 0.f,0.f,0.f,0.f }, { 0.f,0.f,0.f,0.f });
        break;
    }
}

/**
    @brief �� ��ü�� ������Ʈ, �������� �簳�Ѵ�
*/
void UI_Listview::resume(int nDelay)
{
    InputMotion(uiMotion, nDelay);
    uiPaused = FALSE;
}

void UI_Listview::update(unsigned long time)
{
    if (uiPaused) return;
    MBox->update(time);
    MText->update(time);
}

void UI_Listview::render()
{
    if (uiPaused) return;
    MBox->render(pRenderTarget);
    MText->render(pRenderTarget);
}

void UI_Listview::setText(wchar_t* pText, int nDelay)
{
    MOTION_INFO miText;

    if (!pText) return;
    nTextLen = (int)wcslen(pText);
    wcscpy_s(szText, MAX_BUTTONNAME, pText);

    miText.formular = eMotionForm::eMotion_Pulse1;
    miText.nDelay = nDelay;
    miText.nPitch = 200;
    MText->SetText(miText, pText, 0/*Current�� TRUE�̱⶧��*/, nTextLen);
}

/**
    @brief �⺻ ��ư �޼��� �ڵ鷯
    @remark ����� ���� ���ν����� �⺻ �ڵ鷯 ���� �� ȣ��ȴ�.
*/
static void DefaultListProc(UI* pUI, UINT Message, void* param)
{
    UI_Listview* button = (UI_Listview*)pUI;
    pfnUIHandler UserHandler = pUI->MessageHandler;

    switch (Message) {
    default: break;
    }
    if (UserHandler) UserHandler(pUI, Message, param);
}
#endif