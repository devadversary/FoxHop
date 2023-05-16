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
    CaretTrail = FALSE;
    CaretH = 0;
    pRenderTarget->CreateSolidColorBrush(Motion.ColorSelect, &pSelectBrush);
    pBoxBg = new PropBox(pRenderTarget);
    pBoxFrame = new PropBox(pRenderTarget);
    pBoxCaret = new PropBox(pRenderTarget);
    pTextLayout = new PropTextLayout(pRenderTarget, pTextFormat, pUISys->D2DA.pDWFactory, uiPos.x2, uiPos.y2);
    resume(0);
}

UI_Textinput::~UI_Textinput()
{

}

void UI_Textinput::pause(int nDelay)
{
    uiMotionState = eUIMotionState::eUMS_PlayingHide;
    PauseFrame(nDelay + Motion.DelayPauseFrame);
    PauseBg(nDelay + Motion.DelayPauseBg);
    PauseTextlayout(nDelay + Motion.DelayPauseText);
    PauseCaret(nDelay + Motion.DelayPauseCaret);
}

void UI_Textinput::resume(int nDelay)
{
    uiMotionState = eUIMotionState::eUMS_PlayingVisible;
    ResumeFrame(nDelay + Motion.DelayInitFrame);
    ResumeBg(nDelay + Motion.DelayInitBg);
    ResumeTextlayout(nDelay + Motion.DelayInitText);
    ResumeCaret(nDelay + Motion.DelayInitCaret);
    //pBoxCaret->Init({uiPos.x, uiPos.y, Motion.CaretWidth, 0}, Motion.ColorFont);
}

BOOL UI_Textinput::update(unsigned long time)
{
    BOOL bUpdated = FALSE;

    if (uiMotionState == eUIMotionState::eUMS_Hide) return FALSE;

    bUpdated |= pBoxFrame->update(time);
    bUpdated |= pBoxBg->update(time);
    bUpdated |= pTextLayout->update(time);
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
    pTextLayout->render(pRenderTarget);
    pBoxCaret->render(pRenderTarget);
}

BOOL UI_Textinput::DeleteSelectArea()
{
    int si, ei, distance;
    DWRITE_HIT_TEST_METRICS HitMet;

    if (StartSelectIdx == CaretIdx) return FALSE;
    pTextLayout->EraseText(StartSelectIdx, CaretIdx);
    CaretIdx = min(StartSelectIdx, CaretIdx);
    StartSelectIdx = CaretIdx;
    pTextLayout->pLayout->HitTestTextPosition(CaretIdx, FALSE, &CaretX, &CaretY, &HitMet);
    MoveCaret(HitMet.height, TRUE);
    return TRUE;
}

