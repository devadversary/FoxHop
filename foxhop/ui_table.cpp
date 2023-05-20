#include "./include/ui_table.hpp"
#include "./include/ui_system.hpp"
#include <cstdarg>

#define TEXTSHEET_DEFAULT_WIDTH     150 /*�� ������ �⺻ �ȼ�*/
#define TEXTSHEET_DEFAULT_ROWHEIGHT 20  /*�� ������ �⺻ �ȼ�*/

UI_Table::UI_Table(UISystem* pUISys, pfnUIHandler pfnCallback, POSITION Pos, unsigned int ColumnCount, wchar_t** ColumnNameList, unsigned int* ColumnWidth, unsigned int HeaderHeight, unsigned int RowHeight, BOOL MultiSelect, UI_Table_MotionParam MotionParam)
{
    /*�⺻ UI ��� ����*/
    uiSys = pUISys;
    pRenderTarget = pUISys->D2DA.pRenTarget;
    Focusable = TRUE;
    DefaultHandler = DefaultTableProc;
    MessageHandler = pfnCallback;
    uiPos = Pos;
    uiMotionState = eUIMotionState::eUMS_PlayingVisible;

    /*���� Ŭ���� ��� ����*/
    MultiSelectMode = MultiSelect;
    CurrScrollPixel = 0;
    ScrollPixel = 0;
    MaxScrollPixel = 0;
    DataCount = 0;
    CurrMainIndex = 0;
    PrevSelMainIdx = -1;
    HeaderHgt = HeaderHeight;
    RowHgt = RowHeight ? RowHeight : TEXTSHEET_DEFAULT_ROWHEIGHT;
    ScrollComp = new ComponentMotion;
    Motion = MotionParam;
    PinCount = 0;

    /* �� ���� ���� */
    if (!ColumnCount) ColumnCount = 1; /*�ּ� 1���� ���� �ʿ���*/
    ColCnt = ColumnCount;
    ColName = (wchar_t**)malloc(sizeof(wchar_t*) * ColCnt);
    ColWidth = (int*)malloc(sizeof(int) * ColCnt);
    ppTextHdr = (PropText**)malloc(sizeof(PropText*) * ColCnt);
    if (!ColName || !ColWidth) return; /*TODO : ���� ó�� �ʿ�*/

    for (int i = 0; i < ColCnt; i++) {
        ColName[i] = (wchar_t*)_wcsdup(ColumnNameList[i]); /*�� ��� ����*/
        if (!ColumnWidth) ColWidth[i] = TEXTSHEET_DEFAULT_WIDTH;
        else ColWidth[i] = ColumnWidth[i];
    }

    InitializeSRWLock(&lock);

    /*�� ����*/
    ClientHeight = (int)Pos.y2 - HeaderHgt;
    ViewRowCnt = (ClientHeight / RowHgt) + 2; /*������ ������ ������ 1�� ���*/
    for (int i = 0; i < ViewRowCnt; i++) {
        RowObject* obj = new RowObject(pUISys, this, { 0,0,Pos.x2,(float)RowHgt }, ColCnt);
        ViewData.push_back(obj);
    }

    pBoxHeader = new PropBox(pRenderTarget);
    for (int i = 0; i < ColCnt; i++) ppTextHdr[i] = new PropText(pRenderTarget, 512);
    pBoxFrame = new PropBox(pRenderTarget);

    resume(0);
    DefaultHandler(this, UIM_CREATE, NULL, NULL); /*UI���� �޼��� ����*/
}

/**
    @brief ���� ���� ������ üũ
    @param x ���� ��
    @param dx �������κ����� ���� (x�� 3�̰� dx�� 2�̸�, 3~5 ����)
    @param test �׽�Ʈ�� ��
*/
static BOOL IsInRange(long long x, long long dx, long long test)
{
    if (test < x) return FALSE;
    if (test > x+dx) return FALSE;
    return TRUE;
}

/**
    @brief �⺻ �ؽ�Ʈ ����Ʈ�� �޼��� �ڵ鷯
    @remark ����� ���� ���ν����� �⺻ �ڵ鷯 ���� �� ȣ��ȴ�.
*/
void UI_Table::DefaultTableProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam)
{
    UI_Table* pTable = static_cast<UI_Table*>(pUI);
    pfnUIHandler UserHandler = pUI->MessageHandler;
    if (!pUI) return;

    switch (Message) {
    case UIM_MOUSEON: /*ON / LEAVE �޼����� UI_Panel �� ���� SendMessage �� ���޵ȴ� */
        break;

    case UIM_MOUSELEAVE:
        break;

    case WM_LBUTTONDOWN:
        break;

    case WM_MOUSEMOVE:
        pTable->MousePt.x = GET_X_LPARAM(lParam);
        pTable->MousePt.y = GET_Y_LPARAM(lParam);
        break;

    case WM_LBUTTONUP:
    {
        POINT pt;
        int y, idx, OrderIdx, MainIdx;
        BOOL bSel;
        size_t MainDataPoolSize;
        RowObject* pViewRow;

        pt.x = GET_X_LPARAM(lParam);
        pt.y = GET_Y_LPARAM(lParam);
        y = pTable->MousePt.y - pTable->HeaderHgt;
        if (y < 0) break; /*������� Ŭ����*/
        MainDataPoolSize = pTable->MainDataPool.size();
        if (MainDataPoolSize == 0) break;
        idx = ((long long)pTable->CurrScrollPixel + y) / pTable->RowHgt;
        if (idx >= MainDataPoolSize) break;
        OrderIdx = idx % pTable->ViewRowCnt; /*������ �ε����κ��� ȭ�鿡 ���̴� �� �ε��� ���*/
        MainIdx = pTable->ViewData[OrderIdx]->MainDataIdx; /*���õ� ��*/
        pTable->MainDataPool[MainIdx].bSelected ^= 1; /*TRUE / FALSE ���� : ���� ǥ�� �Ϸ�*/
        bSel = pTable->MainDataPool[MainIdx].bSelected;

        pViewRow = pTable->ViewData[OrderIdx];
        pViewRow->OnSelectEvent();
        if (bSel) {
            if (pTable->MessageHandler)
                pTable->MessageHandler(pTable, UIM_TABLE_SELECT, (WPARAM)pTable->MainDataPool[MainIdx].ppData, MainIdx);/*� ������ ���� �Ǿ������� ���� ���� ����*/

            if (!pTable->MultiSelectMode) {
                /*���� �̹� ���õȰ��� ������.*/
                if (pTable->PrevSelMainIdx >= 0) {
                    pTable->MainDataPool[pTable->PrevSelMainIdx].bSelected = FALSE;
                    /*���� ����� �� ���� �ȿ� �������� ��� ����� ���־�� ��*/
                    if (IsInRange(pTable->CurrMainIndex, pTable->ViewRowCnt, pTable->PrevSelMainIdx)) {
                        long long PrevViewRowIdx = pTable->PrevSelMainIdx % pTable->ViewRowCnt;
                        RowObject* pPrevViewRow = pTable->ViewData[PrevViewRowIdx];
                        pPrevViewRow->OnSelectEvent();
                    }
                }
                pTable->PrevSelMainIdx = MainIdx;
            }
        }
        else {
            if (pTable->MessageHandler)
                pTable->MessageHandler(pTable, UIM_TABLE_UNSELECT, NULL, MainIdx);/*� ������ �������� �Ǿ������� ���� ���� ����*/
            pTable->PrevSelMainIdx = -1; /*���� ������ ���� ���� �ε��� ��ȿȭ. (���߼����� ������� �ʾƵ� �ȴ�)*/
        }

        break;
    }

    case WM_MOUSEWHEEL:
    {
        long long ScrollPx = pTable->ScrollPixel - (GET_WHEEL_DELTA_WPARAM(wParam) / 2);
        pTable->SetScroll(ScrollPx);
        break;
    }

    default: break;
    }

    if (UserHandler) UserHandler(pUI, Message, wParam, lParam);
}

