#include "./include/ui_textinput.hpp"

UI_Textinput::UI_Textinput(UISystem* pUISys, pfnUIHandler pfnCallback, POSITION Pos, IDWriteTextFormat* pTextFmt, UI_Textinput_MotionParam MotionParam)
{
    uiSys = pUISys;
    pRenderTarget = pUISys->D2DA.pRenTarget;
    Focusable = TRUE;
    uiMotionState = eUIMotionState::eUMS_PlayingVisible;

    DefaultHandler = DefaultTextinputProc;
    MessageHandler = pfnCallback;
    uiPos = Pos;

    Motion = MotionParam;
    pTextFormat = pTextFmt;
    DragState = FALSE;
    CaretIdx = 0;

    Str.assign(L"");
    pUISys->D2DA.pDWFactory->CreateTextLayout(Str.c_str(), Str.size(), pTextFormat, uiPos.x2, uiPos.y2, &pLayout);
    pRenderTarget->CreateSolidColorBrush({ 0,0,0,0 }, &pTextBrush);
    pBoxBg = new PropBox(pRenderTarget);
    pBoxFrame = new PropBox(pRenderTarget);
    pBoxCaret = new PropBox(pRenderTarget);
    resume(0);
}

UI_Textinput::~UI_Textinput()
{

}

void UI_Textinput::pause(int nDelay)
{

}

void UI_Textinput::resume(int nDelay)
{
    SetBg(Motion.ColorBg, TRUE);
    SetFrame(Motion.ColorFrame, TRUE);
    SetTextlayout(Motion.ColorFont, TRUE);
    pBoxCaret->Init(ALL_ZERO, Motion.ColorFont);
}

BOOL UI_Textinput::update(unsigned long time)
{
    BOOL bUpdated = FALSE;

    if (uiMotionState == eUIMotionState::eUMS_Hide) return FALSE;

    bUpdated |= pBoxFrame->update(time);
    bUpdated |= pBoxBg->update(time);
    //bUpdated |= pText->update(time);
    pBoxCaret->update(time); /*캐럿의 모션은 업데이트 여부에 포함시키지 않는다.*/

    if (!bUpdated) {
        if (uiMotionState == eUIMotionState::eUMS_PlayingHide)
            uiMotionState = eUIMotionState::eUMS_Hide;

        else if (uiMotionState == eUIMotionState::eUMS_PlayingVisible)
            uiMotionState = eUIMotionState::eUMS_Visible;
        return FALSE;
    }
    return TRUE;
}

void UI_Textinput::render()
{
    if (uiMotionState == eUIMotionState::eUMS_Hide) return;
    pBoxBg->render(pRenderTarget);
    pBoxFrame->render(pRenderTarget);
    pRenderTarget->DrawTextLayout({ uiPos.x,uiPos.y }, pLayout, pTextBrush);
    pBoxCaret->render(pRenderTarget);
}

void UI_Textinput::DefaultTextinputProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam)
{
    UI_Textinput* pInput = static_cast<UI_Textinput*>(pUI);

    switch (Message) {
        case WM_LBUTTONDOWN: {
            long x = GET_X_LPARAM(lParam);
            long y = GET_Y_LPARAM(lParam);
            float CaretX, CaretY;
            BOOL Trail, Inside;
            DWRITE_HIT_TEST_METRICS mat;
            DWRITE_HIT_TEST_METRICS mat2;
            POSITION uiPos = pInput->uiPos;

            pInput->DragState = TRUE;
            pInput->pLayout->HitTestPoint((float)x, (float)y, &Trail, &Inside, &mat);
            pInput->pLayout->HitTestTextPosition(mat.textPosition, Trail, &CaretX, &CaretY, &mat2);
            pInput->CaretIdx = mat.textPosition;
            pInput->SetCaret({uiPos.x+CaretX, uiPos.y+CaretY, 3, mat.height}, TRUE);
            break;
        }

        case WM_LBUTTONUP:
            pInput->DragState = FALSE;
            break;

        case WM_KEYDOWN: {
            switch (wParam) {
                case VK_BACK: {
                    IDWriteTextLayout* pNewLayout;
                    IDWriteTextLayout* pOldLayout;
                    DWRITE_HIT_TEST_METRICS mat;
                    POSITION uiPos = pInput->uiPos;
                    float x, y;

                    pInput->CaretIdx--;
                    if (pInput->CaretIdx < 0)pInput->CaretIdx = 0;

                    pInput->Str.erase(pInput->CaretIdx, 1);
                    pInput->UpdateTextLayout();
                    pInput->pLayout->HitTestTextPosition(pInput->CaretIdx, FALSE, &x, &y, &mat);
                    pInput->SetCaret({ uiPos.x + x, uiPos.y + y, 3, mat.height }, TRUE);
                    break;
                }

                case VK_DELETE: {
                    IDWriteTextLayout* pNewLayout;
                    IDWriteTextLayout* pOldLayout;
                    DWRITE_HIT_TEST_METRICS mat;
                    POSITION uiPos = pInput->uiPos;
                    float x, y;

                    pInput->Str.erase(pInput->CaretIdx, 1);
                    pInput->UpdateTextLayout();
                    pInput->pLayout->HitTestTextPosition(pInput->CaretIdx, FALSE, &x, &y, &mat);
                    pInput->SetCaret({ uiPos.x + x, uiPos.y + y, 3, mat.height }, TRUE);
                    break;
                }
            }
            break;
        }


        case WM_CHAR: {
            IDWriteTextLayout* pNewLayout;
            IDWriteTextLayout* pOldLayout;
            DWRITE_HIT_TEST_METRICS mat;
            POSITION uiPos = pInput->uiPos;
            float x, y;

            if (wParam < 0x20 && wParam != VK_RETURN) break;
            pInput->Str.insert(pInput->CaretIdx, 1, wParam);
            pInput->UpdateTextLayout();
            pInput->CaretIdx++;
            pInput->pLayout->HitTestTextPosition(pInput->CaretIdx, FALSE, &x, &y, &mat);
            pInput->SetCaret({ uiPos.x + x, uiPos.y + y, 3, mat.height }, TRUE);
            break;
        }
    }
}

