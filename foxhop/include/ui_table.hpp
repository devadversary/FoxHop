#pragma once

#include "ui_base.hpp"
#include "ui_theme.hpp"
#include <vector>

class UISystem; /*UI�ý��� Ŭ������ ���漱��*/

/**
    @brief ���̺��� ������ �� ������
*/
typedef struct _st_TableRow
{
    bool      bSelected; /**< �� ���� ����*/
    wchar_t** ppData;    /**< ������ */
} TABLE_ROW;

#if 0
/**
    @brief ���̺� ���� ��Ÿ�� ���� Ŭ����.
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
    @brief ���̺� Ŭ����
*/
class UI_Table : public UI {
private:
    std::vector<TABLE_ROW> MainDataPool;    /**< �����ִ� ��� ������*/
    BOOL                   MultiSelectMode; /**< FALSE:���� ���� / TRUE:������ ���� ���*/
    unsigned long long     DataCount;       /**< ���� ������ ����*/
    unsigned long long     ScrollPixel;     /**< ��ũ�ѵ� �ȼ�. �ִ� ��ũ�� ���̴�, ������ ���� x �� ����*/
    unsigned long long     MaxScrollPixel;  /**< ������ ���� x �� ���� = �ִ� ��ũ�Ѱ��� �ȼ�*/
    int       ColCnt;   /**< �� ����*/
    wchar_t** ColName;  /**< �� �̸�*/
    int*      ColWidth; /**< �� ������ �ȼ�*/
    int       RowWidth; /**< �� ���� �ȼ�*/

    std::vector<PropLine> ObjLine;
    std::vector<PropBox>  ObjBox;
    std::vector<PropText> ObjText;
    /*
    ViewStartPtr;
    ViewDataPool;
    */

private:
    //void InputMotion(eListviewAction Action, UITheme* Theme, unsigned int nDelay, void* param);
    static void DefaultTableProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam);

public:
    UI_Table(UISystem* pUISys, pfnUIHandler pfnCallback, POSITION Pos, unsigned int ColumnCount, wchar_t** ColumnNameList, unsigned int* ColumnWidth, unsigned int RowHeight, BOOL MultiSelect);
    ~UI_Table() {};

    void pause(int nDelay);
    void resume(int nDelay);
    void Destroy();
    BOOL update(unsigned long time);
    void render();

public: /*UI�� �ɼ� �ż���*/

    void AddData(wchar_t* Data[]);
};