long long UI_Table::DataIdx2ViewRowIdx(long long DataIdx)
{
    if (DataCount <= DataIdx) return -1;
    return DataIdx % ViewRowCnt;
}

BOOL UI_Table::DataIdxIsInScreen(long long DataIdx)
{
    if (DataCount <= DataIdx) return FALSE;
    if (CurrMainIndex > DataIdx) return FALSE;
    if (CurrMainIndex + ViewRowCnt < DataIdx) return FALSE;
    return TRUE;
}


/**
    @brief ���̺� �����͸� �߰��Ѵ�.
    @param bMotion �ִϸ��̼� ���� ����
    @param bAutoScroll �� �����Ͱ� �߰� �� ��, �ش� �������� �ڵ� ��ũ�� �ȴ�.
*/
void UI_Table::AddData(BOOL bMotion, BOOL bAutoScroll, wchar_t* ...)
{
    va_list args;
    wchar_t* pStr;


    long long TmpScrollPx;
    TABLE_ROW Row;
    size_t    AllocSize;

    Row.bSelected = FALSE;
    AllocSize = sizeof(wchar_t*) * ColCnt;
    Row.ppData = (wchar_t**)malloc(AllocSize);

    va_start(args, bAutoScroll);
    for (int i = 0; i < ColCnt; i++) {
        pStr = va_arg(args, wchar_t*);
        Row.ppData[i] = pStr;
    }
    va_end(args);

    AcquireSRWLockExclusive(&lock);
    Row.bTextMotionPlayed = bMotion ? FALSE : TRUE;
    MainDataPool.emplace_back(Row);
    DataCount++;

    TmpScrollPx = (DataCount * RowHgt) - ClientHeight;
    if (TmpScrollPx <= 0) MaxScrollPixel = 0; /*������ ����.*/
    else MaxScrollPixel = TmpScrollPx;
    //PinCount = DataCount < ViewRowCnt ? DataCount : ViewRowCnt;
    ReleaseSRWLockExclusive(&lock);

    if (uiMotionState != eUIMotionState::eUMS_Visible) return; /*�ʱ�ȭ���/�Ҹ��� �����߿� ��ũ�� X*/
    if (bAutoScroll) SetScroll(MaxScrollPixel);
}

void UI_Table::EditData(BOOL bMotion, unsigned long long RowIdx, wchar_t* ...)
{
    RowObject* pViewRow;
    va_list arglist;
    wchar_t* pStr;
    RowObject* pRow;
    long long vidx;

    if (RowIdx >= DataCount) return;
    vidx = DataIdx2ViewRowIdx(RowIdx);
    if (vidx < 0) return;
    AcquireSRWLockExclusive(&lock);

    va_start(arglist, RowIdx);
    for (int i = 0; i < ColCnt; i++) {
        pStr = va_arg(arglist, wchar_t*);
        MainDataPool[RowIdx].ppData[i] = pStr;
    }
    va_end(arglist);

    if (DataIdxIsInScreen(RowIdx)) {
        pRow = ViewData[vidx];
        pRow->SetText(MainDataPool[RowIdx].ppData, ColCnt);
    }
    ReleaseSRWLockExclusive(&lock);
}

void UI_Table::HighlightData(unsigned long long DataIdx, D2D1_COLOR_F HightlightColor)
{
    RowObject* pRow;
    long long Idx;

    if (!DataIdxIsInScreen(DataIdx)) return;
    Idx = DataIdx2ViewRowIdx(DataIdx);
    if (Idx < 0) return;

    AcquireSRWLockExclusive(&lock);
    ViewData[Idx]->SetHighlight(HightlightColor);
    ReleaseSRWLockExclusive(&lock);
}

/** 
    @brief ���� �ȼ������� ��ũ���� �ű��
    @remark ��� ��ũ�Ѱ��� ������ ���ο��� �˾Ƽ� ���� ������ �����Ѵ�.
*/
void UI_Table::SetScroll(long long TargetScrollPx)
{
    MOTION_PATTERN patt;
    MOTION_INFO    mi;

    /*�ʱ�ȭ / �Ҹ� ��� �����߿� ��ũ�� X*/
    if (uiMotionState != eUIMotionState::eUMS_Visible) return;
    
    AcquireSRWLockExclusive(&lock);
    /*��ũ�� ��� ����*/
    if (TargetScrollPx < 0)
        TargetScrollPx = 0;
    if (TargetScrollPx > MaxScrollPixel)
        TargetScrollPx = MaxScrollPixel;
    ScrollPixel = TargetScrollPx;

    /*��ũ�� ��� ���� �����*/
    ScrollComp->clearChannel();
    mi = InitMotionInfo(eMotionForm::eMotion_x3_2, 0, Motion.PitchScroll);
    patt = InitMotionPattern(mi, NULL);
    AddChain(&patt, &CurrScrollPixel, CurrScrollPixel, (float)TargetScrollPx);
    ScrollComp->addChannel(patt);

    ReleaseSRWLockExclusive(&lock);
};

