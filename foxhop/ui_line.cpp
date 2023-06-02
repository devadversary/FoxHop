#include "./include/ui_line.hpp"
#include "./include/ui_system.hpp"

UI_Line::UI_Line(UISystem* pUISys, POSITION* pPoints, size_t nPointCnt, ID2D1StrokeStyle* pLineStyle, BOOL bClosure, UI_Line_MotionParam MotionParam)
{
    uiSys = pUISys;
    pRenderTarget = pUISys->D2DA.pRenTarget;
    Focusable = FALSE;
    MouseEventCheck = FALSE;
    uiMotionState = eUIMotionState::eUMS_Hide;

    DefaultHandler = NULL;
    MessageHandler = NULL;
    Motion = MotionParam;
    Closure = bClosure;
    PointCnt = nPointCnt;
    Stroke = pLineStyle;
    InitializeSRWLock(&lock);

    if (nPointCnt < 3) Closure = FALSE; /*폐도형은 정점이 3개 이상일때만.*/
    for (int i = 0; i < PointCnt; i++) Points.emplace_back(pPoints[i]);
    /*폐도형 모드일땐 (다각형) 선 갯수와 노드 갯수는 같음*/
    for (int i = 1; i < PointCnt; i++) {
        pLines.emplace_back(new PropLine(pRenderTarget, Stroke));
        pNodes.emplace_back(new PropBox(pRenderTarget));
    }
    pNodes.emplace_back(new PropBox(pRenderTarget));
    if (Closure) pLines.emplace_back(new PropLine(pRenderTarget, Stroke));

}

UI_Line::~UI_Line()
{

}

void UI_Line::ResumeLine(unsigned long PointIdx,  unsigned long Delay)
{
    PropLine* pLine;
    POSITION StartPos, EndPos;
    MOTION_INFO mi;

    if (PointIdx == (PointCnt - 1)) {
        if(!Closure) return;
        EndPos.x = Points[PointIdx].x;
        EndPos.y = Points[PointIdx].y;
        EndPos.x2 = Points[0].x;
        EndPos.y2 = Points[0].y;
    }
    else {
        EndPos.x = Points[PointIdx].x;
        EndPos.y = Points[PointIdx].y;
        EndPos.x2 = Points[PointIdx + 1].x;
        EndPos.y2 = Points[PointIdx + 1].y;
    }
    pLine = pLines[PointIdx];
    AcquireSRWLockExclusive(&lock);
    switch (Motion.MotionInitLine){
        case eLineMotionPattern::eInitLine_Default: {
            pLine->Init(EndPos, ALL_ZERO);
            mi = InitMotionInfo(eMotionForm::eMotion_None, Delay, 0);
            pLine->SetColor(mi, TRUE, ALL_ZERO, Motion.ColorLine);
            break;
        }

        case eLineMotionPattern::eInitLine_Flick: {
            pLine->Init(EndPos, ALL_ZERO);
            mi = InitMotionInfo(eMotionForm::eMotion_Pulse1, Delay, Motion.PitchInitLine);
            pLine->SetColor(mi, TRUE, ALL_ZERO, Motion.ColorLine);
            break;
        }

        case eLineMotionPattern::eInitLine_Connect: {
            StartPos = EndPos;
            StartPos.x2 = EndPos.x;
            StartPos.y2 = EndPos.y;
            pLine->Init(StartPos, ALL_ZERO);
            mi = InitMotionInfo(eMotionForm::eMotion_None, Delay, 0);
            pLine->SetColor(mi, TRUE, ALL_ZERO, Motion.ColorLine);
            mi = InitMotionInfo(eMotionForm::eMotion_Linear1, Delay, Motion.PitchInitLine);
            pLine->SetPos(mi, TRUE, ALL_ZERO, EndPos);
            break;
        }
    }
    ReleaseSRWLockExclusive(&lock);
}

