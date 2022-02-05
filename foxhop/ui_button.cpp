#include "./include/ui_button.hpp"
#include "./include/ui_system.hpp"

static void DefaultButtonProc(UI* pUI, UINT Message, void* parm);

void UI_Button::CreateUI(UISystem* pUISys, int nID, ID2D1RenderTarget* pRT,
                         BUTTON_MOTION_SET UIMotionSet, BUTTON_COLOR_SET UIColorSet,
                         pfnUIHandler pfnCallback, POSITION Pos, wchar_t* pText, int nDelay)
{
    uiSys          = pUISys;
    pRenderTarget  = pRT;
    MotionSet      = UIMotionSet;
    ColorSet       = UIColorSet;

    DefaultHandler = DefaultButtonProc;
    MessageHandler = pfnCallback;
    ID             = nID;

    wcscpy_s(szText, MAX_BUTTONNAME, pText);
    nTextLen       = (int)wcslen(pText);
    MBoxFace       = uiSys->ObjPoolBox.activateObject();
    MBoxHighlight  = uiSys->ObjPoolBox.activateObject();
    MText          = uiSys->ObjPoolText.activateObject();

    InputMotion(eButtonMotionType::eMotionType_Init, MotionSet.Init, nDelay, MotionSet.InitPitch);
    uiMotionState = eUIMotionState::eUMS_PlayingVisible;
    DefaultHandler(this, UIM_CREATE, NULL); /*UI���� �޼��� ����*/
}

/**
    @brief ������ ��� ����
    @param Motion UI�� ���� ���Ÿ��
    @param nDelay UI�� ���� ������
*/
void UI_Button::InputMotion(eButtonMotionType MotionType, eButtonMotionPattern Pattern, unsigned int nDelay, unsigned int nPitch)
{
    MOTION_INFO miMove, miColor;
    POSITION TmpStartPos, TmpEndPos;
    D2D1_COLOR_F TmpStartColor;

    switch (MotionType) {
    case eButtonMotionType::eMotionType_Init: /*��Ÿ����*/
        switch (Pattern) {
        case eButtonMotionPattern::eMotionInit_Default: /*��� ����*/
            miMove  = InitMotionInfo(eMotionForm::eMotion_None, nDelay, nPitch);
            miColor = InitMotionInfo(eMotionForm::eMotion_None, nDelay, nPitch);
            MBoxFace->Init(pRenderTarget, uiPos, ColorSet.Face, TRUE);
            MBoxHighlight->Init(pRenderTarget, uiPos, ColorSet.Highlight, TRUE);
            MText->Init(pRenderTarget, uiSys->ButtonTextForm, szText, nTextLen, uiPos, ColorSet.Font, nTextLen);

            MBoxFace->addMovementMotion(miMove, FALSE, uiPos, uiPos);
            MBoxHighlight->addMovementMotion(miMove, FALSE, uiPos, uiPos);
            MText->addMovementMotion(miMove, FALSE, uiPos, uiPos);

            MBoxFace->addColorMotion(miColor, FALSE, ColorSet.Face, ColorSet.Face);
            MBoxHighlight->addColorMotion(miColor, FALSE, ColorSet.Highlight, ColorSet.Highlight);
            MText->addColorMotion(miColor, FALSE, ColorSet.Font, ColorSet.Font);
            break;

        case eButtonMotionPattern::eMotionInit_Reload:
            /*��ư ������� �Է�*/
            miMove = InitMotionInfo(eMotionForm::eMotion_x3_2, nDelay, nPitch / 2);
            miColor = InitMotionInfo(eMotionForm::eMotion_x3_2, nDelay, nPitch / 2);
            TmpStartPos = uiPos; TmpStartPos.x -= 30; TmpStartPos.y -= 100;
            TmpEndPos = uiPos; TmpEndPos.x -= 30;
            TmpStartColor = ColorSet.Face; TmpStartColor.a = 0;

            /*���� ��ġ�κ��� -30, -100 ��ġ (�»��) �������� ����*/
            MBoxFace->Init(pRenderTarget, TmpStartPos, { 0.f,0.f,0.f,0.f });
            MBoxHighlight->Init(pRenderTarget, TmpStartPos, { 0.f,0.f,0.f,0.f });
            /*ù��° �̵� (�Ʒ��� �������鼭 ������ ��Ÿ���� ���)*/
            MBoxFace->addMovementMotion(miMove, FALSE, TmpStartPos, TmpEndPos);
            MBoxFace->addColorMotion(miColor, FALSE, TmpStartColor, ColorSet.Face);
            MBoxHighlight->addMovementMotion(miMove, FALSE, TmpStartPos, TmpEndPos);
            /*�ι�° �̵� (�������� �̵��ϴ� ���*/
            MText->Init(pRenderTarget, uiSys->ButtonTextForm, szText, nTextLen, TmpEndPos, { 0.f, 0.f, 0.f, 0.f }, nTextLen);
            miMove.nDelay += nPitch / 2;
            miColor.nDelay += nPitch / 2;
            MText->addMovementMotion(miMove, FALSE, TmpEndPos, uiPos);
            MText->addColorMotion(miColor, FALSE, ColorSet.Font, ColorSet.Font);
            MBoxFace->addMovementMotion(miMove, TRUE, TmpEndPos, uiPos);
            MBoxFace->addColorMotion(miColor, TRUE, ColorSet.Highlight, ColorSet.Face);
            MBoxHighlight->addMovementMotion(miMove, TRUE, TmpEndPos, uiPos);
            break;

        case eButtonMotionPattern::eMotionInit_Flick:
            break;
        }
        break;

    case eButtonMotionType::eMotionType_Pause:
        break;

    case eButtonMotionType::eMotionType_Mouseover:
        break;

    case eButtonMotionType::eMotionType_Click:
        break;

    case eButtonMotionType::eMotionType_Color:
        break;

    case eButtonMotionType::eMotionType_Text:
        break;
    }
}