void UI_Textinput::OnKeyInput(UINT Message, WPARAM wParam, LPARAM lParam)
{
    DWRITE_HIT_TEST_METRICS HitMet;
    BOOL Trail, Inside;

    switch (Message) {
        case WM_KEYDOWN: {
            if (DragState) break;

            switch (wParam) {
            case VK_BACK: {
                if (DeleteSelectArea()) break;
                CaretIdx--;
                if (CaretIdx < 0) {
                    CaretIdx = 0;
                    StartSelectIdx = CaretIdx;
                    break;
                }
                StartSelectIdx = CaretIdx;

                pTextLayout->EraseText(CaretIdx, CaretIdx+1);
                pTextLayout->pLayout->HitTestTextPosition(CaretIdx, FALSE, &CaretX, &CaretY, &HitMet);
                MoveCaret(HitMet.height, TRUE);
                break;
            }

            case VK_DELETE: {
                if (DeleteSelectArea()) break;
                pTextLayout->EraseText(CaretIdx, CaretIdx + 1);
                pTextLayout->pLayout->HitTestTextPosition(CaretIdx, FALSE, &CaretX, &CaretY, &HitMet);
                MoveCaret(HitMet.height, TRUE);
                break;
            }

            case VK_UP: {
                int i;
                float tmp = 0;
                DWRITE_HIT_TEST_METRICS TmpMet;

                pTextLayout->pLayout->GetMetrics(&TextMet);
                LineMetric.resize(TextMet.lineCount);
                pTextLayout->pLayout->GetLineMetrics(&LineMetric[0], TextMet.lineCount, &TextMet.lineCount);
                for (i = 0; i < TextMet.lineCount; i++) {
                    tmp += LineMetric[i].height;
                    /*행 높이 합산중, 캐럿Y좌표를 넘는순간*/
                    if (tmp > CaretY) {
                        tmp -= LineMetric[i].height;
                        if (i < 1) break;
                        tmp -= LineMetric[i - 1].height;
                        pTextLayout->pLayout->HitTestPoint(CaretX, tmp, &Trail, &Inside, &HitMet);
                        pTextLayout->pLayout->HitTestTextPosition(HitMet.textPosition, Trail, &CaretX, &CaretY, &TmpMet);
                        CaretIdx = HitMet.textPosition + Trail; /*문자열 중간이 아닌 끄트머리일땐 문자열 인덱스도 끄트머리여야 한다.*/
                        if (!(GetKeyState(VK_SHIFT) & 0x80))
                            StartSelectIdx = CaretIdx;
                        MoveCaret(HitMet.height, TRUE);
                        break;
                    }
                }
                break;
            }

            case VK_DOWN: {
                float tmp = 0;
                DWRITE_HIT_TEST_METRICS TmpMet;

                pTextLayout->pLayout->GetMetrics(&TextMet);
                LineMetric.resize(TextMet.lineCount);
                pTextLayout->pLayout->GetLineMetrics(&LineMetric[0], TextMet.lineCount, &TextMet.lineCount);
                for (int i = 0; i < TextMet.lineCount; i++) {
                    tmp += LineMetric[i].height;
                    /*행 높이 합산중, 캐럿Y좌표를 넘는순간*/
                    if (tmp > CaretY) {
                        pTextLayout->pLayout->HitTestPoint(CaretX, tmp, &Trail, &Inside, &HitMet);
                        pTextLayout->pLayout->HitTestTextPosition(HitMet.textPosition, Trail, &CaretX, &CaretY, &TmpMet);
                        CaretIdx = HitMet.textPosition + Trail; /*문자열 중간이 아닌 끄트머리일땐 문자열 인덱스도 끄트머리여야 한다.*/
                        if (!(GetKeyState(VK_SHIFT) & 0x80))
                            StartSelectIdx = CaretIdx;
                        MoveCaret(HitMet.height, TRUE);
                        break;
                    }
                }
                break;
            }

            case VK_LEFT:
                CaretIdx--;
                if (CaretIdx < 0) CaretIdx = 0;
                if (!(GetKeyState(VK_SHIFT) & 0x80))
                    StartSelectIdx = CaretIdx;

                pTextLayout->pLayout->HitTestTextPosition(CaretIdx, FALSE, &CaretX, &CaretY, &HitMet);
                MoveCaret(HitMet.height, TRUE);
                break;

            case VK_RIGHT:
                CaretIdx++;
                if (CaretIdx > pTextLayout->Str.size()) CaretIdx--;
                if (!(GetKeyState(VK_SHIFT) & 0x80))
                    StartSelectIdx = CaretIdx;

                pTextLayout->pLayout->HitTestTextPosition(CaretIdx, FALSE, &CaretX, &CaretY, &HitMet);
                MoveCaret(HitMet.height, TRUE);
                break;
            }
            break;
        }
    }
}