void UI_Textinput::UpdateTextLayout()
{
    IDWriteTextLayout* pNewLayout;
    IDWriteTextLayout* pOldLayout;

    uiSys->D2DA.pDWFactory->CreateTextLayout(Str.c_str(), Str.size(), pTextFormat, uiPos.x2, uiPos.y2, &pNewLayout);
    pOldLayout = pLayout;
    pLayout = pNewLayout;
    pOldLayout->Release();
}

void UI_Textinput::SetFrame(D2D1_COLOR_F Color, BOOL bMotion)
{
    eTextinputMotionPattern Patt;
    if (bMotion) Patt = eTextinputMotionPattern::eFrameInit_Default;
    else Patt = Motion.MotionFrameInit;

    switch (Patt) {
    case eTextinputMotionPattern::eFrameInit_Default:
        pBoxFrame->Init(uiPos, Color, FALSE);
        break;
    }
}

void UI_Textinput::PauseFrame(unsigned long Delay)
{
    switch (Motion.MotionFramePause) {
    case eTextinputMotionPattern::eFramePause_Default:
        pBoxFrame->Init(uiPos, ALL_ZERO, FALSE);
        break;
    }
}

void UI_Textinput::SetBg(D2D1_COLOR_F Color, BOOL bMotion)
{
    eTextinputMotionPattern Patt;
    if (bMotion) Patt = eTextinputMotionPattern::eBgInit_Default;
    else Patt = Motion.MotionBgInit;

    switch (Patt) {
    case eTextinputMotionPattern::eBgInit_Default:
        pBoxBg->Init(uiPos, Color);
        break;
    }
}

void UI_Textinput::PauseBg(unsigned long Delay)
{
    switch (Motion.MotionBgPause) {
    case eTextinputMotionPattern::eBgPause_Default:
        pBoxBg->Init(uiPos, ALL_ZERO);
        break;
    }
}

void UI_Textinput::SetCaret(POSITION CaretPos, BOOL bMotion)
{
    eTextinputMotionPattern patt;
    MOTION_INFO mi;

    if (bMotion) patt = Motion.MotionCaretMove;
    else patt = eTextinputMotionPattern::eCaretMove_Default;

    switch (patt) {
    case eTextinputMotionPattern::eCaretMove_Default:
        mi = InitMotionInfo(eMotionForm::eMotion_None, 0, 0);
        pBoxCaret->SetPos(mi, TRUE, ALL_ZERO, CaretPos);
        break;

    case eTextinputMotionPattern::eCaretMove_Decel:
        mi = InitMotionInfo(eMotionForm::eMotion_x3_2, 0, Motion.PitchCaretMove);
        pBoxCaret->SetPos(mi, TRUE, ALL_ZERO, CaretPos);
        break;
    }
}

void UI_Textinput::PauseCaret(unsigned long Delay)
{

}

void UI_Textinput::SetTextlayout(D2D1_COLOR_F Color, BOOL bMotion)
{
    pTextBrush->SetColor(Color);
}