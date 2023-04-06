#pragma once

#include "ui_base.hpp"
#include "ui_theme.hpp"
#include <vector>

class UISystem; /*UI�ý��� Ŭ������ ���漱��*/

/**
    @brief ����Ʈ�䰡 ������ �� ������
*/
typedef struct _st_TextRow
{
    bool  bSelected;   /**< �� ���� ����*/
    void* pDataStruct; /**< ������ ����� ������ ����ü */
} TEXTSHEET_ROW;

#if 0
/**
    @brief ����Ʈ�� ���� ��Ÿ�� ���� Ŭ����.
    @remark ���� Ŀ���� ����Ʈ�並 ���鶧 �� Ŭ������ ��ӹ޾� �����Ѵ�.
    @n      �پ��� ����� ����Ʈ�並 ������ �ϱ� ���� �Ű�ü.
    @n      UI_ListviewBase�� ���� ���Ѱ��� �����ȴ�. (�� ȭ�鿡 ���̴� ���� + 1��)
*/
class UI_TextField {
public:
    int Height; /**< �� ����*/
    int* width;
public:
    UI_LvField(int nColCnt);
    ~UI_LvField();
    virtual BOOL update(unsigned long time) = 0;
    virtual void render() = 0;
    virtual void pause(int nDelay) = 0;
    virtual void resume(int nDelay) = 0;
};

/**
    @brief �⺻ �����Ǵ� ���ڿ� ������ Ŭ������ ����Ʈ�� ��
*/
class UI_LvFieldString : public UI_LvField {
public:
};
#endif

/**
    @brief �ؽ�Ʈ ����Ʈ�� Ŭ����
*/
class UI_TextSheet : public UI {
private:
    BOOL MultiSelectMode; /**< FALSE:���� ���� / TRUE:������ ���� ���*/
    unsigned long long DataCount; /**< ���� ������ ����*/
    std::vector<TEXTSHEET_ROW> MainDataPool;
    unsigned long long ScrollPixel; /**< ��ũ�ѵ� �ȼ�. �ִ� ��ũ�� ���̴�, ������ ���� x �� ����*/
    int ColCnt; /**< �� ����*/
    wchar_t** ColName; /**< �� �̸�*/
    int* ColWidth; /**< �� ������ �ȼ�*/
    int RowWidth; /**< �� ���� �ȼ�*/
    /*
    ViewStartPtr;
    ViewDataPool;
    */

private:
    //void InputMotion(eListviewAction Action, UITheme* Theme, unsigned int nDelay, void* param);
    static void DefaultTextSheetProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam);

public:
    UI_TextSheet(UISystem* pUISys, pfnUIHandler pfnCallback, POSITION Pos, unsigned int ColumnCount, wchar_t** ColumnNameList, unsigned int* ColumnWidth, unsigned int RowHeight, BOOL MultiSelect);
    ~UI_TextSheet() {};

    void pause(int nDelay);
    void resume(int nDelay);
    void Destroy();
    BOOL update(unsigned long time);
    void render();

public: /*UI�� �ɼ� �ż���*/
    void AddRow(void* pUserData);
    void DelRow(int index);
    void DelSelectedRow();
    //void MoveLine(int index);
    void EnsureRow(int index);
};