void UI_Table::ResumeFrame(unsigned long Delay)
{
    MOTION_INFO mi;
    D2D1_COLOR_F StartColor, EndColor;
    AcquireSRWLockExclusive(&lock);
    switch (Motion.MotionInitTableFrame) {
        case eTableMotionPattern::eInitTableFrame_Default: {
            pBoxFrame->Init(uiPos, ALL_ZERO, FALSE);
            mi = InitMotionInfo(eMotionForm::eMotion_None, Delay, Motion.PitchInitTableFrame);
            pBoxFrame->SetColor(mi, TRUE, ALL_ZERO, Motion.ColorFrame);
            break;
        }

        case eTableMotionPattern::eInitTableFrame_Flick: {
            pBoxFrame->Init(uiPos, ALL_ZERO, FALSE);
            mi = InitMotionInfo(eMotionForm::eMotion_Pulse1, Delay, Motion.PitchInitTableFrame);
            pBoxFrame->SetColor(mi, TRUE, ALL_ZERO, Motion.ColorFrame);
            break;
        }

        case eTableMotionPattern::eInitTableFrame_ExpendAllDirFlick: {
            unsigned long Pitch = Motion.PitchInitTableFrame;
            POSITION StartPos;

            StartPos = { uiPos.x + uiPos.x2 / 2 , uiPos.y + uiPos.y2 / 2 , 0, 0 };
            pBoxFrame->Init(StartPos, Motion.ColorFrame, FALSE);
            mi = InitMotionInfo(eMotionForm::eMotion_x3_2, Delay, Pitch);
            pBoxFrame->SetPos(mi, TRUE, ALL_ZERO, uiPos);
            mi = InitMotionInfo(eMotionForm::eMotion_Pulse1, Delay + Pitch/2, Pitch/2);
            pBoxFrame->SetColor(mi, FALSE, ALL_ZERO, Motion.ColorFrame);
            break;
        }
    }
    ReleaseSRWLockExclusive(&lock);
}

void UI_Table::PauseFrame(unsigned long Delay)
{
    MOTION_INFO mi;
    D2D1_COLOR_F StartColor, EndColor;
    
    AcquireSRWLockExclusive(&lock);
    switch (Motion.MotionPauseTableFrame) {
        case eTableMotionPattern::ePauseTableFrame_Default: {
            mi = InitMotionInfo(eMotionForm::eMotion_None, Delay, Motion.PitchPauseTableFrame);
            pBoxFrame->SetColor(mi, TRUE, ALL_ZERO, ALL_ZERO);
            break;
        }

        case eTableMotionPattern::ePauseTableFrame_Flick: {
            mi = InitMotionInfo(eMotionForm::eMotion_Pulse1, Delay, Motion.PitchPauseTableFrame);
            pBoxFrame->SetColor(mi, TRUE, ALL_ZERO, ALL_ZERO);
            break;
        }

        case eTableMotionPattern::ePauseTableFrame_ExpendAllDirFlick: {
            unsigned long Pitch = Motion.PitchPauseTableFrame;
            POSITION EndPos;

            EndPos = { uiPos.x - 50 , uiPos.y - 50 , uiPos.x2 + 100, uiPos.y2 + 100 };
            mi = InitMotionInfo(eMotionForm::eMotion_Linear1, Delay, Pitch);
            pBoxFrame->SetPos(mi, TRUE, ALL_ZERO, EndPos);
            mi = InitMotionInfo(eMotionForm::eMotion_Pulse1, Delay+ Pitch / 2, Pitch/2);
            pBoxFrame->SetColor(mi, TRUE, ALL_ZERO, ALL_ZERO);
            break;
        }
    }
    ReleaseSRWLockExclusive(&lock);
}

void UI_Table::ResumeBg(unsigned long Delay)
{
    MOTION_INFO mi;

#if 0
    AcquireSRWLockExclusive(&lock);
    switch (Motion.MotionInitTableBg) {
        case eTableMotionPattern::eInitTableBg_Default: {
            pBoxFrame->Init(uiPos, ALL_ZERO);
            mi = InitMotionInfo(eMotionForm::eMotion_None, Delay, Motion.PitchInitTableBg);
            pBoxFrame->SetColor(mi, TRUE, ALL_ZERO, Motion.ColorBg);
            break;
        }
    }
    ReleaseSRWLockExclusive(&lock);
#endif
}

void UI_Table::PauseBg(unsigned long Delay)
{
    MOTION_INFO mi;
#if 0
    AcquireSRWLockExclusive(&lock);
    switch (Motion.MotionPauseTableBg) {
        case eTableMotionPattern::ePauseTableBg_Default: {
            mi = InitMotionInfo(eMotionForm::eMotion_None, Delay, Motion.PitchPauseTableBg);
            pBoxFrame->SetColor(mi, TRUE, ALL_ZERO, ALL_ZERO);
            break;
        }
    }
    ReleaseSRWLockExclusive(&lock);
#endif
}

