#include "./include/ui_table.hpp"
#include "./include/ui_system.hpp"

#define TEXTSHEET_DEFAULT_WIDTH     150 /*�� ������ �⺻ �ȼ�*/
#define TEXTSHEET_DEFAULT_ROWHEIGHT 20  /*�� ������ �⺻ �ȼ�*/

/**
    @brief �� ��ü�� ������ ��ȯ�Ѵ�.
*/
static VIEWTABLE_ROW CreateRow(int ColCnt)
{
    VIEWTABLE_ROW row;

    if(!ColCnt) return { 0, NULL };
    row.nDelay = 0;
    row.ppText = (PropText**)malloc(sizeof(PropText*) * ColCnt);
    row.ppColLine = (PropLine**)malloc(sizeof(PropLine*) * ColCnt);
    //if (!row.ppText || !row.ppColLine) return { 0, NULL };
    for (int i = 0; i < ColCnt; i++) {
        row.ppText[i] = new PropText();
        row.ppColLine[i] = new PropLine();
    }
    row.pBackgroundBox = new PropBox();
    row.pHighlightBox = new PropBox();
    return row;
}

#if 0
/**
    @brief �� ��ü�� �����Ѵ�
*/
static void DeleteRow(VIEWTABLE_ROW* Row)
{
    VIEWTABLE_ROW row;

    if(!ColCnt) return { 0, NULL };
    row.nDelay = 0;
    row.ppText = (PropText**)malloc(sizeof(PropText*) * ColCnt);
    row.ppColLine = (PropLine**)malloc(sizeof(PropLine*) * ColCnt);
    //if (!row.ppText || !row.ppColLine) return { 0, NULL };
    for (int i = 0; i < ColCnt; i++) {
        delete row.ppText[i];
        delete row.ppColLine[i];
    }
    delete row.pBackgroundBox;
    delete row.pHighlightBox;
}
#endif

UI_Table::UI_Table(UISystem* pUISys, pfnUIHandler pfnCallback, POSITION Pos, unsigned int ColumnCount, wchar_t** ColumnNameList, unsigned int* ColumnWidth, unsigned int HeaderHeight, unsigned int RowHeight, BOOL MultiSelect)
{
    /*�⺻ UI ��� ����*/
    uiSys = pUISys;
    pRenderTarget = pUISys->D2DA.pRenTarget;
    Focusable = TRUE;
    DefaultHandler = DefaultTableProc;
    MessageHandler = pfnCallback;
    uiPos = Pos;
    uiMotionState = eUIMotionState::eUMS_Null;
    //uiMotionState = eUIMotionState::eUMS_Visible;

    /*���� Ŭ���� ��� ����*/
    MultiSelectMode = MultiSelect;
    CurrScrollPixel = 0;
    ScrollPixel = 0;
    MaxScrollPixel = 0;
    DataCount = 0;

    HeaderHgt = HeaderHeight;
    RowHgt = RowHeight ? RowHeight : TEXTSHEET_DEFAULT_ROWHEIGHT;
    ScrollComp = new ComponentMotion;
    /*�� ����*/
    ClientHeight = (int)Pos.y2 - HeaderHgt;
    ViewRowCnt = (ClientHeight / RowHgt) + 2; /*������ ������ ������ 1�� ��� + ���� 1��*/

    /* �� ���� ���� */
    if (!ColumnCount) ColumnCount = 1; /*�ּ� 1���� ���� �ʿ���*/
    ColCnt = ColumnCount;
    ColName = (wchar_t**)malloc(sizeof(wchar_t*) * ColCnt);
    ColWidth = (int*)malloc(sizeof(int) * ColCnt);
    if (!ColName || !ColWidth) return; /*TODO : ���� ó�� �ʿ�*/

    for (int i = 0; i < ColCnt; i++) {
        ColName[i] = (wchar_t*)_wcsdup(ColumnNameList[i]); /*�� ��� ����*/
        if (!ColumnWidth) ColWidth[i] = TEXTSHEET_DEFAULT_WIDTH;
        else ColWidth[i] = ColumnWidth[i];
    }

    DefaultHandler(this, UIM_CREATE, NULL, NULL); /*UI���� �޼��� ����*/
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

    case WM_LBUTTONUP:
        break;

    case WM_MOUSEWHEEL:
    {
        MOTION_PATTERN patt;
        MOTION_INFO    mi;

        pTable->ScrollPixel += GET_WHEEL_DELTA_WPARAM(wParam);
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
    @brief ���̺��� �����͸� �߰��Ѵ�.
    @param ppData �ݵ�� �� ������ �´� �迭�� �Ѱ���� �Ѵ�. (wchar* [x])
    @n            ���������� �ش� �迭�� malloc�� �迭������ �����ʹ� ����ȴ�.
*/
void UI_Table::AddData(wchar_t* Data[])
{
    TABLE_ROW Row;
    size_t    AllocSize;

    Row.bSelected = FALSE;
    AllocSize = sizeof(wchar_t*) * ColCnt;
    Row.ppData = (wchar_t**)malloc(AllocSize);
    memcpy_s(Row.ppData, AllocSize, Data, AllocSize);
    MainDataPool.push_back(Row);
    DataCount++;
}

BOOL UI_Table::update(unsigned long time)
{
    ScrollComp->update(time);
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
    POSITION Pos;

    Pos = uiPos;
    ClipRect.left = Pos.x;
    ClipRect.top = Pos.y;
    ClipRect.right = Pos.x + Pos.x2;
    ClipRect.bottom = Pos.y + Pos.y2;
    pRenderTarget->PushAxisAlignedClip(ClipRect, D2D1_ANTIALIAS_MODE::D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
    pRenderTarget->GetTransform(&OldMat); /*���� ��� ���*/
    //render
    pRenderTarget->SetTransform(OldMat); /*���� ��� ����*/
    pRenderTarget->PopAxisAlignedClip();
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