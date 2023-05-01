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
    @brief ���̺� ��� Ÿ�� ���
*/
enum class eTableMotionPattern {
    eInit_Default = 0,       /**< ����: ��� ����*/

    ePause_Default = 0,      /**< �Ҹ�: ��� ����*/

    eMouseover_Default = 0,  /**< ���콺���� : ��� ����*/

    eMouseleave_Default = 0, /**< ���콺��Ż : ��� ����*/

    eSelect_Default = 0,     /**< ���� : ��� ����*/
    eSelect_Linear,          /**< ���� : ���� ���*/
    eSelect_Decel,           /**< ���� : ���� ���*/

    eUnselect_Default = 0,   /**< ���� ���� : ��� ����*/

    eHighlight_Default = 0,  /**< ���̶����� : ��� ����*/

    eText_Default = 0,       /**< �ؽ�Ʈ : ��� ����*/
};

/**
    @brief ���̺� ��� Ÿ��
*/
enum class eTableAction {
    eAction_Init = 0,   /**< �ʱ�ȭ ���*/
    eAction_Pause,      /**< �Ҹ� ���*/
    eAction_Mouseover,  /**< ���콺 ���� ���*/
    eAction_Mouseleave, /**< ���콺 ��Ż ���*/
    eAction_Select,     /**< �� ���� ���*/
    eAction_Unselect,   /**< �� ���� ���*/
    eAction_Highlight,  /**< ���̶�����*/
};

class RowObject;

/**
    @brief ���̺� Ŭ����
*/
class UI_Table : public UI {
public:
    eTableMotionPattern MotionInit      = eTableMotionPattern::eInit_Default;
    eTableMotionPattern MotionPause     = eTableMotionPattern::ePause_Default;
    eTableMotionPattern MotionRowText   = eTableMotionPattern::eText_Default;
    eTableMotionPattern MotionMouseover = eTableMotionPattern::eMouseover_Default;
    eTableMotionPattern MotionMouseleave = eTableMotionPattern::eMouseover_Default;
    eTableMotionPattern MotionSelect    = eTableMotionPattern::eSelect_Default;
    eTableMotionPattern MotionUnselect  = eTableMotionPattern::eUnselect_Default;
    unsigned long PitchInit      = 0;
    unsigned long PitchPause     = 0;
    unsigned long PitchMouseover = 0;
    unsigned long PitchSelect    = 0;
    unsigned long PitchUnselect  = 0;
    unsigned long PitchScroll    = 200; /**< ��ũ�� ��� �ð��� 0.2�� �⺻��.*/
    unsigned long PitchRowText = 0; /**< �� ��� �ؽ�Ʈ ��� �ð�*/
    unsigned long PitchRowBg   = 0; /**< �� ��� ���� ��� �ð�*/
    D2D1_COLOR_F  ColorFrame          = { 0.f ,0.f, 0.f, 1.f };     /**< ������*/
    D2D1_COLOR_F  ColorHeaderBg       = { 0.9f, 0.9f, 0.9f, 1.f };  /**< ���� ȸ��*/
    D2D1_COLOR_F  ColorHeaderText     = { 0.f, 0.f, 0.f, 1.f };     /**< ������*/
    D2D1_COLOR_F  ColorRowBg1         = { 1.f, 1.f, 1.f, 1.f };     /**< ���*/
    D2D1_COLOR_F  ColorRowBg2         = { 0.96f, 0.96f, 0.96f, 1.f }; /**< ���� ȸ��*/
    D2D1_COLOR_F  ColorRowText        = { 0.f, 0.f, 0.f, 1.f };     /**< ������*/
    D2D1_COLOR_F  ColorRowLine        = { 0.f, 0.f, 0.f, 1.f };     /**< ������*/
    D2D1_COLOR_F  ColorRowBgMouseover = { 0.f, 0.f, 0.f, 0.1f };    /**< ���� 0.1 ������*/
    D2D1_COLOR_F  ColorRowBgHighlight = { 0.f, 0.f, 0.f, 1.f };     /**< ������*/
    D2D1_COLOR_F  ColorRowBgSelect    = { 0.2f, 0.56f, 1.f, 1.f }; /**< ���� �Ķ���*/
    D2D1_COLOR_F  ColorRowTextSelect  = { 1.f, 1.f, 1.f, 1.f }; /**< ���*/

private:
    ComponentMotion*        ScrollComp;   /**< ��ũ�� ��� ������ ���� ������Ʈ*/
    PropBox*  pBoxHeader;
    PropText** ppTextHdr;
    PropBox*  pBoxFrame;
    std::vector<TABLE_ROW>  MainDataPool; /**< �����ִ� ��� ������*/
    std::vector<RowObject*> ViewData;     /**< ȭ�鿡 ���� �� ������ (ȭ�� ũ�⸸ŭ�� ����)*/
    POINT     MousePt;         /**< ���콺 ��ġ*/
    BOOL      MultiSelectMode; /**< FALSE:���� ���� / TRUE:������ ���� ���*/
    long long DataCount;       /**< ���� ������ ����*/
    float     CurrScrollPixel; /**< ���� ��� �������� ��ũ�� �ȼ� (���������� ����, �������� ���)*/
    long long CurrMainIndex;   /**< ���� ������ �� �������� ���� �ε��� (4�̸�, 4,5,6,7...n �� �������ȴ�.)*/
    long long ScrollPixel;     /**< ��ũ�ѵ� �ȼ�. �ִ� ��ũ�� ���̴�, ������ ���� x �� ����*/
    long long MaxScrollPixel;  /**< ������ ���� x �� ���� = �ִ� ��ũ�Ѱ��� �ȼ�*/
    int       ColCnt;          /**< �� ����*/
    wchar_t** ColName;         /**< �� �̸�*/
    int*      ColWidth;        /**< �� ������ �ȼ�*/
    int       HeaderHgt;       /**< ��� ���� �ȼ�*/
    int       RowHgt;          /**< �� ���� �ȼ�*/
    int       ClientHeight;    /**< ��� ���̸� ������ ���� ���̺� ���� ����*/
    int       ViewRowCnt;      /**< ȭ�鿡 �������� �� ����*/
    long long PrevSelMainIdx;  /**< ������ �����ߴ� �� ������ �ε��� (�����̸� �̼���)*/

private:
    static void DefaultTableProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam);