void UI_Table::ResumeHeaderBg(unsigned long Delay)
{
    MOTION_INFO mi;

    AcquireSRWLockExclusive(&lock);
    switch (Motion.MotionInitTableHeaderBg) {
        case eTableMotionPattern::eInitTableHeaderBg_Default: {
            POSITION TmpPos = uiPos;

            TmpPos.x2 = uiPos.x2;
            TmpPos.y2 = HeaderHgt;
            pBoxHeader->Init(TmpPos, ALL_ZERO);
            mi = InitMotionInfo(eMotionForm::eMotion_None, Delay, Motion.PitchInitTableHeaderBg);
            pBoxHeader->SetColor(mi, TRUE, ALL_ZERO, Motion.ColorHeaderBg);
            break;
        }

        case eTableMotionPattern::eInitTableHeaderBg_Linear: {
            POSITION StartPos = uiPos;
            POSITION EndPos = uiPos;

            StartPos.y2 = HeaderHgt;
            StartPos.x2 = 0;

            EndPos.y2 = HeaderHgt;
            pBoxHeader->Init(StartPos, Motion.ColorHeaderBg);
            mi = InitMotionInfo(eMotionForm::eMotion_Linear1, Delay, Motion.PitchInitTableHeaderBg);
            pBoxHeader->SetPos(mi, TRUE, ALL_ZERO, EndPos);
            break;
        }

        case eTableMotionPattern::eInitTableHeaderBg_LinearReverse: {
            POSITION StartPos = uiPos;
            POSITION EndPos = uiPos;

            StartPos.y2 = HeaderHgt;
            StartPos.x = uiPos.x+uiPos.x2;
            StartPos.x2 = 0;

            EndPos.y2 = HeaderHgt;

            pBoxHeader->Init(StartPos, Motion.ColorHeaderBg);
            mi = InitMotionInfo(eMotionForm::eMotion_Linear1, Delay, Motion.PitchInitTableHeaderBg);
            pBoxHeader->SetPos(mi, TRUE, ALL_ZERO, EndPos);
            break;
        }
    }
    ReleaseSRWLockExclusive(&lock);
}

void UI_Table::PauseHeaderBg(unsigned long Delay)
{
    MOTION_INFO mi;

    AcquireSRWLockExclusive(&lock);
    switch (Motion.MotionPauseTableHeaderBg) {
        case eTableMotionPattern::ePauseTableHeaderBg_Default: {
            mi = InitMotionInfo(eMotionForm::eMotion_None, Delay, Motion.PitchPauseTableHeaderBg);
            pBoxHeader->SetColor(mi, TRUE, ALL_ZERO, ALL_ZERO);
            break;
        }

        case eTableMotionPattern::ePauseTableHeaderBg_Linear: {
            POSITION StartPos = uiPos;
            POSITION EndPos = uiPos;

            EndPos.y2 = HeaderHgt;
            EndPos.x2 = 0;
            StartPos.y2 = HeaderHgt;
            pBoxHeader->Init(StartPos, Motion.ColorHeaderBg);
            mi = InitMotionInfo(eMotionForm::eMotion_Linear1, Delay, Motion.PitchInitTableHeaderBg);
            pBoxHeader->SetPos(mi, TRUE, ALL_ZERO, EndPos);
            break;
        }

        case eTableMotionPattern::ePauseTableHeaderBg_LinearReverse: {
            POSITION StartPos = uiPos;
            POSITION EndPos = uiPos;

            EndPos.y2 = HeaderHgt;
            EndPos.x = uiPos.x + uiPos.x2;
            EndPos.x2 = 0;

            StartPos.y2 = HeaderHgt;

            pBoxHeader->Init(StartPos, Motion.ColorHeaderBg);
            mi = InitMotionInfo(eMotionForm::eMotion_Linear1, Delay, Motion.PitchInitTableHeaderBg);
            pBoxHeader->SetPos(mi, TRUE, ALL_ZERO, EndPos);
            break;
        }
    }
    ReleaseSRWLockExclusive(&lock);
}

void UI_Table::ResumeHeaderText(unsigned long Delay)
{
    unsigned long TextLen;

    AcquireSRWLockExclusive(&lock);
    switch (Motion.MotionInitTableHeaderText) {
        MOTION_INFO mi;

        case eTableMotionPattern::eInitTableHeaderText_Default:{
            POSITION TmpPos = uiPos;
            int      WidthOffset = 0;

            TmpPos.y2 = HeaderHgt;
            for (int i = 0; i < ColCnt; i++) {
                TmpPos.x = uiPos.x + WidthOffset;
                TmpPos.x2 = ColWidth[i];
                WidthOffset += ColWidth[i];
                ppTextHdr[i]->Init(uiSys->MediumTextForm, ColName[i], 0, TmpPos, ALL_ZERO, wcslen(ColName[i]));
                mi = InitMotionInfo(eMotionForm::eMotion_None, Delay, Motion.PitchInitTableHeaderText);
                ppTextHdr[i]->SetColor(mi, TRUE, ALL_ZERO, Motion.ColorHeaderText);
            }
            break;
        }

        case eTableMotionPattern::eInitTableHeaderText_SlideIn:{
            POSITION StartPos;
            POSITION EndPos = uiPos;
            int      WidthOffset = 0;
            unsigned long DelayOffset;

            EndPos.y2 = HeaderHgt;
            for (int i = 0; i < ColCnt; i++) {
                EndPos.x = uiPos.x + WidthOffset;
                EndPos.x2 = ColWidth[i];
                WidthOffset += ColWidth[i];
                TextLen = wcslen(ColName[i]);
                StartPos = EndPos;
                StartPos.x += 30;
                DelayOffset = Delay + Motion.GapInitTableHeaderText*i;

                ppTextHdr[i]->Init(uiSys->MediumTextForm, ColName[i], TextLen, StartPos, ALL_ZERO, TextLen);
                mi = InitMotionInfo(eMotionForm::eMotion_Linear1, DelayOffset, Motion.PitchInitTableHeaderText);
                ppTextHdr[i]->SetColor(mi, TRUE, ALL_ZERO, Motion.ColorHeaderText);
                ppTextHdr[i]->SetPos(mi, TRUE, ALL_ZERO, EndPos);
            }
            break;
        }
    }
    ReleaseSRWLockExclusive(&lock);
}

void UI_Table::PauseHeaderText(unsigned long Delay)
{
    MOTION_INFO mi;

    AcquireSRWLockExclusive(&lock);
    switch (Motion.MotionPauseTableHeaderText) {
        case eTableMotionPattern::ePauseTableHeaderText_Default:{
            POSITION TmpPos = uiPos;
            int      WidthOffset = 0;

            TmpPos.y2 = HeaderHgt;
            for (int i = 0; i < ColCnt; i++) {
                TmpPos.x = uiPos.x + WidthOffset;
                TmpPos.x2 = ColWidth[i];
                WidthOffset += ColWidth[i];
                mi = InitMotionInfo(eMotionForm::eMotion_None, Delay, Motion.PitchPauseTableHeaderText);
                ppTextHdr[i]->SetColor(mi, TRUE, ALL_ZERO, ALL_ZERO);
            }
            break;
        }

        case eTableMotionPattern::ePauseTableHeaderText_SlideOut:{
            POSITION EndPos = uiPos;
            int      WidthOffset = 0;
            unsigned long DelayOffset;

            EndPos.y2 = HeaderHgt;
            for (int i = 0; i < ColCnt; i++) {
                EndPos.x = uiPos.x + WidthOffset;
                EndPos.x2 = ColWidth[i];
                WidthOffset += ColWidth[i];
                EndPos.x += 30;
                DelayOffset = Delay + Motion.GapPauseTableHeaderText * i;
                mi = InitMotionInfo(eMotionForm::eMotion_Linear1, DelayOffset, Motion.PitchPauseTableHeaderText);
                ppTextHdr[i]->SetColor(mi, TRUE, ALL_ZERO, ALL_ZERO);
                ppTextHdr[i]->SetPos(mi, TRUE, ALL_ZERO, EndPos);
            }
            break;
        }
    }
    ReleaseSRWLockExclusive(&lock);
}