/**
    @brief UI ����� ������ �ʱ�ȭ �� ��ȯ�Ѵ�.
*/
void UI_Button::Destroy()
{
    uiSys->ObjPoolBox.deactivateObject(MBoxFace);
    uiSys->ObjPoolBox.deactivateObject(MBoxHighlight);
    uiSys->ObjPoolText.deactivateObject(MText);
    MBoxFace      = 0;
    MBoxHighlight = 0;
    MText         = 0;
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
    InputMotion(eButtonMotionType::eMotionType_Init, MotionSet.Init, nDelay, MotionSet.InitPitch);
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
    if (uiMotionState == eUIMotionState::eUMS_PlayingHide)
        uiMotionState = eUIMotionState::eUMS_Hide;
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
    pfnUIHandler       UserHandler = pUI->MessageHandler;

    switch (Message) {
#if 1 /*2021.07.27 devadversary : UI���� ����� ��ȣ�ۿ뿡���� ��Ǻ����� �⺻ �ڵ鷯�� ó���ϵ��� ������*/
    case UIM_MOUSEON:
        break;

    case UIM_MOUSELEAVE:
        break;
#endif
    default: break;
    }

    if(UserHandler) UserHandler(pUI, Message, param);
}

/**
    @brief ���丮 �ʱ�ȭ
*/
void UI_ButtonFactory::Init(UISystem* pUISystem, ID2D1RenderTarget* pRT)
{
    /*�⺻ ȯ�� ��ü �ʱ�ȭ*/
    pUISys = pUISystem;
    pRenderTarget = pRT;

    /*�⺻ ��� ���� (���� ���� ����)*/
    MotionSet.Init      = eButtonMotionPattern::eMotionInit_Default;
    MotionSet.Pause     = eButtonMotionPattern::eMotionPause_Default;
    MotionSet.Color     = eButtonMotionPattern::eMotionMouseover_Default;
    MotionSet.Text      = eButtonMotionPattern::eMotionText_Default;
    MotionSet.Click     = eButtonMotionPattern::eMotionColor_Default;
    MotionSet.MouseOver = eButtonMotionPattern::eMotionText_Default;

    /*�⺻ �������ð� ���� (���� ���� ����) ms����*/
    MotionSet.InitPitch      = 0;
    MotionSet.PausePitch     = 0;
    MotionSet.ColorPitch     = 0;
    MotionSet.TextPitch      = 0;
    MotionSet.ClickPitch     = 0;
    MotionSet.MouseOverPitch = 0;

    /*�⺻ �÷� ���� (���� ���� ����) : R G B A (0.0f ~ 1.0f)*/
    ColorSet.Face      = {0.8, 0.8, 0.8, 1};
    ColorSet.Font      = {0,   0,   0,   1};
    ColorSet.Frame     = {0,   0,   0,   0};
    ColorSet.Highlight = {1,   1,   1,   1};
}

/**
    @brief ��� ���� ��������
*/
void UI_ButtonFactory::GetCurrentMotionSet(BUTTON_MOTION_SET* pMotionSet)
{
    if (!pMotionSet) return;
    *pMotionSet = MotionSet;
}

/**
    @brief ��� ���� �����ϱ�
*/
void UI_ButtonFactory::SetCurrentMotionSet(BUTTON_MOTION_SET* pMotionSet)
{
    if (!pMotionSet) return;
    MotionSet = *pMotionSet;
}

/**
    @brief �÷� ���� ��������
*/
void UI_ButtonFactory::GetCurrentColorSet(BUTTON_COLOR_SET* pColorSet)
{
    if (!pColorSet) return;
    *pColorSet = ColorSet;
}

/**
    @brief �÷� ���� �����ϱ�
*/
void UI_ButtonFactory::SetCurrentColorSet(BUTTON_COLOR_SET* pColorSet)
{
    if (!pColorSet) return;
    ColorSet = *pColorSet;
}

/**
    @brief UI ����
    @param nID UI�ý����� �����ؼ� �Ѱ��ִ� ID��. (����ڿ��� �� ���ڴ� ������� �ʴ´�)
    @remark ���� ���丮�� ���� ������ ����Ǿ� �����ȴ� (���, ���� ��)
*/
UI* UI_ButtonFactory::CreateUI(int nID, POSITION Pos, wchar_t* pText, int nDelay, pfnUIHandler pfnCallback)
{
    UI_Button* pUI = new UI_Button;

    if (!pUI) return NULL;

    pUI->CreateUI(pUISys, nID, pRenderTarget, MotionSet, ColorSet, pfnCallback, Pos, pText, nDelay);
    return pUI;
}