void UI_Line::PauseLine(unsigned long PointIdx, unsigned long Delay)
{
    PropLine* pLine;
    POSITION StartPos, EndPos;
    MOTION_INFO mi;

    if (PointIdx == (PointCnt - 1)) {
        if (!Closure) return;
        StartPos.x = Points[PointIdx].x;
        StartPos.y = Points[PointIdx].y;
        StartPos.x2 = Points[0].x;
        StartPos.y2 = Points[0].y;
    }
    else {
        StartPos.x = Points[PointIdx].x;
        StartPos.y = Points[PointIdx].y;
        StartPos.x2 = Points[PointIdx + 1].x;
        StartPos.y2 = Points[PointIdx + 1].y;
    }
    pLine = pLines[PointIdx];
    AcquireSRWLockExclusive(&lock);
    switch (Motion.MotionPauseLine) {
        case eLineMotionPattern::ePauseLine_Default: {
            mi = InitMotionInfo(eMotionForm::eMotion_None, Delay, 0);
            pLine->SetColor(mi, TRUE, ALL_ZERO, ALL_ZERO);
            break;
        }

        case eLineMotionPattern::ePauseLine_Flick: {
            mi = InitMotionInfo(eMotionForm::eMotion_Pulse1, Delay, Motion.PitchPauseLine);
            pLine->SetColor(mi, TRUE, ALL_ZERO, ALL_ZERO);
            break;
        }

        case eLineMotionPattern::ePauseLine_Disconnect: {
            EndPos = StartPos;
            EndPos.x = StartPos.x2;
            EndPos.y = StartPos.y2;
            mi = InitMotionInfo(eMotionForm::eMotion_Linear1, Delay, Motion.PitchPauseLine);
            pLine->SetPos(mi, TRUE, ALL_ZERO, EndPos);
            mi = InitMotionInfo(eMotionForm::eMotion_None, Delay + Motion.PitchPauseLine, 0);
            pLine->SetColor(mi, TRUE, ALL_ZERO, ALL_ZERO);
            break;
        }
    }
    ReleaseSRWLockExclusive(&lock);
}

void UI_Line::ResumeNode(unsigned long PointIdx, unsigned long Delay)
{
    POSITION StartPos, EndPos;
    MOTION_INFO mi;
    PropBox* pNode;

    StartPos.x = Points[PointIdx].x - ((float)Motion.PointSize/2);
    StartPos.y = Points[PointIdx].y - ((float)Motion.PointSize / 2);
    StartPos.x2 = Motion.PointSize;
    StartPos.y2 = Motion.PointSize;
    pNode = pNodes[PointIdx];

    AcquireSRWLockExclusive(&lock);
    switch (Motion.MotionInitNode) {
        case eLineMotionPattern::eInitNode_Default: {
            pNode->Init(StartPos, ALL_ZERO);
            mi = InitMotionInfo(eMotionForm::eMotion_None, Delay, 0);
            pNode->SetColor(mi, TRUE, ALL_ZERO, Motion.ColorNode);
            break;
        }
        case eLineMotionPattern::eInitNode_Flick: {
            pNode->Init(StartPos, ALL_ZERO);
            mi = InitMotionInfo(eMotionForm::eMotion_Pulse1, Delay, Motion.PitchInitNode);
            pNode->SetColor(mi, TRUE, ALL_ZERO, Motion.ColorNode);
            break;
        }
    }
    ReleaseSRWLockExclusive(&lock);
}

void UI_Line::PauseNode(unsigned long PointIdx, unsigned long Delay)
{
    MOTION_INFO mi;
    PropBox* pNode;

    pNode = pNodes[PointIdx];
    AcquireSRWLockExclusive(&lock);
    switch (Motion.MotionPauseNode) {
        case eLineMotionPattern::ePauseNode_Default: {
            mi = InitMotionInfo(eMotionForm::eMotion_None, Delay, 0);
            pNode->SetColor(mi, TRUE, ALL_ZERO, ALL_ZERO);
            break;
        }
        case eLineMotionPattern::ePauseNode_Flick: {
            mi = InitMotionInfo(eMotionForm::eMotion_Pulse1, Delay, Motion.PitchPauseNode);
            pNode->SetColor(mi, TRUE, ALL_ZERO, ALL_ZERO);
            break;
        }
    }
    ReleaseSRWLockExclusive(&lock);
}

void UI_Line::ResumeLineOrder(unsigned long Delay)
{
    //AcquireSRWLockExclusive(&lock);
    switch (Motion.MotionInitLineOrder) {
        case eLineMotionPattern::eInitLineOrder_Default: {
            for (int i = 0; i < PointCnt; i++) ResumeLine(i, Delay);
            break;
        }

        case eLineMotionPattern::eInitLineOrder_Linear: {
            for (int i = 0; i < PointCnt; i++) ResumeLine(i, Delay + (i * Motion.GapInitLineOrder));
            break;
        }

        case eLineMotionPattern::eInitLineOrder_Rand: {
            for (int i = 0; i < PointCnt; i++) ResumeLine(i, Delay + (i * Motion.GapInitLineOrder) + (rand() % Motion.RandNoiseInitLineOrder));
            break;
        }
    }
    //ReleaseSRWLockExclusive(&lock);
}

