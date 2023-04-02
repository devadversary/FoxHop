#include "./include/ui_listview.hpp"
#include "./include/ui_system.hpp"

UI_ListviewBase::UI_ListviewBase(UISystem* pUISys, pfnUIHandler pfnCallback, POSITION Pos, BOOL MultiSelect)
{

}

/**
    @brief 기본 리스트뷰 메세지 핸들러
    @remark 사용자 지정 프로시저는 기본 핸들러 실행 후 호출된다.
*/
void UI_ListviewBase::DefaultListviewProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam)
{
    pfnUIHandler UserHandler = pUI->MessageHandler;
    if (!pUI) return;

    switch (Message) {

    }

    if (UserHandler) UserHandler(pUI, Message, wParam, lParam);
}

void UI_ListviewBase::AddRow(void* ppData)
{

}

BOOL UI_ListviewBase::update(unsigned long time)
{
    return FALSE;
}

void UI_ListviewBase::pause(int nDelay)
{

}

void UI_ListviewBase::resume(int nDelay)
{

}

void UI_ListviewBase::render()
{

}