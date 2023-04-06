#include "./include/ui_textsheet.hpp"
#include "./include/ui_system.hpp"

#define TEXTSHEET_DEFAULT_WIDTH     150 /*열 가로폭 기본 픽셀*/
#define TEXTSHEET_DEFAULT_ROWHEIGHT 20  /*행 세로폭 기본 픽셀*/

UI_TextSheet::UI_TextSheet(UISystem* pUISys, pfnUIHandler pfnCallback, POSITION Pos, unsigned int ColumnCount, wchar_t** ColumnNameList, unsigned int* ColumnWidth, unsigned int RowHeight, BOOL MultiSelect)
{
    /*기본 UI 멤버 셋팅*/
    uiSys = pUISys;
    pRenderTarget = pUISys->D2DA.pRenTarget;
    Focusable = TRUE;
    DefaultHandler = DefaultTextSheetProc;
    MessageHandler = pfnCallback;
    uiPos = Pos;
    uiMotionState = eUIMotionState::eUMS_PlayingVisible;

    /*하위 클래스 멤버 셋팅*/
    MultiSelectMode = MultiSelect;
    ScrollPixel = 0;
    DataCount = 0;

    /* 열 정보 셋팅 */
    ColCnt = ColumnCount;
    ColName = (wchar_t**)malloc(sizeof(wchar_t*) * ColCnt);
    ColWidth = (int*)malloc(sizeof(int) * ColCnt);
    RowWidth = RowHeight ? RowHeight : TEXTSHEET_DEFAULT_ROWHEIGHT;
    if (!ColName || !ColWidth) return; /*TODO : 예외 처리 필요*/

    for (int i = 0; i < ColCnt; i++) {
        ColName[i] = (wchar_t*)_wcsdup(ColumnNameList[i]);
        if (!ColumnWidth) ColWidth[i] = TEXTSHEET_DEFAULT_WIDTH;
        else ColWidth[i] = ColumnWidth[i];
    }


    DefaultHandler(this, UIM_CREATE, NULL, NULL); /*UI생성 메세지 전송*/
}

/**
    @brief 기본 텍스트 리스트뷰 메세지 핸들러
    @remark 사용자 지정 프로시저는 기본 핸들러 실행 후 호출된다.
*/
void UI_TextSheet::DefaultTextSheetProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam)
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

void UI_TextSheet::AddRow(void* ppData)
{

}

BOOL UI_TextSheet::update(unsigned long time)
{
    return FALSE;
}

void UI_TextSheet::pause(int nDelay)
{

}

void UI_TextSheet::resume(int nDelay)
{

}

void UI_TextSheet::render()
{

}