void UI_Table::ResumeRowOrder(unsigned long Delay)
{
    MOTION_INFO mi;
    //long long ValidViewRowCnt = DataCount < ViewRowCnt ? DataCount : ViewRowCnt;

    /*UI_Table�� ��� ������ ���� ViewData Delay�� ������ �������ش�.
      ���� ������Ʈ�� ViewData �� ���������� �˾Ƽ� �����Ѵ�.*/
    AcquireSRWLockExclusive(&lock);
    switch (Motion.MotionInitTableRowOrder) {
        case eTableMotionPattern::eInitTableRowOrder_Default:{
            for (int i = 0; i < PinCount; i++)
                ViewData[i]->resume(Delay);
            break;
        }
        case eTableMotionPattern::eInitTableRowOrder_Linear:{
            for (int i = 0; i < PinCount; i++)
                ViewData[i]->resume(Delay +( i*Motion.GapInitTableRowOrder));
            break;
        }
        case eTableMotionPattern::eInitTableRowOrder_Random:{
            unsigned long DelayOffset;

            for (int i = 0; i < PinCount; i++) {
                DelayOffset = 0;
                DelayOffset += i*Motion.GapInitTableRowOrder;
                DelayOffset += (rand() % Motion.RangeInitTableRowOrder);
                ViewData[i]->resume(Delay + DelayOffset);
            }
            break;
        }
    }
    ReleaseSRWLockExclusive(&lock);
}

/**
    @brief �� Ŭ���� �Ҹ�
    @param PinCount Pause����� ������ ����. (Pause���¿����� ������ ������ �Ǳ⶧��)
*/
void UI_Table::PauseRowOrder(unsigned long Delay)
{
    MOTION_INFO mi;

    /*UI_Table�� ��� ������ ���� ViewData Delay�� ������ �������ش�.
      ���� ������Ʈ�� ViewData �� ���������� �˾Ƽ� �����Ѵ�.*/
    AcquireSRWLockExclusive(&lock);
    switch (Motion.MotionPauseTableRowOrder) {
        case eTableMotionPattern::ePauseTableRowOrder_Default:{
            for (int i = 0; i < PinCount; i++)
                ViewData[i]->pause(Delay);
            break;
        }
        case eTableMotionPattern::ePauseTableRowOrder_Linear:{
            for (int i = 0; i < PinCount; i++)
                ViewData[i]->pause(Delay + (i * Motion.GapPauseTableRowOrder));
            break;
        }
        case eTableMotionPattern::ePauseTableRowOrder_Random:{
            unsigned long DelayOffset;
            for (int i = 0; i < PinCount; i++) {
                DelayOffset = 0;
                DelayOffset += i * Motion.GapPauseTableRowOrder;
                DelayOffset += (rand() % Motion.RangePauseTableRowOrder);
                ViewData[i]->pause(Delay+ DelayOffset);
            }
            break;
        }
    }
    ReleaseSRWLockExclusive(&lock);
}


void UI_Table::resume(int nDelay)
{
    uiMotionState = eUIMotionState::eUMS_PlayingVisible;
    ResumeFrame(nDelay + Motion.DelayInitTableFrame);
    ResumeBg(nDelay + Motion.DelayInitTableBg);
    ResumeHeaderBg(nDelay + Motion.DelayInitTableHeaderBg);
    ResumeHeaderText(nDelay + Motion.DelayInitTableHeaderText);
    ResumeRowOrder(nDelay + Motion.DelayInitTableRowOrder);
}

void UI_Table::pause(int nDelay)
{
    //PinCount = DataCount < ViewRowCnt ? DataCount : ViewRowCnt;
    uiMotionState = eUIMotionState::eUMS_PlayingHide;
    AcquireSRWLockExclusive(&lock);
    ScrollComp->clearChannel(); /*��ũ�� ��� ����*/
    ReleaseSRWLockExclusive(&lock);
    PauseFrame(nDelay + Motion.DelayPauseTableFrame);
    PauseBg(nDelay + Motion.DelayPauseTableBg);
    PauseHeaderBg(nDelay + Motion.DelayPauseTableHeaderBg);
    PauseHeaderText(nDelay + Motion.DelayPauseTableHeaderText);
    PauseRowOrder(nDelay + Motion.DelayPauseTableRowOrder);
}


