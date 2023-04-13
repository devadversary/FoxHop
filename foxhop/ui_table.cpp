#include "./include/ui_table.hpp"
#include "./include/ui_system.hpp"

#define TEXTSHEET_DEFAULT_WIDTH     150 /*열 가로폭 기본 픽셀*/
#define TEXTSHEET_DEFAULT_ROWHEIGHT 20  /*행 세로폭 기본 픽셀*/

/**
    @brief 행 객체를 생성해 반환한다.
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

UI_Table::UI_Table(UISystem* pUISys, pfnUIHandler pfnCallback, POSITION Pos, unsigned int ColumnCount, wchar_t** ColumnNameList, unsigned int* ColumnWidth, unsigned int RowHeight, BOOL MultiSelect)
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
    ScrollPixel = 0;
    MaxScrollPixel = 0;
    DataCount = 0;

    /* 열 정보 셋팅 */
    if (!ColumnCount) ColumnCount = 1; /*최소 1개의 열은 필요함*/
    ColCnt = ColumnCount;
    ColName = (wchar_t**)malloc(sizeof(wchar_t*) * ColCnt);
    ColWidth = (int*)malloc(sizeof(int) * ColCnt);
    RowWidth = RowHeight ? RowHeight : TEXTSHEET_DEFAULT_ROWHEIGHT;
    if (!ColName || !ColWidth) return; /*TODO : 예외 처리 필요*/

    for (int i = 0; i < ColCnt; i++) {
        ColName[i] = (wchar_t*)_wcsdup(ColumnNameList[i]); /*열 헤더 생성*/
        if (!ColumnWidth) ColWidth[i] = TEXTSHEET_DEFAULT_WIDTH;
        else ColWidth[i] = ColumnWidth[i];
    }

    /*행 생성*/
    int RowCnt = Pos.y2 / RowWidth;


    DefaultHandler(this, UIM_CREATE, NULL, NULL); /*UI생성 메세지 전송*/
}

/**
    @brief 기본 텍스트 리스트뷰 메세지 핸들러
    @remark 사용자 지정 프로시저는 기본 핸들러 실행 후 호출된다.
*/
void UI_Table::DefaultTableProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam)
{
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
        break;

    default: break;
    }

    if (UserHandler) UserHandler(pUI, Message, wParam, lParam);
}

/**
    @brief 테이블에 데이터를 추가한다.
    @param ppData 반드시 열 갯수에 맞는 배열을 넘겨줘야 한다. (wchar* [x])
    @n            내부적으로 해당 배열을 malloc후 배열원소인 포인터는 복사된다.
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
    pRenderTarget->GetTransform(&OldMat); /*기존 행렬 백업*/
    //render
    pRenderTarget->SetTransform(OldMat); /*기존 행렬 복구*/
    pRenderTarget->PopAxisAlignedClip();
}