#include "./include/ui_static.hpp"
#include "./include/ui_system.hpp"

UI_Static::UI_Static(UISystem* pUISys, pfnUIHandler pfnCallback, POSITION Pos, IDWriteTextFormat* pFormat, wchar_t* Text, UI_Static_MotionParam MotionParam)
{
    uiSys = pUISys;
    pRenderTarget = pUISys->D2DA.pRenTarget;
    Focusable = FALSE;
    uiMotionState = eUIMotionState::eUMS_PlayingVisible;

    DefaultHandler = DefaultStaticProc;
    MessageHandler = pfnCallback;
    memset(szText, 0, sizeof(szText));
    uiPos = Pos;
    pTextFmt = pFormat;
    Motion = MotionParam;
    wcscpy_s(szText, ARRAYSIZE(szText), Text);

    pText = new PropText(pRenderTarget, UISTATIC_MAX_TEXTLEN);
    pBoxBg = new PropBox(pRenderTarget);
    pBoxFrame = new PropBox(pRenderTarget);
    resume(0);
}

UI_Static::~UI_Static() {}

void UI_Static::ResumeFrame(unsigned long Delay)
{
    MOTION_INFO mi;

    switch (Motion.MotionInitFrame) {
        case eStaticMotionPattern::eInitFrame_Default:
            pBoxFrame->Init(uiPos, ALL_ZERO, FALSE);
            mi = InitMotionInfo(eMotionForm::eMotion_None, Delay, 0);
            pBoxFrame->SetColor(mi, TRUE, ALL_ZERO, Motion.ColorFrame);
            break;

        case eStaticMotionPattern::eInitFrame_ExpendCenter: {
            POSITION StartPos = uiPos;
            StartPos.x += uiPos.x2 / 2;
            StartPos.x2 = 0;
            pBoxFrame->Init(StartPos, ALL_ZERO, FALSE);
            mi = InitMotionInfo(eMotionForm::eMotion_None, Delay, 0);
            pBoxFrame->SetColor(mi, TRUE, ALL_ZERO, Motion.ColorFrame);
            mi = InitMotionInfo(eMotionForm::eMotion_x3_2, Delay, Motion.PitchInitFrame);
            pBoxFrame->SetPos(mi, TRUE, ALL_ZERO, uiPos);
            break;
        }
    }
}

void UI_Static::PauseFrame(unsigned long Delay)
{
    MOTION_INFO mi;

    switch (Motion.MotionPauseFrame) {
        case eStaticMotionPattern::ePauseFrame_Default:
            mi = InitMotionInfo(eMotionForm::eMotion_None, Delay, 0);
            pBoxFrame->SetColor(mi, TRUE, ALL_ZERO, ALL_ZERO);
            break;

        case eStaticMotionPattern::ePauseFrame_CollapseCenter: {
            POSITION EndPos = uiPos;
            EndPos.x += uiPos.x2 / 2;
            EndPos.x2 = 0;
            mi = InitMotionInfo(eMotionForm::eMotion_x3_1, Delay, Motion.PitchPauseFrame);
            pBoxFrame->SetPos(mi, TRUE, ALL_ZERO, EndPos);
            mi = InitMotionInfo(eMotionForm::eMotion_None, Delay+Motion.PitchPauseFrame, 0);
            pBoxFrame->SetColor(mi, TRUE, ALL_ZERO, ALL_ZERO);
            break;
        }

    }
}

void UI_Static::ResumeBg(unsigned long Delay)
{
    MOTION_INFO mi;

    switch (Motion.MotionInitBg) {
        case eStaticMotionPattern::eInitBg_Default:
            pBoxBg->Init(uiPos, ALL_ZERO);
            mi = InitMotionInfo(eMotionForm::eMotion_None, Delay, 0);
            pBoxBg->SetColor(mi, TRUE, ALL_ZERO, Motion.ColorBg);
            break;

        case eStaticMotionPattern::eInitBg_ExpendRight: {
            POSITION StartPos = uiPos;

            StartPos.x2 = 0;
            pBoxBg->Init(StartPos, ALL_ZERO);
            mi = InitMotionInfo(eMotionForm::eMotion_None, Delay, 0);
            pBoxBg->SetColor(mi, TRUE, ALL_ZERO, Motion.ColorBg);
            mi = InitMotionInfo(eMotionForm::eMotion_Linear1, Delay, Motion.PitchInitBg);
            pBoxBg->SetPos(mi, TRUE, ALL_ZERO, uiPos);
            break;
        }

    }
}

