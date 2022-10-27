#pragma once

/*��ȯ ������ �����ϱ� ����, UISystem ������� ��� ���� UI �� ���� �ϵ��� �Ѵ�.*/

#include "d2dassist.hpp"
#include "ui_base.hpp"
#include "ui_panel.hpp"
#include "ui_button.hpp"
#include "ui_listview.hpp"
#include "ui_static.hpp"
#include "ui_fraggedline.hpp"
#include <list>

/*TODO : ���Ŀ� ��Ʈ�� ���� ���� �����ϵ��� �ϵ��ڵ��� ������ �� �ִ� ������ �����Ұ�.*/
#define UISYSTEM_FONTNAME_DEFAULT (L"monoMMM_5") /*UI�ý����� �⺻������ ����ϴ� ��Ʈ �̸�*/
#define DEFAULT_MAXUI (256) /*UISystem �ʱ�ȭ��, nMaxUI �� 0���� ������ ������ �⺻ UI ����*/

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
    eUI_Undefined = 0, /**< ���� ���ǵ��� ���� UI ��ü (������NULL)*/
    eUI_Custom,        /**< ���� Ŀ���� UI*/
    eUI_Panel,         /**< �г� ��Ʈ��. �� �ȿ� �ٸ� UI�� ��ġ�Ͽ� �ѹ��� �����Ҷ� ���. (�г� �ȿ� �г� ����)*/
    eUI_Button,        /**< winAPI�� ��ư ��Ʈ�Ѱ� ���� �뵵*/
    eUI_Static,        /**< ����ƽ ��Ʈ��*/
    eUI_FragLine,      /**< �и���*/
    eUI_List,          /**< ����Ʈ�� ��Ʈ��*/
    eUI_Edit,          /**< ����Ʈ ��Ʈ��*/
    eUI_Scroll,        /**< ��ũ�� �� (���� ����)*/
    eUI_Progress,      /**< ���α׷��� ��*/
    eUI_Graph          /**< �׷��� ��(���� ���� ���� ���..)*/
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
    HWND                         hBindWnd;             /**< UISystem�� ���ε��� ������ �ڵ�*/
    D2DA_CONTEXT                 D2DA;             /**< D2D ��ü���� ���ؽ�Ʈ (�� �ȿ� �� �ִ�)*/
    IDWriteTextFormat*           TinyTextForm;     /**< ���� �ؽ�Ʈ ��*/
    IDWriteTextFormat*           SmallTextForm;    /**< ���� �ؽ�Ʈ ��*/
    IDWriteTextFormat*           ButtonTextForm;   /**< ��ư ��Ʈ�ѵ��� ���������� ����� �ؽ�Ʈ ��. (winAPI ��ư�� �ؽ�Ʈ ���� ������ ������)*/
    IDWriteTextFormat*           MediumTextForm;   /**< �߰�ũ�� �ؽ�Ʈ ��*/

    ObjectPool<ObjectMotionBox>  ObjPoolBox;
    ObjectPool<ObjectMotionLine> ObjPoolLine;
    ObjectPool<ObjectMotionText> ObjPoolText;

    UI_ButtonFactory*            pUIButtonFactory; /**< ��ưUI ���丮*/
    UI_Panel*                    pMainPanel;       /**< ������ ��� UI�� �׷��� ���� �г� (�� �гη� UI�� �����Ѵ�)*/

public:
    UISystem(HWND hWnd);
    ~UISystem();

    UI_Panel* InitMainPanel(HWND hWnd, pfnUIHandler MainPanelProc);
    BOOL SendUIMessage(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam);
    void ReleaseUI();
};