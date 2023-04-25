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
    uiMotionState = eUIMotionState::eUMS_Null;
    //uiMotionState = eUIMotionState::eUMS_Visible;

    /*하위 클래스 멤버 셋팅*/
    MultiSelectMode = MultiSelect;
    CurrScrollPixel = 0;
    ScrollPixel = 0;
    MaxScrollPixel = 0;
    DataCount = 0;
    CurrIndex = 0;

    HeaderHgt = HeaderHeight;
    RowHgt = RowHeight ? RowHeight : TEXTSHEET_DEFAULT_ROWHEIGHT;
    ScrollComp = new ComponentMotion;
    /*행 생성*/
    ClientHeight = (int)Pos.y2 - HeaderHgt;
    ViewRowCnt = (ClientHeight / RowHgt) + 1; /*자투리 영역도 온전한 1개 취급*/

    /* 열 정보 셋팅 */
    if (!ColumnCount) ColumnCount = 1; /*최소 1개의 열은 필요함*/
    ColCnt = ColumnCount;
    ColName = (wchar_t**)malloc(sizeof(wchar_t*) * ColCnt);
    ColWidth = (int*)malloc(sizeof(int) * ColCnt);
    if (!ColName || !ColWidth) return; /*TODO : 예외 처리 필요*/

    for (int i = 0; i < ColCnt; i++) {
        ColName[i] = (wchar_t*)_wcsdup(ColumnNameList[i]); /*열 헤더 생성*/
        if (!ColumnWidth) ColWidth[i] = TEXTSHEET_DEFAULT_WIDTH;
        else ColWidth[i] = ColumnWidth[i];
    }

    DefaultHandler(this, UIM_CREATE, NULL, NULL); /*UI생성 메세지 전송*/
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

    case WM_LBUTTONUP:
        break;

    case WM_MOUSEWHEEL:
    {
        MOTION_PATTERN patt;
        MOTION_INFO    mi;

        pTable->ScrollPixel += GET_WHEEL_DELTA_WPARAM(wParam);
        /*스크롤 경계 관리*/
        if (pTable->ScrollPixel < 0)
            pTable->ScrollPixel = 0;
        if (pTable->ScrollPixel > pTable->MaxScrollPixel - pTable->ClientHeight)
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
    TABLE_ROW Row;
    size_t    AllocSize;

    Row.bSelected = FALSE;
    AllocSize = sizeof(wchar_t*) * ColCnt;
    Row.ppData = (wchar_t**)malloc(AllocSize);
    memcpy_s(Row.ppData, AllocSize, Data, AllocSize);
    MainDataPool.push_back(Row);
    DataCount++;
    MaxScrollPixel = (DataCount * RowHgt) - ClientHeight;
    if (MaxScrollPixel < 0) MaxScrollPixel = 0; /*음수는 없다.*/
}

BOOL UI_Table::update(unsigned long time)
{
    ScrollComp->update(time); /*스크롤 상태 먼저 업데이트*/

    /*주요 변수 업데이트*/
    CurrIndex = (long long)CurrScrollPixel / RowHgt;
    return FALSE;
}

void UI_Table::pause(int nDelay)
{

}

void UI_Table::resume(int nDelay)
{

}

void UI_Table::render()
{
    D2D1_RECT_F ClipRect;
    D2D_MATRIX_3X2_F OldMat;
    D2D_MATRIX_3X2_F TmpMat;
    POSITION Pos;
    unsigned long Mod;
    int BaseY;

    Pos = uiPos;
    ClipRect.left = Pos.x;
    ClipRect.top = Pos.y;
    ClipRect.right = Pos.x + Pos.x2;
    ClipRect.bottom = Pos.y + Pos.y2;

    pRenderTarget->PushAxisAlignedClip(ClipRect, D2D1_ANTIALIAS_MODE::D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
    pRenderTarget->GetTransform(&OldMat); /*기존 행렬 백업*/
    TmpMat = OldMat;

    Mod = (unsigned long)CurrScrollPixel % RowHgt;
    /* TmpMat._31 는 X, TmpMat._32 는 Y */
    TmpMat._32 -= (float)Mod; /* Mod 음수값부터 높이를 차차 더해가며 렌더링*/
    for (int i = 0; i < ViewRowCnt; i++) {
        pRenderTarget->SetTransform(TmpMat);
        ViewData[i].render(); /*행 렌더링*/
        TmpMat._32 += (float)RowHgt;
    }

    pRenderTarget->SetTransform(OldMat); /*기존 행렬 복구*/
    pRenderTarget->PopAxisAlignedClip();
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
    uisys = pUISys;
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

/**
    @brief 행 진행시간 재설정
    @n     뷰 영역 진입/이탈시 조정되며
    @n     이탈시엔 INT_MAX 로 지정하면 된다.
*/
void RowObject::SetElapse(unsigned long value)
{
    for (int i = 0; i < nColumn; i++) {
        ppText[i]->setRuntime(value);
        ppColLine[i]->setRuntime(value);
    }
    pBackgroundBox->setRuntime(value);
    pMouseoverBox->setRuntime(value);
    pSelectBox->setRuntime(value);
    pHighlightBox->setRuntime(value);
}

void RowObject::SetSelect(BOOL bSel, D2D1_COLOR_F Color)
{
    D2D1_COLOR_F TargetColor = bSel ? Color : D2D1_COLOR_F{ 0.f, 0.f, 0.f, 0.f };
    unsigned long pitch = bSel ? pParent->PitchSelect : pParent->PitchUnselect;
    MOTION_INFO mi;

    switch (pParent->MotionSelect) {
    case eTableMotionPattern::eSelect_Default:
        pSelectBox->Init(uiSys->D2DA.pRenTarget, Pos, TargetColor);
        break;
    }
}

void RowObject::SetHighlight(D2D1_COLOR_F Color)
{
    //
}

void RowObject::SetBgColor(D2D1_COLOR_F Color)
{
    pBackgroundBox->Init(uiSys->D2DA.pRenTarget, Pos, Color);
}

void RowObject::SetData(wchar_t** ppData, int* pWidth, int nCnt, int Height)
{
    wchar_t* pStr;
    int CurrentX = 0;
    POSITION TmpPos;

    for (int i = 0; i < nCnt; i++) {
        pStr = ppData[i];
        TmpPos = {0, (float)CurrentX, (float)pWidth[i], (float)Height};
        CurrentX += pWidth[i];
        ppText[i]->Init(uiSys->D2DA.pRenTarget, uiSys->SmallTextForm, pStr, wcslen(pStr), TmpPos, pParent->ColorRowText, 0);
    }
}

void RowObject::pause(int nDelay)
{
}

void RowObject::resume(int nDelay)
{
}

BOOL RowObject::update(unsigned long time)
{
    BOOL bResult;

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

    for (int i = 0; i < nColumn; i++) {
        ppText[i]->render(pRT);
        ppColLine[i]->render(pRT);
    }
    pBackgroundBox->render(pRT);
    pMouseoverBox->render(pRT);
    pSelectBox->render(pRT);
    pHighlightBox->render(pRT);
}