void UI_Static::PauseBg(unsigned long Delay)
{
    MOTION_INFO mi;

    switch (Motion.MotionPauseBg) {
        case eStaticMotionPattern::ePauseBg_Default:
            mi = InitMotionInfo(eMotionForm::eMotion_None, Delay, 0);
            pBoxBg->SetColor(mi, TRUE, ALL_ZERO, ALL_ZERO);
            break;

        case eStaticMotionPattern::ePauseBg_CollapseLeft: {
            POSITION EndPos = uiPos;

            EndPos.x += uiPos.x2;
            EndPos.x2 = 0;
            mi = InitMotionInfo(eMotionForm::eMotion_Linear1, Delay, Motion.PitchPauseBg);
            pBoxBg->SetPos(mi, TRUE, ALL_ZERO, EndPos);
            mi = InitMotionInfo(eMotionForm::eMotion_None, Delay + Motion.PitchPauseBg, 0);
            pBoxBg->SetColor(mi, TRUE, ALL_ZERO, ALL_ZERO);
            break;
        }

    }
}

void UI_Static::ResumeText(unsigned long Delay)
{
    MOTION_INFO mi;

    switch (Motion.MotionInitText) {
        case eStaticMotionPattern::eInitText_Default:
            pText->Init(pTextFmt, szText, 0, uiPos, ALL_ZERO, wcslen(szText));
            mi = InitMotionInfo(eMotionForm::eMotion_None, Delay, Motion.PitchInitText);
            pText->SetColor(mi, TRUE, ALL_ZERO, Motion.ColorText);
            break;

        case eStaticMotionPattern::eInitText_Typing:
            pText->Init(pTextFmt, szText, 0, uiPos, ALL_ZERO, 0);
            mi = InitMotionInfo(eMotionForm::eMotion_None, Delay, 0);
            pText->SetColor(mi, TRUE, ALL_ZERO, Motion.ColorText);
            mi = InitMotionInfo(eMotionForm::eMotion_Linear1, Delay, Motion.PitchInitText);
            pText->SetLen(mi, TRUE, NULL, wcslen(szText));
            break;
    }
}

void UI_Static::PauseText(unsigned long Delay)
{
    MOTION_INFO mi;

    switch (Motion.MotionPauseText) {
        case eStaticMotionPattern::ePauseText_Default:
            mi = InitMotionInfo(eMotionForm::eMotion_None, Delay, Motion.PitchPauseText);
            pText->SetColor(mi, TRUE, ALL_ZERO, ALL_ZERO);
            break;

        case eStaticMotionPattern::ePauseText_Flick:
            mi = InitMotionInfo(eMotionForm::eMotion_Pulse1, Delay, Motion.PitchPauseText);
            pText->SetColor(mi, TRUE, ALL_ZERO, ALL_ZERO);
            break;
    }
}

/**
    @brief 이 개체의 업데이트, 렌더링을 일시정지한다 (일종의 임시 비활성화)
*/
void UI_Static::pause(int nDelay)
{
    uiMotionState = eUIMotionState::eUMS_PlayingHide;
    PauseFrame(nDelay + Motion.DelayPauseFrame);
    PauseBg(nDelay + Motion.DelayPauseBg);
    PauseText(nDelay + Motion.DelayPauseText);
}

/**
    @brief 이 개체의 업데이트, 렌더링을 재개한다
*/
void UI_Static::resume(int nDelay)
{
    uiMotionState = eUIMotionState::eUMS_PlayingVisible;
    ResumeFrame(nDelay + Motion.DelayInitFrame);
    ResumeBg(nDelay + Motion.DelayInitBg);
    ResumeText(nDelay + Motion.DelayInitText);
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
    ResumeText(nDelay);
}

/**
    @brief 기본 메세지 핸들러
    @remark 사용자 지정 프로시저는 기본 핸들러 실행 후 호출된다.
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