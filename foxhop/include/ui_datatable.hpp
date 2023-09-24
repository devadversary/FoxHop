#pragma once

#include "ui_base.hpp"
#include <vector>

class UISystem; /*UI�ý��� Ŭ������ ���漱��*/

/**
    @brief ���̺��� ������ �� ������
*/
typedef struct _st_DataTableRow
{
    bool      bSelected; /**< �� ���� ����*/
    bool      bTextMotionPlayed; /**< ����� ���� �Ǿ����� ����*/
    wchar_t** ppData;    /**< ������ */
} DATATABLE_ROW;

/**
    @brief ���̺� ��� Ÿ�� ���
*/
enum class eDataTableMotionPattern {
    eInitTableFrame_Default = 0,
    eInitTableFrame_Flick,
    eInitTableFrame_ExpendAllDirFlick,
    ePauseTableFrame_Default,
    ePauseTableFrame_Flick,
    ePauseTableFrame_ExpendAllDirFlick,

    eInitTableBg_Default,
    ePauseTableBg_Default,

    eInitTableHeaderBg_Default,
    eInitTableHeaderBg_Linear,
    eInitTableHeaderBg_LinearReverse,
    ePauseTableHeaderBg_Default,
    ePauseTableHeaderBg_Linear,
    ePauseTableHeaderBg_LinearReverse,

    eInitTableHeaderText_Default,
    eInitTableHeaderText_SlideIn,
    ePauseTableHeaderText_Default,
    ePauseTableHeaderText_SlideOut,

    eInitTableRowOrder_Default,
    eInitTableRowOrder_Linear,
    eInitTableRowOrder_Random,
    ePauseTableRowOrder_Default,
    ePauseTableRowOrder_Linear,
    ePauseTableRowOrder_Random,

    eInitTableSelect_Default,
    eInitTableSelect_Linear,
    eInitTableSelect_Decel,
    ePauseTableSelect_Default,
    ePauseTableSelect_Linear,
    ePauseTableSelect_Decel,

    eInitRowBg_Default,
    ePauseRowBg_Default,
    eInitRowText_Default,
    eInitRowText_Typing,
    ePauseRowText_Default,
    ePauseRowText_FlickLinear,
    ePauseRowText_FlickRandom,

    eInitRowHighlight_Blink,
};

class DataTableRowObject;

class UI_DataTable_MotionParam {
public:
    eDataTableMotionPattern MotionInitTableFrame;
    unsigned long PitchInitTableFrame;
    unsigned long DelayInitTableFrame;
    eDataTableMotionPattern MotionPauseTableFrame;
    unsigned long PitchPauseTableFrame;
    unsigned long DelayPauseTableFrame;

    eDataTableMotionPattern MotionInitTableBg;
    unsigned long PitchInitTableBg;
    unsigned long DelayInitTableBg;
    eDataTableMotionPattern MotionPauseTableBg;
    unsigned long PitchPauseTableBg;
    unsigned long DelayPauseTableBg;

    eDataTableMotionPattern MotionInitTableHeaderBg;
    unsigned long PitchInitTableHeaderBg;
    unsigned long DelayInitTableHeaderBg;
    eDataTableMotionPattern MotionPauseTableHeaderBg;
    unsigned long PitchPauseTableHeaderBg;
    unsigned long DelayPauseTableHeaderBg;

    eDataTableMotionPattern MotionInitTableHeaderText;
    unsigned long PitchInitTableHeaderText;
    unsigned long DelayInitTableHeaderText;
    unsigned long GapInitTableHeaderText;
    unsigned long RangeInitTableHeaderText;
    eDataTableMotionPattern MotionPauseTableHeaderText;
    unsigned long PitchPauseTableHeaderText;
    unsigned long DelayPauseTableHeaderText;
    unsigned long GapPauseTableHeaderText;
    unsigned long RangePauseTableHeaderText;

