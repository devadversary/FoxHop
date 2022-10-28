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
    DefaultHandler(this, UIM_CREATE, NULL, NULL); /*UI생성 메세지 전송*/
}

/**
    @brief 모션 기입
    @param MotionType 기입할 모션의 타입 (생성, 소멸, 기타 조작모션)
    @param Pattern    기입할 모션의 패턴 (깜빡임, 슬라이딩 등등)
    @param nDelay     모션 재생 딜레이
    @param nPitch     모션 재생 진행시간
    @param param      사용자 파라미터 (없으면 NULL / ex : 패턴이 eType_Mouseover 일때, BOOL 타입으로사용 - 마우스 진입, 퇴장을 구분)
*/
void UI_Button::InputMotion(eButtonMotionType MotionType, eButtonMotionPattern Pattern, unsigned int nDelay, unsigned int nPitch, void* param)
{
    MOTION_INFO miMove, miColor;
    POSITION TmpStartPos, TmpEndPos;
    D2D1_COLOR_F TmpStartColor;
    int TmpPitch;
    int TmpLen;

    switch (MotionType) {
    case eButtonMotionType::eType_Init: /*나타나기*/
        switch (Pattern) {
        case eButtonMotionPattern::eInit_Default: /*모션 없음*/
            MBoxFace->Init(pRenderTarget, uiPos, ColorSet.Face );
            MBoxHighlight->Init(pRenderTarget, uiPos, ALL_ZERO);
            MText->Init(pRenderTarget, uiSys->ButtonTextForm, szText, nTextLen, uiPos, ColorSet.Font, nTextLen);
            break;

        case eButtonMotionPattern::eInit_Reload:
            /*버튼 생성모션 입력*/
            miMove = InitMotionInfo(eMotionForm::eMotion_x3_2, nDelay, nPitch / 2);
            miColor = InitMotionInfo(eMotionForm::eMotion_x3_2, nDelay, nPitch / 2);
            TmpStartPos = uiPos; TmpStartPos.x -= 30; TmpStartPos.y -= 100;
            TmpEndPos = uiPos; TmpEndPos.x -= 30;
            TmpStartColor = ColorSet.Face; TmpStartColor.a = 0;
            TmpPitch = nPitch / 2; /*동작이 두개이므로(아래로 이동 후 옆으로 이동) 피치를 적절히 분할한다*/
            /*최종 위치로부터 -30, -100 위치 (좌상단) 에서부터 시작*/
            MBoxFace->Init(pRenderTarget, TmpStartPos, ALL_ZERO);
            MBoxHighlight->Init(pRenderTarget, TmpStartPos, ALL_ZERO);
            /*첫번째 이동 (아래로 내려오면서 서서히 나타나는 모션)*/
            MBoxFace->addMovementMotion(miMove, FALSE, TmpStartPos, TmpEndPos);
            MBoxFace->addColorMotion(miColor, FALSE, TmpStartColor, ColorSet.Face);
            MBoxHighlight->addMovementMotion(miMove, FALSE, TmpStartPos, TmpEndPos);
            /*두번째 이동 (우측으로 이동하는 모션*/
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
#if 1 /*모션은 없어도 딜레이는 줄 수 있음*/
            MBoxFace->addColorMotion(miColor, FALSE, ColorSet.Face, ColorSet.Face);
            MBoxHighlight->addColorMotion(miColor, FALSE, ColorSet.Highlight, ColorSet.Highlight);
            MText->addColorMotion(miColor, FALSE, ColorSet.Font, ColorSet.Font);
#endif
            break;

        case eButtonMotionPattern::ePause_Flick:
            TmpPitch = nPitch / 2; /*동작이 두개이므로(텍스트가 사라진 후에 버튼이 사라져야함) 피치를 적절히 분할한다*/
            miColor = InitMotionInfo(eMotionForm::eMotion_Pulse2, nDelay, TmpPitch);
            MBoxFace->Init(pRenderTarget, uiPos, ALL_ZERO);
            MBoxHighlight->Init(pRenderTarget, uiPos, ALL_ZERO); /*사라지기전 하이라이트는 먼저 제거*/
            MText->Init(pRenderTarget, uiSys->ButtonTextForm, szText, nTextLen, uiPos, ALL_ZERO, nTextLen);

            MText->addColorMotion(miColor, FALSE, ColorSet.Font, ALL_ZERO); /*텍스트 사라짐*/
            miColor.nDelay += TmpPitch; /*텍스트가 사라진 후에 버튼사라짐 모션 진행*/
            MBoxFace->addColorMotion(miColor, FALSE, ColorSet.Face, ALL_ZERO); /*버튼 사라짐*/
            break;
        }
        break;

    case eButtonMotionType::eType_Mouseover: /*마우스가 들어왔을때와 떠났을때 param으로 구분 (BOOL)*/
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

    case eButtonMotionType::eType_Click: /* 마우스 왼쪽버튼이 눌렸을때와 떼어졌을때가 param으로 구분 (UIM 메세지)*/
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
            if ((int)param != WM_LBUTTONDOWN) break; /*점멸 효과의 경우 버튼이 떼어졌을땐 반응X*/

            miColor = InitMotionInfo(eMotionForm::eMotion_x3_1to0_2, nDelay, nPitch);
            MBoxHighlight->Init(pRenderTarget, uiPos, ALL_ZERO);
            MBoxHighlight->addColorMotion(miColor, FALSE, ColorSet.Highlight, ALL_ZERO);
            break;
        }
        break;

    case eButtonMotionType::eType_Color: /*색상 바꾸기 모션은 필요시 추가로 기입 하도록 함*/
        switch (Pattern) {
        case eButtonMotionPattern::eColor_Default:
            break;

        case eButtonMotionPattern::eColor_Flash:
            break;
        }

        break;

    case eButtonMotionType::eType_Text: /*텍스트 교체 모션*/
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
    MOTION_INFO miColor;

    InputMotion(eButtonMotionType::eType_Pause, MotionSet.Pause, nDelay, MotionSet.PausePitch, NULL);
    uiMotionState = eUIMotionState::eUMS_PlayingHide;
    
}

