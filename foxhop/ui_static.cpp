#include "./include/ui_static.hpp"
#include "./include/ui_system.hpp"

UI_Static::UI_Static(UISystem* pUISys, pfnUIHandler pfnCallback, POSITION Pos, wchar_t* Text, UI_Static_MotionParam MotionParam)
{
    uiSys = pUISys;
    pRenderTarget = pUISys->D2DA.pRenTarget;
    Focusable = FALSE;
    uiMotionState = eUIMotionState::eUMS_PlayingVisible;

    DefaultHandler = DefaultStaticProc;
    MessageHandler = pfnCallback;
    memset(szText, 0, sizeof(szText));
    uiPos = Pos;

    Motion = MotionParam;
    pText = new PropText(pRenderTarget);
    pBoxBg = new PropBox(pRenderTarget);
    pBoxFrame = new PropBox(pRenderTarget);
    wcscpy_s(szText, ARRAYSIZE(szText), Text);
    resume(0);
}

UI_Static::~UI_Static() {}


/**
    @brief �� ��ü�� ������Ʈ, �������� �Ͻ������Ѵ� (������ �ӽ� ��Ȱ��ȭ)
*/
void UI_Static::pause(int nDelay)
{
    MOTION_INFO miColor;

    uiMotionState = eUIMotionState::eUMS_PlayingHide;

    /*TODO : �Ҹ� ��ǵ� ������ �� �ֵ��� �� ����*/
    switch (Motion.MotionPause) {
    case eStaticMotionPattern::ePause_Default:
        break;
#if 0 /*���߿� �߰��� ���*/
    default:
        miColor.formular = eMotionForm::eMotion_Pulse1;
        miColor.nDelay = nDelay;
        miColor.nPitch = 200;
        pText->SetColor(miColor, TRUE, { 0.f,0.f,0.f,0.f }, { 0.f,0.f,0.f,0.f });
        miColor.nDelay += 400;
        pBoxBg->SetColor(miColor, TRUE, { 0.f,0.f,0.f,0.f }, { 0.f,0.f,0.f,0.f });
        break;
#endif
    }
}

/**
    @brief �� ��ü�� ������Ʈ, �������� �簳�Ѵ�
*/
void UI_Static::resume(int nDelay)
{
    switch (Motion.MotionInit) {
    case eStaticMotionPattern::eInit_Default:
        pBoxBg->Init(uiPos, Motion.ColorBg);
        pBoxFrame->Init(uiPos, Motion.ColorFrame, FALSE);
        SetText(szText, 0);
        break;
    }
}

BOOL UI_Static::update(unsigned long time)
{
    BOOL bUpdated = FALSE;

    if (uiMotionState == eUIMotionState::eUMS_Hide) return FALSE;

    bUpdated |= pBoxBg->update(time);
    bUpdated |= pBoxFrame->update(time);
    bUpdated |= pText->update(time);

    if (!bUpdated) {
        if (uiMotionState == eUIMotionState::eUMS_PlayingHide)
            uiMotionState = eUIMotionState::eUMS_Hide;

        else if (uiMotionState == eUIMotionState::eUMS_PlayingVisible)
            uiMotionState = eUIMotionState::eUMS_Visible;
        return FALSE;
    }
    return TRUE;
}

void UI_Static::render()
{
    if (uiMotionState == eUIMotionState::eUMS_Hide) return;

    pBoxBg->render(pRenderTarget);
    pBoxFrame->render(pRenderTarget);
    pText->render(pRenderTarget);
}

void UI_Static::SetText(wchar_t* pStr, int nDelay)
{
    MOTION_INFO mi;

    if (!pStr) return;
    wcscpy_s(szText, ARRAYSIZE(szText), pStr);

    switch(Motion.MotionText) {
    case eStaticMotionPattern::eText_Default:
        pText->Init(uiSys->MediumTextForm, szText, 0, uiPos, Motion.ColorFont, wcslen(szText));
        break;

    case eStaticMotionPattern::eText_Flick:
        mi = InitMotionInfo(eMotionForm::eMotion_Pulse1, nDelay, Motion.PitchText);
        pText->Init(uiSys->MediumTextForm, szText, 0, uiPos, {0,0,0,0}, wcslen(szText));
        pText->addColorMotion(mi, TRUE, {0,0,0,0}, Motion.ColorFont);
        break;
    }
}

/**
    @brief �⺻ ��ư �޼��� �ڵ鷯
    @remark ����� ���� ���ν����� �⺻ �ڵ鷯 ���� �� ȣ��ȴ�.
*/
void UI_Static::DefaultStaticProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam)
{
    UI_Static* pStatic = (UI_Static*)pUI;
    pfnUIHandler UserHandler = pUI->MessageHandler;

    switch (Message) {
    default: break;
    }
    if (UserHandler) UserHandler(pUI, Message, wParam, lParam);
}