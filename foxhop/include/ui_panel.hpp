#pragma once

#include "ui_base.hpp"
#include <list>

class UISystem; /*UI�ý��� Ŭ������ ���漱��*/

/**
    @brief �г� UI Ŭ����
    @remark �г� UI�� Ÿ ��Ʈ�� Ŭ�����ʹ� �ٸ���
    @n      �ڽ��� �������ִ� ���� �г��� ����Ʈ��
    @n      ��Ʈ��, ��ǿ�����Ʈ���� ����Ʈ�� ���� �־�� �Ѵ�.
    @n      ���콺 ����, ��Ŀ��, Ű �Է� �� ����� �Է��� ó���ϱ� ���ؼ���
    @n      �� ����Ʈ�� �ʿ��ϱ� �����̴�.
    @n      ��ư, üũ�ڽ�, ����Ʈ, ����ƽ ���� ��Ʈ�ѵ��� �����ΰ�
    @n      ���콺 �Է�, Ŭ���� ���� ������ �� ����.
*/
class UI_Panel : public UI {
private:
    int                  PanelDelay;   /**< �г��� ������Ʈ ������*/
    std::list<UI_Panel*> PanelList;    /**< �г��� ���� ���� �г��� ����Ʈ*/
    std::list<UI*>       UIList;       /**< �г��� ���� ���� UI�� ����Ʈ*/
    D2D1::Matrix3x2F     TransMat;     /**< �ش� �г��� ��ȯ���*/
    BOOL                 IsMainPanel;  /**< ���� �г� ���� (�����г��� �ֻ��� �г� �ϳ�������)*/
    UI*                  pMouseOverUI; /**< ���콺 �̺�Ʈ �ڵ鸵�� ���� �ӽú���*/
    UI*                  pFocusedUI;   /**< ��Ŀ�̵� UI*/

private:
    static void DefaultPanelHandler(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam);
    void        DefaultMouseHandler(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam);

public:
    UI_Panel(UISystem* pUISys, ID2D1RenderTarget* pRT, pfnUIHandler pfnCallback, POSITION Pos, BOOL IsMain = FALSE, int nDelay = 0);
    ~UI_Panel();
    void pause(int nDelay);
    void resume(int nDelay);
    void Destroy();
    BOOL update(unsigned long time);
    void render();

public: /*UI�� �ɼ� �ż���*/
    void RegisterPanel(UI_Panel* pPanel);
    void RegisterUI(UI* pUI);
};