BOOL UI_Table::update(unsigned long time)
{
    RowObject* pViewRow;
    int ValidViewRowCnt;
    int UpdateIdx;
    long long ModIndex, CurrBindIndex;
    BOOL bUpdated = FALSE;
    BOOL bReplace; /*�ܼ� ������ ��ü ���� (�̶� ��������� �׻� Default�� ��)*/
    size_t MainDataPoolSize;

    if (uiMotionState == eUIMotionState::eUMS_Hide) return FALSE;

    AcquireSRWLockExclusive(&lock);
    bUpdated = ScrollComp->update(time); /*��ũ�� ���� ���� ������Ʈ*/
    bUpdated |= pBoxFrame->update(time);

    CurrMainIndex = (long long)CurrScrollPixel / RowHgt;
    ModIndex = CurrMainIndex % ViewRowCnt;
    ValidViewRowCnt = DataCount < ViewRowCnt ? DataCount : ViewRowCnt;
    MainDataPoolSize = MainDataPool.size();

    /*������ ���ε�*/
    for (int i = 0; i < ValidViewRowCnt; i++) {
        CurrBindIndex = CurrMainIndex + i;
        if (MainDataPoolSize <= CurrBindIndex) break; /*������Ȳ���� ������ ���� X*/
        UpdateIdx = (ModIndex + i) % ViewRowCnt; /*���� �� ������ �ε��� ���*/
        pViewRow = ViewData[UpdateIdx];
        pViewRow->OnBind(CurrBindIndex, ColWidth);
    }
    PinCount = ValidViewRowCnt;

    /*�� �� ������Ʈ*/
    for (int i = 0; i < ValidViewRowCnt; i++) {
        UpdateIdx = (ModIndex + i) % ViewRowCnt; /*���� �� ������ �ε��� ���*/
        pViewRow = ViewData[UpdateIdx];
        bUpdated |= pViewRow->update(time);
    }

    /*��� ������Ʈ*/
    bUpdated |= pBoxHeader->update(time);
    for (int i = 0; i < ColCnt; i++) bUpdated |= ppTextHdr[i]->update(time);
    ReleaseSRWLockExclusive(&lock);
    if (!bUpdated) {
        if (uiMotionState == eUIMotionState::eUMS_PlayingHide)
            uiMotionState = eUIMotionState::eUMS_Hide;

        else if (uiMotionState == eUIMotionState::eUMS_PlayingVisible)
            uiMotionState = eUIMotionState::eUMS_Visible;
    }
    return bUpdated;
}