void UI_Textinput::DefaultTextinputProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam)
{
    UI_Textinput* pInput = static_cast<UI_Textinput*>(pUI);
    IDWriteTextLayout* pNewLayout;
    IDWriteTextLayout* pOldLayout;
    DWRITE_TEXT_METRICS TextMet;
    DWRITE_HIT_TEST_METRICS HitMet;
    BOOL Trail, Inside;
    POSITION uiPos = pInput->uiPos;

    switch (Message) {
        case WM_LBUTTONDOWN: {
            long x = GET_X_LPARAM(lParam);
            long y = GET_Y_LPARAM(lParam);
            DWRITE_HIT_TEST_METRICS mat2;

            pInput->DragState = TRUE;

            pInput->pTextLayout->pLayout->HitTestPoint((float)x, (float)y, &Trail, &Inside, &mat2);
            pInput->pTextLayout->pLayout->HitTestTextPosition(mat2.textPosition, Trail, &pInput->CaretX, &pInput->CaretY, &mat2);
            pInput->CaretIdx = mat2.textPosition + Trail; /*문자열 중간이 아닌 끄트머리일땐 문자열 인덱스도 끄트머리여야 한다.*/
            pInput->StartSelectIdx = pInput->CaretIdx;
            pInput->MoveCaret(mat2.height, TRUE);
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
            pInput->pTextLayout->pLayout->HitTestPoint((float)x, (float)y, &Trail, &Inside, &mat2);

            pInput->CaretTrail = Trail;
            pInput->pTextLayout->pLayout->HitTestTextPosition(mat2.textPosition, Trail, &pInput->CaretX, &pInput->CaretY, &mat2);
            pInput->CaretIdx = mat2.textPosition + Trail; /*문자열 중간이 아닌 끄트머리일땐 문자열 인덱스도 끄트머리여야 한다.*/
            pInput->MoveCaret(mat2.height, TRUE);
            break;
        }

        case WM_KEYDOWN:
            pInput->OnKeyInput(Message, wParam, lParam);
            break;

        case WM_IME_COMPOSITION: {
            if (pInput->DragState) break;
            pInput->DeleteSelectArea();
            if (lParam & GCS_RESULTSTR) {
                pInput->pTextLayout->EraseText(pInput->CaretIdx, pInput->CaretIdx+1);
                pInput->ImeCompBoot = FALSE;
            }
            if (lParam & GCS_COMPSTR) {
                if (!pInput->ImeCompBoot) {
                    pInput->pTextLayout->InsertChar(pInput->CaretIdx, wParam);
                    pInput->ImeCompBoot = TRUE;
                }
                else pInput->pTextLayout->ReplaceChar(pInput->CaretIdx, wParam);
            }
            pInput->pTextLayout->pLayout->HitTestTextPosition(pInput->CaretIdx + 1, FALSE, &pInput->CaretX, &pInput->CaretY, &HitMet);
            pInput->MoveCaret(HitMet.height, TRUE);
            break;
        }
        
        case WM_CHAR: {
            if (pInput->DragState) break;
            if (wParam < 0x20 && wParam != VK_RETURN) break;
            pInput->DeleteSelectArea();
            pInput->pTextLayout->InsertChar(pInput->CaretIdx, wParam);
            pInput->CaretIdx++;
            pInput->StartSelectIdx = pInput->CaretIdx;
            pInput->pTextLayout->pLayout->HitTestTextPosition(pInput->CaretIdx + pInput->ImeCompBoot, FALSE, &pInput->CaretX, &pInput->CaretY, &HitMet);
            pInput->MoveCaret(HitMet.height, TRUE);
            break;
        }
    }
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

    if (uiMotionState != eUIMotionState::eUMS_Visible) return; /*온전한 UI 상태에서만*/
    if (StartSelectIdx == CaretIdx) return;
    if (StartSelectIdx > CaretIdx) { si = CaretIdx; ei = StartSelectIdx; }
    else { si = StartSelectIdx; ei = CaretIdx; }

    pTextLayout->pLayout->GetMetrics(&TextMet);
    LineMetric.resize(TextMet.lineCount);
    pTextLayout->pLayout->GetLineMetrics(&LineMetric[0], TextMet.lineCount, &TextMet.lineCount);

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
        pTextLayout->pLayout->HitTestTextPosition(TargetIdx, FALSE, &DrawStartX, &CharY, &TmpMet);

        if (LineEndIdx >= ei) TargetIdx = ei;
        else TargetIdx = LineEndIdx;
        pTextLayout->pLayout->HitTestTextPosition(TargetIdx, pTextLayout->Str.size() == TargetIdx ? TRUE : FALSE, &DrawEndX, &CharY, &TmpMet);

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

void UI_Textinput::ResumeFrame(unsigned long Delay)
{

    switch (Motion.MotionInitFrame) {
    case eTextinputMotionPattern::eInitFrame_Default:
        pBoxFrame->Init(uiPos, Motion.ColorFrame, FALSE);
        break;
    }
}

void UI_Textinput::PauseFrame(unsigned long Delay)
{
    switch (Motion.MotionPauseFrame) {
    case eTextinputMotionPattern::ePauseFrame_Default:
        pBoxFrame->Init(uiPos, ALL_ZERO, FALSE);
        break;
    }
}

void UI_Textinput::ResumeBg(unsigned long Delay)
{
    eTextinputMotionPattern Patt;

    switch (Motion.MotionInitBg) {
    case eTextinputMotionPattern::eInitBg_Default:
        pBoxBg->Init(uiPos, Motion.ColorBg);
        break;
    }
}

void UI_Textinput::PauseBg(unsigned long Delay)
{
    switch (Motion.MotionPauseBg) {
    case eTextinputMotionPattern::ePauseBg_Default:
        pBoxBg->Init(uiPos, ALL_ZERO);
        break;
    }
}

void UI_Textinput::ResumeCaret(unsigned long Delay)
{
    MOTION_INFO mi;

    switch (Motion.MotionInitCaret) {
    case eTextinputMotionPattern::eInitCaret_Default:
        pBoxCaret->Init({ uiPos.x, uiPos.y, Motion.CaretWidth, CaretH}, ALL_ZERO);
        mi = InitMotionInfo(eMotionForm::eMotion_None, Delay, Motion.PitchInitCaret);
        pBoxCaret->SetColor(mi, TRUE, ALL_ZERO, Motion.ColorCaret);
        break;
    }
}

void UI_Textinput::MoveCaret(float CaretHeight, BOOL bMotion)
{
    eTextinputMotionPattern patt;
    MOTION_INFO mi;
    POSITION CaretPos = {uiPos.x+CaretX, uiPos.y+CaretY, Motion.CaretWidth, CaretHeight };

    if (bMotion) patt = Motion.MotionMoveCaret;
    else patt = eTextinputMotionPattern::eMoveCaret_Default;
    CaretH = CaretHeight;

    switch (patt) {
    case eTextinputMotionPattern::eMoveCaret_Default:
        mi = InitMotionInfo(eMotionForm::eMotion_None, 0, 0);
        pBoxCaret->SetPos(mi, TRUE, ALL_ZERO, CaretPos);
        break;

    case eTextinputMotionPattern::eMoveCaret_Decel:
        mi = InitMotionInfo(eMotionForm::eMotion_x3_2, 0, Motion.PitchMoveCaret);
        pBoxCaret->SetPos(mi, TRUE, ALL_ZERO, CaretPos);
        break;
    }
}

void UI_Textinput::PauseCaret(unsigned long Delay)
{
    MOTION_INFO mi;

    switch (Motion.MotionPauseCaret) {
    case eTextinputMotionPattern::ePauseCaret_Default:
        mi = InitMotionInfo(eMotionForm::eMotion_None, Delay, Motion.PitchPauseCaret);
        pBoxCaret->SetColor(mi, TRUE, ALL_ZERO, ALL_ZERO);
        break;
    }
}

void UI_Textinput::ResumeTextlayout(unsigned long Delay)
{
    MOTION_INFO mi;

    switch (Motion.MotionInitText) {
    case eTextinputMotionPattern::eInitText_Default:
        pTextLayout->Init(uiPos, ALL_ZERO);
        mi = InitMotionInfo(eMotionForm::eMotion_None, Delay, Motion.PitchInitText);
        pTextLayout->SetColor(mi, TRUE, ALL_ZERO, Motion.ColorText);
        break;
    }
}

void UI_Textinput::PauseTextlayout(unsigned long Delay)
{
    MOTION_INFO mi;

    switch (Motion.MotionPauseText) {
    case eTextinputMotionPattern::ePauseText_Default:
        mi = InitMotionInfo(eMotionForm::eMotion_None, Delay, Motion.PitchPauseText);
        pTextLayout->SetColor(mi, TRUE, ALL_ZERO, ALL_ZERO);
        break;
    }
}