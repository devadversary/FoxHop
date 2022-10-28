#include "./include/ui_button.hpp"
#include "./include/ui_system.hpp"

UI_Button::UI_Button(UISystem* pUISys, ID2D1RenderTarget* pRT,
                     BUTTON_MOTION_SET UIMotionSet, BUTTON_COLOR_SET UIColorSet,
                     pfnUIHandler pfnCallback, POSITION Pos, wchar_t* pText, int nDelay)
{
    uiSys          = pUISys;
    pRenderTarget  = pRT;
    Focusable      = FALSE;
    MotionSet      = UIMotionSet;
    ColorSet       = UIColorSet;

    DefaultHandler = DefaultButtonProc;
    MessageHandler = pfnCallback;

    wcscpy_s(szText, MAX_BUTTONNAME, pText);
    uiPos          = Pos;
    nTextLen       = (int)wcslen(pText);
    MBoxFace       = uiSys->ObjPoolBox.activateObject();
    MBoxHighlight  = uiSys->ObjPoolBox.activateObject();
    MText          = uiSys->ObjPoolText.activateObject();

    InputMotion(eButtonMotionType::eType_Init, MotionSet.Init, nDelay, MotionSet.InitPitch, NULL);
    uiMotionState = eUIMotionState::eUMS_PlayingVisible;
    DefaultHandler(this, UIM_CREATE, NULL, NULL); /*UI���� �޼��� ����*/
}

