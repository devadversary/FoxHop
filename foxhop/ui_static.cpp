#include "./include/ui_static.hpp"
#include "./include/ui_system.hpp"


static void DefaultStaticProc(UI* pUI, UINT Message, void* parm);

/**
    @brief 모션 기입
    @param Motion UI의 등장 모션
    @param nDelay UI의 등장 딜레이
*/
void UI_Static::InputMotion(int Motion, int nDelay)
{
    MOTION_INFO miMove, miTextlen;
    POSITION TmpStartPos, TmpEndPos, TmpPos;
    D2D1_COLOR_F TmpStartColor;

    switch (uiMotion) {
#if 0 /*TODO : 스태틱의 모션 스타일을 지정하여 다양한 연출이 가능하도록 만들 예정*/
    case UIBS_RELOADMOTION:
        break;
#endif /*우선은 한가지 모션만 제공*/
    default:
        /*버튼 생성모션 입력*/
        miMove.formular = eMotionForm::eMotion_Linear1;
        miMove.nDelay = nDelay;
        miMove.nPitch = 200;
        TmpStartPos = uiPos;
        TmpStartPos.width = 0;
        MBox->Init(pRenderTarget, TmpStartPos, FaceColor);
        /*첫번째 이동*/
        MBox->addMovementMotion(miMove, FALSE, TmpStartPos, uiPos);
        /*두번째 이동*/
        TmpPos = uiPos;
        TmpPos.x += 10; /*텍스트 살짝 띄워서*/
        TmpPos.width -= 10;
        MText->Init(pRenderTarget, uiSys->MediumTextForm, szText, nTextLen, TmpPos, FontColor, 0);
        miMove.nDelay += miMove.nPitch;
        miMove.nPitch = 300;
        MText->addLenMotion(miMove, FALSE, 0, nTextLen);
        uiMotionState = eUIMotionState::eUMS_PlayingVisible;
        //MText->addColorMotion(miColor, FALSE, { 1.f, 1.f, 1.f, 1.f }, FontColor);
        //MBox->addMovementMotion(miMove, TRUE, TmpEndPos, uiPos);
        //MBox->addColorMotion(miColor, TRUE, { 1.f,1.f,1.f,1.f }, FaceColor);
        break;
    }
}

/**
    @brief UI 사용을 끝내고 초기화 후 반환한다.
*/
void UI_Static::Destroy()
{
    PoolBox->deactivateObject(MBox);
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
void UI_Static::pause(int nDelay)
{
    MOTION_INFO miColor;

    /*TODO : 소멸 모션도 지정할 수 있도록 할 예정*/
    switch (0) {
    default:
        miColor.formular = eMotionForm::eMotion_Pulse1;
        miColor.nDelay = nDelay;
        miColor.nPitch = 200;
        MText->SetColor(miColor, TRUE, { 0.f,0.f,0.f,0.f }, { 0.f,0.f,0.f,0.f });
        miColor.nDelay += 400;
        MBox->SetColor(miColor, TRUE, { 0.f,0.f,0.f,0.f }, { 0.f,0.f,0.f,0.f });
        break;
    }
}

/**
    @brief 이 개체의 업데이트, 렌더링을 재개한다
*/
void UI_Static::resume(int nDelay)
{
    InputMotion(uiMotion, nDelay);
    uiMotionState = eUIMotionState::eUMS_PlayingVisible;
}

void UI_Static::update(unsigned long time)
{
    if (uiMotionState == eUIMotionState::eUMS_Hide) return;
    MBox->update(time);
    MText->update(time);
    // TODO : if(숨김모션 진행중 상태일때 업데이트가 완료되면) Hide로 돌입;
}

void UI_Static::render()
{
    if (uiMotionState == eUIMotionState::eUMS_Hide) return;
    MBox->render(pRenderTarget);
    MText->render(pRenderTarget);
}

void UI_Static::setText(wchar_t* pText, int nDelay)
{
    MOTION_INFO miText;

    if (!pText) return;
    nTextLen = (int)wcslen(pText);
    wcscpy_s(szText, MAX_BUTTONNAME, pText);

    miText.formular = eMotionForm::eMotion_Pulse1;
    miText.nDelay = nDelay;
    miText.nPitch = 200;
    MText->SetText(miText, pText, 0/*Current가 TRUE이기때문*/, nTextLen);
}

/**
    @brief 기본 버튼 메세지 핸들러
    @remark 사용자 지정 프로시저는 기본 핸들러 실행 후 호출된다.
*/
static void DefaultStaticProc(UI* pUI, UINT Message, void* param)
{
    UI_Static* button = (UI_Static*)pUI;
    pfnUIHandler UserHandler = pUI->MessageHandler;

    switch (Message) {
    default: break;
    }
    if (UserHandler) UserHandler(pUI, Message, param);
}