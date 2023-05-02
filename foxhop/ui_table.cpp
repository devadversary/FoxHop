#include "./include/ui_table.hpp"
#include "./include/ui_system.hpp"

#define TEXTSHEET_DEFAULT_WIDTH     150 /*열 가로폭 기본 픽셀*/
#define TEXTSHEET_DEFAULT_ROWHEIGHT 20  /*행 세로폭 기본 픽셀*/

UI_Table::UI_Table(UISystem* pUISys, pfnUIHandler pfnCallback, POSITION Pos, unsigned int ColumnCount, wchar_t** ColumnNameList, unsigned int* ColumnWidth, unsigned int HeaderHeight, unsigned int RowHeight, BOOL MultiSelect)
{
    /*기본 UI 멤버 셋팅*/
    uiSys = pUISys;
    pRenderTarget = pUISys->D2DA.pRenTarget;
    Focusable = TRUE;
    DefaultHandler = DefaultTableProc;
    MessageHandler = pfnCallback;
    uiPos = Pos;
    uiMotionState = eUIMotionState::eUMS_PlayingVisible;

    /*하위 클래스 멤버 셋팅*/
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

    /*행 생성*/
    ClientHeight = (int)Pos.y2 - HeaderHgt;
    ViewRowCnt = (ClientHeight / RowHgt) + 2; /*자투리 영역도 온전한 1개 취급*/
    for (int i = 0; i < ViewRowCnt; i++) {
        RowObject* obj = new RowObject(pUISys, this, { 0,0,Pos.x2,(float)RowHgt }, ColCnt);
        ViewData.push_back(obj);
    }

    pBoxHeader = new PropBox();
    for (int i = 0; i < ColCnt; i++) ppTextHdr[i] = new PropText();
    pBoxFrame = new PropBox();

    /* 초기화 / 재개 모션 기입 */
    switch (MotionInit) {
    case eTableMotionPattern::eInit_Default:
    {
        POSITION TmpPos;
        int      WidthOffset = 0;

        TmpPos.x  = uiPos.x;
        TmpPos.y  = uiPos.y;
        TmpPos.x2 = uiPos.x2;
        TmpPos.y2 = HeaderHgt;

        pBoxHeader->Init(pRenderTarget, TmpPos, ColorHeaderBg);
        for (int i = 0; i < ColCnt; i++) {
            TmpPos.x = uiPos.x + WidthOffset;
            TmpPos.x2 = ColWidth[i];
            WidthOffset += ColWidth[i];
            ppTextHdr[i]->Init(pRenderTarget, uiSys->MediumTextForm, ColName[i], 0, TmpPos, ColorHeaderText, wcslen(ColName[i]));
        }
        pBoxFrame->Init(pRenderTarget, uiPos, ColorFrame, FALSE);
        break;
    }

        break;
    }
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
    if (test > x+dx) return FALSE;
    return TRUE;
}

