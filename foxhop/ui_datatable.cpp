#include "./include/ui_datatable.hpp"
#include "./include/ui_system.hpp"
#include <cstdarg>

#define TEXTSHEET_DEFAULT_WIDTH     150 /*열 가로폭 기본 픽셀*/
#define TEXTSHEET_DEFAULT_ROWHEIGHT 20  /*행 세로폭 기본 픽셀*/

UI_DataTable::UI_DataTable(UISystem* pUISys, pfnUIHandler pfnCallback, POSITION Pos, unsigned int ColumnCount, wchar_t** ColumnNameList, unsigned int* ColumnWidth, unsigned int HeaderHeight, unsigned int RowHeight, BOOL MultiSelect, UI_DataTable_MotionParam MotionParam)
{
    /*기본 UI 멤버 셋팅*/
    uiSys = pUISys;
    pRenderTarget = pUISys->D2DA.pRenTarget;
    Focusable = TRUE;
    DefaultHandler = DefaultDataTableProc;
    MessageHandler = pfnCallback;
    uiPos = Pos;
    uiMotionState = eUIMotionState::eUMS_Hide;

    /*하위 클래스 멤버 셋팅*/
    MultiSelectMode = MultiSelect;
    CurrScrollPixel = 0;
    ScrollPixel = 0;
    MaxScrollPixel = 0;
    DataCount = 0;
    ViewStartIdx = 0;
    PrevSelMainIdx = -1;
    HeaderHgt = HeaderHeight;
    RowHgt = RowHeight ? RowHeight : TEXTSHEET_DEFAULT_ROWHEIGHT;
    ScrollComp = new ComponentMotion;
    Motion = MotionParam;
    PinCount = 0;
    ValidViewDataCnt = 0;

    /* 열 정보 셋팅 */
    if (!ColumnCount) ColumnCount = 1; /*최소 1개의 열은 필요함*/
    ColCnt = ColumnCount;
    ColName = (wchar_t**)malloc(sizeof(wchar_t*) * ColCnt);
    ColWidth = (int*)malloc(sizeof(int) * ColCnt);
    ppTextHdr = (PropText**)malloc(sizeof(PropText*) * ColCnt);
    if (!ColName || !ColWidth) return; /*TODO : 예외 처리 필요*/

    for (int i = 0; i < ColCnt; i++) {
        ColName[i] = (wchar_t*)_wcsdup(ColumnNameList[i]); /*열 헤더 생성*/
        if (!ColumnWidth) ColWidth[i] = TEXTSHEET_DEFAULT_WIDTH;
        else ColWidth[i] = ColumnWidth[i];
    }

    InitializeSRWLock(&lock);

    /*행 생성*/
    ClientHeight = (int)Pos.y2 - HeaderHgt;
    ViewRowCnt = (ClientHeight / RowHgt) + 2; /*자투리 영역도 온전한 1개 취급*/
    for (int i = 0; i < ViewRowCnt; i++) {
        DataTableRowObject* obj = new DataTableRowObject(pUISys, this, { 0,0,Pos.x2,(float)RowHgt }, ColCnt);
        ViewData.push_back(obj);
    }

    pBoxHeader = new PropBox(pRenderTarget);
    for (int i = 0; i < ColCnt; i++) ppTextHdr[i] = new PropText(pRenderTarget, 512, uiSys->MediumTextForm);
    pBoxFrame = new PropBox(pRenderTarget);

    //resume(0);
    DefaultHandler(this, UIM_CREATE, NULL, NULL); /*UI생성 메세지 전송*/
}

/**
    @brief 범위 내의 값인지 체크
    @param x 기준 값
    @param dx 기준으로부터의 범위 (x가 3이고 dx가 2이면, 3~5 범위)
    @param test 테스트할 값
*/
static BOOL IsInRange(long long x, long long dx, long long test)
{
    if (test < x) return FALSE;
    if (test > x + dx) return FALSE;
    return TRUE;
}