    eDataTableMotionPattern MotionInitTableRowOrder;
    unsigned long PitchInitTableRowOrder;
    unsigned long DelayInitTableRowOrder;
    unsigned long GapInitTableRowOrder;
    unsigned long RangeInitTableRowOrder;
    eDataTableMotionPattern MotionPauseTableRowOrder;
    unsigned long PitchPauseTableRowOrder;
    unsigned long DelayPauseTableRowOrder;
    unsigned long GapPauseTableRowOrder;
    unsigned long RangePauseTableRowOrder;

    eDataTableMotionPattern MotionInitRowSelect;
    unsigned long PitchInitRowSelect;
    unsigned long DelayInitRowSelect;
    eDataTableMotionPattern MotionPauseRowSelect; /*unselect*/
    unsigned long PitchPauseRowSelect;
    unsigned long DelayPauseRowSelect;

    eDataTableMotionPattern MotionInitRowText;
    unsigned long PitchInitRowText;
    unsigned long DelayInitRowText;
    unsigned long GapInitRowText; /*�ؽ�Ʈ�� ������ ���� (��->��)*/
    unsigned long RangeInitRowText; /*���� ��ǽ� ������ ����*/
    eDataTableMotionPattern MotionPauseRowText;
    unsigned long PitchPauseRowText;
    unsigned long DelayPauseRowText;
    unsigned long GapPauseRowText; /*�ؽ�Ʈ�� ������ ���� (��->��)*/
    unsigned long RangePauseRowText; /*���� ��ǽ� ������ ����*/

    eDataTableMotionPattern MotionInitRowBg;
    unsigned long PitchInitRowBg;
    unsigned long DelayInitRowBg;
    eDataTableMotionPattern MotionPauseRowBg;
    unsigned long PitchPauseRowBg;
    unsigned long DelayPauseRowBg;

    eDataTableMotionPattern MotionInitRowHighlight;
    unsigned long PitchInitRowHighlight;

    unsigned long PitchScroll;

    D2D1_COLOR_F  ColorFrame;
    D2D1_COLOR_F  ColorBg;
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
    UI_DataTable_MotionParam() {
        MotionInitTableFrame = eDataTableMotionPattern::eInitTableFrame_Default;
        PitchInitTableFrame = 0;
        DelayInitTableFrame = 0;
        MotionPauseTableFrame = eDataTableMotionPattern::ePauseTableFrame_Default;
        PitchPauseTableFrame = 0;
        DelayPauseTableFrame = 0;

        MotionInitTableBg = eDataTableMotionPattern::eInitTableBg_Default;
        PitchInitTableBg = 0;
        DelayInitTableBg = 0;
        MotionPauseTableBg = eDataTableMotionPattern::ePauseTableBg_Default;
        PitchPauseTableBg = 0;
        DelayPauseTableBg = 0;

        MotionInitTableHeaderBg = eDataTableMotionPattern::eInitTableHeaderBg_Default;
        PitchInitTableHeaderBg = 0;
        DelayInitTableHeaderBg = 0;
        MotionPauseTableHeaderBg = eDataTableMotionPattern::ePauseTableHeaderBg_Default;
        PitchPauseTableHeaderBg = 0;
        DelayPauseTableHeaderBg = 0;

        MotionInitTableHeaderText = eDataTableMotionPattern::eInitTableHeaderText_Default;
        PitchInitTableHeaderText = 0;
        DelayInitTableHeaderText = 0;
        MotionPauseTableHeaderText = eDataTableMotionPattern::ePauseTableHeaderText_Default;
        PitchPauseTableHeaderText = 0;
        DelayPauseTableHeaderText = 0;

        MotionInitTableRowOrder = eDataTableMotionPattern::eInitTableRowOrder_Default;
        PitchInitTableRowOrder = 0;
        DelayInitTableRowOrder = 0;
        GapInitTableRowOrder = 0;
        RangeInitTableRowOrder = 0;
        MotionPauseTableRowOrder = eDataTableMotionPattern::ePauseTableRowOrder_Default;
        PitchPauseTableRowOrder = 0;
        DelayPauseTableRowOrder = 0;
        GapPauseTableRowOrder = 0;
        RangePauseTableRowOrder = 0;

        MotionInitRowSelect = eDataTableMotionPattern::eInitTableSelect_Default;
        PitchInitRowSelect = 0;
        DelayInitRowSelect = 0;
        MotionPauseRowSelect = eDataTableMotionPattern::ePauseTableSelect_Default;
        PitchPauseRowSelect = 0;
        DelayPauseRowSelect = 0;

        MotionInitRowText = eDataTableMotionPattern::eInitRowText_Default;
        PitchInitRowText = 0;
        DelayInitRowText = 0;
        GapInitRowText = 0;
        RangeInitRowText = 0;
        MotionPauseRowText = eDataTableMotionPattern::ePauseRowText_Default;
        PitchPauseRowText = 0;
        DelayPauseRowText = 0;
        GapPauseRowText = 0;
        RangePauseRowText = 0;

        MotionInitRowBg = eDataTableMotionPattern::eInitRowBg_Default;
        PitchInitRowBg = 0;
        DelayInitRowBg = 0;
        MotionPauseRowBg = eDataTableMotionPattern::ePauseRowBg_Default;
        PitchPauseRowBg = 0;
        DelayPauseRowBg = 0;

        MotionInitRowHighlight = eDataTableMotionPattern::eInitRowHighlight_Blink;
        PitchInitRowHighlight = 200;

        PitchScroll = 200;

        ColorFrame = { 0.f ,0.f, 0.f, 1.f };
        ColorBg = { 1.f ,1.f, 1.f, 1.f };
        ColorHeaderBg = { 0.9f, 0.9f, 0.9f, 1.f };
        ColorHeaderText = { 0.f, 0.f, 0.f, 1.f };
        ColorRowBg1 = { 1.f, 1.f, 1.f, 1.f };
        ColorRowBg2 = { 0.96f, 0.96f, 0.96f, 1.f };
        ColorRowText = { 0.f, 0.f, 0.f, 1.f };
        ColorRowLine = { 0.f, 0.f, 0.f, 1.f };
        ColorRowBgMouseover = { 0.f, 0.f, 0.f, 0.1f };
        ColorRowBgSelect = { 0.2f, 0.56f, 1.f, 1.f };
        ColorRowTextSelect = { 1.f, 1.f, 1.f, 1.f };
    }
};