void UI_Table::render()
{
    D2D1_RECT_F ClipRect;
    D2D_MATRIX_3X2_F OldMat;
    D2D_MATRIX_3X2_F TmpMat;
    POSITION Pos;
    size_t MainDataPoolSize;
    unsigned long ModScroll, ModIndex, idx;
    int BaseY;
    int ValidViewRowCnt;

    Pos = uiPos;
    ClipRect.left = Pos.x;
    ClipRect.top = Pos.y+HeaderHgt;
    ClipRect.right = Pos.x + Pos.x2;
    ClipRect.bottom = Pos.y + Pos.y2;

    AcquireSRWLockShared(&lock);
    pRenderTarget->PushAxisAlignedClip(ClipRect, D2D1_ANTIALIAS_MODE::D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
    pRenderTarget->GetTransform(&OldMat); /*���� ��� ���*/
    TmpMat = OldMat;
    TmpMat.dx += Pos.x;
    TmpMat.dy += HeaderHgt;
    TmpMat.dy += Pos.y;
    ModScroll = (unsigned long)CurrScrollPixel % RowHgt;
    /* TmpMat._31 �� X, TmpMat._32 �� Y */
    TmpMat.dy -= (float)ModScroll; /* Mod ���������� ���̸� ���� ���ذ��� ������*/
    ValidViewRowCnt = DataCount < ViewRowCnt ? DataCount : ViewRowCnt;
    ModIndex = CurrMainIndex% ViewRowCnt;

    MainDataPoolSize = MainDataPool.size();
    for (int i = 0; i < PinCount; i++) {
        if (MainDataPoolSize <= CurrMainIndex + i) break; /*������ ������ X*/
        pRenderTarget->SetTransform(TmpMat);
        idx = (ModIndex + i) % PinCount;
        ViewData[idx]->render(); /*�� ������*/
        TmpMat.dy += (float)RowHgt;
    }

    pRenderTarget->SetTransform(OldMat); /*���� ��� ����*/
    pRenderTarget->PopAxisAlignedClip();

    pBoxHeader->render(pRenderTarget);
    for (int i = 0; i < ColCnt; i++) ppTextHdr[i]->render(pRenderTarget);
    pBoxFrame->render(pRenderTarget);
    ReleaseSRWLockShared(&lock);
}

/**
    @brief �� ��ü�� ������ ��ȯ�Ѵ�.
*/
RowObject::RowObject(UISystem* pUISys, UI_Table* pParentTable, POSITION pos, unsigned int ColCnt)
{
    if (!ColCnt) ColCnt = 1;
    pParent = pParentTable;
    nColumn = ColCnt;
    uiSys = pUISys;
    Pos = pos;
    ppText = (PropText**)malloc(sizeof(PropText*) * ColCnt);
    ppColLine = (PropLine**)malloc(sizeof(PropLine*) * ColCnt);
    if (!ppText || !ppColLine) return;
    for (int i = 0; i < ColCnt; i++) {
        ppText[i] = new PropText(pParent->pRenderTarget, 512);
        ppColLine[i] = new PropLine(pParent->pRenderTarget);
    }
    pBackgroundBox = new PropBox(pParent->pRenderTarget);
    pSelectBox = new PropBox(pParent->pRenderTarget);
    pHighlightBox = new PropBox(pParent->pRenderTarget);
}

RowObject::~RowObject()
{
    for (int i = 0; i < nColumn; i++) {
        delete ppText[i];
        delete ppColLine[i];
    }
    delete pBackgroundBox;
    delete pSelectBox;
    delete pHighlightBox;
    free(ppText);
    free(ppColLine);
}

void RowObject::ResumeSelect(unsigned long Delay)
{
    unsigned long pitch = pParent->Motion.PitchInitRowSelect;
    MOTION_INFO mi;
    D2D1_COLOR_F StartColor, EndColor;
    TABLE_ROW* pRealData;

    if (MainDataIdx < 0) return;
    pRealData = &pParent->MainDataPool[MainDataIdx];
    if (!pRealData->bSelected) return; /*���õ� ���� �ƴϸ� ��� ��� X*/

    switch (pParent->Motion.MotionInitRowSelect) {
    case eTableMotionPattern::eInitTableSelect_Default:
        pSelectBox->Init(Pos, ALL_ZERO);
        mi = InitMotionInfo(eMotionForm::eMotion_None, Delay, pitch);
        pSelectBox->addColorMotion(mi, TRUE, ALL_ZERO, pParent->Motion.ColorRowBgSelect);
        break;

    case eTableMotionPattern::eInitTableSelect_Linear:
        pSelectBox->Init(Pos, pParent->Motion.ColorRowBgSelect);
        mi = InitMotionInfo(eMotionForm::eMotion_Linear1, Delay, pitch);
        pSelectBox->addMovementMotion(mi, TRUE, { Pos.x, Pos.y,0, Pos.y2 }, Pos);
        break;

    case eTableMotionPattern::eInitTableSelect_Decel:
        pSelectBox->Init(Pos, pParent->Motion.ColorRowBgSelect);
        mi = InitMotionInfo(eMotionForm::eMotion_x3_2, Delay, pitch);
        pSelectBox->addMovementMotion(mi, TRUE, { Pos.x, Pos.y,0, Pos.y2 }, Pos);
        break;
    }
}

void RowObject::PauseSelect(BOOL bMotion, unsigned long Delay)
{
    unsigned long pitch = pParent->Motion.PitchPauseRowSelect;
    MOTION_INFO mi;
    eTableMotionPattern Patt;

    /*��ũ�� / Resume ���� ���� ������Ʈ�ÿ� �� � ��ǵ� �������� �ʴ´�.*/
    if (!bMotion) {
        pSelectBox->Init(Pos, ALL_ZERO);
        return;
    }

    switch (pParent->Motion.MotionPauseRowSelect) {
    case eTableMotionPattern::ePauseTableSelect_Default:
        pSelectBox->Init(Pos, pParent->Motion.ColorRowBgSelect);
        mi = InitMotionInfo(eMotionForm::eMotion_None, Delay, pitch);
        pSelectBox->addColorMotion(mi, TRUE, pParent->Motion.ColorRowBgSelect, ALL_ZERO);
        break;

    case eTableMotionPattern::ePauseTableSelect_Linear:
        //pSelectBox->Init(Pos, pParent->Motion.ColorRowBgSelect);
        mi = InitMotionInfo(eMotionForm::eMotion_Linear1, Delay, pitch);
        //pSelectBox->addMovementMotion(mi, TRUE, Pos, { Pos.x, Pos.y,0, Pos.y2 });
        pSelectBox->SetPos(mi, TRUE, Pos, { Pos.x, Pos.y,0, Pos.y2 });
        break;

    case eTableMotionPattern::ePauseTableSelect_Decel:
        //pSelectBox->Init(Pos, pParent->Motion.ColorRowBgSelect);
        mi = InitMotionInfo(eMotionForm::eMotion_x3_2, Delay, pitch);
        //pSelectBox->addMovementMotion(mi, TRUE, Pos, { Pos.x, Pos.y,0, Pos.y2 });
        pSelectBox->SetPos(mi, TRUE, Pos, { Pos.x, Pos.y,0, Pos.y2 });
        break;
    }
}

void RowObject::SetHighlight(D2D1_COLOR_F Color)
{
    MOTION_INFO mi;
    D2D1_COLOR_F EndColor;

    switch (pParent->Motion.MotionInitRowHighlight) {
    case eTableMotionPattern::eInitRowHighlight_Blink:
        pHighlightBox->Init(Pos, Color);
        mi = InitMotionInfo(eMotionForm::eMotion_Linear1, 0, pParent->Motion.PitchInitRowHighlight);
        EndColor = Color;
        EndColor.a = 0;
        pHighlightBox->SetColor(mi, TRUE, ALL_ZERO, EndColor);
        break;
    }
}

void RowObject::SetTextColor(D2D1_COLOR_F Color, BOOL bMotion)
{
    MOTION_INFO mi;
    mi = InitMotionInfo(eMotionForm::eMotion_None, 0, 0);
    for (int i = 0; i < nColumn; i++) ppText[i]->SetColor(mi, FALSE, Color, Color);
}

void RowObject::SetText(wchar_t** ppData, int nCnt)
{
    for (int i = 0; i < nCnt; i++) ppText[i]->SetText(ppData[i]);
}

void RowObject::OnBind(unsigned long long TargetDataIdx, int* pColWidth, BOOL bNeedUpdate) {
    TABLE_ROW* pTableData;

    /*�̹� ����� ���ε� �ϰ� ������ ó�� X*/
    if (!bNeedUpdate && MainDataIdx == TargetDataIdx) return;
    MainDataIdx = TargetDataIdx;
    /*���� �غ�*/
    ResumeBg(FALSE, 0); /*���ο��� �˾Ƽ� ������� ó����*/

    pTableData = &pParent->MainDataPool[TargetDataIdx];
    ppRealData = pTableData->ppData;
    pWidth = pColWidth;
    /*�̹� Bind �Ǿ������� ���� �ѹ��� Resume �ǹǷ�
      ���߿� Pause�ÿ� �� ��ƾ�� ũ�� �ǽ� ���� �ʾƵ� �ȴ�.*/
    ResumeText(pTableData->bTextMotionPlayed ? FALSE : TRUE, 0); /*���ε�� ������ ������ ����*/
    if (pTableData->bSelected) SetTextColor(pParent->Motion.ColorRowTextSelect, FALSE);
    else                       SetTextColor(pParent->Motion.ColorRowText, FALSE);

    if (pTableData->bSelected) ResumeSelect(0);
    else PauseSelect(FALSE, 0);

    pTableData->bTextMotionPlayed = TRUE;
};

void RowObject::OnSelectEvent() {
    /*�� �ż��� ȣ��� �̹� ȭ�� �ȿ� ������ ����ǹǷ� ����� ������ ����*/
    BOOL bSel;
    TABLE_ROW* pTableData;

    pTableData = &pParent->MainDataPool[MainDataIdx];
    bSel = pTableData->bSelected;
    if (bSel) ResumeSelect(0);
    else PauseSelect(TRUE, 0);
    SetTextColor(bSel ? pParent->Motion.ColorRowTextSelect : pParent->Motion.ColorRowText, FALSE);
};

/**
    @brief ���� �����͸� ��ü�Ѵ�.
    @param ppData ��ü�� �����͹�ġ
    @param pWidth �� �迭
    @param nCnt �������� ����
    @param bReplace �ܼ� ������ ��ü���� (��ũ�ѿ����� ������ ��ü��, ��� ������ ����)
*/
void RowObject::ResumeText(BOOL bMotion, unsigned long Delay)
{
    wchar_t* pStr;
    size_t TextLen;
    int CurrentX = 0;
    POSITION TmpPos;
    MOTION_INFO mi;
    eTableMotionPattern Patt;

    if (!bMotion) Patt = eTableMotionPattern::eInitRowText_Default;
    else          Patt = pParent->Motion.MotionInitRowText;

    switch (Patt) {
        case eTableMotionPattern::eInitRowText_Default: {
            for (int i = 0; i < nColumn; i++) {
                pStr = ppRealData[i];
                TmpPos = { (float)CurrentX, 0, (float)pWidth[i], Pos.y2 };
                CurrentX += pWidth[i];
                TextLen = wcslen(pStr);
                ppText[i]->Init(uiSys->MediumTextForm, pStr, TextLen, TmpPos, pParent->Motion.ColorRowText, 0);
                mi = InitMotionInfo(eMotionForm::eMotion_None, Delay, 0);
                ppText[i]->addLenMotion(mi, TRUE, 0, TextLen);
            }
            break;
        }

        case eTableMotionPattern::eInitRowText_Typing: {
            unsigned long MotionGap = pParent->Motion.GapInitRowText;

            for (int i = 0; i < nColumn; i++) {
                pStr = ppRealData[i];
                TmpPos = { (float)CurrentX, 0, (float)pWidth[i], Pos.y2 };
                CurrentX += pWidth[i];
                TextLen = wcslen(pStr);

                ppText[i]->Init(uiSys->MediumTextForm, pStr, TextLen, TmpPos, pParent->Motion.ColorRowText, 0);
                mi = InitMotionInfo(eMotionForm::eMotion_Linear1, Delay+(i*MotionGap), pParent->Motion.PitchInitRowText);
                ppText[i]->addLenMotion(mi, FALSE, 0, TextLen);
            }
        }

    }
}

void RowObject::PauseText(unsigned long Delay)
{
    wchar_t* pStr;
    size_t TextLen;
    int CurrentX = 0;
    POSITION TmpPos;
    MOTION_INFO mi;
    eTableMotionPattern Patt;
    unsigned long pitch = pParent->Motion.PitchPauseRowText;
    unsigned long DelayOffset;

    if (!ppRealData) return;

    switch (pParent->Motion.MotionPauseRowText) {
    case eTableMotionPattern::ePauseRowText_Default:
        for (int i = 0; i < nColumn; i++) {
            pStr = ppRealData[i];
            TmpPos = { (float)CurrentX, 0, (float)pWidth[i], Pos.y2 };
            CurrentX += pWidth[i];
            mi = InitMotionInfo(eMotionForm::eMotion_None, Delay, pitch);
            ppText[i]->SetColor(mi, TRUE, ALL_ZERO, ALL_ZERO);
        }
        break;
    
    case eTableMotionPattern::ePauseRowText_FlickLinear:
        for (int i = 0; i < nColumn; i++) {
            pStr = ppRealData[i];
            DelayOffset = pParent->Motion.GapPauseRowText * i;
            mi = InitMotionInfo(eMotionForm::eMotion_Pulse1, Delay + DelayOffset , pitch);
            ppText[i]->SetColor(mi, TRUE, ALL_ZERO, ALL_ZERO);
        }
        break;

    case eTableMotionPattern::ePauseRowText_FlickRandom:
        for (int i = 0; i < nColumn; i++) {
            //pStr = ppRealData[i];
            //TmpPos = { (float)CurrentX, 0, (float)pWidth[i], Pos.y2 };
            //CurrentX += pWidth[i];
            DelayOffset = pParent->Motion.GapPauseRowText * i;
            DelayOffset += rand() % pParent->Motion.RangePauseRowText;
            mi = InitMotionInfo(eMotionForm::eMotion_Pulse1, Delay, pitch);
            ppText[i]->SetColor(mi, TRUE, ALL_ZERO, ALL_ZERO);
        }
        break;
    }
}

void RowObject::ResumeBg(BOOL bMotion, unsigned long Delay)
{
    eTableMotionPattern patt;
    MOTION_INFO mi;
    unsigned long Pitch = pParent->Motion.PitchInitRowBg;
    D2D_COLOR_F TargetColor = MainDataIdx & 1 ? pParent->Motion.ColorRowBg1 : pParent->Motion.ColorRowBg2;

    if (bMotion) patt = pParent->Motion.MotionInitRowBg;
    else patt = eTableMotionPattern::eInitRowBg_Default;

    switch (patt) {
    case eTableMotionPattern::eInitRowBg_Default:
        pBackgroundBox->Init(Pos, ALL_ZERO);
        mi = InitMotionInfo(eMotionForm::eMotion_None, Delay, Pitch);
        pBackgroundBox->SetColor(mi, TRUE, ALL_ZERO, TargetColor);
        break;
    }
}

void RowObject::PauseBg(unsigned long Delay)
{
    eTableMotionPattern patt;
    MOTION_INFO mi;
    unsigned long Pitch = pParent->Motion.PitchInitRowBg;

    switch (pParent->Motion.MotionPauseRowBg) {
    case eTableMotionPattern::ePauseRowBg_Default:
        mi = InitMotionInfo(eMotionForm::eMotion_None, Delay, Pitch);
        pBackgroundBox->SetColor(mi, TRUE, ALL_ZERO, ALL_ZERO);
        break;
    }
}



void RowObject::pause(int nDelay)
{
    PauseBg(nDelay + pParent->Motion.DelayPauseRowBg);
    PauseSelect(TRUE, nDelay + pParent->Motion.DelayPauseRowSelect);
    PauseText(nDelay + pParent->Motion.DelayPauseRowText);
}

void RowObject::resume(int nDelay)
{
    ResumeBg(TRUE, nDelay + pParent->Motion.DelayInitRowBg);
    ResumeSelect(nDelay + pParent->Motion.DelayInitRowSelect);
    ResumeText(TRUE, nDelay + pParent->Motion.DelayInitRowText);
}

BOOL RowObject::update(unsigned long time)
{
    BOOL bResult = FALSE;

    for (int i = 0; i < nColumn; i++) {
        bResult |= ppText[i]->update(time);
        bResult |= ppColLine[i]->update(time);
    }
    bResult |= pBackgroundBox->update(time);
    bResult |= pSelectBox->update(time);
    bResult |= pHighlightBox->update(time);
    return bResult;
}

void RowObject::render()
{
    ID2D1RenderTarget* pRT = uiSys->D2DA.pRenTarget;

    pBackgroundBox->render(pRT);
    pSelectBox->render(pRT);
    pHighlightBox->render(pRT);
    for (int i = 0; i < nColumn; i++) {
        ppText[i]->render(pRT);
        //ppColLine[i]->render(pRT);
    }
}