/**
    @brief 기본 텍스트 리스트뷰 메세지 핸들러
    @remark 사용자 지정 프로시저는 기본 핸들러 실행 후 호출된다.
*/
void UI_Table::DefaultTableProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam)
{
    UI_Table* pTable = static_cast<UI_Table*>(pUI);
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

        pt.x = GET_X_LPARAM(lParam);
        pt.y = GET_Y_LPARAM(lParam);
        y = pTable->MousePt.y - pTable->HeaderHgt;
        if (y < 0) break; /*헤더영역 클릭됨*/
        if (pTable->MainDataPool.size() == 0) break;
        idx = ((long long)pTable->CurrScrollPixel + y) / pTable->RowHgt;
        OrderIdx = idx % pTable->ViewRowCnt;
        MainIdx = pTable->ViewData[OrderIdx]->MainDataIdx; /*선택된 행*/
        pTable->MainDataPool[MainIdx].bSelected ^= 1; /*TRUE / FALSE 교대 : 선택 표기 완료*/
        bSel = pTable->MainDataPool[MainIdx].bSelected;

        /* 다중선택모드가 아닐경우 이전 선택 라인은 해제해줘야 한다.*/
        if (bSel) {
            RowObject* pViewRow = pTable->ViewData[OrderIdx];
            pViewRow->bNeedUpdate = TRUE;

            if (pTable->MessageHandler)
                pTable->MessageHandler(pTable, UIM_TABLE_SELECT, (WPARAM)pTable->MainDataPool[MainIdx].ppData, MainIdx);/*어떤 라인이 선택 되었는지에 대한 정보 전달*/

            if (!pTable->MultiSelectMode) {
                /*전에 이미 선택된것이 있을때.*/
                if (pTable->PrevSelMainIdx >= 0) {
                    pTable->MainDataPool[pTable->PrevSelMainIdx].bSelected = FALSE;
                    /*변경 대상이 뷰 영억 안에 있을때엔 모션 재생을 해주어야 함*/
                    if (IsInRange(pTable->CurrMainIndex, pTable->ViewRowCnt, pTable->PrevSelMainIdx)) {
                        long long PrevViewRowIdx = pTable->PrevSelMainIdx % pTable->ViewRowCnt;
                        RowObject* pPrevViewRow  = pTable->ViewData[PrevViewRowIdx];
                        pPrevViewRow->bNeedUpdate = TRUE;
                        pPrevViewRow->bBindComplete = FALSE;
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

        pTable->ViewData[OrderIdx]->bBindComplete = FALSE;
        break;
    }

    case WM_MOUSEWHEEL:
    {
        MOTION_PATTERN patt;
        MOTION_INFO    mi;

        pTable->ScrollPixel -= (GET_WHEEL_DELTA_WPARAM(wParam)/2);
        /*스크롤 경계 관리*/
        if (pTable->ScrollPixel < 0)
            pTable->ScrollPixel = 0;
        if (pTable->ScrollPixel > pTable->MaxScrollPixel)
            pTable->ScrollPixel = pTable->MaxScrollPixel;

        /*스크롤 모션 진행 재시작*/
        pTable->ScrollComp->clearChannel();
        mi = InitMotionInfo(eMotionForm::eMotion_x3_2, 0, pTable->PitchScroll);
        patt = InitMotionPattern(mi, NULL);
        AddChain(&patt, (float*)&pTable->CurrScrollPixel, pTable->CurrScrollPixel, (float)pTable->ScrollPixel);
        pTable->ScrollComp->addChannel(patt);
        break;
    }

    default: break;
    }

    if (UserHandler) UserHandler(pUI, Message, wParam, lParam);
}

/**
    @brief 테이블에 데이터를 추가한다.
    @param ppData 반드시 열 갯수에 맞는 배열을 넘겨줘야 한다. (wchar* [x])
    @n            내부적으로 해당 배열을 malloc후 배열원소인 포인터는 복사된다.
    @param bEnsure 새 데이터가 추가 될 때, 해당 라인으로 자동 스크롤 된다.
*/
void UI_Table::AddData(wchar_t* Data[], BOOL bEnsure = FALSE)
{
    long long TmpScrollPx;
    TABLE_ROW Row;
    size_t    AllocSize;

    Row.bSelected = FALSE;
    AllocSize = sizeof(wchar_t*) * ColCnt;
    Row.ppData = (wchar_t**)malloc(AllocSize);
    memcpy_s(Row.ppData, AllocSize, Data, AllocSize);
    MainDataPool.push_back(Row);
    DataCount++;

    TmpScrollPx = (DataCount * RowHgt) - ClientHeight;
    if (TmpScrollPx <= 0) MaxScrollPixel = 0; /*음수는 없다.*/
    else MaxScrollPixel = TmpScrollPx;
}

void UI_Table::pause(int nDelay)
{

}

void UI_Table::resume(int nDelay)
{
    /* 초기화 / 재개 모션 기입 */
    /*TODO : InputMotion 함수는 사용 하지 않을 예정이므로, resume 함수로 이 코드를 옮긴다.*/
    switch (MotionInit) {
    case eTableMotionPattern::eInit_Default:
    {
        POSITION TmpPos;
        int      WidthOffset = 0;

        TmpPos.x = uiPos.x;
        TmpPos.y = uiPos.y;
        TmpPos.x2 = uiPos.x2;
        TmpPos.y2 = HeaderHgt;

        pBoxHeader->Init(pRenderTarget, TmpPos, ColorHeaderBg);
        for (int i = 0; i < ColCnt; i++) {
            TmpPos.x = uiPos.x + WidthOffset;
            TmpPos.x2 = ColWidth[i];
            WidthOffset += ColWidth[i];
            ppTextHdr[i]->Init(pRenderTarget, uiSys->MediumTextForm, ColName[i], 0, TmpPos, ColorHeaderText, wcslen(ColName[i]));
        }
        pBoxFrame->Init(pRenderTarget, uiPos, ColorFrame, FALSE);
        break;
    }

    }
}

BOOL UI_Table::update(unsigned long time)
{
    RowObject* pViewRow;
    int ValidViewRowCnt;
    int UpdateIdx;
    long long ModIndex, CurrBindIndex;
    BOOL bUpdated = FALSE;
    BOOL bReplace; /*단순 데이터 교체 여부 (이때 모션진행은 항상 Default로 함)*/
    size_t MainDataPoolSize;

    if (uiMotionState == eUIMotionState::eUMS_Hide) return FALSE;

    bUpdated = ScrollComp->update(time); /*스크롤 상태 먼저 업데이트*/

    CurrMainIndex = (long long)CurrScrollPixel / RowHgt;
    ModIndex = CurrMainIndex % ViewRowCnt;
    ValidViewRowCnt = DataCount < ViewRowCnt ? DataCount : ViewRowCnt;
    MainDataPoolSize = MainDataPool.size();

    /*데이터 바인딩*/
    for (int i = 0; i < ValidViewRowCnt; i++) {
        CurrBindIndex = CurrMainIndex + i;
        if (MainDataPoolSize <= CurrBindIndex) break; /*딱뎀상황에선 자투리 접근 X*/
        UpdateIdx = (ModIndex + i) % ViewRowCnt; /*현재 뷰 영역의 인덱스 계산*/
        pViewRow = ViewData[UpdateIdx];

        /*이미 바인드된거 skip*/
        if (pViewRow->bBindComplete && (pViewRow->MainDataIdx == CurrBindIndex)&& !pViewRow->bNeedUpdate) continue;

        /*업데이트가 필요한 상황인지 판단*/
        if (pViewRow->bNeedUpdate) bReplace = FALSE;
        else                       bReplace = TRUE;

        pViewRow->SetData(MainDataPool[CurrBindIndex].ppData, ColWidth, ColCnt, bReplace);
        if(CurrBindIndex & 1) pViewRow->SetBgColor(ColorRowBg2, bReplace);
        else                  pViewRow->SetBgColor(ColorRowBg1, bReplace);

        if (MainDataPool[CurrBindIndex].bSelected) pViewRow->SetFontColor(ColorRowTextSelect, bReplace);
        else                                       pViewRow->SetFontColor(ColorRowText, bReplace);

        pViewRow->SetSelect(MainDataPool[CurrBindIndex].bSelected, ColorRowBgSelect, bReplace);
        pViewRow->bBindComplete = TRUE;
        pViewRow->bNeedUpdate = FALSE; /*업데이트가 필요했다면, 요구사항 들어줬으니 다시 FALSE*/
        pViewRow->MainDataIdx   = CurrBindIndex;
    }

    /*뷰 행 업데이트*/
    for (int i = 0; i < ValidViewRowCnt; i++) {
        UpdateIdx = (ModIndex + i) % ViewRowCnt; /*현재 뷰 영역의 인덱스 계산*/
        pViewRow = ViewData[UpdateIdx];
        bUpdated |= pViewRow->update(time);
    }

    /*헤더 업데이트*/
    pBoxHeader->update(time);
    for (int i = 0; i < ColCnt; i++) bUpdated |= ppTextHdr[i]->update(time);

    if (!bUpdated) {
        if (uiMotionState == eUIMotionState::eUMS_PlayingHide)
            uiMotionState = eUIMotionState::eUMS_Hide;

        else if (uiMotionState == eUIMotionState::eUMS_PlayingVisible)
            uiMotionState = eUIMotionState::eUMS_Visible;
        return FALSE;
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

    pRenderTarget->PushAxisAlignedClip(ClipRect, D2D1_ANTIALIAS_MODE::D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
    pRenderTarget->GetTransform(&OldMat); /*기존 행렬 백업*/
    TmpMat = OldMat;
    TmpMat.dx += Pos.x;
    TmpMat.dy += HeaderHgt;
    TmpMat.dy += Pos.y;
    ModScroll = (unsigned long)CurrScrollPixel % RowHgt;
    /* TmpMat._31 는 X, TmpMat._32 는 Y */
    TmpMat.dy -= (float)ModScroll; /* Mod 음수값부터 높이를 차차 더해가며 렌더링*/
    ValidViewRowCnt = DataCount < ViewRowCnt ? DataCount : ViewRowCnt;
    ModIndex = CurrMainIndex% ViewRowCnt;

    MainDataPoolSize = MainDataPool.size();
    for (int i = 0; i < ValidViewRowCnt; i++) {
        if (MainDataPoolSize <= CurrMainIndex + i) break; /*딱뎀시 자투리 X*/
        pRenderTarget->SetTransform(TmpMat);
        idx = (ModIndex + i) % ValidViewRowCnt;
        ViewData[idx]->render(); /*행 렌더링*/
        TmpMat.dy += (float)RowHgt;
    }

    pRenderTarget->SetTransform(OldMat); /*기존 행렬 복구*/
    pRenderTarget->PopAxisAlignedClip();

    pBoxHeader->render(pRenderTarget);
    for (int i = 0; i < ColCnt; i++) ppTextHdr[i]->render(pRenderTarget);
    pBoxFrame->render(pRenderTarget);
#if 0 /*스크롤 모션값 테스트용*/
    {
        static ID2D1SolidColorBrush* Brush = 0;
        static ID2D1SolidColorBrush* Brush2 = 0;
        wchar_t test[54];
        if (!Brush) pRenderTarget->CreateSolidColorBrush({1,1,1,1}, &Brush );
        if (!Brush2) pRenderTarget->CreateSolidColorBrush({1,1,1,0.1}, &Brush2 );
        wsprintfW(test, L"%d", (int)CurrScrollPixel);
        pRenderTarget->DrawRectangle({ uiPos.x, uiPos.y, uiPos.x+uiPos.x2, uiPos.y + uiPos.y2 }, Brush2);
        pRenderTarget->DrawTextW(test, wcslen(test), uiSys->MediumTextForm, { uiPos.x, uiPos.y, uiPos.x + uiPos.x2, uiPos.y + uiPos.y2 },Brush );
    }
#endif
}











/**
    @brief 행 객체를 생성해 반환한다.
*/
RowObject::RowObject(UISystem* pUISys, UI_Table* pParentTable, POSITION pos, unsigned int ColCnt)
{
    if (!pParentTable) return;
    if (!ColCnt) ColCnt = 1;
    pParent = pParentTable;
    nColumn = ColCnt;
    uiSys = pUISys;
    Pos = pos;
    ppText = (PropText**)malloc(sizeof(PropText*) * ColCnt);
    ppColLine = (PropLine**)malloc(sizeof(PropLine*) * ColCnt);
    if (!ppText || !ppColLine) return;
    for (int i = 0; i < ColCnt; i++) {
        ppText[i] = new PropText();
        ppColLine[i] = new PropLine();
    }
    pBackgroundBox = new PropBox();
    pMouseoverBox = new PropBox();
    pSelectBox = new PropBox();
    pHighlightBox = new PropBox();
}

RowObject::~RowObject()
{
    for (int i = 0; i < nColumn; i++) {
        delete ppText[i];
        delete ppColLine[i];
    }
    delete pBackgroundBox;
    delete pMouseoverBox;
    delete pSelectBox;
    delete pHighlightBox;
    free(ppText);
    free(ppColLine);
}

void RowObject::SetSelect(BOOL bSel, D2D1_COLOR_F Color, BOOL bReplace)
{
    D2D1_COLOR_F TargetColor = bSel ? Color : D2D1_COLOR_F{ 0.f, 0.f, 0.f, 0.f };
    unsigned long pitch = bSel ? pParent->PitchSelect : pParent->PitchUnselect;
    MOTION_INFO mi;
    eTableMotionPattern Patt;

    if (bReplace) Patt = eTableMotionPattern::eSelect_Default;
    else Patt = pParent->MotionSelect;

    switch (Patt) {
    case eTableMotionPattern::eSelect_Default:
        pSelectBox->Init(uiSys->D2DA.pRenTarget, Pos, TargetColor);
        break;

    case eTableMotionPattern::eSelect_Linear:
        pSelectBox->Init(uiSys->D2DA.pRenTarget, Pos, TargetColor);
        mi = InitMotionInfo(eMotionForm::eMotion_Linear1, 0, pitch);
        pSelectBox->addMovementMotion(mi, TRUE, { Pos.x, Pos.y,0, Pos.y2 }, Pos);
        break;

    case eTableMotionPattern::eSelect_Decel:
        pSelectBox->Init(uiSys->D2DA.pRenTarget, Pos, TargetColor);
        mi = InitMotionInfo(eMotionForm::eMotion_x3_2, 0, pitch);
        pSelectBox->addMovementMotion(mi, TRUE, { Pos.x, Pos.y,0, Pos.y2 }, Pos);
        break;
    }
}

void RowObject::SetHighlight(D2D1_COLOR_F Color)
{
    //
}

void RowObject::SetBgColor(D2D1_COLOR_F Color, BOOL bReplace)
{
    pBackgroundBox->Init(uiSys->D2DA.pRenTarget, Pos, Color);
}

void RowObject::SetFontColor(D2D1_COLOR_F Color, BOOL bReplace)
{
    MOTION_INFO mi;
    eTableMotionPattern Patt;

    if (bReplace) Patt = eTableMotionPattern::eText_Default;
    else Patt = pParent->MotionRowText;

    switch (Patt) {
    case eTableMotionPattern::eText_Default:
        mi = InitMotionInfo(eMotionForm::eMotion_None, 0, 0);
        for (int i = 0; i < nColumn; i++) ppText[i]->SetColor(mi, FALSE, Color, Color);
        break;
    }
}

/**
    @brief 행의 데이터를 교체한다.
    @param ppData 교체할 데이터뭉치
    @param pWidth 폭 배열
    @param nCnt 데이터의 갯수
    @param bReplace 단순 데이터 교체여부 (스크롤에의한 데이터 교체시, 모션 생략을 위함)
*/
void RowObject::SetData(wchar_t** ppData, int* pWidth, int nCnt, BOOL bReplace)
{
    wchar_t* pStr;
    size_t TextLen;
    int CurrentX = 0;
    POSITION TmpPos;
    eTableMotionPattern Patt;

    //if (bReplace) Patt = pParent
    //Patt = pParent->MotionInit;
    /*부모 UI_Table 의 모션 정보에 따라 프랍을 초기화 한다.*/
    /*TODO : 현재는 모두 Default지만, switch-case 로 나눠야 한다.*/
    for (int i = 0; i < nCnt; i++) {
        pStr = ppData[i];
        TmpPos = {(float)CurrentX, 0, (float)pWidth[i], Pos.y2};
        CurrentX += pWidth[i];
        TextLen = wcslen(pStr);
        ppText[i]->Init(uiSys->D2DA.pRenTarget, uiSys->MediumTextForm, pStr, TextLen, TmpPos, pParent->ColorRowText, TextLen);
    }
}

void RowObject::pause(int nDelay)
{
    switch (pParent->MotionPause) {

    }
}

void RowObject::resume(int nDelay)
{
    switch (pParent->MotionInit) {

    }
}

BOOL RowObject::update(unsigned long time)
{
    BOOL bResult = FALSE;

    for (int i = 0; i < nColumn; i++) {
        bResult |= ppText[i]->update(time);
        bResult |= ppColLine[i]->update(time);
    }
    bResult |= pBackgroundBox->update(time);
    bResult |= pMouseoverBox->update(time);
    bResult |= pSelectBox->update(time);
    bResult |= pHighlightBox->update(time);
    return bResult;
}

void RowObject::render()
{
    ID2D1RenderTarget* pRT = uiSys->D2DA.pRenTarget;

    pBackgroundBox->render(pRT);
    //pMouseoverBox->render(pRT);
    pSelectBox->render(pRT);
    //pHighlightBox->render(pRT);
    for (int i = 0; i < nColumn; i++) {
        ppText[i]->render(pRT);
        //ppColLine[i]->render(pRT);
    }
}
