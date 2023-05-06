#include "./include/ui_button.hpp"
#include "./include/ui_system.hpp"

UI_Button::UI_Button(UISystem* pUISys, pfnUIHandler pfnCallback, POSITION Pos, wchar_t* pText, int nDelay, UI_Button_MotionParam MotionParam)
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

    MBoxFace       = new PropBox();
    MBoxHighlight  = new PropBox();
    MText          = new PropText();
    Motion         = MotionParam;

    InputMotion(eButtonAction::eAction_Init, nDelay, NULL);
    uiMotionState = eUIMotionState::eUMS_PlayingVisible;
    DefaultHandler(this, UIM_CREATE, NULL, NULL); /*UI���� �޼��� ����*/
}

UI_Button::~UI_Button()
{
    delete MBoxFace;
    delete MBoxHighlight;
    delete MText;
}
/**
    @brief ��� ����
    @param Action ������ ����� Ÿ�� (����, �Ҹ�, ��Ÿ ���۸��)
    @param Theme  UI�� ��� �����Ӽ�
    @param nDelay ��� ��� ������
    @param param  ����� �Ķ���� (������ NULL / ex : ������ eType_Mouseover �϶�, BOOL Ÿ�����λ�� - ���콺 ����, ������ ����)
*/
void UI_Button::InputMotion(eButtonAction Action, unsigned int nDelay, void* param)
{
    MOTION_INFO miMove, miColor;
    POSITION TmpStartPos, TmpEndPos;
    D2D1_COLOR_F TmpStartColor, TmpEndColor;
    int TmpPitch;
    int TmpLen;

    switch (Action) {
    case eButtonAction::eAction_Init: /*��Ÿ����*/
        switch (Motion.InitMotion) {
        case eButtonMotionPattern::eInit_Default: /*��� ����*/
            MBoxFace->Init(pRenderTarget, uiPos, Motion.FaceColor );
            MBoxHighlight->Init(pRenderTarget, uiPos, ALL_ZERO);
            MText->Init(pRenderTarget, uiSys->ButtonTextForm, szText, nTextLen, uiPos, Motion.FontColor, nTextLen);
            break;

        case eButtonMotionPattern::eInit_Reload:
            /*��ư ������� �Է�*/
            TmpStartPos = uiPos; TmpStartPos.x -= 30; TmpStartPos.y -= 100;
            TmpEndPos = uiPos; TmpEndPos.x -= 30;
            TmpStartColor = Motion.FaceColor; TmpStartColor.a = 0;
            TmpPitch = Motion.InitPitch / 2; /*������ �ΰ��̹Ƿ�(�Ʒ��� �̵� �� ������ �̵�) ��ġ�� ������ �����Ѵ�*/
            miMove = InitMotionInfo(eMotionForm::eMotion_x3_2, nDelay, TmpPitch);
            miColor = InitMotionInfo(eMotionForm::eMotion_x3_2, nDelay, TmpPitch);
            /*���� ��ġ�κ��� -30, -100 ��ġ (�»��) �������� ����*/
            MBoxFace->Init(pRenderTarget, TmpStartPos, ALL_ZERO);
            MBoxHighlight->Init(pRenderTarget, TmpStartPos, ALL_ZERO);
            /*ù��° �̵� (�Ʒ��� �������鼭 ������ ��Ÿ���� ���)*/
            MBoxFace->addMovementMotion(miMove, FALSE, TmpStartPos, TmpEndPos);
            MBoxFace->addColorMotion(miColor, FALSE, TmpStartColor, Motion.FaceColor);
            MBoxHighlight->addMovementMotion(miMove, FALSE, TmpStartPos, TmpEndPos);
            /*�ι�° �̵� (�������� �̵��ϴ� ���*/
            MText->Init(pRenderTarget, uiSys->ButtonTextForm, szText, nTextLen, TmpEndPos, ALL_ZERO, nTextLen);
            miMove.nDelay += TmpPitch;
            miColor.nDelay += TmpPitch;
            MText->addMovementMotion(miMove, FALSE, TmpEndPos, uiPos);
            MText->addColorMotion(miColor, FALSE, Motion.FontColor, Motion.FontColor);
            MBoxFace->addMovementMotion(miMove, TRUE, TmpEndPos, uiPos);
            //MBoxFace->addColorMotion(miColor, TRUE, ButtonHighlightColor, ALL_ZERO);
            MBoxHighlight->addMovementMotion(miMove, TRUE, TmpEndPos, uiPos);
            TmpEndColor = Motion.HighlightColor;
            TmpEndColor.a = 0;
            MBoxHighlight->addColorMotion(miColor, FALSE, Motion.HighlightColor, TmpEndColor);
            break;

        case eButtonMotionPattern::eInit_Flick:
            TmpPitch = Motion.InitPitch / 2;
            miMove = InitMotionInfo(eMotionForm::eMotion_Pulse1, nDelay, TmpPitch);
            miColor = InitMotionInfo(eMotionForm::eMotion_Pulse1, nDelay, TmpPitch);

            MBoxFace->Init(pRenderTarget, uiPos, ALL_ZERO);
            MBoxHighlight->Init(pRenderTarget, uiPos, ALL_ZERO);
            MBoxFace->addColorMotion(miColor, FALSE, ALL_ZERO, Motion.FaceColor);

            miMove.nDelay += TmpPitch;
            miColor.nDelay += TmpPitch;
            MText->Init(pRenderTarget, uiSys->ButtonTextForm, szText, nTextLen, uiPos, ALL_ZERO, nTextLen);
            MText->addColorMotion(miColor, FALSE, ALL_ZERO, Motion.FontColor);
            break;
        }
        break; /* Init ���� */

    case eButtonAction::eAction_Pause:
        switch (Motion.PauseMotion) {
        case eButtonMotionPattern::ePause_Default:
            miMove = InitMotionInfo(eMotionForm::eMotion_None, nDelay, Motion.PausePitch);
            miColor = InitMotionInfo(eMotionForm::eMotion_None, nDelay, Motion.PausePitch);
            MBoxFace->Init(pRenderTarget, uiPos, ALL_ZERO);
            //MBoxHighlight->Init(pRenderTarget, uiPos, ALL_ZERO);
            MText->Init(pRenderTarget, uiSys->ButtonTextForm, szText, nTextLen, uiPos, ALL_ZERO, nTextLen);
#if 1 /*����� ��� �����̴� �� �� ����*/
            MBoxFace->addColorMotion(miColor, FALSE, Motion.FaceColor, Motion.FaceColor);
            //MBoxHighlight->addColorMotion(miColor, FALSE, ButtonHighlightColor, ColorSet.Highlight);
            MText->addColorMotion(miColor, FALSE, Motion.FontColor, Motion.FontColor);
#endif
            break;

        case eButtonMotionPattern::ePause_Flick:
            TmpPitch = Motion.PausePitch / 2; /*������ �ΰ��̹Ƿ�(�ؽ�Ʈ�� ����� �Ŀ� ��ư�� ���������) ��ġ�� ������ �����Ѵ�*/
            miColor = InitMotionInfo(eMotionForm::eMotion_Pulse1, nDelay, TmpPitch);
#if 0
            MBoxFace->Init(pRenderTarget, uiPos, ButtonFaceColor);
            MBoxHighlight->Init(pRenderTarget, uiPos, ALL_ZERO); /*��������� ���̶���Ʈ�� ���� ����*/
            MText->Init(pRenderTarget, uiSys->ButtonTextForm, szText, nTextLen, uiPos, ALL_ZERO, nTextLen);

            MText->addColorMotion(miColor, FALSE, t, ALL_ZERO); /*�ؽ�Ʈ �����*/
            miColor.nDelay += TmpPitch; /*�ؽ�Ʈ�� ����� �Ŀ� ��ư����� ��� ����*/
            MBoxFace->addColorMotion(miColor, FALSE, ButtonFaceColor, ALL_ZERO); /*��ư �����*/
#else
            MText->SetColor(miColor, TRUE, ALL_ZERO, ALL_ZERO); /*�ؽ�Ʈ �����*/
            miColor.nDelay += TmpPitch; /*�ؽ�Ʈ�� ����� �Ŀ� ��ư����� ��� ����*/
            MBoxFace->SetColor(miColor, TRUE, ALL_ZERO, ALL_ZERO); /*��ư �����*/

#endif

            break;
        }
        break; /* Pause ���� */

    case eButtonAction::eAction_Mouseover: /*���콺�� ���������� �������� param���� ���� (BOOL)*/
        switch (Motion.MouseoverMotion) {
        case eButtonMotionPattern::eMouseover_Default:
            miColor = InitMotionInfo(eMotionForm::eMotion_None, nDelay, Motion.MouseoverPitch);
            if (param)
                MBoxHighlight->Init(pRenderTarget, uiPos, Motion.MouseoverColor);
            else
                MBoxHighlight->Init(pRenderTarget, uiPos, ALL_ZERO);
            break;

        case eButtonMotionPattern::eMouseover_Flick:
            if (param) { 
                miColor = InitMotionInfo(eMotionForm::eMotion_Pulse1, nDelay, Motion.MouseoverPitch);
                MBoxHighlight->Init(pRenderTarget, uiPos, ALL_ZERO);
                MBoxHighlight->addColorMotion(miColor, FALSE, ALL_ZERO, Motion.MouseoverColor);
            }
            else {
                miColor = InitMotionInfo(eMotionForm::eMotion_Pulse2, nDelay, Motion.MouseoverPitch);
                MBoxHighlight->Init(pRenderTarget, uiPos, ALL_ZERO);
                MBoxHighlight->addColorMotion(miColor, FALSE, Motion.MouseoverColor, ALL_ZERO);
            }
            break;
        }
        break; /* MouseOver ����*/

    case eButtonAction::eAction_Click: /* ���콺 ���ʹ�ư�� ���������� ������������ param���� ���� (UIM �޼���)*/
        switch (Motion.ClickMotion) {
        case eButtonMotionPattern::eClick_Default:
            if ((__int64)param == TRUE) {
                miColor = InitMotionInfo(eMotionForm::eMotion_None, nDelay, Motion.ClickPitch);
                MBoxHighlight->Init(pRenderTarget, uiPos, Motion.HighlightColor);
            }
            else {
                miColor = InitMotionInfo(eMotionForm::eMotion_None, nDelay, Motion.ClickPitch);
                MBoxHighlight->Init(pRenderTarget, uiPos, ALL_ZERO);
            }
            break;

        case eButtonMotionPattern::eClick_Flash:
            if ((__int64)param != TRUE) break; /*���� ȿ���� ��� ��ư�� ���������� ����X*/
            miColor = InitMotionInfo(eMotionForm::eMotion_x3_1to0_2, nDelay, Motion.ClickPitch);
            MBoxHighlight->Init(pRenderTarget, uiPos, ALL_ZERO);
            MBoxHighlight->addColorMotion(miColor, FALSE, Motion.HighlightColor, ALL_ZERO);
            break;
        }
        break;

    case eButtonAction::eAction_Color: /*���� �ٲٱ� ����� �ʿ�� �߰��� ���� �ϵ��� ��*/
        switch (Motion.ColorMotion) {
        case eButtonMotionPattern::eColor_Default:
            break;

        case eButtonMotionPattern::eColor_Flash:
            break;
        }

        break;

    case eButtonAction::eAction_Text: /*�ؽ�Ʈ ��ü ���*/
        TmpLen = (int)wcslen((wchar_t*)param);
        switch (Motion.TextMotion) {
        case eButtonMotionPattern::eText_Default:
            MText->Init(pRenderTarget, uiSys->ButtonTextForm, (wchar_t*)param, TmpLen, uiPos, Motion.FontColor, TmpLen);
            break;
        }

        break;
    }
}

