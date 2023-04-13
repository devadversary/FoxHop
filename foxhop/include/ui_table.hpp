#pragma once

#include "ui_base.hpp"
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

/**
    @brief ���̺��� ȭ�鿡 �������� �� ������
*/
typedef struct _st_viewrow
{
    unsigned int nDelay;    /**< �ش� ���� ������*/
    PropText**   ppText; /**< �ش� ���� ����ؽ�Ʈ��ü�� (UI_Table�� �� ������ŭ)*/
    PropLine**   ppColLine; /**< �� ���м�*/
    PropBox*     pBackgroundBox; /**< �ش� ���� ���� (�� ���� ���� ���� ���)*/
    PropBox*     pHighlightBox; /**< �ش� ���� ���̶���Ʈ �� (���뺯�� ���� ��ǿ� ���� ����)*/

} VIEWTABLE_ROW;

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
    @brief ���̺� ��� Ÿ�� ���
*/
enum class eTableMotionPattern {
    eInit_Default = 0, /**< ����: ��� ����*/

    ePause_Default = 0, /**< �Ҹ�: ��� ����*/

    eMouseover_Default = 0, /**< ���콺���� : ��� ����*/

    eSelect_Default = 0, /**< ���� : ��� ����*/

    eUnselect_Default = 0, /**< ���� ���� : ��� ����*/
};

/**
    @brief ���̺� ��� Ÿ��
*/
enum class eTableAction {
    eAction_Init = 0,  /**< �ʱ�ȭ ���*/
    eAction_Pause,     /**< �Ҹ� ���*/
    eAction_Mouseover, /**< ���콺 ���� ��� (Ŀ���� ���������� ����)*/
    eAction_Select,    /**< �� ���� ���*/
    eAction_Unselect,  /**< �� ���� ���*/
};

/**
    @brief ���̺� Ŭ����
*/
class UI_Table : public UI {
public:
    eTableMotionPattern MotionInit      = eTableMotionPattern::eInit_Default;
    eTableMotionPattern MotionPause     = eTableMotionPattern::ePause_Default;
    eTableMotionPattern MotionMouseover = eTableMotionPattern::eMouseover_Default;
    eTableMotionPattern MotionSelect    = eTableMotionPattern::eSelect_Default;
    eTableMotionPattern MotionUnselect  = eTableMotionPattern::eUnselect_Default;
    unsigned long PitchInit      = 0;
    unsigned long PitchPause     = 0;
    unsigned long PitchMouseover = 0;
    unsigned long PitchSelect    = 0;
    unsigned long PitchUnselect  = 0;
    D2D1_COLOR_F  ColorHeaderBg       = { 0.9f, 0.9f, 0.9f, 1.f };  /**< ���� ȸ��*/
    D2D1_COLOR_F  ColorHeaderText     = { 0.f, 0.f, 0.f, 1.f };     /**< ������*/
    D2D1_COLOR_F  ColorRowBg1         = { 1.f, 1.f, 1.f, 1.f };     /**< ���*/
    D2D1_COLOR_F  ColorRowBg2         = { 1.f, 1.f, 1.f, 1.f };     /**< ���*/
    D2D1_COLOR_F  ColorRowText        = { 0.f, 0.f, 0.f, 1.f };     /**< ������*/
    D2D1_COLOR_F  ColorRowLine        = { 0.f, 0.f, 0.f, 1.f };     /**< ������*/
    D2D1_COLOR_F  ColorRowBgMouseover = { 0.f, 0.f, 0.f, 0.1f };    /**< ���� 0.1 ������*/
    D2D1_COLOR_F  ColorRowBgHighlight = { 0.f, 0.f, 0.f, 1.f };     /**< ������*/
    D2D1_COLOR_F  ColorRowBgSelect    = { 0.2f, 0.56f, 0.1f, 1.f }; /**< ���� �Ķ���*/

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
    std::vector<VIEWTABLE_ROW> ViewData; /**< ȭ�鿡 ���� �� ������ (ȭ�� ũ�⸸ŭ�� ����)*/

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
