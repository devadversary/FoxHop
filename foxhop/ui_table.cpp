#include "./include/ui_table.hpp"
#include "./include/ui_system.hpp"

#define TEXTSHEET_DEFAULT_WIDTH     150 /*�� ������ �⺻ �ȼ�*/
#define TEXTSHEET_DEFAULT_ROWHEIGHT 20  /*�� ������ �⺻ �ȼ�*/

UI_Table::UI_Table(UISystem* pUISys, pfnUIHandler pfnCallback, POSITION Pos, unsigned int ColumnCount, wchar_t** ColumnNameList, unsigned int* ColumnWidth, unsigned int HeaderHeight, unsigned int RowHeight, BOOL MultiSelect)
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

    /*�� ����*/
    ClientHeight = (int)Pos.y2 - HeaderHgt;
    ViewRowCnt = (ClientHeight / RowHgt) + 2; /*������ ������ ������ 1�� ���*/
    for (int i = 0; i < ViewRowCnt; i++) {
        RowObject* obj = new RowObject(pUISys, this, { 0,0,Pos.x2,(float)RowHgt }, ColCnt);
        ViewData.push_back(obj);
    }

    pBoxHeader = new PropBox();
    for (int i = 0; i < ColCnt; i++) ppTextHdr[i] = new PropText();
    pBoxFrame = new PropBox();

    /* �ʱ�ȭ / �簳 ��� ���� */
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

        pt.x = GET_X_LPARAM(lParam);
        pt.y = GET_Y_LPARAM(lParam);
        y = pTable->MousePt.y - pTable->HeaderHgt;
        if (y < 0) break; /*������� Ŭ����*/
        if (pTable->MainDataPool.size() == 0) break;
        idx = ((long long)pTable->CurrScrollPixel + y) / pTable->RowHgt;
        OrderIdx = idx % pTable->ViewRowCnt;
        MainIdx = pTable->ViewData[OrderIdx]->MainDataIdx; /*���õ� ��*/
        pTable->MainDataPool[MainIdx].bSelected ^= 1; /*TRUE / FALSE ���� : ���� ǥ�� �Ϸ�*/
        bSel = pTable->MainDataPool[MainIdx].bSelected;

        /* ���߼��ø�尡 �ƴҰ�� ���� ���� ������ ��������� �Ѵ�.*/
        if (bSel) {
            RowObject* pViewRow = pTable->ViewData[OrderIdx];
            pViewRow->bNeedUpdate = TRUE;

            if (pTable->MessageHandler)
                pTable->MessageHandler(pTable, UIM_TABLE_SELECT, (WPARAM)pTable->MainDataPool[MainIdx].ppData, MainIdx);/*� ������ ���� �Ǿ������� ���� ���� ����*/

            if (!pTable->MultiSelectMode) {
                /*���� �̹� ���õȰ��� ������.*/
                if (pTable->PrevSelMainIdx >= 0) {
                    pTable->MainDataPool[pTable->PrevSelMainIdx].bSelected = FALSE;
                    /*���� ����� �� ���� �ȿ� �������� ��� ����� ���־�� ��*/
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
                pTable->MessageHandler(pTable, UIM_TABLE_UNSELECT, NULL, MainIdx);/*� ������ �������� �Ǿ������� ���� ���� ����*/
            pTable->PrevSelMainIdx = -1; /*���� ������ ���� ���� �ε��� ��ȿȭ. (���߼����� ������� �ʾƵ� �ȴ�)*/
        }

        pTable->ViewData[OrderIdx]->bBindComplete = FALSE;
        break;
    }

    case WM_MOUSEWHEEL:
    {
        MOTION_PATTERN patt;
        MOTION_INFO    mi;

        pTable->ScrollPixel -= (GET_WHEEL_DELTA_WPARAM(wParam)/2);
        /*��ũ�� ��� ����*/
        if (pTable->ScrollPixel < 0)
            pTable->ScrollPixel = 0;
        if (pTable->ScrollPixel > pTable->MaxScrollPixel)
            pTable->ScrollPixel = pTable->MaxScrollPixel;

        /*��ũ�� ��� ���� �����*/
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
    @brief ���̺� �����͸� �߰��Ѵ�.
    @param ppData �ݵ�� �� ������ �´� �迭�� �Ѱ���� �Ѵ�. (wchar* [x])
    @n            ���������� �ش� �迭�� malloc�� �迭������ �����ʹ� ����ȴ�.
    @param bEnsure �� �����Ͱ� �߰� �� ��, �ش� �������� �ڵ� ��ũ�� �ȴ�.
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
    if (TmpScrollPx <= 0) MaxScrollPixel = 0; /*������ ����.*/
    else MaxScrollPixel = TmpScrollPx;
}

void UI_Table::pause(int nDelay)
{

}

void UI_Table::resume(int nDelay)
{
    /* �ʱ�ȭ / �簳 ��� ���� */
    /*TODO : InputMotion �Լ��� ��� ���� ���� �����̹Ƿ�, resume �Լ��� �� �ڵ带 �ű��.*/
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
    BOOL bReplace; /*�ܼ� ������ ��ü ���� (�̶� ��������� �׻� Default�� ��)*/
    size_t MainDataPoolSize;

    if (uiMotionState == eUIMotionState::eUMS_Hide) return FALSE;

    bUpdated = ScrollComp->update(time); /*��ũ�� ���� ���� ������Ʈ*/

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

        /*�̹� ���ε�Ȱ� skip*/
        if (pViewRow->bBindComplete && (pViewRow->MainDataIdx == CurrBindIndex)&& !pViewRow->bNeedUpdate) continue;

        /*������Ʈ�� �ʿ��� ��Ȳ���� �Ǵ�*/
        if (pViewRow->bNeedUpdate) bReplace = FALSE;
        else                       bReplace = TRUE;

        pViewRow->SetData(MainDataPool[CurrBindIndex].ppData, ColWidth, ColCnt, bReplace);
        if(CurrBindIndex & 1) pViewRow->SetBgColor(ColorRowBg2, bReplace);
        else                  pViewRow->SetBgColor(ColorRowBg1, bReplace);

        if (MainDataPool[CurrBindIndex].bSelected) pViewRow->SetFontColor(ColorRowTextSelect, bReplace);
        else                                       pViewRow->SetFontColor(ColorRowText, bReplace);

        pViewRow->SetSelect(MainDataPool[CurrBindIndex].bSelected, ColorRowBgSelect, bReplace);
        pViewRow->bBindComplete = TRUE;
        pViewRow->bNeedUpdate = FALSE; /*������Ʈ�� �ʿ��ߴٸ�, �䱸���� ��������� �ٽ� FALSE*/
        pViewRow->MainDataIdx   = CurrBindIndex;
    }

    /*�� �� ������Ʈ*/
    for (int i = 0; i < ValidViewRowCnt; i++) {
        UpdateIdx = (ModIndex + i) % ViewRowCnt; /*���� �� ������ �ε��� ���*/
        pViewRow = ViewData[UpdateIdx];
        bUpdated |= pViewRow->update(time);
    }

    /*��� ������Ʈ*/
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
    for (int i = 0; i < ValidViewRowCnt; i++) {
        if (MainDataPoolSize <= CurrMainIndex + i) break; /*������ ������ X*/
        pRenderTarget->SetTransform(TmpMat);
        idx = (ModIndex + i) % ValidViewRowCnt;
        ViewData[idx]->render(); /*�� ������*/
        TmpMat.dy += (float)RowHgt;
    }

    pRenderTarget->SetTransform(OldMat); /*���� ��� ����*/
    pRenderTarget->PopAxisAlignedClip();

    pBoxHeader->render(pRenderTarget);
    for (int i = 0; i < ColCnt; i++) ppTextHdr[i]->render(pRenderTarget);
    pBoxFrame->render(pRenderTarget);
#if 0 /*��ũ�� ��ǰ� �׽�Ʈ��*/
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
    @brief �� ��ü�� ������ ��ȯ�Ѵ�.
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
    @brief ���� �����͸� ��ü�Ѵ�.
    @param ppData ��ü�� �����͹�ġ
    @param pWidth �� �迭
    @param nCnt �������� ����
    @param bReplace �ܼ� ������ ��ü���� (��ũ�ѿ����� ������ ��ü��, ��� ������ ����)
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
    /*�θ� UI_Table �� ��� ������ ���� ������ �ʱ�ȭ �Ѵ�.*/
    /*TODO : ����� ��� Default����, switch-case �� ������ �Ѵ�.*/
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
