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
    CaretX = 0;
    CaretY = 0;
    CaretIdx = 0;
    StartSelectIdx = 0;
    ImeCompBoot = FALSE;
    IsSetSelect = FALSE;
    CaretTrail = FALSE;

    Str.assign(L"");
    pUISys->D2DA.pDWFactory->CreateTextLayout(Str.c_str(), Str.size(), pTextFormat, uiPos.x2, uiPos.y2, &pLayout);
    pLayout->SetMaxWidth(uiPos.x2);
    pLayout->SetMaxHeight(uiPos.y2);
    pRenderTarget->CreateSolidColorBrush({ 0,0,0,0 }, &pTextBrush);
    pRenderTarget->CreateSolidColorBrush(Motion.ColorSelect, &pSelectBrush);
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
    pBoxCaret->Init({uiPos.x, uiPos.y, Motion.CaretWidth, 0}, Motion.ColorFont);
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

    DrawSelectArea();
    pRenderTarget->DrawTextLayout({ uiPos.x,uiPos.y }, pLayout, pTextBrush);
    pBoxCaret->render(pRenderTarget);
}

void UI_Textinput::DefaultTextinputProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam)
{
    UI_Textinput* pInput = static_cast<UI_Textinput*>(pUI);
    IDWriteTextLayout* pNewLayout;
    IDWriteTextLayout* pOldLayout;
    DWRITE_HIT_TEST_METRICS HitMet;
    DWRITE_TEXT_METRICS TextMet;
    BOOL Trail, Inside;
    POSITION uiPos = pInput->uiPos;

    switch (Message) {
        case WM_LBUTTONDOWN: {
            long x = GET_X_LPARAM(lParam);
            long y = GET_Y_LPARAM(lParam);
            DWRITE_HIT_TEST_METRICS mat2;

            pInput->IsSetSelect = FALSE;
            pInput->DragState = TRUE;

            pInput->pLayout->HitTestPoint((float)x, (float)y, &Trail, &Inside, &HitMet);
            pInput->pLayout->HitTestTextPosition(HitMet.textPosition, Trail, &pInput->CaretX, &pInput->CaretY, &mat2);
            pInput->CaretIdx = HitMet.textPosition + Trail; /*문자열 중간이 아닌 끄트머리일땐 문자열 인덱스도 끄트머리여야 한다.*/
            pInput->StartSelectIdx = pInput->CaretIdx;
            pInput->SetCaret(HitMet.height, TRUE);
            break;
        }

        case WM_LBUTTONUP:
            pInput->DragState = FALSE;
            break;

        case WM_MOUSEMOVE: {
            long x = GET_X_LPARAM(lParam);
            long y = GET_Y_LPARAM(lParam);
            DWRITE_HIT_TEST_METRICS mat2;
            
            if (!pInput->DragState) break;
            pInput->pLayout->HitTestPoint((float)x, (float)y, &Trail, &Inside, &HitMet);

            if (!pInput->IsSetSelect) pInput->IsSetSelect = TRUE;

            pInput->CaretTrail = Trail;
            pInput->pLayout->HitTestTextPosition(HitMet.textPosition, Trail, &pInput->CaretX, &pInput->CaretY, &mat2);
            pInput->CaretIdx = HitMet.textPosition + Trail; /*문자열 중간이 아닌 끄트머리일땐 문자열 인덱스도 끄트머리여야 한다.*/
            pInput->SetCaret(HitMet.height, TRUE);
            break;
        }


        case WM_KEYDOWN: {
            if (pInput->DragState) break;
            switch (wParam) {
                case VK_BACK: {
                    pInput->CaretIdx--;
                    if (pInput->CaretIdx < 0) {
                        pInput->CaretIdx = 0;
                        break;
                    }

                    pInput->Str.erase(pInput->CaretIdx, 1);
                    pInput->UpdateTextLayout();
                    pInput->pLayout->HitTestTextPosition(pInput->CaretIdx, FALSE, &pInput->CaretX, &pInput->CaretY, &HitMet);
                    pInput->SetCaret(HitMet.height, TRUE);
                    break;
                }

                case VK_DELETE: {
                    pInput->Str.erase(pInput->CaretIdx, 1);
                    pInput->UpdateTextLayout();
                    pInput->pLayout->HitTestTextPosition(pInput->CaretIdx, FALSE, &pInput->CaretX, &pInput->CaretY, &HitMet);
                    pInput->SetCaret(HitMet.height, TRUE);
                    break;
                }

                case VK_UP: {
                    int i;
                    float tmp=0;
                    DWRITE_HIT_TEST_METRICS TmpMet;

                    pInput->pLayout->GetMetrics(&TextMet);
                    pInput->LineMetric.resize(TextMet.lineCount);
                    pInput->pLayout->GetLineMetrics(&pInput->LineMetric[0], TextMet.lineCount, &TextMet.lineCount);
                    for (i=0; i < TextMet.lineCount; i++) {
                        tmp += pInput->LineMetric[i].height;
                        /*행 높이 합산중, 캐럿Y좌표를 넘는순간*/
                        if (tmp > pInput->CaretY) {
                            tmp -= pInput->LineMetric[i].height;
                            if (i < 1) break;
                            tmp -= pInput->LineMetric[i-1].height;
                            pInput->pLayout->HitTestPoint(pInput->CaretX, tmp, &Trail, &Inside, &HitMet);
                            pInput->pLayout->HitTestTextPosition(HitMet.textPosition, Trail, &pInput->CaretX, &pInput->CaretY, &TmpMet);
                            pInput->CaretIdx = HitMet.textPosition + Trail; /*문자열 중간이 아닌 끄트머리일땐 문자열 인덱스도 끄트머리여야 한다.*/
                            pInput->SetCaret(HitMet.height, TRUE);
                            break;
                        }
                    }
                    break;
                }

                case VK_DOWN: {
                    int i;
                    float tmp = 0;
                    DWRITE_HIT_TEST_METRICS TmpMet;

                    pInput->pLayout->GetMetrics(&TextMet);
                    pInput->LineMetric.resize(TextMet.lineCount);
                    pInput->pLayout->GetLineMetrics(&pInput->LineMetric[0], TextMet.lineCount, &TextMet.lineCount);
                    for (i = 0; i < TextMet.lineCount; i++) {
                        tmp += pInput->LineMetric[i].height;
                        /*행 높이 합산중, 캐럿Y좌표를 넘는순간*/
                        if (tmp > pInput->CaretY) {
                            pInput->pLayout->HitTestPoint(pInput->CaretX, tmp, &Trail, &Inside, &HitMet);
                            pInput->pLayout->HitTestTextPosition(HitMet.textPosition, Trail, &pInput->CaretX, &pInput->CaretY, &TmpMet);
                            pInput->CaretIdx = HitMet.textPosition + Trail; /*문자열 중간이 아닌 끄트머리일땐 문자열 인덱스도 끄트머리여야 한다.*/
                            pInput->SetCaret(HitMet.height, TRUE);
                            break;
                        }
                    }
                    break;
                }

                case VK_LEFT:
                    pInput->CaretIdx--;
                    if (pInput->CaretIdx < 0) {
                        pInput->CaretIdx = 0;
                        break;
                    }
                    pInput->pLayout->HitTestTextPosition(pInput->CaretIdx, FALSE, &pInput->CaretX, &pInput->CaretY, &HitMet);
                    pInput->SetCaret(HitMet.height, TRUE);
                    break;

                case VK_RIGHT:
                    pInput->CaretIdx++;
                    if (pInput->CaretIdx > pInput->Str.size()) {
                        pInput->CaretIdx--;
                        break;
                    }
                    pInput->pLayout->HitTestTextPosition(pInput->CaretIdx, FALSE, &pInput->CaretX, &pInput->CaretY, &HitMet);
                    pInput->SetCaret(HitMet.height, TRUE);
                    break;
            }
            break;
        }

        case WM_IME_COMPOSITION: {
            int offset = 1;
            if (pInput->DragState) break;
            pInput->IsSetSelect = FALSE;

            if (lParam & GCS_RESULTSTR) {
                pInput->Str.erase(pInput->CaretIdx, 1);
                pInput->ImeCompBoot = FALSE;
            }
            if (lParam & GCS_COMPSTR) {
                if (!pInput->ImeCompBoot) {
                    pInput->Str.insert(pInput->CaretIdx, 1, wParam);
                    pInput->ImeCompBoot = TRUE;
                }
                else {
                    pInput->Str.replace(pInput->CaretIdx, 1, 1, wParam);
                }
            }
            pInput->UpdateTextLayout();
            pInput->pLayout->HitTestTextPosition(pInput->CaretIdx+offset, FALSE, &pInput->CaretX, &pInput->CaretY, &HitMet);
            pInput->SetCaret(HitMet.height, TRUE);
            break;
        }
        
        case WM_CHAR: {
            if (pInput->DragState) break;
            if (wParam < 0x20 && wParam != VK_RETURN) break;
            pInput->IsSetSelect = FALSE;

            pInput->Str.insert(pInput->CaretIdx, 1, wParam);
            pInput->UpdateTextLayout();
            pInput->CaretIdx++;
            pInput->pLayout->HitTestTextPosition(pInput->CaretIdx, FALSE, &pInput->CaretX, &pInput->CaretY, &HitMet);
            pInput->SetCaret(HitMet.height, TRUE);
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

void UI_Textinput::DrawSelectArea()
{
    int i;
    float TmpHeight;
    BOOL Trail, Inside;
    DWRITE_HIT_TEST_METRICS TmpMet;
    DWRITE_HIT_TEST_METRICS HitMet;
    int si, ei;
    int TmpLen;
    float CharX, CharY;

    if (!IsSetSelect) return;
    if (StartSelectIdx == CaretIdx) return;

    if (StartSelectIdx > CaretIdx) { si = CaretIdx; ei = StartSelectIdx; }
    else { si = StartSelectIdx; ei = CaretIdx; }

    pLayout->GetMetrics(&TextMet);
    LineMetric.resize(TextMet.lineCount);
    pLayout->GetLineMetrics(&LineMetric[0], TextMet.lineCount, &TextMet.lineCount);

    TmpLen = 0;
    TmpHeight = 0;
    for (i = 0; i < TextMet.lineCount; i++) {
        int LineStartIdx = TmpLen;
        int LineEndIdx = TmpLen + LineMetric[i].length-1;
        float DrawStartX, DrawEndX;
        float Distance;
        int TargetIdx;
        D2D1_RECT_F rect;

        if (LineEndIdx < si || LineStartIdx > ei) {
            TmpLen += LineMetric[i].length;
            TmpHeight += LineMetric[i].height;
            continue;
        }

        if (LineStartIdx <= si) TargetIdx = si;
        else TargetIdx = LineStartIdx;
        pLayout->HitTestTextPosition(TargetIdx, FALSE, &DrawStartX, &CharY, &TmpMet);

        if (LineEndIdx >= ei) TargetIdx = ei;
        else TargetIdx = LineEndIdx;
        pLayout->HitTestTextPosition(TargetIdx, FALSE, &DrawEndX, &CharY, &TmpMet);

        Distance = DrawEndX - DrawStartX;

        rect = { uiPos.x + DrawStartX, 
                 uiPos.y + TmpHeight,
                 uiPos.x + DrawStartX + Distance,
                 uiPos.y + TmpHeight + LineMetric[i].height };
        pRenderTarget->FillRectangle(rect, pSelectBrush);
        TmpLen += LineMetric[i].length;
        TmpHeight += LineMetric[i].height;
    }
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

void UI_Textinput::SetCaret(float CaretHeight, BOOL bMotion)
{
    eTextinputMotionPattern patt;
    MOTION_INFO mi;
    POSITION CaretPos = {uiPos.x+CaretX, uiPos.y+CaretY, Motion.CaretWidth, CaretHeight };

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