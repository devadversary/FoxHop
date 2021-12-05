#include "./include/ui_static.hpp"
#include "./include/ui_system.hpp"


static void DefaultStaticProc(UI* pUI, UINT Message, void* parm);

/**
    @brief ��� ����
    @param Motion UI�� ���� ���
    @param nDelay UI�� ���� ������
*/
void UI_Static::InputMotion(int Motion, int nDelay)
{
    MOTION_INFO miMove, miTextlen;
    POSITION TmpStartPos, TmpEndPos, TmpPos;
    D2D1_COLOR_F TmpStartColor;

    switch (uiMotion) {
#if 0 /*TODO : ����ƽ�� ��� ��Ÿ���� �����Ͽ� �پ��� ������ �����ϵ��� ���� ����*/
    case UIBS_RELOADMOTION:
        break;
#endif /*�켱�� �Ѱ��� ��Ǹ� ����*/
    default:
        /*��ư ������� �Է�*/
        miMove.formular = eMotionForm::eMotion_Linear1;
        miMove.nDelay = nDelay;
        miMove.nPitch = 200;
        TmpStartPos = uiPos;
        TmpStartPos.width = 0;
        MBox->Init(pRenderTarget, TmpStartPos, FaceColor);
        /*ù��° �̵�*/
        MBox->addMovementMotion(miMove, FALSE, TmpStartPos, uiPos);
        /*�ι�° �̵�*/
        TmpPos = uiPos;
        TmpPos.x += 10; /*�ؽ�Ʈ ��¦ �����*/
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
    @brief UI ����� ������ �ʱ�ȭ �� ��ȯ�Ѵ�.
*/
void UI_Static::Destroy()
{
    PoolBox->deactivateObject(MBox);
    PoolText->deactivateObject(MText);
    uiSys->BoxPoolStorage.returnPool(PoolBox);
    uiSys->TextPoolStorage.returnPool(PoolText);
    PoolBox = 0;
    PoolText = 0;
    uiSys->ReleaseUI(ID); /*�����θ� UI �ý��ۿ� ��ȯ*/
}

/**
    @brief �� ��ü�� ������Ʈ, �������� �Ͻ������Ѵ� (������ �ӽ� ��Ȱ��ȭ)
*/
void UI_Static::pause(int nDelay)
{
    MOTION_INFO miColor;

    /*TODO : �Ҹ� ��ǵ� ������ �� �ֵ��� �� ����*/
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
    @brief �� ��ü�� ������Ʈ, �������� �簳�Ѵ�
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
    // TODO : if(������ ������ �����϶� ������Ʈ�� �Ϸ�Ǹ�) Hide�� ����;
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
    MText->SetText(miText, pText, 0/*Current�� TRUE�̱⶧��*/, nTextLen);
}

/**
    @brief �⺻ ��ư �޼��� �ڵ鷯
    @remark ����� ���� ���ν����� �⺻ �ڵ鷯 ���� �� ȣ��ȴ�.
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