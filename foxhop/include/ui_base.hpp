#pragma once
#include <windows.h>

#include "object_pool.hpp"
#include "object_motion_box.hpp"
#include "object_motion_line.hpp"
#include "object_motion_text.hpp"

#define UIM_CREATE        100
#define UIM_DELETE        101
#define UIM_LBUTTONDOWN   102 /*Ŭ�� �̺�Ʈ*/
#define UIM_LBUTTONUP     103 /*Ŭ�� �̺�Ʈ*/
#define UIM_DOUBLECLICK   104
#define UIM_UPSCROLL      105
#define UIM_DOWNSCROLL    106
#define UIM_KEYDOWN       107
#define UIM_KEYUP         108
#define UIM_FOCUS         109
#define UIM_UNFOCUS       110
#define UIM_SETPOS        111
#define UIM_SETCOLOR      112
#define UIM_SETTEXT       113
#define UIM_PAUSE         114 /*UI �ӽ� �Ҹ�*/
#define UIM_RESUME        115 /*UI �簳*/
#define UIM_MOUSEMOVE     116
#define UIM_MOUSEON       117
#define UIM_MOUSELEAVE    118
#define UIM_LIST_SELECT   300

#define ALL_ZERO { 0,0,0,0 }

/*�ʿ� ��ü ���漱��*/
class UI;
class UISystem;
enum class UIType;

/**
    @brief UI���� �޼��� �ڵ鷯
    @param [IN] UI       �޼����� ���� UI. WndProc�� HWND���ڿ� ����ϴ�.
    @param [IN] Message  ���޵� �޼���
    @param [OPT] parm    �޼��������� �߰�����. WndProc�� WPARAM, LPARAM�� ����ϴ�.
    @remark Win32API�� WndProc �ݹ��Լ��� ���� ����.
*/
typedef void (*pfnUIHandler)(UI* pUI, UINT Message, void* param);

/**
    @brief ��ü�� ������Ʈ �ڵ鷯
    @param [IN] nTime ms���� ����ð�
*/
typedef void (*pfnUpdate)(unsigned long nTime);

/**
    @brief ��ü�� ������ �ڵ鷯
*/
typedef void (*pfnRender)();

/**
    @brief UI ��� ���� ����
*/
enum class eUIMotionState {
    eUMS_Null = 0,       /**< �� ����*/
    eUMS_PlayingVisible, /**< �ʱ�ȭ ��� ������ (delay ��⵵ ����)*/
    eUMS_Visible,        /**< ���ӻ��� (pause ����, init �Ϸ� ���)*/
    eUMS_PlayingHide,    /**< ���� ��� ������*/
    eUMS_Hide            /**< ������� (pause �Ϸ�, init ���� ���)*/
};

/**
    @brief UI �Ϲ�ȭ Ŭ����
    @remark �����Լ��� ������� �ʵ��� �Ѵ�.
    @n      update() render() ���� �ʼ� �ż��带
    @n      �� �� ��ü�� ��ӹ��� ��� Ŭ�������� ������ �����Ѵ�.
    @n      �̷����� �ڵ� Ŀ�ø��� �߻�������, vtbl ���� ����� ���� �� ����.
    @n      �� ���� ����� �ִٸ� ���� �ٶ�.
    @n      �� Ŭ������ ����� �ʱ�ȭ ���� ������� �ʴ°��� ����
*/
class UI {
public :
    UISystem*          uiSys;
    ID2D1RenderTarget* pRenderTarget;
    UIType             uiType;          /**< UIŸ��*/
    POINT              uiBasePoint;     /**< UI ���̽� ��ġ*/
    POSITION           uiPos;           /**< ���̽��κ����� UI ������*/
    int                uiMotion;        /**< UI�� ��� Ÿ�� (UI�������� ���� �ٸ�)*/
    //int                ID;              /**< UI�� ID�� (HWND ó��)*/
    eUIMotionState     uiMotionState;   /**< UI�� ��ǻ��� (�������� ���� �̺�Ʈ�� ���� �ϱ� ���� : ex-������� ������ ���콺������� ��� ����)*/
    pfnUIHandler       DefaultHandler;  /**< UI ���� ���� �⺻�޼��� �ڵ鷯*/
    pfnUIHandler       MessageHandler;  /**< UI ���� ���� ����ڸ޼��� �ڵ鷯*/

public:
    virtual BOOL update(unsigned long time) = 0;
    virtual void render() = 0;
};

/**
    @brief Ư�� ��ǥ�� �簢������ ���� �Ǿ��ִ��� Ȯ��
    @param pos �˻��� ����
    @param pt  �˻��� ��ǥ
    @remark 
*/
inline BOOL IsInRect(POSITION pos, POINT pt)
{
    if (pos.x > pt.x) return FALSE;
    if (pos.y > pt.y) return FALSE;
    if (pos.x + pos.width > pt.x)
        if (pos.y + pos.height > pt.y) 
            return TRUE;
    return FALSE;
}