#pragma once

#include "ui_base.hpp"
#include "ui_theme.hpp"

class UISystem; /*UI�ý��� Ŭ������ ���漱��*/

#define LISTVIEW_ALLOC_ROW_UNIT 100000 /* �޸� �Ҵ��, �߰��Ҵ�� �� ��������*/

/**
    @brief ��ư Ŭ����
    @remark ������ ���丮���� ���ش�.
*/
class UI_Listview : public UI {
private:
    int Count; /**< ���� ����Ʈ ��� ����*/
    int MaxRow; /**< �ִ� ����Ʈ ��� ���� (�ø��� ����)*/
    int ScrollPixel; /**< ���� ��ũ�ѵ� �ȼ�*/
    BOOL IsMultiSelMode; /**< �������� ���� ���� �� �� �ִ� ����ΰ�?*/
    int* SelectedIndex; /**< ���õ� �ε�����. IsMultiSelMode�� FALSE�̸� �׻� ���Ҵ� 1�����̴�.*/

private:
    void InputMotion(eListviewAction Action, UITheme* Theme, unsigned int nDelay, void* param);
    static void DefaultListviewProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam);

public:
    UI_Listview(UISystem* pUISys, pfnUIHandler pfnCallback, POSITION Pos, wchar_t* ColList, int* AdjustColWidth, int RowHeight, int nDelay);
    ~UI_Listview() {};

    void pause(int nDelay);
    void resume(int nDelay);
    void Destroy();
    BOOL update(unsigned long time);
    void render();

public: /*UI�� �ɼ� �ż���*/
    void AddRow(wchar_t** ppText);
    void DelRow(int index);
    void MoveLine(int index);
    void ClickLine(int index);
};