/**
    @brief �� ��ü�� ������Ʈ, �������� �Ͻ������Ѵ� (������ �ӽ� ��Ȱ��ȭ)
*/
void UI_Button::pause(int nDelay)
{
    InputMotion(eButtonAction::eAction_Pause, nDelay, NULL);
    uiMotionState = eUIMotionState::eUMS_PlayingHide;
}

/**
    @brief �� ��ü�� ������Ʈ, �������� �簳�Ѵ�
*/
void UI_Button::resume(int nDelay)
{
    InputMotion(eButtonAction::eAction_Init, nDelay, NULL);
    uiMotionState = eUIMotionState::eUMS_PlayingVisible;
}

/**
    @brief ��ư�� ��� �Ӽ��� ������Ʈ�Ѵ�
    @return ������Ʈ �Ұ� ������ FALSE, ������Ʈ ������ TRUE
*/
BOOL UI_Button::update(unsigned long time)
{
    BOOL bUpdated = 0;

    /*UI �� ���� ���·� �����ϸ� ���� ����*/
    if (uiMotionState == eUIMotionState::eUMS_Hide) return FALSE;

    bUpdated |= MBoxFace->update(time);
    bUpdated |= MBoxHighlight->update(time);
    bUpdated |= MText->update(time);

    if (!bUpdated) {
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
    InputMotion(eButtonAction::eAction_Text, 0, szText);
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
        pButton->InputMotion(eButtonAction::eAction_Mouseover, 0, (void*)TRUE);
        break;

    case UIM_MOUSELEAVE:
        pButton->InputMotion(eButtonAction::eAction_Mouseover, 0, (void*)FALSE);
        break;

    case WM_LBUTTONDOWN:
        pButton->InputMotion(eButtonAction::eAction_Click, 0, (void*)TRUE);
        break;
    
    case WM_LBUTTONUP:
        pButton->InputMotion(eButtonAction::eAction_Click, 0, (void*)FALSE);
        break;

    default: break;
    }

    if(UserHandler) UserHandler(pUI, Message, wParam, lParam);
}