public:
    UI_Table(UISystem* pUISys, pfnUIHandler pfnCallback, POSITION Pos, unsigned int ColumnCount, wchar_t** ColumnNameList, unsigned int* ColumnWidth, unsigned int HeaderHeight, unsigned int RowHeight, BOOL MultiSelect);
    ~UI_Table() {};

    void pause(int nDelay);
    void resume(int nDelay);
    BOOL update(unsigned long time);
    void render();

public: /*UI�� �ɼ� �ż���*/
    void Resize(POSITION Pos);
    void AddData(wchar_t* Data[], BOOL bEnsure);
};

/**
    @brief ���̺��� ȭ�鿡 �������� �� ������
*/
class RowObject : public UI {
public: 
    unsigned long long MainDataIdx = 0;   /**< �� ������ ����Ű�� MainDataPool �������� �ε���*/
    BOOL               bBindComplete = FALSE; /**< ���ε� �Ϸ� ����*/
    BOOL               bNeedUpdate   = FALSE; /**< ������Ʈ �ʿ俩��*/
private:
    UI_Table*  pParent;
    POSITION   Pos;
    int        nColumn;        /**< �� ����*/
    PropText** ppText;
    PropLine** ppColLine;      /**< �� ���м�*/
    PropBox*   pBackgroundBox; /**< ����*/
    PropBox*   pMouseoverBox;  /**< ���콺������*/
    PropBox*   pSelectBox;     /**< ���� ����*/
    PropBox*   pHighlightBox;  /**< ���̶���Ʈ �� (���뺯�� ���� ��ǿ� ���� ����)*/

public:
    RowObject(UISystem* pUISys, UI_Table* pParentTable, POSITION pos, unsigned int ColCnt);
    ~RowObject();
    void SetData(wchar_t** ppData, int* pWidth, int nCnt, BOOL bReplace);
    void SetSelect(BOOL bSel, D2D1_COLOR_F Color, BOOL bReplace);
    void SetHighlight(D2D1_COLOR_F Color);
    void SetBgColor(D2D1_COLOR_F Color, BOOL bReplace);
    void SetFontColor(D2D1_COLOR_F Color, BOOL bReplace);

    void pause(int nDelay);
    void resume(int nDelay);
    BOOL update(unsigned long time);
    void render();
};