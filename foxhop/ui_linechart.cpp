#include "./include/ui_linechart.hpp"
#include "./include/ui_system.hpp"
#include <cstdarg>

UI_LineChart::UI_LineChart(UISystem* pUISys, pfnUIHandler pfnCallback, POSITION Pos, IDWriteTextFormat* pTextFmt, unsigned long ChartPixelGap, float EnsureMinVal, float EnsureMaxVal, float BaseSplitVal, unsigned long nSplitCnt, unsigned long Delay, UI_LineChart_MotionParam MotionParam)
{
    D2D1_STROKE_STYLE_PROPERTIES style;

    uiSys = pUISys;
    pRenderTarget = pUISys->D2DA.pRenTarget;
    Focusable = FALSE;
    uiMotionState = eUIMotionState::eUMS_PlayingVisible;

    DefaultHandler = DefaultLineChartProc;
    MessageHandler = pfnCallback;
    uiPos = Pos;
    Motion = MotionParam;
    
    style.dashCap = D2D1_CAP_STYLE_FLAT;
    style.startCap = D2D1_CAP_STYLE_FLAT;
    style.endCap = D2D1_CAP_STYLE_FLAT;
    style.lineJoin = D2D1_LINE_JOIN_MITER;
    style.dashStyle = D2D1_DASH_STYLE_DASH;
    style.miterLimit = 0;
    style.dashOffset = 0;
    uiSys->D2DA.pFactory->CreateStrokeStyle(&style, NULL, 0, &pStroke);

    InitializeSRWLock(&lock);
    ViewDataWidth = ChartPixelGap;
    ViewDataCnt = (uiPos.x2 / ViewDataWidth) + 1;
    ValidViewDataCnt = 0;
    EnsureMin = EnsureMinVal;
    EnsureMax = EnsureMaxVal;
    IntervalMin = EnsureMinVal;
    IntervalMax = EnsureMaxVal;
    SplitCnt = nSplitCnt;
    BaseSplit = BaseSplitVal;
    for (int i = 0; i < ViewDataCnt; i++) {
        ChartObject* pObj = new ChartObject(this, pStroke, ViewDataWidth, uiPos.y2);
        ViewData.emplace_back(pObj);
        PropLines.emplace_back(new PropLine(pRenderTarget));
    }
    PropLines.emplace_back(new PropLine(pRenderTarget)); /*선 갯수는 하나 더 많아야 함.*/

    pBoxBg = new PropBox(pRenderTarget);
    pBoxFrame = new PropBox(pRenderTarget);
    resume(Delay);
}

UI_LineChart::~UI_LineChart()
{

}

void UI_LineChart::DefaultLineChartProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam)
{
    UI_LineChart* pChart = (UI_LineChart*)pUI;

    switch (Message) {
        case WM_MOUSEWHEEL: {
            long long ScrollPx;
            AcquireSRWLockExclusive(&pChart->lock);
            ScrollPx = pChart->ScrollPixel - (GET_WHEEL_DELTA_WPARAM(wParam) / 2);
            ReleaseSRWLockExclusive(&pChart->lock);
            pChart->SetScroll(ScrollPx);
            break;
        }
    }
}

void UI_LineChart::SetScroll(long long TargetScrollPx)
{
    MOTION_PATTERN patt;
    MOTION_INFO    mi;
    unsigned long long DataCnt;
    long long MaxScroll;

    /*초기화 / 소멸 모션 진행중엔 스크롤 X*/
    if (uiMotionState != eUIMotionState::eUMS_Visible) return;

    AcquireSRWLockExclusive(&lock);

    /*스크롤 경계 관리*/
    DataCnt = MainData.size();
    MaxScroll = (DataCnt * ViewDataWidth) - uiPos.x2;
    if (MaxScroll < 0) MaxScroll = 0;
    if (TargetScrollPx < 0) TargetScrollPx = 0;
    else if (TargetScrollPx > MaxScroll) TargetScrollPx = MaxScroll;
    ScrollPixel = TargetScrollPx;

    /*스크롤 모션 진행 재시작*/
    ScrollComp->clearChannel();
    mi = InitMotionInfo(eMotionForm::eMotion_x3_2, 0, Motion.PitchScroll);
    patt = InitMotionPattern(mi, NULL);
    AddChain(&patt, &CurrentScroll, CurrentScroll, (float)ScrollPixel);
    ScrollComp->addChannel(patt);

    ReleaseSRWLockExclusive(&lock);
}

void UI_LineChart::AddValue(float Val, BOOL bAutoScroll)
{
    unsigned long long DataCnt;

    AcquireSRWLockExclusive(&lock);
    MainData.emplace_back(Val);
    DataCnt = MainData.size();
    ValidViewDataCnt = ViewDataCnt < DataCnt ? DataCnt : ViewDataCnt;
    ReleaseSRWLockExclusive(&lock);

    if (uiMotionState != eUIMotionState::eUMS_Visible) return;
    if (bAutoScroll) SetScroll(DataCnt * ViewDataWidth);
}

