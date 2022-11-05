#include "./include/ui_button.hpp"
#include "./include/ui_system.hpp"

UI_Button::UI_Button(UISystem* pUISys, pfnUIHandler pfnCallback, POSITION Pos, wchar_t* pText, int nDelay)
{
    uiSys          = pUISys;
    pRenderTarget  = pUISys->D2DA.pRenTarget;
    Focusable      = FALSE;

    DefaultHandler = DefaultButtonProc;
    MessageHandler = pfnCallback;
    memset(szText, 0, sizeof(szText));
    wcscpy_s(szText, ARRAYSIZE(szText), pText);
    uiPos          = Pos;
    nTextLen       = (int)wcslen(szText);
    MBoxFace       = uiSys->ObjPoolBox.activateObject();
    MBoxHighlight  = uiSys->ObjPoolBox.activateObject();
    MText          = uiSys->ObjPoolText.activateObject();

    InputMotion(eButtonAction::eAction_Init, pUISys->Theme, nDelay, NULL);
    uiMotionState = eUIMotionState::eUMS_PlayingVisible;
    DefaultHandler(this, UIM_CREATE, NULL, NULL); /*UI���� �޼��� ����*/
}

/**
    @brief ��� ����
    @param Action ������ ����� Ÿ�� (����, �Ҹ�, ��Ÿ ���۸��)
    @param Theme  UI�� ��� �����Ӽ�
    @param nDelay ��� ��� ������
    @param param  ����� �Ķ���� (������ NULL / ex : ������ eType_Mouseover �϶�, BOOL Ÿ�����λ�� - ���콺 ����, ������ ����)
*/
void UI_Button::InputMotion(eButtonAction Action, UITheme* Theme, unsigned int nDelay, void* param)
{
    MOTION_INFO miMove, miColor;
    POSITION TmpStartPos, TmpEndPos;
    D2D1_COLOR_F TmpStartColor, TmpEndColor;
    int TmpPitch;
    int TmpLen;

    switch (Action) {
    case eButtonAction::eAction_Init: /*��Ÿ����*/
        switch (Theme->ButtonInitMotion) {
        case eButtonMotionPattern::eInit_Default: /*��� ����*/
            MBoxFace->Init(pRenderTarget, uiPos, Theme->ButtonFaceColor );
            MBoxHighlight->Init(pRenderTarget, uiPos, ALL_ZERO);
            MText->Init(pRenderTarget, uiSys->ButtonTextForm, szText, nTextLen, uiPos, Theme->ButtonFontColor, nTextLen);
            break;

        case eButtonMotionPattern::eInit_Reload:
            /*��ư ������� �Է�*/
            TmpStartPos = uiPos; TmpStartPos.x -= 30; TmpStartPos.y -= 100;
            TmpEndPos = uiPos; TmpEndPos.x -= 30;
            TmpStartColor = Theme->ButtonFaceColor; TmpStartColor.a = 0;
            TmpPitch = Theme->ButtonInitPitch / 2; /*������ �ΰ��̹Ƿ�(�Ʒ��� �̵� �� ������ �̵�) ��ġ�� ������ �����Ѵ�*/
            miMove = InitMotionInfo(eMotionForm::eMotion_x3_2, nDelay, TmpPitch);
            miColor = InitMotionInfo(eMotionForm::eMotion_x3_2, nDelay, TmpPitch);
            /*���� ��ġ�κ��� -30, -100 ��ġ (�»��) �������� ����*/
            MBoxFace->Init(pRenderTarget, TmpStartPos, ALL_ZERO);
            MBoxHighlight->Init(pRenderTarget, TmpStartPos, ALL_ZERO);
            /*ù��° �̵� (�Ʒ��� �������鼭 ������ ��Ÿ���� ���)*/
            MBoxFace->addMovementMotion(miMove, FALSE, TmpStartPos, TmpEndPos);
            MBoxFace->addColorMotion(miColor, FALSE, TmpStartColor, Theme->ButtonFaceColor);
            MBoxHighlight->addMovementMotion(miMove, FALSE, TmpStartPos, TmpEndPos);
            /*�ι�° �̵� (�������� �̵��ϴ� ���*/
            MText->Init(pRenderTarget, uiSys->ButtonTextForm, szText, nTextLen, TmpEndPos, ALL_ZERO, nTextLen);
            miMove.nDelay += TmpPitch;
            miColor.nDelay += TmpPitch;
            MText->addMovementMotion(miMove, FALSE, TmpEndPos, uiPos);
            MText->addColorMotion(miColor, FALSE, Theme->ButtonFontColor, Theme->ButtonFontColor);
            MBoxFace->addMovementMotion(miMove, TRUE, TmpEndPos, uiPos);
            //MBoxFace->addColorMotion(miColor, TRUE, Theme->ButtonHighlightColor, ALL_ZERO);
            MBoxHighlight->addMovementMotion(miMove, TRUE, TmpEndPos, uiPos);
            TmpEndColor = Theme->ButtonHighlightColor;
            TmpEndColor.a = 0;
            MBoxHighlight->addColorMotion(miColor, FALSE, Theme->ButtonHighlightColor, TmpEndColor);
            break;

        case eButtonMotionPattern::eInit_Flick:
            TmpPitch = Theme->ButtonInitPitch / 2;
            miMove = InitMotionInfo(eMotionForm::eMotion_Pulse1, nDelay, TmpPitch);
            miColor = InitMotionInfo(eMotionForm::eMotion_Pulse1, nDelay, TmpPitch);

            MBoxFace->Init(pRenderTarget, uiPos, ALL_ZERO);
            MBoxHighlight->Init(pRenderTarget, uiPos, ALL_ZERO);
            MBoxFace->addColorMotion(miColor, FALSE, ALL_ZERO, Theme->ButtonFaceColor);

            miMove.nDelay += TmpPitch;
            miColor.nDelay += TmpPitch;
            MText->Init(pRenderTarget, uiSys->ButtonTextForm, szText, nTextLen, uiPos, ALL_ZERO, nTextLen);
            MText->addColorMotion(miColor, FALSE, ALL_ZERO, Theme->ButtonFontColor);
            break;
        }
        break; /* Init ���� */

    case eButtonAction::eAction_Pause:
        switch (Theme->ButtonPauseMotion) {
        case eButtonMotionPattern::ePause_Default:
            miMove = InitMotionInfo(eMotionForm::eMotion_None, nDelay, Theme->ButtonPausePitch);
            miColor = InitMotionInfo(eMotionForm::eMotion_None, nDelay, Theme->ButtonPausePitch);
            MBoxFace->Init(pRenderTarget, uiPos, ALL_ZERO);
            //MBoxHighlight->Init(pRenderTarget, uiPos, ALL_ZERO);
            MText->Init(pRenderTarget, uiSys->ButtonTextForm, szText, nTextLen, uiPos, ALL_ZERO, nTextLen);
#if 1 /*����� ��� �����̴� �� �� ����*/
            MBoxFace->addColorMotion(miColor, FALSE, Theme->ButtonFaceColor, Theme->ButtonFaceColor);
            //MBoxHighlight->addColorMotion(miColor, FALSE, Theme->ButtonHighlightColor, ColorSet.Highlight);
            MText->addColorMotion(miColor, FALSE, Theme->ButtonFontColor, Theme->ButtonFontColor);
#endif
            break;

        case eButtonMotionPattern::ePause_Flick:
            TmpPitch = Theme->ButtonPausePitch / 2; /*������ �ΰ��̹Ƿ�(�ؽ�Ʈ�� ����� �Ŀ� ��ư�� ���������) ��ġ�� ������ �����Ѵ�*/
            miColor = InitMotionInfo(eMotionForm::eMotion_Pulse1, nDelay, TmpPitch);
#if 0
            MBoxFace->Init(pRenderTarget, uiPos, Theme->ButtonFaceColor);
            MBoxHighlight->Init(pRenderTarget, uiPos, ALL_ZERO); /*��������� ���̶���Ʈ�� ���� ����*/
            MText->Init(pRenderTarget, uiSys->ButtonTextForm, szText, nTextLen, uiPos, ALL_ZERO, nTextLen);

            MText->addColorMotion(miColor, FALSE, t, ALL_ZERO); /*�ؽ�Ʈ �����*/
            miColor.nDelay += TmpPitch; /*�ؽ�Ʈ�� ����� �Ŀ� ��ư����� ��� ����*/
            MBoxFace->addColorMotion(miColor, FALSE, Theme->ButtonFaceColor, ALL_ZERO); /*��ư �����*/
#else
            MText->SetColor(miColor, TRUE, ALL_ZERO, ALL_ZERO); /*�ؽ�Ʈ �����*/
            miColor.nDelay += TmpPitch; /*�ؽ�Ʈ�� ����� �Ŀ� ��ư����� ��� ����*/
            MBoxFace->SetColor(miColor, TRUE, ALL_ZERO, ALL_ZERO); /*��ư �����*/

#endif

            break;
        }
        break; /* Pause ���� */

    case eButtonAction::eAction_Mouseover: /*���콺�� ���������� �������� param���� ���� (BOOL)*/
        switch (Theme->ButtonMouseOverMotion) {
        case eButtonMotionPattern::eMouseover_Default:
            miColor = InitMotionInfo(eMotionForm::eMotion_None, nDelay, Theme->ButtonMouseOverPitch);
            if (param)
                MBoxHighlight->Init(pRenderTarget, uiPos, Theme->ButtonMouseoverColor);
            else
                MBoxHighlight->Init(pRenderTarget, uiPos, ALL_ZERO);
            break;

        case eButtonMotionPattern::eMouseover_Flick:
            if (param) { 
                miColor = InitMotionInfo(eMotionForm::eMotion_Pulse1, nDelay, Theme->ButtonMouseOverPitch);
                MBoxHighlight->Init(pRenderTarget, uiPos, ALL_ZERO);
                MBoxHighlight->addColorMotion(miColor, FALSE, ALL_ZERO, Theme->ButtonMouseoverColor);
            }
            else {
                miColor = InitMotionInfo(eMotionForm::eMotion_Pulse2, nDelay, Theme->ButtonMouseOverPitch);
                MBoxHighlight->Init(pRenderTarget, uiPos, ALL_ZERO);
                MBoxHighlight->addColorMotion(miColor, FALSE, Theme->ButtonMouseoverColor, ALL_ZERO);
            }
            break;
        }
        break; /* MouseOver ����*/

    case eButtonAction::eAction_Click: /* ���콺 ���ʹ�ư�� ���������� ������������ param���� ���� (UIM �޼���)*/
        switch (Theme->ButtonClickMotion) {
        case eButtonMotionPattern::eClick_Default:
            if ((__int64)param == TRUE) {
                miColor = InitMotionInfo(eMotionForm::eMotion_None, nDelay, Theme->ButtonClickPitch);
                MBoxHighlight->Init(pRenderTarget, uiPos, Theme->ButtonHighlightColor);
            }
            else {
                miColor = InitMotionInfo(eMotionForm::eMotion_None, nDelay, Theme->ButtonClickPitch);
                MBoxHighlight->Init(pRenderTarget, uiPos, ALL_ZERO);
            }
            break;

        case eButtonMotionPattern::eClick_Flash:
            if ((__int64)param != TRUE) break; /*���� ȿ���� ��� ��ư�� ���������� ����X*/
            miColor = InitMotionInfo(eMotionForm::eMotion_x3_1to0_2, nDelay, Theme->ButtonClickPitch);
            MBoxHighlight->Init(pRenderTarget, uiPos, ALL_ZERO);
            MBoxHighlight->addColorMotion(miColor, FALSE, Theme->ButtonHighlightColor, ALL_ZERO);
            break;
        }
        break;

    case eButtonAction::eAction_Color: /*���� �ٲٱ� ����� �ʿ�� �߰��� ���� �ϵ��� ��*/
        switch (Theme->ButtonColorMotion) {
        case eButtonMotionPattern::eColor_Default:
            break;

        case eButtonMotionPattern::eColor_Flash:
            break;
        }

        break;

    case eButtonAction::eAction_Text: /*�ؽ�Ʈ ��ü ���*/
        TmpLen = (int)wcslen((wchar_t*)param);
        switch (Theme->ButtonTextMotion) {
        case eButtonMotionPattern::eText_Default:
            MText->Init(pRenderTarget, uiSys->ButtonTextForm, (wchar_t*)param, TmpLen, uiPos, Theme->ButtonFontColor, TmpLen);
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
    InputMotion(eButtonAction::eAction_Pause, uiSys->Theme, nDelay, NULL);
    uiMotionState = eUIMotionState::eUMS_PlayingHide;
    
}

/**
    @brief �� ��ü�� ������Ʈ, �������� �簳�Ѵ�
*/
void UI_Button::resume(int nDelay)
{
    InputMotion(eButtonAction::eAction_Init, uiSys->Theme, nDelay, NULL);
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
    if (!pText) return;
    memset(szText, 0, sizeof(szText));
    wcscpy_s(szText, ARRAYSIZE(szText), pText);
    nTextLen = (int)wcslen(pText);
    InputMotion(eButtonAction::eAction_Text, uiSys->Theme, 0, szText);
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
        pButton->InputMotion(eButtonAction::eAction_Mouseover, pButton->uiSys->Theme, 0, (void*)TRUE);
        break;

    case UIM_MOUSELEAVE:
        pButton->InputMotion(eButtonAction::eAction_Mouseover, pButton->uiSys->Theme, 0, (void*)FALSE);
        break;

    case WM_LBUTTONDOWN:
        pButton->InputMotion(eButtonAction::eAction_Click, pButton->uiSys->Theme, 0, (void*)TRUE);
        break;
    
    case WM_LBUTTONUP:
        pButton->InputMotion(eButtonAction::eAction_Click, pButton->uiSys->Theme, 0, (void*)FALSE);
        break;

    default: break;
    }

    if(UserHandler) UserHandler(pUI, Message, wParam, lParam);
}