void UI_Line::PauseLineOrder(unsigned long Delay)
{
    //AcquireSRWLockExclusive(&lock);
    switch (Motion.MotionPauseLineOrder) {
        case eLineMotionPattern::ePauseLineOrder_Default: {
            for (int i = 0; i < PointCnt; i++) PauseLine(i, Delay);
            break;
        }

        case eLineMotionPattern::ePauseLineOrder_Linear: {
            for (int i = 0; i < PointCnt; i++) PauseLine(i, Delay + (i * Motion.GapPauseLineOrder));
            break;
        }

        case eLineMotionPattern::ePauseLineOrder_Rand: {
            for (int i = 0; i < PointCnt; i++) PauseLine(i, Delay + (i * Motion.GapPauseLineOrder) + (rand() % Motion.RandNoisePauseLineOrder));
            break;
        }
    }
    //ReleaseSRWLockExclusive(&lock);
}

void UI_Line::ResumeNodeOrder(unsigned long Delay)
{
    //AcquireSRWLockExclusive(&lock);
    switch (Motion.MotionInitNodeOrder) {
        case eLineMotionPattern::eInitNodeOrder_Default: {
            for (int i = 0; i < PointCnt; i++) ResumeNode(i, Delay);
            break;
        }

        case eLineMotionPattern::eInitNodeOrder_Linear: {
            for (int i = 0; i < PointCnt; i++) ResumeNode(i, Delay + (i * Motion.GapInitNodeOrder));
            break;
        }

        case eLineMotionPattern::eInitNodeOrder_Rand: {
            for (int i = 0; i < PointCnt; i++) ResumeNode(i, Delay + (i * Motion.GapInitNodeOrder) + (rand() % Motion.RandNoiseInitNodeOrder));
            break;
        }
    }
    //ReleaseSRWLockExclusive(&lock);
}

void UI_Line::PauseNodeOrder(unsigned long Delay)
{
    //AcquireSRWLockExclusive(&lock);
    switch (Motion.MotionPauseNodeOrder) {
        case eLineMotionPattern::ePauseNodeOrder_Default: {
            for (int i = 0; i < PointCnt; i++) PauseNode(i, Delay);
            break;
        }

        case eLineMotionPattern::ePauseNodeOrder_Linear: {
            for (int i = 0; i < PointCnt; i++) PauseNode(i, Delay + (i * Motion.GapPauseNodeOrder));
            break;
        }

        case eLineMotionPattern::ePauseNodeOrder_Rand: {
            for (int i = 0; i < PointCnt; i++) PauseNode(i, Delay + (i * Motion.GapPauseNodeOrder) + (rand() % Motion.RandNoisePauseNodeOrder));
            break;
        }
    }
    //ReleaseSRWLockExclusive(&lock);
}

void UI_Line::resume(int nDelay)
{
    uiMotionState = eUIMotionState::eUMS_PlayingVisible;
    ResumeLineOrder(nDelay + Motion.DelayInitLine);
    ResumeNodeOrder(nDelay + Motion.DelayInitNode);
}

void UI_Line::pause(int nDelay)
{
    uiMotionState = eUIMotionState::eUMS_PlayingHide;
    PauseLineOrder(nDelay + Motion.DelayPauseLine);
    PauseNodeOrder(nDelay + Motion.DelayPauseNode);
}

BOOL UI_Line::update(unsigned long time)
{
    BOOL bUpdated = FALSE;
    size_t LineLen, NodeLen;

    if (uiMotionState == eUIMotionState::eUMS_Hide) return FALSE;

    AcquireSRWLockExclusive(&lock);
    LineLen = pLines.size();
    NodeLen = pNodes.size();

    for (int i = 0; i < LineLen; i++) {
        bUpdated |= pLines[i]->update(time);
    }
    for (int i = 0; i < NodeLen; i++) {
        bUpdated |= pNodes[i]->update(time);
    }
    ReleaseSRWLockExclusive(&lock);

    if (!bUpdated) {
        if (uiMotionState == eUIMotionState::eUMS_PlayingHide)
            uiMotionState = eUIMotionState::eUMS_Hide;

        else if (uiMotionState == eUIMotionState::eUMS_PlayingVisible)
            uiMotionState = eUIMotionState::eUMS_Visible;
        return FALSE;
    }
    return TRUE;
}

void UI_Line::render()
{
    size_t LineLen, NodeLen;

    if (uiMotionState == eUIMotionState::eUMS_Hide) return;

    AcquireSRWLockShared(&lock);
    LineLen = pLines.size();
    NodeLen = pNodes.size();
    for (int i = 0; i < LineLen; i++) {
        pLines[i]->render(pRenderTarget);
    }
    for (int i = 0; i < NodeLen; i++) {
        pNodes[i]->render(pRenderTarget);
    }
    ReleaseSRWLockShared(&lock);
}
