#include "./include/ui_button.hpp"

static void DefaultButtonProc(UI* pUI, UINT Message, void* parm);

UI_Button::~UI_Button() {}

UI_Button::UI_Button() {}

UI_Button::UI_Button(UISystem* pUISys, ID2D1RenderTarget* pRT, int nID, pfnUIHandler pfnCallback)
{
    preInit(pUISys, pRT, nID, pfnCallback);
}

void UI_Button::preInit(UISystem* pUISys, ID2D1RenderTarget* pRT, int nID, pfnUIHandler pfnCallback)
{
    uiSys = pUISys;
    ID = nID;
    pRenderTarget = pRT;
    DefaultHandler = DefaultButtonProc;
    MessageHandler = pfnCallback;
    PoolBox = pUISys->BoxPoolStorage.getPool();
    PoolText = pUISys->TextPoolStorage.getPool();
    szText[0] = 0;
    nTextLen = 0;
    MBoxFace = NULL;
    MBoxHighlight = NULL;
    MText = NULL;
}

void UI_Button::CreateUI(UISystem* pUISys, ID2D1RenderTarget* pRT,
                         BUTTON_MOTION_SET UIMotionSet, BUTTON_COLOR_SET UIColorSet,
                         pfnUIHandler pfnCallback, POSITION Pos, int nID, wchar_t* pText, int nDelay)
{
    uiSys          = pUISys;
    pRenderTarget  = pRT;
    MotionSet      = UIMotionSet;
    ColorSet       = UIColorSet;

    DefaultHandler = DefaultButtonProc;
    MessageHandler = pfnCallback;
    ID             = nID;

    PoolBox        = pUISys->BoxPoolStorage.getPool();
    PoolText       = pUISys->TextPoolStorage.getPool();

    wcscpy_s(szText, MAX_BUTTONNAME, pText);
    nTextLen       = (int)wcslen(pText);
    MBoxFace       = PoolBox->activateObject();
    MBoxHighlight  = PoolBox->activateObject();
    MText          = PoolText->activateObject();

    //InputMotion(Motion, nDelay);
    uiMotionState = eUIMotionState::eUMS_PlayingVisible;
    DefaultHandler(this, UIM_CREATE, NULL); /*UI���� �޼��� ����*/
}

/**
    @brief ��ưUI�� ����� �ڽ��� �ؽ�Ʈ�� ���� Ǯ�� �ʱ�ȭ�Ѵ�.
    @remark TODO : ��ư ��Ÿ���� �����ؼ� �� �� �پ��� ����� ���������� �����ϵ��� ����.
*/
void UI_Button::Init(int Motion, POSITION Pos, D2D1_COLOR_F Color, wchar_t* pText, D2D1_COLOR_F TextColor, int nDelay)
{
    if (!PoolBox || !PoolText) return;
    uiPos = Pos;
    uiMotion = Motion;
    FaceColor = Color;
    FontColor = TextColor;
    nTextLen = (int)wcslen(pText);
    wcscpy_s(szText, MAX_BUTTONNAME, pText);
    /*Init�� ���ֱ� ������ ������Ʈ, �������� �� �ʿ䰡 ���⶧���� ���⼭ ������Ʈ�� �����Ѵ�.*/
    MBoxFace = PoolBox->activateObject();
    MBoxHighlight = PoolBox->activateObject();
    MText = PoolText->activateObject();
    InputMotion(Motion, nDelay);
    uiMotionState = eUIMotionState::eUMS_PlayingVisible;
    DefaultHandler(this, UIM_CREATE, NULL); /*UI���� �޼��� ����*/
}

/**
    @brief ��� ����
    @param Motion UI�� ���� ���
    @param nDelay UI�� ���� ������
*/
void UI_Button::InputMotion(int Motion, int nDelay)
{
    MOTION_INFO miMove, miColor;
    POSITION TmpStartPos, TmpEndPos;
    D2D1_COLOR_F TmpStartColor;

    switch (uiMotion) {
#if 0 /*TODO : ��ư�� ��� ��Ÿ���� �����Ͽ� �پ��� ������ �����ϵ��� ���� ����*/
    case UIBS_RELOADMOTION:
        break;
#endif /*�켱�� �Ѱ��� ��Ǹ� ����*/
    default:
        /*��ư ������� �Է�*/
        miMove.formular = eMotionForm::eMotion_x3_2;
        miMove.nDelay = nDelay;
        miMove.nPitch = 250;
        miColor.formular = eMotionForm::eMotion_x3_2;
        miColor.nDelay = nDelay;
        miColor.nPitch = 250;
        TmpStartPos = uiPos;
        TmpStartPos.x -= 30;
        TmpStartPos.y -= 100;
        TmpEndPos = uiPos;
        TmpEndPos.x -= 30;
        TmpStartColor = FaceColor;
        TmpStartColor.a = 0;
        /*���� ��ġ�κ��� -30, -100 ��ġ (�»��) �������� ����*/
        MBoxFace->Init(pRenderTarget, TmpStartPos, { 0.f,0.f,0.f,0.f });
        MBoxHighlight->Init(pRenderTarget, TmpStartPos, { 0.f,0.f,0.f,0.f });
        /*ù��° �̵� (�Ʒ��� �������鼭 ������ ��Ÿ���� ���)*/
        MBoxFace->addMovementMotion(miMove, FALSE, TmpStartPos, TmpEndPos);
        MBoxFace->addColorMotion(miColor, FALSE, TmpStartColor, FaceColor);
        MBoxHighlight->addMovementMotion(miMove, FALSE, TmpStartPos, TmpEndPos);
        /*�ι�° �̵� (�������� �̵��ϴ� ���*/
        MText->Init(pRenderTarget, uiSys->ButtonTextForm, szText, nTextLen, TmpEndPos, { 0.f, 0.f, 0.f, 0.f }, nTextLen);
        miMove.nDelay += 250;
        miColor.nDelay += 250;
        MText->addMovementMotion(miMove, FALSE, TmpEndPos, uiPos);
        MText->addColorMotion(miColor, FALSE, { 1.f, 1.f, 1.f, 1.f }, FontColor);
        MBoxFace->addMovementMotion(miMove, TRUE, TmpEndPos, uiPos);
        MBoxFace->addColorMotion(miColor, TRUE, { 1.f,1.f,1.f,1.f }, FaceColor);
        MBoxHighlight->addMovementMotion(miMove, TRUE, TmpEndPos, uiPos);
        break;
    }
}

