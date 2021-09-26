#pragma once

/*��ȯ ������ �����ϱ� ����, UISystem ������� ��� ���� UI �� ���� �ϵ��� �Ѵ�.*/

#include "d2dassist.hpp"
#include "ui_base.hpp"
#include "ui_panel.hpp"
#include "ui_button.hpp"
#include "ui_listview.hpp"
#include "ui_static.hpp"
#include "ui_divisionline.hpp"

/*TODO : ���Ŀ� ��Ʈ�� ���� ���� �����ϵ��� �ϵ��ڵ��� ������ �� �ִ� ������ �����Ұ�.*/
#define UISYSTEM_FONTNAME_DEFAULT (L"monoMMM_5") /*UI�ý����� �⺻������ ����ϴ� ��Ʈ �̸�*/

/**
    @brief ID���� UI�� ������ ���̺�.
    @remark UI�� ID�� �迭�� �ε����� ����Ѵ�
*/
typedef struct _st_UI_TABLE
{
    UI* pUI;
}UI_TABLE;

/**
    @brief UI ���� ������
*/
enum class UIType {
    eUI_Panel = 1,    /**< �г� ��Ʈ��. �� �ȿ� �ٸ� UI�� ��ġ�Ͽ� �ѹ��� �����Ҷ� ���. (�г� �ȿ� �г� ����)*/
    eUI_Button = 100, /**< winAPI�� ��ư ��Ʈ�Ѱ� ���� �뵵*/
    eUI_Static,       /**< ����ƽ ��Ʈ��*/
    eUI_DivLine,      /**< �и���*/
    eUI_List,         /**< ����Ʈ�� ��Ʈ��*/
    eUI_Edit,         /**< ����Ʈ ��Ʈ��*/
    eUI_Scroll,       /**< ��ũ�� �� (���� ����)*/
    eUI_Progress,     /**< ���α׷��� ��*/
    eUI_Graph         /**< �׷��� ��(���� ���� ���� ���..)*/
};

/**
    @brief ��� UI�� �����ϰ� �����ϴ� ������ü
    @remark ������ �ϳ��� �ΰ� ���°��� �ǵ� ������
    @n      ��Ȳ�� ���� ������ �ΰ� �ᵵ �ȴ�.
    @n      ��Ʈ �ʱ�ȭ �۾��� �����ϹǷ�, �ʿ��� ��Ʈ�� �ִٸ�
    @n      ������ �̸� �ε� �صξ���Ѵ�.
*/
class UISystem {
public:
    D2DA_CONTEXT                 D2DA;            /**< D2D ��ü���� ���ؽ�Ʈ (�� �ȿ� �� �ִ�)*/
    IDWriteTextFormat*           TinyTextForm;    /**< ���� �ؽ�Ʈ ��*/
    IDWriteTextFormat*           SmallTextForm;   /**< ���� �ؽ�Ʈ ��*/
    IDWriteTextFormat*           ButtonTextForm;  /**< ��ư ��Ʈ�ѵ��� ���������� ����� �ؽ�Ʈ ��. (winAPI ��ư�� �ؽ�Ʈ ���� ������ ������)*/
    IDWriteTextFormat*           MediumTextForm;  /**< �߰�ũ�� �ؽ�Ʈ ��*/

    ObjectPool<ObjectMotionBox>  ObjPoolBox;
    ObjectPool<ObjectMotionLine> ObjPoolLine;
    ObjectPool<ObjectMotionText> ObjPoolText;

private:
    UI**                         pUITbl;          /**< UI �ν��Ͻ��� ��Ƶ� �迭*/
    int                          nUITblCnt;       /**< �迭�� ���� (UI�� ID�� �迭�� �ε����� ���)*/
    UI_ButtonFactory*            pUIButtonFactory; /**< ��ưUI ���丮*/

public:
    UISystem();
    ~UISystem();
    void Init(HWND hWnd, unsigned int nMaxUI);
    UI*  CreateUI(UIType type, unsigned int nID, POSITION pos, wchar_t* pText, int nDelay, pfnUIHandler callback);
    void SendUIMessage(UI* pUI, UINT Message, void* param);
    void SendUIMessageByID(unsigned int nID, UINT Message, void* param);
    void ReleaseUI(unsigned int nID);
};