/**
    @brief ���̺� Ŭ����
*/
class UI_DataTable : public UI {
public:
    std::vector<DATATABLE_ROW>  MainDataPool; /**< �����ִ� ��� ������*/
    UI_DataTable_MotionParam Motion;

private:
    SRWLOCK lock;
    ComponentMotion* ScrollComp;   /**< ��ũ�� ��� ������ ���� ������Ʈ*/
    PropBox* pBoxHeader;
    PropText** ppTextHdr;
    PropBox* pBoxFrame;
    std::vector<DataTableRowObject*> ViewData;     /**< ȭ�鿡 ���� �� ������ (ȭ�� ũ�⸸ŭ�� ����)*/
    POINT     MousePt;         /**< ���콺 ��ġ*/
    BOOL      MultiSelectMode; /**< FALSE:���� ���� / TRUE:������ ���� ���*/
    long long DataCount;       /**< ���� ������ ����*/
    long long ValidViewDataCnt;       /**< ���� ������ ����*/
    float     CurrScrollPixel; /**< ���� ��� �������� ��ũ�� �ȼ� (���������� ����, �������� ���)*/
    long long ViewStartIdx;   /**< ���� ������ �� �������� ���� �ε��� (4�̸�, 4,5,6,7...n �� �������ȴ�.)*/
    long long ScrollPixel;     /**< ��ũ�ѵ� �ȼ�. �ִ� ��ũ�� ���̴�, ������ ���� x �� ����*/
    long long MaxScrollPixel;  /**< ������ ���� x �� ���� = �ִ� ��ũ�Ѱ��� �ȼ�*/
    int       ColCnt;          /**< �� ����*/
    wchar_t** ColName;         /**< �� �̸�*/
    int* ColWidth;        /**< �� ������ �ȼ�*/
    int       HeaderHgt;       /**< ��� ���� �ȼ�*/
    int       RowHgt;          /**< �� ���� �ȼ�*/
    int       ClientHeight;    /**< ��� ���̸� ������ ���� ���̺� ���� ����*/
    int       ViewRowCnt;      /**< ȭ��2�� �������� �� ����*/
    long long PrevSelMainIdx;  /**< ������ �����ߴ� �� ������ �ε��� (�����̸� �̼���)*/
    long long PinCount;
private:
    static void DefaultDataTableProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam);

    void ResumeFrame(unsigned long Delay);
    void PauseFrame(unsigned long Delay);

    void ResumeBg(unsigned long Delay);
    void PauseBg(unsigned long Delay);

    void ResumeHeaderBg(unsigned long Delay);
    void PauseHeaderBg(unsigned long Delay);

    void ResumeHeaderText(unsigned long Delay);
    void PauseHeaderText(unsigned long Delay);

    void ResumeRowOrder(unsigned long Delay);
    void PauseRowOrder(unsigned long Delay);