/**
    @brief UI ����� ������ �ʱ�ȭ �� ��ȯ�Ѵ�.
*/
void UI_Button::Destroy()
{
    PoolBox->deactivateObject(MBoxFace);
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
void UI_Button::pause(int nDelay)
{
    MOTION_INFO miColor;

    uiMotionState = eUIMotionState::eUMS_PlayingHide;
    /*TODO : �Ҹ� ��ǵ� ������ �� �ֵ��� �� ����*/
    switch (0) {
    default:
        miColor.formular = eMotionForm::eMotion_Pulse1;
        miColor.nDelay = nDelay;
        miColor.nPitch = 200;
        MText->SetColor(miColor, TRUE, {0.f,0.f,0.f,0.f}, { 0.f,0.f,0.f,0.f });
        miColor.nDelay += 400;
        MBoxFace->SetColor(miColor, TRUE, { 0.f,0.f,0.f,0.f }, { 0.f,0.f,0.f,0.f });
        MBoxHighlight->SetColor(miColor, TRUE, { 0.f,0.f,0.f,0.f }, { 0.f,0.f,0.f,0.f });
        break;
    }
}

/**
    @brief �� ��ü�� ������Ʈ, �������� �簳�Ѵ�
*/
void UI_Button::resume(int nDelay)
{
    InputMotion(uiMotion, nDelay);
    uiMotionState = eUIMotionState::eUMS_PlayingVisible;
}

/**
    @brief ��ư�� ��� �Ӽ��� ������Ʈ�Ѵ�
    @return ������Ʈ �Ұ� ������ FALSE, ������Ʈ ������ TRUE
*/
BOOL UI_Button::update(unsigned long time)
{
    int nTrue = 0;

    if (uiMotionState == eUIMotionState::eUMS_Hide) return FALSE;
    nTrue += MBoxFace->update(time);
    nTrue += MBoxHighlight->update(time);
    nTrue += MText->update(time);
    if (!nTrue) return FALSE; /*Hide �Ϸ�, �Ǵ� Init�Ϸ� �ؾߴ�*/
    return TRUE;
}

/**
    @brief ������Ʈ�� ��� �Ӽ��� �����Ͽ� �������Ѵ�.
*/
void UI_Button::render()
{
    if (uiMotionState == eUIMotionState::eUMS_Hide) return;
    MBoxFace->render(pRenderTarget);
    MBoxHighlight->render(pRenderTarget);
    MText->render(pRenderTarget);
}

void UI_Button::setText(wchar_t* pText, int nDelay)
{
    MOTION_INFO miText;

    if (!pText) return;
    nTextLen = (int)wcslen(pText);
    wcscpy_s(szText, MAX_BUTTONNAME, pText);
    miText.formular = eMotionForm::eMotion_Pulse1;
    miText.nDelay = nDelay;
    miText.nPitch = 200;
    MText->SetText(miText, pText, 0/*Currnet�� TRUE�̱⶧��*/, nTextLen );
}

/**
    @brief ��ư ����� ����
*/
void UI_Button::setFaceColor(D2D1_COLOR_F Color, MOTION_INFO miColor)
{
    MBoxFace->SetColor(miColor, TRUE, {0,0,0,0}, Color);
}

/**
    @brief ��ư ���̶���Ʈ ���� ����
*/
void UI_Button::setHighlightColor(D2D1_COLOR_F Color, MOTION_INFO miColor)
{
    MBoxHighlight->SetColor(miColor, TRUE, { 0,0,0,0 }, Color);
}

/**
    @brief �⺻ ��ư �޼��� �ڵ鷯
    @remark ����� ���� ���ν����� �⺻ �ڵ鷯 ���� �� ȣ��ȴ�.
*/
static void DefaultButtonProc(UI* pUI, UINT Message, void* param)
{
    UI_Button* pButton = (UI_Button*)pUI;
    pfnUIHandler UserHandler = pUI->MessageHandler;

    switch (Message) {
#if 0 /* TODO : ��ư ���콺������ �⺻������ ó�� �Ұ�����, ����ڿ��� �ñ������ �����ؾ���.*/
    case UIM_MOUSEON:
        break;

    case UIM_MOUSELEAVE:
        break;
#endif
    default: break;
    }
    if(UserHandler) UserHandler(pUI, Message, param);
}