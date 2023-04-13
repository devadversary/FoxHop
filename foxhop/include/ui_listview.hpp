#pragma once

#include "ui_base.hpp"
#include <vector>

class UISystem; /*UI�ý��� Ŭ������ ���漱��*/

/**
    @brief ����Ʈ�䰡 ������ �� ������
*/
typedef struct _st_ListviewRow
{
    bool  bSelected;   /**< �� ���� ����*/
    void* pDataStruct; /**< ������ ����� ������ ����ü */
} LISTVIEW_ROW;

#if 0
/**
    @brief ����Ʈ�� ���� ��Ÿ�� ���� Ŭ����.
    @remark ���� Ŀ���� ����Ʈ�並 ���鶧 �� Ŭ������ ��ӹ޾� �����Ѵ�.
    @n      �پ��� ����� ����Ʈ�並 ������ �ϱ� ���� �Ű�ü.
    @n      UI_ListviewBase�� ���� ���Ѱ��� �����ȴ�. (�� ȭ�鿡 ���̴� ���� + 1��)
*/
class UI_LvField {
public :
    int Height; /**< �� ����*/
    int* width;
public :
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
public :
};
#endif

/**
    @brief ���� ������ �����ϴ� ����Ʈ�� Ŭ����
    @n     UI_Field Ÿ���� �Ѱ�����.
*/
class UI_ListviewBase : public UI {
private:
    BOOL                      MultiSelectMode; /**< FALSE:���� ���� / TRUE:������ ���� ���*/
    std::vector<LISTVIEW_ROW> MainDataPool;
    /*
    ViewStartPtr;
    ViewDataPool;
    */

private:
    //void InputMotion(eListviewAction Action, UITheme* Theme, unsigned int nDelay, void* param);
    static void DefaultListviewProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam);

public:
    UI_ListviewBase(UISystem* pUISys, pfnUIHandler pfnCallback, POSITION Pos, int RowHeight, BOOL MultiSelect);
    ~UI_ListviewBase() {};

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