private:
    long long DataIdx2ViewRowIdx(long long DataIdx);
    //RowObject* DataIdx2ViewRow(long long DataIdx);
    BOOL DataIdxIsInScreen(long long DataIdx);

    void SetViewRowBg();
    void SetViewRowData();
    void SetViewRowSelect();
    void SetViewRowHighlight();

public:
    UI_DataTable(UISystem* pUISys, pfnUIHandler pfnCallback, POSITION Pos, unsigned int ColumnCount, wchar_t** ColumnNameList, unsigned int* ColumnWidth, unsigned int HeaderHeight, unsigned int RowHeight, BOOL MultiSelect, UI_DataTable_MotionParam MotionParam = UI_DataTable_MotionParam());
    ~UI_DataTable() {};

    void pause(int nDelay);
    void resume(int nDelay);
    BOOL update(unsigned long time);
    void render();

public: /*UI�� �ɼ� �ż���*/
    void Resize(POSITION Pos);
    //void AddData(wchar_t* Data[], BOOL bAutoScroll = FALSE);
    void AddData(BOOL bMotion, BOOL bAutoScroll, wchar_t* ...);
    void EditData(BOOL bMotion, unsigned long long RowIdx, wchar_t* ...);
    void HighlightData(unsigned long long DataIdx, D2D1_COLOR_F HightlightColor);

    void SetScroll(long long TargetScrollPx);
};

/**
    @brief ���̺��� ȭ�鿡 �������� �� ������
*/
class DataTableRowObject : public UI {
public:
    unsigned long long MainDataIdx = -1;   /**< �� ������ ����Ű�� MainDataPool �������� �ε���*/

private:
    UI_DataTable* pParent;
    POSITION   Pos;
    PropText** ppText;
    PropLine** ppColLine;      /**< �� ���м�*/
    PropBox* pBackgroundBox; /**< ����*/
    PropBox* pSelectBox;     /**< ���� ����*/
    PropBox* pHighlightBox;  /**< ���̶���Ʈ �� (���뺯�� ���� ��ǿ� ���� ����)*/
    wchar_t** ppRealData;
    int* pWidth;
    int        nColumn;        /**< �� ����*/

public:
    DataTableRowObject(UISystem* pUISys, UI_DataTable* pParentTable, POSITION pos, unsigned int ColCnt);
    ~DataTableRowObject();
    void SetSelectBox(BOOL bSel, D2D1_COLOR_F Color, BOOL bMotion);
    void SetHighlight(D2D1_COLOR_F Color);
    void SetTextColor(D2D1_COLOR_F Color, BOOL bMotion);
    void SetText(wchar_t** ppText, int nCnt);
    void OnBind(unsigned long long TargetDataIdx, int* pColWidth, BOOL bNeedUpdate = FALSE);
    void OnSelectEvent();
    void pause(int nDelay);
    void resume(int nDelay);
    BOOL update(unsigned long time);
    void render();

    void ResumeBg(BOOL bMotion, unsigned long Delay);
    void PauseBg(unsigned long Delay);

    void ResumeSelect(unsigned long Delay);
    void PauseSelect(BOOL bMotion, unsigned long Delay);

    void ResumeText(BOOL bMotion, unsigned long Delay);
    void PauseText(unsigned long Delay);
};