/**
    @brief 기본 텍스트 리스트뷰 메세지 핸들러
    @remark 사용자 지정 프로시저는 기본 핸들러 실행 후 호출된다.
*/
void UI_DataTable::DefaultDataTableProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam)
{
    UI_DataTable* pTable = static_cast<UI_DataTable*>(pUI);
    pfnUIHandler UserHandler = pUI->MessageHandler;
    if (!pUI) return;

    switch (Message) {
    case UIM_MOUSEON: /*ON / LEAVE 메세지는 UI_Panel 로 부터 SendMessage 로 전달된다 */
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
        DataTableRowObject* pViewRow;

        pt.x = GET_X_LPARAM(lParam);
        pt.y = GET_Y_LPARAM(lParam);
        y = pTable->MousePt.y - pTable->HeaderHgt;
        if (y < 0) break; /*헤더영역 클릭됨*/

        MainDataPoolSize = pTable->MainDataPool.size();
        if (MainDataPoolSize == 0) break;
        idx = ((long long)pTable->CurrScrollPixel + y) / pTable->RowHgt;
        if (idx >= MainDataPoolSize) break;
        OrderIdx = idx % pTable->ViewRowCnt; /*선택한 인덱스로부터 화면에 보이는 뷰 인덱스 계산*/
        MainIdx = pTable->ViewData[OrderIdx]->MainDataIdx; /*선택된 행*/
        pTable->MainDataPool[MainIdx].bSelected ^= 1; /*TRUE / FALSE 교대 : 선택 표기 완료*/
        bSel = pTable->MainDataPool[MainIdx].bSelected;

        pViewRow = pTable->ViewData[OrderIdx];
        pViewRow->OnSelectEvent();
        if (bSel) {
            if (pTable->MessageHandler)
                pTable->MessageHandler(pTable, UIM_TABLE_SELECT, (WPARAM)pTable->MainDataPool[MainIdx].ppData, MainIdx);/*어떤 라인이 선택 되었는지에 대한 정보 전달*/

            if (!pTable->MultiSelectMode) {
                /*전에 이미 선택된것이 있을때.*/
                if (pTable->PrevSelMainIdx >= 0) {
                    pTable->MainDataPool[pTable->PrevSelMainIdx].bSelected = FALSE;
                    /*변경 대상이 뷰 영억 안에 있을때엔 모션 재생을 해주어야 함*/
                    if (IsInRange(pTable->ViewStartIdx, pTable->ViewRowCnt, pTable->PrevSelMainIdx)) {
                        long long PrevViewRowIdx = pTable->PrevSelMainIdx % pTable->ViewRowCnt;
                        DataTableRowObject* pPrevViewRow = pTable->ViewData[PrevViewRowIdx];
                        pPrevViewRow->OnSelectEvent();
                    }
                }
                pTable->PrevSelMainIdx = MainIdx;
            }
        }
        else {
            if (pTable->MessageHandler)
                pTable->MessageHandler(pTable, UIM_TABLE_UNSELECT, NULL, MainIdx);/*어떤 라인이 선택해제 되었는지에 대한 정보 전달*/
            pTable->PrevSelMainIdx = -1; /*선택 해제시 이전 선택 인덱스 무효화. (다중선택은 고려하지 않아도 된다)*/
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

long long UI_DataTable::DataIdx2ViewRowIdx(long long DataIdx)
{
    if (DataCount <= DataIdx) return -1;
    return DataIdx % ViewRowCnt;
}

BOOL UI_DataTable::DataIdxIsInScreen(long long DataIdx)
{
    if (DataCount <= DataIdx) return FALSE;
    if (ViewStartIdx > DataIdx) return FALSE;
    if (ViewStartIdx + ViewRowCnt < DataIdx) return FALSE;
    return TRUE;
}


/**
    @brief 테이블에 데이터를 추가한다.
    @param bMotion 애니메이션 진행 여부
    @param bAutoScroll 새 데이터가 추가 될 때, 해당 라인으로 자동 스크롤 된다.
*/
void UI_DataTable::AddData(BOOL bMotion, BOOL bAutoScroll, wchar_t* ...)
{
    va_list args;
    wchar_t* pStr;


    long long TmpScrollPx;
    DATATABLE_ROW Row;
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
    ValidViewDataCnt = ViewRowCnt > DataCount ? DataCount : ViewRowCnt;
    TmpScrollPx = (DataCount * RowHgt) - ClientHeight;
    if (TmpScrollPx <= 0) MaxScrollPixel = 0; /*음수는 없다.*/
    else MaxScrollPixel = TmpScrollPx;
    //PinCount = DataCount < ViewRowCnt ? DataCount : ViewRowCnt;
    ReleaseSRWLockExclusive(&lock);

    if (uiMotionState != eUIMotionState::eUMS_Visible) return; /*초기화모션/소멸모션 진행중엔 스크롤 X*/
    if (bAutoScroll) SetScroll(MaxScrollPixel);
}

void UI_DataTable::EditData(BOOL bMotion, unsigned long long RowIdx, wchar_t* ...)
{
    DataTableRowObject* pViewRow;
    va_list arglist;
    wchar_t* pStr;
    DataTableRowObject* pRow;
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
        if (uiMotionState == eUIMotionState::eUMS_Visible)
            pRow->SetText(MainDataPool[RowIdx].ppData, ColCnt);
    }
    ReleaseSRWLockExclusive(&lock);
}

void UI_DataTable::HighlightData(unsigned long long DataIdx, D2D1_COLOR_F HightlightColor)
{
    DataTableRowObject* pRow;
    long long Idx;

    if (uiMotionState != eUIMotionState::eUMS_Visible) return;
    if (!DataIdxIsInScreen(DataIdx)) return;
    Idx = DataIdx2ViewRowIdx(DataIdx);
    if (Idx < 0) return;

    AcquireSRWLockExclusive(&lock);
    ViewData[Idx]->SetHighlight(HightlightColor);
    ReleaseSRWLockExclusive(&lock);
}

/**
    @brief 지정 픽셀값으로 스크롤을 옮긴다
    @remark 허용 스크롤값을 벗어날경우 내부에서 알아서 적정 값으로 관리한다.
*/
void UI_DataTable::SetScroll(long long TargetScrollPx)
{
    MOTION_PATTERN patt;
    MOTION_INFO    mi;

    /*초기화 / 소멸 모션 진행중엔 스크롤 X*/
    if (uiMotionState != eUIMotionState::eUMS_Visible) return;

    AcquireSRWLockExclusive(&lock);
    /*스크롤 경계 관리*/
    if (TargetScrollPx < 0)
        TargetScrollPx = 0;
    if (TargetScrollPx > MaxScrollPixel)
        TargetScrollPx = MaxScrollPixel;
    ScrollPixel = TargetScrollPx;

    /*스크롤 모션 진행 재시작*/
    ScrollComp->clearChannel();
    mi = InitMotionInfo(eMotionForm::eMotion_x3_2, 0, Motion.PitchScroll);
    patt = InitMotionPattern(mi, NULL);
    AddChain(&patt, &CurrScrollPixel, CurrScrollPixel, (float)TargetScrollPx);
    ScrollComp->addChannel(patt);

    ReleaseSRWLockExclusive(&lock);
};

void UI_DataTable::ResumeFrame(unsigned long Delay)
{
    MOTION_INFO mi;
    D2D1_COLOR_F StartColor, EndColor;
    AcquireSRWLockExclusive(&lock);
    switch (Motion.MotionInitTableFrame) {
    case eDataTableMotionPattern::eInitTableFrame_Default: {
        pBoxFrame->Init(uiPos, ALL_ZERO, FALSE);
        mi = InitMotionInfo(eMotionForm::eMotion_None, Delay, Motion.PitchInitTableFrame);
        pBoxFrame->SetColor(mi, TRUE, ALL_ZERO, Motion.ColorFrame);
        break;
    }

    case eDataTableMotionPattern::eInitTableFrame_Flick: {
        pBoxFrame->Init(uiPos, ALL_ZERO, FALSE);
        mi = InitMotionInfo(eMotionForm::eMotion_Pulse1, Delay, Motion.PitchInitTableFrame);
        pBoxFrame->SetColor(mi, TRUE, ALL_ZERO, Motion.ColorFrame);
        break;
    }

    case eDataTableMotionPattern::eInitTableFrame_ExpendAllDirFlick: {
        unsigned long Pitch = Motion.PitchInitTableFrame;
        POSITION StartPos;

        StartPos = { uiPos.x + uiPos.x2 / 2 , uiPos.y + uiPos.y2 / 2 , 0, 0 };
        pBoxFrame->Init(StartPos, Motion.ColorFrame, FALSE);
        mi = InitMotionInfo(eMotionForm::eMotion_x3_2, Delay, Pitch);
        pBoxFrame->SetPos(mi, TRUE, ALL_ZERO, uiPos);
        mi = InitMotionInfo(eMotionForm::eMotion_Pulse1, Delay + Pitch / 2, Pitch / 2);
        pBoxFrame->SetColor(mi, FALSE, ALL_ZERO, Motion.ColorFrame);
        break;
    }
    }
    ReleaseSRWLockExclusive(&lock);
}

void UI_DataTable::PauseFrame(unsigned long Delay)
{
    MOTION_INFO mi;
    D2D1_COLOR_F StartColor, EndColor;

    AcquireSRWLockExclusive(&lock);
    switch (Motion.MotionPauseTableFrame) {
    case eDataTableMotionPattern::ePauseTableFrame_Default: {
        mi = InitMotionInfo(eMotionForm::eMotion_None, Delay, Motion.PitchPauseTableFrame);
        pBoxFrame->SetColor(mi, TRUE, ALL_ZERO, ALL_ZERO);
        break;
    }

    case eDataTableMotionPattern::ePauseTableFrame_Flick: {
        mi = InitMotionInfo(eMotionForm::eMotion_Pulse1, Delay, Motion.PitchPauseTableFrame);
        pBoxFrame->SetColor(mi, TRUE, ALL_ZERO, ALL_ZERO);
        break;
    }

    case eDataTableMotionPattern::ePauseTableFrame_ExpendAllDirFlick: {
        unsigned long Pitch = Motion.PitchPauseTableFrame;
        POSITION EndPos;

        EndPos = { uiPos.x - 50 , uiPos.y - 50 , uiPos.x2 + 100, uiPos.y2 + 100 };
        mi = InitMotionInfo(eMotionForm::eMotion_Linear1, Delay, Pitch);
        pBoxFrame->SetPos(mi, TRUE, ALL_ZERO, EndPos);
        mi = InitMotionInfo(eMotionForm::eMotion_Pulse1, Delay + Pitch / 2, Pitch / 2);
        pBoxFrame->SetColor(mi, TRUE, ALL_ZERO, ALL_ZERO);
        break;
    }
    }
    ReleaseSRWLockExclusive(&lock);
}

void UI_DataTable::ResumeBg(unsigned long Delay)
{
    MOTION_INFO mi;

#if 0
    AcquireSRWLockExclusive(&lock);
    switch (Motion.MotionInitTableBg) {
    case eDataTableMotionPattern::eInitTableBg_Default: {
        pBoxFrame->Init(uiPos, ALL_ZERO);
        mi = InitMotionInfo(eMotionForm::eMotion_None, Delay, Motion.PitchInitTableBg);
        pBoxFrame->SetColor(mi, TRUE, ALL_ZERO, Motion.ColorBg);
        break;
    }
    }
    ReleaseSRWLockExclusive(&lock);
#endif
}

void UI_DataTable::PauseBg(unsigned long Delay)
{
    MOTION_INFO mi;
#if 0
    AcquireSRWLockExclusive(&lock);
    switch (Motion.MotionPauseTableBg) {
    case eDataTableMotionPattern::ePauseTableBg_Default: {
        mi = InitMotionInfo(eMotionForm::eMotion_None, Delay, Motion.PitchPauseTableBg);
        pBoxFrame->SetColor(mi, TRUE, ALL_ZERO, ALL_ZERO);
        break;
    }
    }
    ReleaseSRWLockExclusive(&lock);
#endif
}

void UI_DataTable::ResumeHeaderBg(unsigned long Delay)
{
    MOTION_INFO mi;

    AcquireSRWLockExclusive(&lock);
    switch (Motion.MotionInitTableHeaderBg) {
    case eDataTableMotionPattern::eInitTableHeaderBg_Default: {
        POSITION TmpPos = uiPos;

        TmpPos.x2 = uiPos.x2;
        TmpPos.y2 = HeaderHgt;
        pBoxHeader->Init(TmpPos, ALL_ZERO);
        mi = InitMotionInfo(eMotionForm::eMotion_None, Delay, Motion.PitchInitTableHeaderBg);
        pBoxHeader->SetColor(mi, TRUE, ALL_ZERO, Motion.ColorHeaderBg);
        break;
    }

    case eDataTableMotionPattern::eInitTableHeaderBg_Linear: {
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

    case eDataTableMotionPattern::eInitTableHeaderBg_LinearReverse: {
        POSITION StartPos = uiPos;
        POSITION EndPos = uiPos;

        StartPos.y2 = HeaderHgt;
        StartPos.x = uiPos.x + uiPos.x2;
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

void UI_DataTable::PauseHeaderBg(unsigned long Delay)
{
    MOTION_INFO mi;

    AcquireSRWLockExclusive(&lock);
    switch (Motion.MotionPauseTableHeaderBg) {
    case eDataTableMotionPattern::ePauseTableHeaderBg_Default: {
        mi = InitMotionInfo(eMotionForm::eMotion_None, Delay, Motion.PitchPauseTableHeaderBg);
        pBoxHeader->SetColor(mi, TRUE, ALL_ZERO, ALL_ZERO);
        break;
    }

    case eDataTableMotionPattern::ePauseTableHeaderBg_Linear: {
        POSITION StartPos = uiPos;
        POSITION EndPos = uiPos;

        EndPos.y2 = HeaderHgt;
        EndPos.x2 = 0;
        mi = InitMotionInfo(eMotionForm::eMotion_Linear1, Delay, Motion.PitchInitTableHeaderBg);
        pBoxHeader->SetPos(mi, TRUE, ALL_ZERO, EndPos);
        break;
    }

    case eDataTableMotionPattern::ePauseTableHeaderBg_LinearReverse: {
        POSITION StartPos = uiPos;
        POSITION EndPos = uiPos;

        EndPos.y2 = HeaderHgt;
        EndPos.x = uiPos.x + uiPos.x2;
        EndPos.x2 = 0;
        mi = InitMotionInfo(eMotionForm::eMotion_Linear1, Delay, Motion.PitchInitTableHeaderBg);
        pBoxHeader->SetPos(mi, TRUE, ALL_ZERO, EndPos);
        break;
    }
    }
    ReleaseSRWLockExclusive(&lock);
}

void UI_DataTable::ResumeHeaderText(unsigned long Delay)
{
    unsigned long TextLen;

    AcquireSRWLockExclusive(&lock);
    switch (Motion.MotionInitTableHeaderText) {
        MOTION_INFO mi;

    case eDataTableMotionPattern::eInitTableHeaderText_Default: {
        POSITION TmpPos = uiPos;
        int      WidthOffset = 0;

        TmpPos.y2 = HeaderHgt;
        for (int i = 0; i < ColCnt; i++) {
            TmpPos.x = uiPos.x + WidthOffset;
            TmpPos.x2 = ColWidth[i];
            WidthOffset += ColWidth[i];
            ppTextHdr[i]->Init(ColName[i], 0, TmpPos, ALL_ZERO, wcslen(ColName[i]));
            mi = InitMotionInfo(eMotionForm::eMotion_None, Delay, Motion.PitchInitTableHeaderText);
            ppTextHdr[i]->SetColor(mi, TRUE, ALL_ZERO, Motion.ColorHeaderText);
        }
        break;
    }

    case eDataTableMotionPattern::eInitTableHeaderText_SlideIn: {
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
            DelayOffset = Delay + Motion.GapInitTableHeaderText * i;

            ppTextHdr[i]->Init(ColName[i], TextLen, StartPos, ALL_ZERO, TextLen);
            mi = InitMotionInfo(eMotionForm::eMotion_Linear1, DelayOffset, Motion.PitchInitTableHeaderText);
            ppTextHdr[i]->SetColor(mi, TRUE, ALL_ZERO, Motion.ColorHeaderText);
            ppTextHdr[i]->SetPos(mi, TRUE, ALL_ZERO, EndPos);
        }
        break;
    }
    }
    ReleaseSRWLockExclusive(&lock);
}

void UI_DataTable::PauseHeaderText(unsigned long Delay)
{
    MOTION_INFO mi;

    AcquireSRWLockExclusive(&lock);
    switch (Motion.MotionPauseTableHeaderText) {
    case eDataTableMotionPattern::ePauseTableHeaderText_Default: {
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

    case eDataTableMotionPattern::ePauseTableHeaderText_SlideOut: {
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

void UI_DataTable::ResumeRowOrder(unsigned long Delay)
{
    MOTION_INFO mi;
    //long long ValidViewRowCnt = DataCount < ViewRowCnt ? DataCount : ViewRowCnt;

    /*UI_DataTable은 모션 종류에 따라 ViewData Delay만 적절히 주입해준다.
      하위 오브젝트는 ViewData 가 내부적으로 알아서 조정한다.*/
    AcquireSRWLockExclusive(&lock);
    switch (Motion.MotionInitTableRowOrder) {
    case eDataTableMotionPattern::eInitTableRowOrder_Default: {
        for (int i = 0; i < PinCount; i++)
            ViewData[i]->resume(Delay);
        break;
    }
    case eDataTableMotionPattern::eInitTableRowOrder_Linear: {
        for (int i = 0; i < PinCount; i++)
            ViewData[i]->resume(Delay + (i * Motion.GapInitTableRowOrder));
        break;
    }
    case eDataTableMotionPattern::eInitTableRowOrder_Random: {
        unsigned long DelayOffset;

        for (int i = 0; i < PinCount; i++) {
            DelayOffset = 0;
            DelayOffset += i * Motion.GapInitTableRowOrder;
            DelayOffset += (rand() % Motion.RangeInitTableRowOrder);
            ViewData[i]->resume(Delay + DelayOffset);
        }
        break;
    }
    }
    ReleaseSRWLockExclusive(&lock);
}

/**
    @brief 행 클래스 소멸
    @param PinCount Pause당시의 데이터 갯수. (Pause상태에서도 데이터 삽입이 되기때문)
*/
void UI_DataTable::PauseRowOrder(unsigned long Delay)
{
    MOTION_INFO mi;

    /*UI_DataTable은 모션 종류에 따라 ViewData Delay만 적절히 주입해준다.
      하위 오브젝트는 ViewData 가 내부적으로 알아서 조정한다.*/
    AcquireSRWLockExclusive(&lock);
    switch (Motion.MotionPauseTableRowOrder) {
    case eDataTableMotionPattern::ePauseTableRowOrder_Default: {
        for (int i = 0; i < PinCount; i++)
            ViewData[i]->pause(Delay);
        break;
    }
    case eDataTableMotionPattern::ePauseTableRowOrder_Linear: {
        for (int i = 0; i < PinCount; i++)
            ViewData[i]->pause(Delay + (i * Motion.GapPauseTableRowOrder));
        break;
    }
    case eDataTableMotionPattern::ePauseTableRowOrder_Random: {
        unsigned long DelayOffset;
        for (int i = 0; i < PinCount; i++) {
            DelayOffset = 0;
            DelayOffset += i * Motion.GapPauseTableRowOrder;
            DelayOffset += (rand() % Motion.RangePauseTableRowOrder);
            ViewData[i]->pause(Delay + DelayOffset);
        }
        break;
    }
    }
    ReleaseSRWLockExclusive(&lock);
}


void UI_DataTable::resume(int nDelay)
{
    AcquireSRWLockExclusive(&lock);
    uiMotionState = eUIMotionState::eUMS_PlayingVisible;
    ReleaseSRWLockExclusive(&lock);
    ResumeFrame(nDelay + Motion.DelayInitTableFrame);
    ResumeBg(nDelay + Motion.DelayInitTableBg);
    ResumeHeaderBg(nDelay + Motion.DelayInitTableHeaderBg);
    ResumeHeaderText(nDelay + Motion.DelayInitTableHeaderText);
    ResumeRowOrder(nDelay + Motion.DelayInitTableRowOrder);
}

void UI_DataTable::pause(int nDelay)
{
    //PinCount = DataCount < ViewRowCnt ? DataCount : ViewRowCnt;
    AcquireSRWLockExclusive(&lock);
    uiMotionState = eUIMotionState::eUMS_PlayingHide;
    PinCount = ValidViewDataCnt; /*현재의 유효 뷰 갯수 (Resume시엔 PinCount를 따르지 않음)*/
    ScrollComp->clearChannel(); /*스크롤 모션 정지*/
    ReleaseSRWLockExclusive(&lock);
    PauseFrame(nDelay + Motion.DelayPauseTableFrame);
    PauseBg(nDelay + Motion.DelayPauseTableBg);
    PauseHeaderBg(nDelay + Motion.DelayPauseTableHeaderBg);
    PauseHeaderText(nDelay + Motion.DelayPauseTableHeaderText);
    PauseRowOrder(nDelay + Motion.DelayPauseTableRowOrder);
}


BOOL UI_DataTable::update(unsigned long time)
{
    DataTableRowObject* pViewRow;
    int UpdateIdx;
    long long ModIndex, CurrBindIndex;
    BOOL bUpdated = FALSE;
    BOOL bReplace; /*단순 데이터 교체 여부 (이때 모션진행은 항상 Default로 함)*/
    size_t MainDataPoolSize;

    if (uiMotionState == eUIMotionState::eUMS_Hide) return FALSE;

    AcquireSRWLockExclusive(&lock);
    bUpdated = ScrollComp->update(time); /*스크롤 상태 먼저 업데이트*/
    bUpdated |= pBoxFrame->update(time);

    ViewStartIdx = (long long)CurrScrollPixel / RowHgt;
    ModIndex = ViewStartIdx % ViewRowCnt;
    MainDataPoolSize = MainDataPool.size();

    /*데이터 바인딩*/
    for (int i = 0; i < ValidViewDataCnt; i++) {
        CurrBindIndex = ViewStartIdx + i;
        if (MainDataPoolSize <= CurrBindIndex) break; /*딱뎀상황에선 자투리 접근 X*/
        UpdateIdx = (ModIndex + i) % ViewRowCnt; /*현재 뷰 영역의 인덱스 계산*/
        pViewRow = ViewData[UpdateIdx];
        pViewRow->OnBind(CurrBindIndex, ColWidth);
    }

    /*뷰 행 업데이트*/
    if (uiMotionState == eUIMotionState::eUMS_PlayingHide) {
        for (int i = 0; i < PinCount; i++) {
            pViewRow = ViewData[i];
            bUpdated |= pViewRow->update(time);
        }
    }
    else {
        for (int i = 0; i < ValidViewDataCnt; i++) {
            pViewRow = ViewData[i];
            bUpdated |= pViewRow->update(time);
        }
    }

    /*헤더 업데이트*/
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

void UI_DataTable::render()
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
    ClipRect.top = Pos.y + HeaderHgt;
    ClipRect.right = Pos.x + Pos.x2;
    ClipRect.bottom = Pos.y + Pos.y2;

    AcquireSRWLockShared(&lock);
    pRenderTarget->GetTransform(&OldMat); /*기존 행렬 백업*/
    pRenderTarget->PushAxisAlignedClip(ClipRect, D2D1_ANTIALIAS_MODE::D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
    TmpMat = OldMat;
    TmpMat.dx += Pos.x;
    TmpMat.dy += HeaderHgt;
    TmpMat.dy += Pos.y;
    ModScroll = (unsigned long)CurrScrollPixel % RowHgt;
    /* TmpMat._31 는 X, TmpMat._32 는 Y */
    TmpMat.dy -= (float)ModScroll; /* Mod 음수값부터 높이를 차차 더해가며 렌더링*/
    ValidViewRowCnt = DataCount < ViewRowCnt ? DataCount : ViewRowCnt;
    ModIndex = ViewStartIdx % ViewRowCnt;

    MainDataPoolSize = MainDataPool.size();
    for (int i = 0; i < ValidViewDataCnt; i++) {
        if (MainDataPoolSize <= ViewStartIdx + i) break; /*딱뎀시 자투리 X*/
        pRenderTarget->SetTransform(TmpMat);
        idx = (ModIndex + i) % ValidViewDataCnt;
        ViewData[idx]->render(); /*행 렌더링*/
        TmpMat.dy += (float)RowHgt;
    }

    pRenderTarget->PopAxisAlignedClip();
    pRenderTarget->SetTransform(OldMat); /*기존 행렬 복구*/

    pBoxHeader->render(pRenderTarget);
    for (int i = 0; i < ColCnt; i++) ppTextHdr[i]->render(pRenderTarget);
    pBoxFrame->render(pRenderTarget);
    ReleaseSRWLockShared(&lock);
}

/**
    @brief 행 객체를 생성해 반환한다.
*/
DataTableRowObject::DataTableRowObject(UISystem* pUISys, UI_DataTable* pParentTable, POSITION pos, unsigned int ColCnt)
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
        ppText[i] = new PropText(pParent->pRenderTarget, 512, uiSys->MediumTextForm);
        ppColLine[i] = new PropLine(pParent->pRenderTarget);
    }
    pBackgroundBox = new PropBox(pParent->pRenderTarget);
    pSelectBox = new PropBox(pParent->pRenderTarget);
    pHighlightBox = new PropBox(pParent->pRenderTarget);
}

DataTableRowObject::~DataTableRowObject()
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

void DataTableRowObject::ResumeSelect(unsigned long Delay)
{
    unsigned long pitch = pParent->Motion.PitchInitRowSelect;
    MOTION_INFO mi;
    D2D1_COLOR_F StartColor, EndColor;
    DATATABLE_ROW* pRealData;

    if (MainDataIdx < 0) return;
    pRealData = &pParent->MainDataPool[MainDataIdx];
    if (!pRealData->bSelected) return; /*선택된 행이 아니면 모션 재생 X*/

    switch (pParent->Motion.MotionInitRowSelect) {
    case eDataTableMotionPattern::eInitTableSelect_Default:
        pSelectBox->Init(Pos, ALL_ZERO);
        mi = InitMotionInfo(eMotionForm::eMotion_None, Delay, pitch);
        pSelectBox->addColorMotion(mi, TRUE, ALL_ZERO, pParent->Motion.ColorRowBgSelect);
        break;

    case eDataTableMotionPattern::eInitTableSelect_Linear:
        pSelectBox->Init(Pos, pParent->Motion.ColorRowBgSelect);
        mi = InitMotionInfo(eMotionForm::eMotion_Linear1, Delay, pitch);
        pSelectBox->addMovementMotion(mi, TRUE, { Pos.x, Pos.y,0, Pos.y2 }, Pos);
        break;

    case eDataTableMotionPattern::eInitTableSelect_Decel:
        pSelectBox->Init(Pos, pParent->Motion.ColorRowBgSelect);
        mi = InitMotionInfo(eMotionForm::eMotion_x3_2, Delay, pitch);
        pSelectBox->addMovementMotion(mi, TRUE, { Pos.x, Pos.y,0, Pos.y2 }, Pos);
        break;
    }
}

void DataTableRowObject::PauseSelect(BOOL bMotion, unsigned long Delay)
{
    unsigned long pitch = pParent->Motion.PitchPauseRowSelect;
    MOTION_INFO mi;
    eDataTableMotionPattern Patt;

    /*스크롤 / Resume 으로 인한 업데이트시엔 그 어떤 모션도 진행하지 않는다.*/
    if (!bMotion) {
        pSelectBox->Init(Pos, ALL_ZERO);
        return;
    }

    switch (pParent->Motion.MotionPauseRowSelect) {
    case eDataTableMotionPattern::ePauseTableSelect_Default:
        pSelectBox->Init(Pos, pParent->Motion.ColorRowBgSelect);
        mi = InitMotionInfo(eMotionForm::eMotion_None, Delay, pitch);
        pSelectBox->addColorMotion(mi, TRUE, pParent->Motion.ColorRowBgSelect, ALL_ZERO);
        break;

    case eDataTableMotionPattern::ePauseTableSelect_Linear:
        mi = InitMotionInfo(eMotionForm::eMotion_Linear1, Delay, pitch);
        pSelectBox->SetPos(mi, TRUE, Pos, { Pos.x, Pos.y,0, Pos.y2 });
        break;

    case eDataTableMotionPattern::ePauseTableSelect_Decel:
        mi = InitMotionInfo(eMotionForm::eMotion_x3_2, Delay, pitch);
        pSelectBox->SetPos(mi, TRUE, Pos, { Pos.x, Pos.y,0, Pos.y2 });
        break;
    }
}

void DataTableRowObject::SetHighlight(D2D1_COLOR_F Color)
{
    MOTION_INFO mi;
    D2D1_COLOR_F EndColor;

    switch (pParent->Motion.MotionInitRowHighlight) {
    case eDataTableMotionPattern::eInitRowHighlight_Blink:
        pHighlightBox->Init(Pos, Color);
        mi = InitMotionInfo(eMotionForm::eMotion_Linear1, 0, pParent->Motion.PitchInitRowHighlight);
        EndColor = Color;
        EndColor.a = 0;
        pHighlightBox->SetColor(mi, TRUE, ALL_ZERO, EndColor);
        break;
    }
}

void DataTableRowObject::SetTextColor(D2D1_COLOR_F Color, BOOL bMotion)
{
    MOTION_INFO mi;
    mi = InitMotionInfo(eMotionForm::eMotion_None, 0, 0);
    for (int i = 0; i < nColumn; i++) ppText[i]->SetColor(mi, FALSE, Color, Color);
}

void DataTableRowObject::SetText(wchar_t** ppData, int nCnt)
{
    for (int i = 0; i < nCnt; i++) ppText[i]->SetText(ppData[i]);
}

void DataTableRowObject::OnBind(unsigned long long TargetDataIdx, int* pColWidth, BOOL bNeedUpdate) {
    DATATABLE_ROW* pTableData;

    /*이미 대상을 바인딩 하고 있으면 처리 X*/
    if (!bNeedUpdate && MainDataIdx == TargetDataIdx) return;
    MainDataIdx = TargetDataIdx;
    /*배경색 준비*/
    ResumeBg(FALSE, 0); /*내부에서 알아서 교대색상 처리됨*/

    pTableData = &pParent->MainDataPool[TargetDataIdx];
    ppRealData = pTableData->ppData;
    pWidth = pColWidth;
    /*이미 Bind 되어있으면 최초 한번만 Resume 되므로
      나중에 Pause시에 이 루틴을 크게 의식 하지 않아도 된다.*/
    ResumeText(pTableData->bTextMotionPlayed ? FALSE : TRUE, 0); /*바인드는 무조건 딜레이 없이*/
    if (pTableData->bSelected) SetTextColor(pParent->Motion.ColorRowTextSelect, FALSE);
    else                       SetTextColor(pParent->Motion.ColorRowText, FALSE);

    if (pTableData->bSelected) ResumeSelect(0);
    else PauseSelect(FALSE, 0);

    pTableData->bTextMotionPlayed = TRUE;
};

void DataTableRowObject::OnSelectEvent() {
    /*이 매서드 호출시 이미 화면 안에 있음이 보장되므로 모션은 무조건 진행*/
    BOOL bSel;
    DATATABLE_ROW* pTableData;

    pTableData = &pParent->MainDataPool[MainDataIdx];
    bSel = pTableData->bSelected;
    if (bSel) ResumeSelect(0);
    else PauseSelect(TRUE, 0);
    SetTextColor(bSel ? pParent->Motion.ColorRowTextSelect : pParent->Motion.ColorRowText, FALSE);
};

/**
    @brief 행의 데이터를 교체한다.
    @param ppData 교체할 데이터뭉치
    @param pWidth 폭 배열
    @param nCnt 데이터의 갯수
    @param bReplace 단순 데이터 교체여부 (스크롤에의한 데이터 교체시, 모션 생략을 위함)
*/
void DataTableRowObject::ResumeText(BOOL bMotion, unsigned long Delay)
{
    wchar_t* pStr;
    size_t TextLen;
    int CurrentX = 0;
    POSITION TmpPos;
    MOTION_INFO mi;
    eDataTableMotionPattern Patt;

    if (!bMotion) Patt = eDataTableMotionPattern::eInitRowText_Default;
    else          Patt = pParent->Motion.MotionInitRowText;

    switch (Patt) {
    case eDataTableMotionPattern::eInitRowText_Default: {
        for (int i = 0; i < nColumn; i++) {
            pStr = ppRealData[i];
            TmpPos = { (float)CurrentX, 0, (float)pWidth[i], Pos.y2 };
            CurrentX += pWidth[i];
            TextLen = wcslen(pStr);
            ppText[i]->Init(pStr, TextLen, TmpPos, pParent->Motion.ColorRowText, 0);
            mi = InitMotionInfo(eMotionForm::eMotion_None, Delay, 0);
            ppText[i]->addLenMotion(mi, TRUE, 0, TextLen);
        }
        break;
    }

    case eDataTableMotionPattern::eInitRowText_Typing: {
        unsigned long MotionGap = pParent->Motion.GapInitRowText;

        for (int i = 0; i < nColumn; i++) {
            pStr = ppRealData[i];
            TmpPos = { (float)CurrentX, 0, (float)pWidth[i], Pos.y2 };
            CurrentX += pWidth[i];
            TextLen = wcslen(pStr);

            ppText[i]->Init(pStr, TextLen, TmpPos, pParent->Motion.ColorRowText, 0);
            mi = InitMotionInfo(eMotionForm::eMotion_Linear1, Delay + (i * MotionGap), pParent->Motion.PitchInitRowText);
            ppText[i]->addLenMotion(mi, FALSE, 0, TextLen);
        }
    }

    }
}

void DataTableRowObject::PauseText(unsigned long Delay)
{
    wchar_t* pStr;
    size_t TextLen;
    int CurrentX = 0;
    POSITION TmpPos;
    MOTION_INFO mi;
    eDataTableMotionPattern Patt;
    unsigned long pitch = pParent->Motion.PitchPauseRowText;
    unsigned long DelayOffset;

    if (!ppRealData) return;

    switch (pParent->Motion.MotionPauseRowText) {
    case eDataTableMotionPattern::ePauseRowText_Default:
        for (int i = 0; i < nColumn; i++) {
            pStr = ppRealData[i];
            TmpPos = { (float)CurrentX, 0, (float)pWidth[i], Pos.y2 };
            CurrentX += pWidth[i];
            mi = InitMotionInfo(eMotionForm::eMotion_None, Delay, pitch);
            ppText[i]->SetColor(mi, TRUE, ALL_ZERO, ALL_ZERO);
        }
        break;

    case eDataTableMotionPattern::ePauseRowText_FlickLinear:
        for (int i = 0; i < nColumn; i++) {
            pStr = ppRealData[i];
            DelayOffset = pParent->Motion.GapPauseRowText * i;
            mi = InitMotionInfo(eMotionForm::eMotion_Pulse1, Delay + DelayOffset, pitch);
            ppText[i]->SetColor(mi, TRUE, ALL_ZERO, ALL_ZERO);
        }
        break;

    case eDataTableMotionPattern::ePauseRowText_FlickRandom:
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

void DataTableRowObject::ResumeBg(BOOL bMotion, unsigned long Delay)
{
    eDataTableMotionPattern patt;
    MOTION_INFO mi;
    unsigned long Pitch = pParent->Motion.PitchInitRowBg;
    D2D_COLOR_F TargetColor = MainDataIdx & 1 ? pParent->Motion.ColorRowBg1 : pParent->Motion.ColorRowBg2;

    if (bMotion) patt = pParent->Motion.MotionInitRowBg;
    else patt = eDataTableMotionPattern::eInitRowBg_Default;

    switch (patt) {
    case eDataTableMotionPattern::eInitRowBg_Default:
        pBackgroundBox->Init(Pos, ALL_ZERO);
        mi = InitMotionInfo(eMotionForm::eMotion_None, Delay, Pitch);
        pBackgroundBox->SetColor(mi, TRUE, ALL_ZERO, TargetColor);
        break;
    }
}

void DataTableRowObject::PauseBg(unsigned long Delay)
{
    eDataTableMotionPattern patt;
    MOTION_INFO mi;
    unsigned long Pitch = pParent->Motion.PitchInitRowBg;

    switch (pParent->Motion.MotionPauseRowBg) {
    case eDataTableMotionPattern::ePauseRowBg_Default:
        mi = InitMotionInfo(eMotionForm::eMotion_None, Delay, Pitch);
        pBackgroundBox->SetColor(mi, TRUE, ALL_ZERO, ALL_ZERO);
        break;
    }
}



void DataTableRowObject::pause(int nDelay)
{
    PauseBg(nDelay + pParent->Motion.DelayPauseRowBg);
    PauseSelect(TRUE, nDelay + pParent->Motion.DelayPauseRowSelect);
    PauseText(nDelay + pParent->Motion.DelayPauseRowText);
}

void DataTableRowObject::resume(int nDelay)
{
    ResumeBg(TRUE, nDelay + pParent->Motion.DelayInitRowBg);
    ResumeSelect(nDelay + pParent->Motion.DelayInitRowSelect);
    ResumeText(TRUE, nDelay + pParent->Motion.DelayInitRowText);
}

BOOL DataTableRowObject::update(unsigned long time)
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

void DataTableRowObject::render()
{
    ID2D1RenderTarget* pRT = pParent->pRenderTarget;

    pBackgroundBox->render(pRT);
    pSelectBox->render(pRT);
    pHighlightBox->render(pRT);
    for (int i = 0; i < nColumn; i++) {
        ppText[i]->render(pRT);
        //ppColLine[i]->render(pRT);
    }
}