/**
    @brief 이 개체의 업데이트, 렌더링을 재개한다
*/
void UI_Button::resume(int nDelay)
{
    InputMotion(eButtonMotionType::eType_Init, MotionSet.Init, nDelay, MotionSet.InitPitch, NULL);
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
    MOTION_INFO miText;

    if (!pText) return;
    memset(szText, 0, sizeof(szText));
    wcscpy_s(szText, ARRAYSIZE(szText), pText);
    InputMotion(eButtonMotionType::eType_Text, MotionSet.Text, 0, MotionSet.TextPitch, szText);
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
    @brief 팩토리 초기화
    @remark UISystem::Init 에서 호출된다.
*/
UI_ButtonFactory::UI_ButtonFactory(UISystem* pUISystem, ID2D1RenderTarget* pRT)
{
    /*기본 환경 개체 초기화*/
    pUISys = pUISystem;
    pRenderTarget = pRT;

    /*기본 모션 지정 (추후 변경 가능)*/
    MotionSet.Init      = eButtonMotionPattern::eInit_Default;
    MotionSet.Pause     = eButtonMotionPattern::ePause_Default;
    MotionSet.Color     = eButtonMotionPattern::eMouseover_Default;
    MotionSet.Text      = eButtonMotionPattern::eText_Default;
    MotionSet.Click     = eButtonMotionPattern::eColor_Default;
    MotionSet.MouseOver = eButtonMotionPattern::eText_Default;

    /*기본 모션진행시간 지정 (추후 변경 가능) ms단위*/
    MotionSet.InitPitch      = 0;
    MotionSet.PausePitch     = 0;
    MotionSet.ColorPitch     = 0;
    MotionSet.TextPitch      = 0;
    MotionSet.ClickPitch     = 0;
    MotionSet.MouseOverPitch = 0;

    /*기본 컬러 지정 (추후 변경 가능) : R G B A (0.0f ~ 1.0f)*/
    ColorSet.Face      = {0.8f, 0.8f, 0.8f, 1.f};  /*밝은 회색*/
    ColorSet.Font      = {0.f,  0.f,  0.f,  1.f};
    ColorSet.Frame     = {0.f,  0.f,  0.f,  0.f};
    ColorSet.Mouseover = {1.f,  1.f,  1.f,  0.5f}; /*반투명 흰색*/
    ColorSet.Highlight = {1.f,  1.f,  1.f,  1.f};
}

/**
    @brief 모션 집합 가져오기
*/
void UI_ButtonFactory::GetCurrentMotionSet(BUTTON_MOTION_SET* pMotionSet)
{
    if (!pMotionSet) return;
    *pMotionSet = MotionSet;
}

/**
    @brief 모션 집합 적용하기
*/
void UI_ButtonFactory::SetCurrentMotionSet(BUTTON_MOTION_SET* pMotionSet)
{
    if (!pMotionSet) return;
    MotionSet = *pMotionSet;
}

/**
    @brief 컬러 집합 가져오기
*/
void UI_ButtonFactory::GetCurrentColorSet(BUTTON_COLOR_SET* pColorSet)
{
    if (!pColorSet) return;
    *pColorSet = ColorSet;
}

/**
    @brief 컬러 집합 적용하기
*/
void UI_ButtonFactory::SetCurrentColorSet(BUTTON_COLOR_SET* pColorSet)
{
    if (!pColorSet) return;
    ColorSet = *pColorSet;
}

/**
    @brief 버튼 생성
    @param Pos 생성할 UI의 위치와 크기
    @param pText 생성할 UI가 가질 텍스트 (필요 없는 UI들은 알아서 이 인자를 무시함)
    @param nDelay 생성할 UI의 모션 딜레이
    @param pfnCallback 생성할 UI가 가질 이벤트 핸들러
    @remark 현재 팩토리의 지정 설정이 적용되어 생성된다 (모션, 색상 등)
*/
UI_Button* UI_ButtonFactory::CreateUI(POSITION Pos, wchar_t* pText, int nDelay, pfnUIHandler pfnCallback)
{
    return new UI_Button(pUISys, pRenderTarget, MotionSet, ColorSet, pfnCallback, Pos, pText, nDelay);
}