/**
    @brief ��� ����
    @param MotionType ������ ����� Ÿ�� (����, �Ҹ�, ��Ÿ ���۸��)
    @param Pattern    ������ ����� ���� (������, �����̵� ���)
    @param nDelay     ��� ��� ������
    @param nPitch     ��� ��� ����ð�
    @param param      ����� �Ķ���� (������ NULL / ex : ������ eType_Mouseover �϶�, BOOL Ÿ�����λ�� - ���콺 ����, ������ ����)
*/
void UI_Button::InputMotion(eButtonMotionType MotionType, eButtonMotionPattern Pattern, unsigned int nDelay, unsigned int nPitch, void* param)
{
    MOTION_INFO miMove, miColor;
    POSITION TmpStartPos, TmpEndPos;
    D2D1_COLOR_F TmpStartColor;
    int TmpPitch;
    int TmpLen;

    switch (MotionType) {
    case eButtonMotionType::eType_Init: /*��Ÿ����*/
        switch (Pattern) {
        case eButtonMotionPattern::eInit_Default: /*��� ����*/
            MBoxFace->Init(pRenderTarget, uiPos, ColorSet.Face );
            MBoxHighlight->Init(pRenderTarget, uiPos, ALL_ZERO);
            MText->Init(pRenderTarget, uiSys->ButtonTextForm, szText, nTextLen, uiPos, ColorSet.Font, nTextLen);
            break;

        case eButtonMotionPattern::eInit_Reload:
            /*��ư ������� �Է�*/
            miMove = InitMotionInfo(eMotionForm::eMotion_x3_2, nDelay, nPitch / 2);
            miColor = InitMotionInfo(eMotionForm::eMotion_x3_2, nDelay, nPitch / 2);
            TmpStartPos = uiPos; TmpStartPos.x -= 30; TmpStartPos.y -= 100;
            TmpEndPos = uiPos; TmpEndPos.x -= 30;
            TmpStartColor = ColorSet.Face; TmpStartColor.a = 0;
            TmpPitch = nPitch / 2; /*������ �ΰ��̹Ƿ�(�Ʒ��� �̵� �� ������ �̵�) ��ġ�� ������ �����Ѵ�*/
            /*���� ��ġ�κ��� -30, -100 ��ġ (�»��) �������� ����*/
            MBoxFace->Init(pRenderTarget, TmpStartPos, ALL_ZERO);
            MBoxHighlight->Init(pRenderTarget, TmpStartPos, ALL_ZERO);
            /*ù��° �̵� (�Ʒ��� �������鼭 ������ ��Ÿ���� ���)*/
            MBoxFace->addMovementMotion(miMove, FALSE, TmpStartPos, TmpEndPos);
            MBoxFace->addColorMotion(miColor, FALSE, TmpStartColor, ColorSet.Face);
            MBoxHighlight->addMovementMotion(miMove, FALSE, TmpStartPos, TmpEndPos);
            /*�ι�° �̵� (�������� �̵��ϴ� ���*/
            MText->Init(pRenderTarget, uiSys->ButtonTextForm, szText, nTextLen, TmpEndPos, ALL_ZERO, nTextLen);
            miMove.nDelay += TmpPitch;
            miColor.nDelay += TmpPitch;
            MText->addMovementMotion(miMove, FALSE, TmpEndPos, uiPos);
            MText->addColorMotion(miColor, FALSE, ColorSet.Font, ColorSet.Font);
            MBoxFace->addMovementMotion(miMove, TRUE, TmpEndPos, uiPos);
            MBoxFace->addColorMotion(miColor, TRUE, ColorSet.Highlight, ColorSet.Face);
            MBoxHighlight->addMovementMotion(miMove, TRUE, TmpEndPos, uiPos);
            break;

        case eButtonMotionPattern::eInit_Flick:
            TmpPitch = nPitch / 2;
            miMove = InitMotionInfo(eMotionForm::eMotion_Pulse1, nDelay, TmpPitch);
            miColor = InitMotionInfo(eMotionForm::eMotion_Pulse1, nDelay, TmpPitch);
            MBoxFace->Init(pRenderTarget, uiPos, ALL_ZERO);
            MBoxHighlight->Init(pRenderTarget, uiPos, ALL_ZERO);

            //MBoxFace->addMovementMotion(miMove, FALSE, uiPos, uiPos);
            //MBoxHighlight->addMovementMotion(miMove, FALSE, uiPos, uiPos);
            MBoxFace->addColorMotion(miColor, FALSE, ALL_ZERO, ColorSet.Face);
            //MBoxHighlight->addColorMotion(miColor, FALSE, ALL_ZERO, ColorSet.Highlight);

            miMove.nDelay += TmpPitch;
            miColor.nDelay += TmpPitch;
            MText->Init(pRenderTarget, uiSys->ButtonTextForm, szText, nTextLen, uiPos, ALL_ZERO, nTextLen);
            //MText->addMovementMotion(miMove, FALSE, uiPos, uiPos);
            MText->addColorMotion(miColor, FALSE, ALL_ZERO, ColorSet.Font);
            break;
        }
        break;

    case eButtonMotionType::eType_Pause:
        switch (Pattern) {
        case eButtonMotionPattern::ePause_Default:
            miMove = InitMotionInfo(eMotionForm::eMotion_None, nDelay, nPitch);
            miColor = InitMotionInfo(eMotionForm::eMotion_None, nDelay, nPitch);
            MBoxFace->Init(pRenderTarget, uiPos, ALL_ZERO);
            MBoxHighlight->Init(pRenderTarget, uiPos, ALL_ZERO);
            MText->Init(pRenderTarget, uiSys->ButtonTextForm, szText, nTextLen, uiPos, ALL_ZERO, nTextLen);
#if 1 /*����� ��� �����̴� �� �� ����*/
            MBoxFace->addColorMotion(miColor, FALSE, ColorSet.Face, ColorSet.Face);
            MBoxHighlight->addColorMotion(miColor, FALSE, ColorSet.Highlight, ColorSet.Highlight);
            MText->addColorMotion(miColor, FALSE, ColorSet.Font, ColorSet.Font);
#endif
            break;

        case eButtonMotionPattern::ePause_Flick:
            TmpPitch = nPitch / 2; /*������ �ΰ��̹Ƿ�(�ؽ�Ʈ�� ����� �Ŀ� ��ư�� ���������) ��ġ�� ������ �����Ѵ�*/
            miColor = InitMotionInfo(eMotionForm::eMotion_Pulse2, nDelay, TmpPitch);
            MBoxFace->Init(pRenderTarget, uiPos, ALL_ZERO);
            MBoxHighlight->Init(pRenderTarget, uiPos, ALL_ZERO); /*��������� ���̶���Ʈ�� ���� ����*/
            MText->Init(pRenderTarget, uiSys->ButtonTextForm, szText, nTextLen, uiPos, ALL_ZERO, nTextLen);

            MText->addColorMotion(miColor, FALSE, ColorSet.Font, ALL_ZERO); /*�ؽ�Ʈ �����*/
            miColor.nDelay += TmpPitch; /*�ؽ�Ʈ�� ����� �Ŀ� ��ư����� ��� ����*/
            MBoxFace->addColorMotion(miColor, FALSE, ColorSet.Face, ALL_ZERO); /*��ư �����*/
            break;
        }
        break;

    case eButtonMotionType::eType_Mouseover: /*���콺�� ���������� �������� param���� ���� (BOOL)*/
        switch (Pattern) {
        case eButtonMotionPattern::eMouseover_Default:
            miColor = InitMotionInfo(eMotionForm::eMotion_None, nDelay, nPitch);
            if (param)
                MBoxHighlight->Init(pRenderTarget, uiPos, ColorSet.Mouseover);
            else
                MBoxHighlight->Init(pRenderTarget, uiPos, ALL_ZERO);
            break;

        case eButtonMotionPattern::eMouseover_Flick:
            if (param) { 
                miColor = InitMotionInfo(eMotionForm::eMotion_Pulse1, nDelay, nPitch);
                MBoxHighlight->Init(pRenderTarget, uiPos, ALL_ZERO);
                MBoxHighlight->addColorMotion(miColor, FALSE, ALL_ZERO, ColorSet.Highlight);
            }
            else {
                miColor = InitMotionInfo(eMotionForm::eMotion_Pulse2, nDelay, nPitch);
                MBoxHighlight->Init(pRenderTarget, uiPos, ALL_ZERO);
                MBoxHighlight->addColorMotion(miColor, FALSE, ColorSet.Highlight, ALL_ZERO);
            }
            break;
        }
        break;

    case eButtonMotionType::eType_Click: /* ���콺 ���ʹ�ư�� ���������� ������������ param���� ���� (UIM �޼���)*/
        switch (Pattern) {
        case eButtonMotionPattern::eClick_Default:
            if ((int)param == WM_LBUTTONDOWN) {
                miColor = InitMotionInfo(eMotionForm::eMotion_None, nDelay, nPitch);
                MBoxHighlight->Init(pRenderTarget, uiPos, ALL_ZERO);
                MBoxHighlight->addColorMotion(miColor, FALSE, ColorSet.Highlight, ColorSet.Highlight);
            }
            else {
                miColor = InitMotionInfo(eMotionForm::eMotion_None, nDelay, nPitch);
                MBoxHighlight->Init(pRenderTarget, uiPos, ALL_ZERO);
                MBoxHighlight->addColorMotion(miColor, FALSE, ALL_ZERO, ALL_ZERO);
            }
            break;

        case eButtonMotionPattern::eClick_Flash:
            if ((int)param != WM_LBUTTONDOWN) break; /*���� ȿ���� ��� ��ư�� ���������� ����X*/

            miColor = InitMotionInfo(eMotionForm::eMotion_x3_1to0_2, nDelay, nPitch);
            MBoxHighlight->Init(pRenderTarget, uiPos, ALL_ZERO);
            MBoxHighlight->addColorMotion(miColor, FALSE, ColorSet.Highlight, ALL_ZERO);
            break;
        }
        break;

    case eButtonMotionType::eType_Color: /*���� �ٲٱ� ����� �ʿ�� �߰��� ���� �ϵ��� ��*/
        switch (Pattern) {
        case eButtonMotionPattern::eColor_Default:
            break;

        case eButtonMotionPattern::eColor_Flash:
            break;
        }

        break;

    case eButtonMotionType::eType_Text: /*�ؽ�Ʈ ��ü ���*/
        TmpLen = wcslen((wchar_t*)param);
        switch (Pattern) {
        case eButtonMotionPattern::eText_Default:
            MText->Init(pRenderTarget, uiSys->ButtonTextForm, (wchar_t*)param, TmpLen, uiPos, ColorSet.Font, TmpLen);
            break;
        }

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
    uiSys->ReleaseUI(); /*�����θ� UI �ý��ۿ� ��ȯ*/
}

/**
    @brief �� ��ü�� ������Ʈ, �������� �Ͻ������Ѵ� (������ �ӽ� ��Ȱ��ȭ)
*/
void UI_Button::pause(int nDelay)
{
    MOTION_INFO miColor;

    InputMotion(eButtonMotionType::eType_Pause, MotionSet.Pause, nDelay, MotionSet.PausePitch, NULL);
    uiMotionState = eUIMotionState::eUMS_PlayingHide;
    
}

/**
    @brief �� ��ü�� ������Ʈ, �������� �簳�Ѵ�
*/
void UI_Button::resume(int nDelay)
{
    InputMotion(eButtonMotionType::eType_Init, MotionSet.Init, nDelay, MotionSet.InitPitch, NULL);
    uiMotionState = eUIMotionState::eUMS_PlayingVisible;
}

/**
    @brief ��ư�� ��� �Ӽ��� ������Ʈ�Ѵ�
    @return ������Ʈ �Ұ� ������ FALSE, ������Ʈ ������ TRUE
*/
BOOL UI_Button::update(unsigned long time)
{
    int nTrue = 0;

    /*UI �� ���� ���·� �����ϸ� ���� ����*/
    if (uiMotionState == eUIMotionState::eUMS_Hide) return FALSE;

    nTrue += MBoxFace->update(time);
    nTrue += MBoxHighlight->update(time);
    nTrue += MText->update(time);

    /*nTrue�� 0�̸� ��� ������ ����� ��. ~ing ���¸� Ȯ����Ų��.*/
    if (!nTrue) {
        if (uiMotionState == eUIMotionState::eUMS_PlayingHide)
            uiMotionState = eUIMotionState::eUMS_Hide;

        else if (uiMotionState == eUIMotionState::eUMS_PlayingVisible)
            uiMotionState = eUIMotionState::eUMS_Visible;
        return FALSE;
    }
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
    memset(szText, 0, sizeof(szText));
    wcscpy_s(szText, ARRAYSIZE(szText), pText);
    InputMotion(eButtonMotionType::eType_Text, MotionSet.Text, 0, MotionSet.TextPitch, szText);
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
void UI_Button::DefaultButtonProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam)
{
    pfnUIHandler UserHandler = pUI->MessageHandler;
    UI_Button* pButton = static_cast<UI_Button*>(pUI);

    if (!pUI) return;

    /*2021.07.27 devadversary : UI���� ����� ��ȣ�ۿ뿡���� ��Ǻ����� �⺻ �ڵ鷯�� ó���ϵ��� ������*/
    switch (Message) {
    case UIM_MOUSEON: /*ON / LEAVE �޼����� UI_Panel �� ���� SendMessage �� ���޵ȴ� */
        pButton->InputMotion(eButtonMotionType::eType_Mouseover, pButton->MotionSet.MouseOver, 0, pButton->MotionSet.MouseOverPitch, (void*)TRUE);
        break;

    case UIM_MOUSELEAVE:
        pButton->InputMotion(eButtonMotionType::eType_Mouseover, pButton->MotionSet.MouseOver, 0, pButton->MotionSet.MouseOverPitch, (void*)FALSE);
        break;

    case WM_LBUTTONDOWN:
        break;
    
    case WM_LBUTTONUP:
        break;

    default: break;
    }

    if(UserHandler) UserHandler(pUI, Message, wParam, lParam);
}

/**
    @brief ���丮 �ʱ�ȭ
    @remark UISystem::Init ���� ȣ��ȴ�.
*/
UI_ButtonFactory::UI_ButtonFactory(UISystem* pUISystem, ID2D1RenderTarget* pRT)
{
    /*�⺻ ȯ�� ��ü �ʱ�ȭ*/
    pUISys = pUISystem;
    pRenderTarget = pRT;

    /*�⺻ ��� ���� (���� ���� ����)*/
    MotionSet.Init      = eButtonMotionPattern::eInit_Default;
    MotionSet.Pause     = eButtonMotionPattern::ePause_Default;
    MotionSet.Color     = eButtonMotionPattern::eMouseover_Default;
    MotionSet.Text      = eButtonMotionPattern::eText_Default;
    MotionSet.Click     = eButtonMotionPattern::eColor_Default;
    MotionSet.MouseOver = eButtonMotionPattern::eText_Default;

    /*�⺻ �������ð� ���� (���� ���� ����) ms����*/
    MotionSet.InitPitch      = 0;
    MotionSet.PausePitch     = 0;
    MotionSet.ColorPitch     = 0;
    MotionSet.TextPitch      = 0;
    MotionSet.ClickPitch     = 0;
    MotionSet.MouseOverPitch = 0;

    /*�⺻ �÷� ���� (���� ���� ����) : R G B A (0.0f ~ 1.0f)*/
    ColorSet.Face      = {0.8f, 0.8f, 0.8f, 1.f};  /*���� ȸ��*/
    ColorSet.Font      = {0.f,  0.f,  0.f,  1.f};
    ColorSet.Frame     = {0.f,  0.f,  0.f,  0.f};
    ColorSet.Mouseover = {1.f,  1.f,  1.f,  0.5f}; /*������ ���*/
    ColorSet.Highlight = {1.f,  1.f,  1.f,  1.f};
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
    @brief ��ư ����
    @param Pos ������ UI�� ��ġ�� ũ��
    @param pText ������ UI�� ���� �ؽ�Ʈ (�ʿ� ���� UI���� �˾Ƽ� �� ���ڸ� ������)
    @param nDelay ������ UI�� ��� ������
    @param pfnCallback ������ UI�� ���� �̺�Ʈ �ڵ鷯
    @remark ���� ���丮�� ���� ������ ����Ǿ� �����ȴ� (���, ���� ��)
*/
UI_Button* UI_ButtonFactory::CreateUI(POSITION Pos, wchar_t* pText, int nDelay, pfnUIHandler pfnCallback)
{
    return new UI_Button(pUISys, pRenderTarget, MotionSet, ColorSet, pfnCallback, Pos, pText, nDelay);
}
