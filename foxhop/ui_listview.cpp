#include "./include/ui_listview.hpp"
#include "./include/ui_system.hpp"

UI_ListviewBase::UI_ListviewBase(UISystem* pUISys, pfnUIHandler pfnCallback, POSITION Pos, BOOL MultiSelect)
{

}

/**
    @brief �⺻ ����Ʈ�� �޼��� �ڵ鷯
    @remark ����� ���� ���ν����� �⺻ �ڵ鷯 ���� �� ȣ��ȴ�.
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