void UI_LineChart::EditValue(unsigned long long DataIdx, float NewVal)
{
    AcquireSRWLockExclusive(&lock);
    MainData[DataIdx] = NewVal;
    ReleaseSRWLockExclusive(&lock);
}

void UI_LineChart::resume(int nDelay)
{
    uiMotionState = eUIMotionState::eUMS_PlayingVisible;
    ResumeFrame(nDelay + Motion.DelayInitFrame);
    ResumeBg(nDelay + Motion.DelayInitBg);
    ResumeText(nDelay + Motion.DelayInitText);
    ResumeDataOrder(nDelay + Motion.DelayInitFrame);
}

void UI_LineChart::pause(int nDelay)
{
    uiMotionState = eUIMotionState::eUMS_PlayingHide;
    AcquireSRWLockExclusive(&lock);
    ScrollComp->clearChannel(); /*스크롤 모션 정지*/
    ReleaseSRWLockExclusive(&lock);
    PauseFrame(nDelay + Motion.DelayPauseFrame);
    PauseBg(nDelay + Motion.DelayPauseBg);
    PauseText(nDelay + Motion.DelayPauseText);
    PauseDataOrder(nDelay + Motion.DelayPauseFrame);
}

BOOL UI_LineChart::update(unsigned long time)
{
    BOOL bUpdated;
    BOOL bNeedRangeUpdate = FALSE;
    long long ModIdx;
    float IntervalMin, IntervalMax; /*구간 최대최솟값*/

    if (uiMotionState == eUIMotionState::eUMS_Hide) return FALSE;

    AcquireSRWLockExclusive(&lock);
    bUpdated = ScrollComp->update(time);
    bUpdated |= pBoxFrame->update(time);
    bUpdated |= pBoxBg->update(time);
    //bUpdated |= p->update(time);

    ViewStartIdx = (long long)CurrentScroll / ViewDataWidth;
    ModIdx = ViewStartIdx % ViewDataCnt;

    /*데이터 바인딩*/
    for (int i = 0; i < ValidViewDataCnt; i++) {
        unsigned long long BindTargetIdx = ViewStartIdx + i;
        long long TmpIdx = (ModIdx+i) % ViewDataCnt;
        ChartObject* pObj = ViewData[TmpIdx];

        if (pObj->MainDataIdx == BindTargetIdx) continue; /*이미 바인드된 데이터*/

        /*구간 최대최소값 갱신*/
        IntervalMax = max(EnsureMax, pObj->Value);
        IntervalMin = min(EnsureMin, pObj->Value);
        bNeedRangeUpdate = TRUE; /*좀 더 최적화 필요. 범위 벗어난게 있어야 어쩌구*/

        pObj->MainDataIdx = BindTargetIdx;
        pObj->Value = MainData[BindTargetIdx]; /*모션처리 X. 값만 갱신*/
    }

    /*바인딩이 일어났다면 최대최소값 갱신*/
    if (bNeedRangeUpdate) {
        for (int k = 0; k < ValidViewDataCnt; k++) {
            ViewData[k]->SetValue(TRUE, ViewData[k]->Value, IntervalMax, IntervalMin);
        }
    }


    /*뷰 행 업데이트*/
    for (int i = 0; i < ValidViewDataCnt; i++) ViewData[i]->update(time);

    ReleaseSRWLockExclusive(&lock);

    if (!bUpdated) {
        if (uiMotionState == eUIMotionState::eUMS_PlayingHide)
            uiMotionState = eUIMotionState::eUMS_Hide;

        else if (uiMotionState == eUIMotionState::eUMS_PlayingVisible)
            uiMotionState = eUIMotionState::eUMS_Visible;
    }
    return bUpdated;
}

