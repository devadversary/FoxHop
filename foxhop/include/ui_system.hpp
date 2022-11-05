#pragma once

/*��ȯ ������ �����ϱ� ����, UISystem ������� ��� ���� UI �� ���� �ϵ��� �Ѵ�.*/

#include "d2dassist.hpp"
#include "ui_base.hpp"
#include "ui_panel.hpp"
#include "ui_button.hpp"
#include "ui_listview.hpp"
#include "ui_static.hpp"
#include "ui_fraggedline.hpp"
#include "ui_theme.hpp"

#include <list>

/*TODO : ���Ŀ� ��Ʈ�� ���� ���� �����ϵ��� �ϵ��ڵ��� ������ �� �ִ� ������ �����Ұ�.*/
#define UISYSTEM_FONTNAME_DEFAULT (L"monoMMM_5") /*UI�ý����� �⺻������ ����ϴ� ��Ʈ �̸�*/
#define DEFAULT_MAXUI (256) /*UISystem �ʱ�ȭ��, nMaxUI �� 0���� ������ ������ �⺻ UI ����*/

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

    UITheme*                     Theme;            /**< ���� UI �׸� ����*/
    ObjectPool<PropBox>  ObjPoolBox;
    ObjectPool<PropLine> ObjPoolLine;
    ObjectPool<PropText> ObjPoolText;

    UI_Panel*                    pMainPanel;       /**< ������ ��� UI�� �׷��� ���� �г� (�� �гη� UI�� �����Ѵ�)*/

public:
    UISystem(HWND hWnd);
    ~UISystem();

    UI_Panel* InitMainPanel(HWND hWnd, pfnUIHandler MainPanelProc);
    BOOL SendUIMessage(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam);
    void ReleaseUI();
};