#pragma once

#include "ui_base.hpp"

class UISystem; /*UI�ý��� Ŭ������ ���漱��*/
enum class UIType;

#define MAX_SUB_PANEL_CNT 32 /*�ִ� ���� �г� ���� (�޸� ���� �г� ����)*/
#define MAX_SUB_UI_CNT    32 /*�ִ� ���� UI ���� (�޸� ���� UI ����)*/

typedef struct _st_ui_info
{
    UIType type; /**< �� UI�� ���� Ÿ�� (��ư���� ����Ʈ���� ��)*/
    UI*    pUI;  /**< UI�� �ν��Ͻ�*/
} UI_INFO;

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
    POSITION             PanelPos;                     /**< �г��� ���� ��ǥ*/
    int                  PanelDelay;                   /**< �г��� ������Ʈ ������*/
    int                  nTimeSum;                     /**< �� ���� �ð�. ��Ȳ�� ���� �� ���� ������ �ٲܼ� �ִ�*/
    std::list<UI_Panel>  PanelList;                    /**< �г��� ���� ���� �г��� ����Ʈ*/
    std::list<UI>        UIList;                       /**< �г��� ���� ���� UI�� ����Ʈ*/
    D2D1::Matrix3x2F     transform;                    /**< �ش� �г��� ��ȯ���*/
    UI* pTmpUI;                       /**< ���콺 �̺�Ʈ �ڵ鸵�� ���� �ӽú���*/

public:
    UI_Panel() {};
    UI_Panel(UISystem* pUISys, ID2D1RenderTarget* pRT, int nID, pfnUIHandler pfnCallback);
    ~UI_Panel();
    void Init(POSITION Pos, int nDelay = 0);
    void pause(int nDelay);
    void resume(int nDelay);
    void Destroy();
    void update(unsigned long time);
    void render();

public: /*UI�� �ɼ� �ż���*/
    UI*  CreateUI(UIType type, unsigned int nID, POSITION pos, wchar_t* pText, int nDelay, pfnUIHandler callback);
    void DefaultMouseHandler(POINT pt, UINT Message, void* param);
};