void UI_LineChart::render()
{
    D2D_MATRIX_3X2_F OldMat;
    D2D_MATRIX_3X2_F TmpMat;
    D2D1_RECT_F ClipRect;
    unsigned long ModIdx;
    unsigned long ModScroll;
    unsigned long long MainDataCnt;

    AcquireSRWLockShared(&lock);
    pBoxBg->render(pRenderTarget);
    
    ClipRect.left = uiPos.x;
    ClipRect.top = uiPos.y;
    ClipRect.right = uiPos.x + uiPos.x2;
    ClipRect.bottom = uiPos.y + uiPos.y2;

    ModScroll = (unsigned long)CurrentScroll % ViewDataWidth;
    ModIdx = ViewStartIdx % ViewDataCnt;

    pRenderTarget->GetTransform(&OldMat); /*기존 행렬 백업*/
    pRenderTarget->PushAxisAlignedClip(ClipRect, D2D1_ANTIALIAS_MODE::D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
    TmpMat = OldMat;
    TmpMat.dx += uiPos.x;
    TmpMat.dy += uiPos.y; /*UI 위치에 맞게 행렬 조정*/
    TmpMat.dx -= (float)ModScroll; /*스크롤된 오프셋 적용 준비*/
    MainDataCnt = MainData.size();
    for (int i = 0; i < ValidViewDataCnt; i++) {
        unsigned long TmpIdx = (ModIdx + i) % ValidViewDataCnt;
        if (MainDataCnt <= ViewStartIdx + i) break; /*딱뎀시 자투리 X*/
        pRenderTarget->SetTransform(TmpMat);
        ViewData[TmpIdx]->render();
        TmpMat.dx += (float)ViewDataWidth;
    }
    pRenderTarget->PopAxisAlignedClip();
    pRenderTarget->SetTransform(OldMat);
    
    /*
    for() text 렌더링
    */

    pBoxFrame->render(pRenderTarget);
    ReleaseSRWLockShared(&lock);
}
































ChartObject::ChartObject(UI_LineChart* pParent, ID2D1StrokeStyle* Stroke, unsigned long width, unsigned long height)
{
    pChart = pParent;
    Pos = { 0.f, 0.f, (float)width, (float)height};
    MainDataIdx = -1;
    Value = 0;
    ValueMax = 0;
    ValueMin = 0;
    pStroke = Stroke;
    pGuideLine = new PropLine(pChart->pRenderTarget, pStroke);
    pBoxPoint = new PropBox(pChart->pRenderTarget);
    pCircle = new PropCircle(pChart->pRenderTarget);
    pLabel = new PropText(pChart->pRenderTarget, 32);
}

ChartObject::~ChartObject()
{

}

void ChartObject::SetValue(BOOL bMotion, float Value, float Max, float Min)
{
    // Max Min과 Value에 따라 사각형 포인트 이동
    // 원 모양의 장식은 ... X
}

BOOL ChartObject::update(unsigned long time)
{
    BOOL bUpdated;
    
    bUpdated = pGuideLine->update(time);
    bUpdated |= pBoxPoint->update(time);
    bUpdated |= pCircle->update(time);
    bUpdated |= pLabel->update(time);
    return bUpdated;
}

void ChartObject::ResumeGuideLine(BOOL bMotion, unsigned long Delay)
{
    eLineChartMotionPattern patt;

    if (bMotion) patt = pChart->Motion.MotionInitChartGuideLine;
    else patt = eLineChartMotionPattern::eInitChartGuideLine_Default;

    switch (patt) {
        case eLineChartMotionPattern::eInitChartGuideLine_Default: {
            break;
        }
    }
}

void ChartObject::PauseGuideLine(unsigned long Delay)
{
    switch (pChart->Motion.MotionPauseChartGuideLine) {
        case eLineChartMotionPattern::ePauseChartGuideLine_Default: {
            break;
        }
    }
}

void ChartObject::ResumePoint(BOOL bMotion, unsigned long Delay)
{
    eLineChartMotionPattern patt;

    if (bMotion) patt = pChart->Motion.MotionInitChartPoint;
    else patt = eLineChartMotionPattern::eInitChartPoint_Default;

    switch (patt) {
        case eLineChartMotionPattern::eInitChartPoint_Default: {
            break;
        }
    }
}

void ChartObject::PausePoint(unsigned long Delay)
{
    switch (pChart->Motion.MotionPauseChartPoint) {
        case eLineChartMotionPattern::ePauseChartPoint_Default: {
            break;
        }
    }
}

void ChartObject::ResumeLabel(BOOL bMotion, unsigned long Delay)
{
    eLineChartMotionPattern patt;

    if (bMotion) patt = pChart->Motion.MotionInitChartLabel;
    else patt = eLineChartMotionPattern::eInitChartLabel_Default;

    switch (patt) {
        case eLineChartMotionPattern::eInitChartLabel_Default: {
            break;
        }
    }
}

void ChartObject::PauseLabel(unsigned long Delay)
{
    switch (pChart->Motion.MotionPauseChartLabel) {
        case eLineChartMotionPattern::ePauseChartLabel_Default: {
            break;
        }
    }
}

void ChartObject::render()
{
    ID2D1RenderTarget* pRT = pChart->pRenderTarget;

    pGuideLine->render(pRT);
    pCircle->render(pRT);
    pBoxPoint->render(pRT);
    pLabel->render(pRT);
}

void ChartObject::pause(unsigned long Delay)
{

}

void ChartObject::resume(unsigned long Delay)
{

}








