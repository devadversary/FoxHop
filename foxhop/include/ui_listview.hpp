#pragma once

#include "ui_base.hpp"
#include "ui_theme.hpp"
#include <vector>

class UISystem; /*UI�ý��� Ŭ������ ���漱��*/

#define LISTVIEW_ALLOC_ROW_UNIT 100000 /* �޸� �Ҵ��, �߰��Ҵ�� �� ��������*/
#define LISTVIEW_MAX_COLS 256

/**
    @brief ����Ʈ�� Į�� Ÿ�� ������
*/
enum class ListviewColType {
    eType_Null = 0,
    eType_String,
    eType_CustomDraw, /**< �ش� ĭ�� ����ڰ� ���� �׸� �� ����.*/
    eType_UI,         /**< ��ư, üũ�ڽ����� UI�� ���� �� ����.*/
    eType_Undef
};

typedef struct st_ListviewCol {
    ListviewColType ColType;
    wchar_t*        ColName;
    int             DataSize; /**< �������� ũ��. (���ڿ��� ��� "���ڼ� x wchar_t�� ũ��" /
    /*Ŀ���� ��ο��϶��� ���� �ڵ鷯�� �޾ƾ� ��.*/
}LISTVIEW_COL;

/**
    @brief ����Ʈ�信 ���� ������ �� ��
*/
typedef struct st_ListViewDataLine {
    int    nDataCnt;
    void** Data; /**< ���ڿ��̸� char*, �ٸ� �����͸� �ڵ鸵�� �������� ������. */
};

/**
    @brief ����Ʈ���� Į�� ������ Ÿ���� ������ ����ü
*/
typedef struct st_ListviewColInfo {
    char            nColCnt;
    ListviewColType ColType[LISTVIEW_MAX_COLS];
}LISTVIEW_COL_INFO;

/**
    @brief ���� ������ �����ϴ� ����Ʈ�� Ŭ����
    @remark ������ ���丮���� ���ش�.
*/
class UI_Listview : public UI {
private:
    int  Count; /**< ���� ����Ʈ ��� ����*/
    int  MaxRow; /**< �ִ� ����Ʈ ��� ���� (�ø��� ����)*/
    int  ScrollPixel; /**< ���� ��ũ�ѵ� �ȼ� (0���� �� ���̱��� ��ȯ)*/
    BOOL IsMultiSelMode; /**< �������� ���� ���� �� �� �ִ� ����ΰ�?*/
    int* SelectedIndex; /**< ���õ� �ε�����. IsMultiSelMode�� FALSE�̸� �׻� ���Ҵ� 1�����̴�.*/
    //std::vector<> MainDataPool;
    //unsigned int ViewStartIndex;
    //std::vector<> ViewDataPool;

    /*
    ViewStartPtr;
    ViewDataPool;
    */

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
    void SetColType(LISTVIEW_COL_INFO ColInfo);
    void AddRow(wchar_t** ppText);
    void DelRow(int index);
    //void MoveLine(int index);
    void SelectRow(int index);
};
