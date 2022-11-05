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
    DefaultHandler(this, UIM_CREATE, NULL, NULL); /*UI생성 메세지 전송*/
}

/**
    @brief 모션 기입
    @param Action 기입할 모션의 타입 (생성, 소멸, 기타 조작모션)
    @param Theme  UI별 모션 전역속성
    @param nDelay 모션 재생 딜레이
    @param param  사용자 파라미터 (없으면 NULL / ex : 패턴이 eType_Mouseover 일때, BOOL 타입으로사용 - 마우스 진입, 퇴장을 구분)
*/
void UI_Button::InputMotion(eButtonAction Action, UITheme* Theme, unsigned int nDelay, void* param)
{
    MOTION_INFO miMove, miColor;
    POSITION TmpStartPos, TmpEndPos;
    D2D1_COLOR_F TmpStartColor, TmpEndColor;
    int TmpPitch;
    int TmpLen;

    switch (Action) {
    case eButtonAction::eAction_Init: /*나타나기*/
        switch (Theme->ButtonInitMotion) {
        case eButtonMotionPattern::eInit_Default: /*모션 없음*/
            MBoxFace->Init(pRenderTarget, uiPos, Theme->ButtonFaceColor );
            MBoxHighlight->Init(pRenderTarget, uiPos, ALL_ZERO);
            MText->Init(pRenderTarget, uiSys->ButtonTextForm, szText, nTextLen, uiPos, Theme->ButtonFontColor, nTextLen);
            break;

        case eButtonMotionPattern::eInit_Reload:
            /*버튼 생성모션 입력*/
            TmpStartPos = uiPos; TmpStartPos.x -= 30; TmpStartPos.y -= 100;
            TmpEndPos = uiPos; TmpEndPos.x -= 30;
            TmpStartColor = Theme->ButtonFaceColor; TmpStartColor.a = 0;
            TmpPitch = Theme->ButtonInitPitch / 2; /*동작이 두개이므로(아래로 이동 후 옆으로 이동) 피치를 적절히 분할한다*/
            miMove = InitMotionInfo(eMotionForm::eMotion_x3_2, nDelay, TmpPitch);
            miColor = InitMotionInfo(eMotionForm::eMotion_x3_2, nDelay, TmpPitch);
            /*최종 위치로부터 -30, -100 위치 (좌상단) 에서부터 시작*/
            MBoxFace->Init(pRenderTarget, TmpStartPos, ALL_ZERO);
            MBoxHighlight->Init(pRenderTarget, TmpStartPos, ALL_ZERO);
            /*첫번째 이동 (아래로 내려오면서 서서히 나타나는 모션)*/
            MBoxFace->addMovementMotion(miMove, FALSE, TmpStartPos, TmpEndPos);
            MBoxFace->addColorMotion(miColor, FALSE, TmpStartColor, Theme->ButtonFaceColor);
            MBoxHighlight->addMovementMotion(miMove, FALSE, TmpStartPos, TmpEndPos);
            /*두번째 이동 (우측으로 이동하는 모션*/
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
        break; /* Init 종료 */

    case eButtonAction::eAction_Pause:
        switch (Theme->ButtonPauseMotion) {
        case eButtonMotionPattern::ePause_Default:
            miMove = InitMotionInfo(eMotionForm::eMotion_None, nDelay, Theme->ButtonPausePitch);
            miColor = InitMotionInfo(eMotionForm::eMotion_None, nDelay, Theme->ButtonPausePitch);
            MBoxFace->Init(pRenderTarget, uiPos, ALL_ZERO);
            //MBoxHighlight->Init(pRenderTarget, uiPos, ALL_ZERO);
            MText->Init(pRenderTarget, uiSys->ButtonTextForm, szText, nTextLen, uiPos, ALL_ZERO, nTextLen);
#if 1 /*모션은 없어도 딜레이는 줄 수 있음*/
            MBoxFace->addColorMotion(miColor, FALSE, Theme->ButtonFaceColor, Theme->ButtonFaceColor);
            //MBoxHighlight->addColorMotion(miColor, FALSE, Theme->ButtonHighlightColor, ColorSet.Highlight);
            MText->addColorMotion(miColor, FALSE, Theme->ButtonFontColor, Theme->ButtonFontColor);
#endif
            break;

        case eButtonMotionPattern::ePause_Flick:
            TmpPitch = Theme->ButtonPausePitch / 2; /*동작이 두개이므로(텍스트가 사라진 후에 버튼이 사라져야함) 피치를 적절히 분할한다*/
            miColor = InitMotionInfo(eMotionForm::eMotion_Pulse1, nDelay, TmpPitch);
#if 0
            MBoxFace->Init(pRenderTarget, uiPos, Theme->ButtonFaceColor);
            MBoxHighlight->Init(pRenderTarget, uiPos, ALL_ZERO); /*사라지기전 하이라이트는 먼저 제거*/
            MText->Init(pRenderTarget, uiSys->ButtonTextForm, szText, nTextLen, uiPos, ALL_ZERO, nTextLen);

            MText->addColorMotion(miColor, FALSE, t, ALL_ZERO); /*텍스트 사라짐*/
            miColor.nDelay += TmpPitch; /*텍스트가 사라진 후에 버튼사라짐 모션 진행*/
            MBoxFace->addColorMotion(miColor, FALSE, Theme->ButtonFaceColor, ALL_ZERO); /*버튼 사라짐*/
#else
            MText->SetColor(miColor, TRUE, ALL_ZERO, ALL_ZERO); /*텍스트 사라짐*/
            miColor.nDelay += TmpPitch; /*텍스트가 사라진 후에 버튼사라짐 모션 진행*/
            MBoxFace->SetColor(miColor, TRUE, ALL_ZERO, ALL_ZERO); /*버튼 사라짐*/

#endif

            break;
        }
        break; /* Pause 종료 */

    case eButtonAction::eAction_Mouseover: /*마우스가 들어왔을때와 떠났을때 param으로 구분 (BOOL)*/
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
        break; /* MouseOver 종료*/

    case eButtonAction::eAction_Click: /* 마우스 왼쪽버튼이 눌렸을때와 떼어졌을때가 param으로 구분 (UIM 메세지)*/
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
            if ((__int64)param != TRUE) break; /*점멸 효과의 경우 버튼이 떼어졌을땐 반응X*/
            miColor = InitMotionInfo(eMotionForm::eMotion_x3_1to0_2, nDelay, Theme->ButtonClickPitch);
            MBoxHighlight->Init(pRenderTarget, uiPos, ALL_ZERO);
            MBoxHighlight->addColorMotion(miColor, FALSE, Theme->ButtonHighlightColor, ALL_ZERO);
            break;
        }
        break;

    case eButtonAction::eAction_Color: /*색상 바꾸기 모션은 필요시 추가로 기입 하도록 함*/
        switch (Theme->ButtonColorMotion) {
        case eButtonMotionPattern::eColor_Default:
            break;

        case eButtonMotionPattern::eColor_Flash:
            break;
        }

        break;

    case eButtonAction::eAction_Text: /*텍스트 교체 모션*/
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
    @brief UI 사용을 끝내고 초기화 후 반환한다.
*/
void UI_Button::Destroy()
{
    uiSys->ObjPoolBox.deactivateObject(MBoxFace);
    uiSys->ObjPoolBox.deactivateObject(MBoxHighlight);
    uiSys->ObjPoolText.deactivateObject(MText);
    MBoxFace      = 0;
    MBoxHighlight = 0;
    MText         = 0;
    uiSys->ReleaseUI(); /*스스로를 UI 시스템에 반환*/
}

/**
    @brief 이 개체의 업데이트, 렌더링을 일시정지한다 (일종의 임시 비활성화)
*/
void UI_Button::pause(int nDelay)
{
    InputMotion(eButtonAction::eAction_Pause, uiSys->Theme, nDelay, NULL);
    uiMotionState = eUIMotionState::eUMS_PlayingHide;
    
}

/**
    @brief 이 개체의 업데이트, 렌더링을 재개한다
*/
void UI_Button::resume(int nDelay)
{
    InputMotion(eButtonAction::eAction_Init, uiSys->Theme, nDelay, NULL);
    uiMotionState = eUIMotionState::eUMS_PlayingVisible;
}

/**
    @brief 버튼의 모든 속성을 업데이트한다
    @return 업데이트 할게 없으면 FALSE, 업데이트 했으면 TRUE
*/
BOOL UI_Button::update(unsigned long time)
{
    int nTrue = 0;

    /*UI 가 숨김 상태로 돌입하면 연산 중지*/
    if (uiMotionState == eUIMotionState::eUMS_Hide) return FALSE;

    nTrue += MBoxFace->update(time);
    nTrue += MBoxHighlight->update(time);
    nTrue += MText->update(time);

    /*nTrue가 0이면 모션 연산이 종료된 것. ~ing 상태를 확정시킨다.*/
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
    @brief 업데이트된 모든 속성을 종합하여 렌더링한다.
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
    @brief 버튼 전경색 변경
*/
void UI_Button::setFaceColor(D2D1_COLOR_F Color, MOTION_INFO miColor)
{
    MBoxFace->SetColor(miColor, TRUE, {0,0,0,0}, Color);
}

/**
    @brief 버튼 하이라이트 색상 변경
*/
void UI_Button::setHighlightColor(D2D1_COLOR_F Color, MOTION_INFO miColor)
{
    MBoxHighlight->SetColor(miColor, TRUE, { 0,0,0,0 }, Color);
}

/**
    @brief 기본 버튼 메세지 핸들러
    @remark 사용자 지정 프로시저는 기본 핸들러 실행 후 호출된다.
*/
void UI_Button::DefaultButtonProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam)
{
    pfnUIHandler UserHandler = pUI->MessageHandler;
    UI_Button* pButton = static_cast<UI_Button*>(pUI);

    if (!pUI) return;

    /*2021.07.27 devadversary : UI들의 사용자 상호작용에따른 모션변경은 기본 핸들러가 처리하도록 결정함*/
    switch (Message) {
    case UIM_MOUSEON: /*ON / LEAVE 메세지는 UI_Panel 로 부터 SendMessage 로 전달된다 */
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