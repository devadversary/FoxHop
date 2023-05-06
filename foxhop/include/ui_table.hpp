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
    bool      bTextMotionPlayed; /**< ����� ���� �Ǿ����� ����*/
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
    eText_Typing,            /**< �ؽ�Ʈ : Ÿ����*/
};

class RowObject;

class UI_Table_MotionParam {
public:
    eTableMotionPattern MotionInit;
    eTableMotionPattern MotionPause;
    eTableMotionPattern MotionRowText;
    eTableMotionPattern MotionMouseover;
    eTableMotionPattern MotionMouseleave;
    eTableMotionPattern MotionSelect;
    eTableMotionPattern MotionUnselect;
    unsigned long PitchInit;
    unsigned long PitchPause;
    unsigned long PitchMouseover;
    unsigned long PitchSelect;
    unsigned long PitchUnselect;
    unsigned long PitchScroll;
    unsigned long PitchRowOneText;
    unsigned long PitchRowAllText;
    unsigned long PitchRowBg;
    D2D1_COLOR_F  ColorFrame;
    D2D1_COLOR_F  ColorHeaderBg;
    D2D1_COLOR_F  ColorHeaderText;
    D2D1_COLOR_F  ColorRowBg1;
    D2D1_COLOR_F  ColorRowBg2;
    D2D1_COLOR_F  ColorRowText;
    D2D1_COLOR_F  ColorRowLine;
    D2D1_COLOR_F  ColorRowBgMouseover;
    D2D1_COLOR_F  ColorRowBgHighlight;
    D2D1_COLOR_F  ColorRowBgSelect;
    D2D1_COLOR_F  ColorRowTextSelect;

public:
    UI_Table_MotionParam() {
        MotionInit = eTableMotionPattern::eInit_Default;
        MotionPause = eTableMotionPattern::ePause_Default;
        MotionRowText = eTableMotionPattern::eText_Default;
        MotionMouseover = eTableMotionPattern::eMouseover_Default;
        MotionMouseleave = eTableMotionPattern::eMouseover_Default;
        MotionSelect = eTableMotionPattern::eSelect_Default;
        MotionUnselect = eTableMotionPattern::eUnselect_Default;
        PitchInit = 0;
        PitchPause = 0;
        PitchMouseover = 0;
        PitchSelect = 0;
        PitchUnselect = 0;
        PitchScroll = 200;
        PitchRowOneText = 0;
        PitchRowAllText = 0;
        PitchRowBg = 0;
        ColorFrame = { 0.f ,0.f, 0.f, 1.f };
        ColorHeaderBg = { 0.9f, 0.9f, 0.9f, 1.f };
        ColorHeaderText = { 0.f, 0.f, 0.f, 1.f };
        ColorRowBg1 = { 1.f, 1.f, 1.f, 1.f };
        ColorRowBg2 = { 0.96f, 0.96f, 0.96f, 1.f };
        ColorRowText = { 0.f, 0.f, 0.f, 1.f };
        ColorRowLine = { 0.f, 0.f, 0.f, 1.f };
        ColorRowBgMouseover = { 0.f, 0.f, 0.f, 0.1f };
        ColorRowBgHighlight = { 0.f, 0.f, 0.f, 1.f };
        ColorRowBgSelect = { 0.2f, 0.56f, 1.f, 1.f };
        ColorRowTextSelect = { 1.f, 1.f, 1.f, 1.f };
    }
};
/**
    @brief ���̺� Ŭ����
*/
class UI_Table : public UI {
public:
    std::vector<TABLE_ROW>  MainDataPool; /**< �����ִ� ��� ������*/
    UI_Table_MotionParam Motion;

private:
    ComponentMotion*        ScrollComp;   /**< ��ũ�� ��� ������ ���� ������Ʈ*/
    PropBox*  pBoxHeader;
    PropText** ppTextHdr;
    PropBox*  pBoxFrame;
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
    UI_Table(UISystem* pUISys, pfnUIHandler pfnCallback, POSITION Pos, unsigned int ColumnCount, wchar_t** ColumnNameList, unsigned int* ColumnWidth, unsigned int HeaderHeight, unsigned int RowHeight, BOOL MultiSelect, UI_Table_MotionParam MotionParam = UI_Table_MotionParam());
    ~UI_Table() {};

    void pause(int nDelay);
    void resume(int nDelay);
    BOOL update(unsigned long time);
    void render();

public: /*UI�� �ɼ� �ż���*/
    void Resize(POSITION Pos);
    void AddData(wchar_t* Data[], BOOL bAutoScroll);
    void SetScroll(long long TargetScrollPx);
};

/**
    @brief ���̺��� ȭ�鿡 �������� �� ������
*/
class RowObject : public UI {
public: 
    unsigned long long MainDataIdx = -1;   /**< �� ������ ����Ű�� MainDataPool �������� �ε���*/

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
    void SetData(wchar_t** ppData, int* pWidth, int nCnt, BOOL bMotion);
    void SetSelectBox(BOOL bSel, D2D1_COLOR_F Color, BOOL bMotion);
    void SetHighlight(D2D1_COLOR_F Color);
    void SetBgColor(D2D1_COLOR_F Color, BOOL bMotion);
    void SetFontColor(D2D1_COLOR_F Color, BOOL bMotion);
    void OnBind(unsigned long long TargetDataIdx, int* pColWidth, BOOL bNeedUpdate = FALSE);
    void OnSelectEvent();
    void pause(int nDelay);
    void resume(int nDelay);
    BOOL update(unsigned long time);
    void render();
};