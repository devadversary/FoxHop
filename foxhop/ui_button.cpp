#include "./include/ui_button.hpp"
#include "./include/ui_system.hpp"

static void DefaultButtonProc(UI* pUI, UINT Message, void* parm);

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

    InputMotion(MotionSet.Init, nDelay);
    uiMotionState = eUIMotionState::eUMS_PlayingVisible;
    DefaultHandler(this, UIM_CREATE, NULL); /*UI생성 메세지 전송*/
}

/**
    @brief 모션 기입
    @param Motion UI의 등장 모션
    @param nDelay UI의 등장 딜레이
*/
void UI_Button::InputMotion(eButtonMotionInit Motion, int nDelay)
{
    MOTION_INFO miMove, miColor;
    POSITION TmpStartPos, TmpEndPos;
    D2D1_COLOR_F TmpStartColor;

    switch (Motion) {
#if 1 /*TODO : 버튼의 모션 스타일을 지정하여 다양한 연출이 가능하도록 만들 예정*/
    case eButtonMotionInit::eMotionReload :
        break;
#endif /*우선은 한가지 모션만 제공*/
    default:
        /*버튼 생성모션 입력*/
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
        /*최종 위치로부터 -30, -100 위치 (좌상단) 에서부터 시작*/
        MBoxFace->Init(pRenderTarget, TmpStartPos, { 0.f,0.f,0.f,0.f });
        MBoxHighlight->Init(pRenderTarget, TmpStartPos, { 0.f,0.f,0.f,0.f });
        /*첫번째 이동 (아래로 내려오면서 서서히 나타나는 모션)*/
        MBoxFace->addMovementMotion(miMove, FALSE, TmpStartPos, TmpEndPos);
        MBoxFace->addColorMotion(miColor, FALSE, TmpStartColor, FaceColor);
        MBoxHighlight->addMovementMotion(miMove, FALSE, TmpStartPos, TmpEndPos);
        /*두번째 이동 (우측으로 이동하는 모션*/
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
    @brief UI 사용을 끝내고 초기화 후 반환한다.
*/
void UI_Button::Destroy()
{
    PoolBox->deactivateObject(MBoxFace);
    PoolText->deactivateObject(MText);
    uiSys->BoxPoolStorage.returnPool(PoolBox);
    uiSys->TextPoolStorage.returnPool(PoolText);
    PoolBox = 0;
    PoolText = 0;
    uiSys->ReleaseUI(ID); /*스스로를 UI 시스템에 반환*/
}

/**
    @brief 이 개체의 업데이트, 렌더링을 일시정지한다 (일종의 임시 비활성화)
*/
void UI_Button::pause(int nDelay)
{
    MOTION_INFO miColor;

    uiMotionState = eUIMotionState::eUMS_PlayingHide;
    /*TODO : 소멸 모션도 지정할 수 있도록 할 예정*/
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
    @brief 이 개체의 업데이트, 렌더링을 재개한다
*/
void UI_Button::resume(int nDelay)
{
    InputMotion(MotionSet.Init, nDelay);
    uiMotionState = eUIMotionState::eUMS_PlayingVisible;
}

/**
    @brief 버튼의 모든 속성을 업데이트한다
    @return 업데이트 할게 없으면 FALSE, 업데이트 했으면 TRUE
*/
BOOL UI_Button::update(unsigned long time)
{
    int nTrue = 0;

    if (uiMotionState == eUIMotionState::eUMS_Hide) return FALSE;
    nTrue += MBoxFace->update(time);
    nTrue += MBoxHighlight->update(time);
    nTrue += MText->update(time);
    if (!nTrue) return FALSE; /*Hide 완료, 또는 Init완료 해야댐*/
    if (uiMotionState == eUIMotionState::eUMS_PlayingHide)
        uiMotionState = eUIMotionState::eUMS_Hide;
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
    nTextLen = (int)wcslen(pText);
    wcscpy_s(szText, MAX_BUTTONNAME, pText);
    miText.formular = eMotionForm::eMotion_Pulse1;
    miText.nDelay = nDelay;
    miText.nPitch = 200;
    MText->SetText(miText, pText, 0/*Currnet가 TRUE이기때문*/, nTextLen );
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
static void DefaultButtonProc(UI* pUI, UINT Message, void* param)
{
    UI_Button*         pButton = (UI_Button*)pUI;
    pfnUIHandler       UserHandler = pUI->MessageHandler;
    BUTTON_MOTION_SET  MotionSet;
    BUTTON_COLOR_SET   ColorSet;

    MotionSet = pButton->MotionSet;
    ColorSet = pButton->ColorSet;

    switch (Message) {
#if 1 /*2021.07.27 devadversary : UI들의 사용자 상호작용에따른 모션변경은 기본 핸들러가 처리하도록 결정함*/
    case UIM_MOUSEON:
        //pButton->setHighlightColor( ColorSet.Highlight,  );
        break;

    case UIM_MOUSELEAVE:
        break;
#endif
    default: break;
    }
    if(UserHandler) UserHandler(pUI, Message, param);
}

/**
    @brief 팩토리 초기화
*/
void UI_ButtonFactory::Init(UISystem* pUISystem, ID2D1RenderTarget* pRT)
{
    /*기본 환경 개체 초기화*/
    pUISys = pUISystem;
    pRenderTarget = pRT;

    /*기본 모션 지정 (추후 변경 가능)*/
    MotionSet.Init      = eButtonMotionInit::eMotionFlick;
    MotionSet.Pause     = eButtonMotionPause::eMotionFlick;
    MotionSet.Color     = eButtonMotionColor::eMotionFlash;
    MotionSet.Text      = eButtonMotionText::eMotionDefault;
    MotionSet.Click     = eButtonMotionClick::eMotionFlash;
    MotionSet.MouseOver = eButtonMotionMouseover::eMotionFlick;

    /*기본 모션진행시간 지정 (추후 변경 가능)*/
    MotionSet.InitPitch      = 250;
    MotionSet.PausePitch     = 250;
    MotionSet.ColorPitch     = 200;
    MotionSet.TextPitch      = 0;
    MotionSet.ClickPitch     = 200;
    MotionSet.MouseOverPitch = 200;

    /*기본 컬러 지정 (추후 변경 가능) : R G B A (0.0f ~ 1.0f)*/
    ColorSet.Face      = {0.8, 0.8, 0.8, 1};
    ColorSet.Font      = {0,   0,   0,   1};
    ColorSet.Frame     = {0,   0,   0,   0};
    ColorSet.Highlight = {1,   1,   1,   1};
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
    @brief UI 생성
    @remark 현재 팩토리의 지정 설정이 적용되어 생성된다 (모션, 색상 등)
*/
UI* UI_ButtonFactory::CreateUI(int nID, POSITION Pos, wchar_t* pText, int nDelay, pfnUIHandler pfnCallback)
{
    return NULL;
}