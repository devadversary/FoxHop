#pragma once

#include "ui_base.hpp"
#include <list>

class UISystem; /*UI�ý��� Ŭ������ ���漱��*/
enum class UIType;

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
    UI*                  pMouseOverUI; /**< ���콺 �̺�Ʈ �ڵ鸵�� ���� �ӽú���*/
    UI*                  pFocusedUI;   /**< ��Ŀ�̵� UI*/
public:
    UI_Panel(UISystem* pUISys, ID2D1RenderTarget* pRT, pfnUIHandler pfnCallback, POSITION Pos, int nDelay = 0);
    ~UI_Panel();
    void pause(int nDelay);
    void resume(int nDelay);
    void Destroy();
    BOOL update(unsigned long time);
    void render();

public: /*UI�� �ɼ� �ż���*/
    UI_Panel* CreatePanel() {};
    UI*       CreateUI(UIType type, POSITION pos, wchar_t* pText, int nDelay, pfnUIHandler callback);
    void      DefaultMouseHandler(POINT pt, UINT Message, void* param);
};

#if 0 /*Panel ���� ���丮�� ���� �ʿ� ���������� ����.*/
/**
    @brief �г� ���� ���丮
*/
class UI_PanelFactory {
private:
    UISystem*          pUISys;        /**< UI�ý��� �ν��Ͻ�*/
    ID2D1RenderTarget* pRenderTarget; /**< ������ Ÿ��*/

public:
    UI_PanelFactory(UISystem* pUISystem, ID2D1RenderTarget* pRT) {};
    ~UI_PanelFactory(){};

    UI*  CreateUI(POSITION Pos, wchar_t* pText, int nDelay, pfnUIHandler pfnCallback);
};
#endif