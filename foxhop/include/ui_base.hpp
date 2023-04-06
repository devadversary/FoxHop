#pragma once
#include <windows.h>

#include "object_pool.hpp"
#include "prop_box.hpp"
#include "prop_line.hpp"
#include "prop_text.hpp"

/*�⺻������ WM_XXX �޼����� ����ϵ�,
  ������ �ʿ��� UI�޼����� ���⼭ �����Ѵ�.*/
#define UIM_CREATE        WM_USER + 100
#define UIM_DELETE        WM_USER + 101
#define UIM_FOCUS         WM_USER + 109
#define UIM_UNFOCUS       WM_USER + 110
#define UIM_SETPOS        WM_USER + 111
#define UIM_SETCOLOR      WM_USER + 112
#define UIM_SETTEXT       WM_USER + 113
#define UIM_PAUSE         WM_USER + 114 /*UI �ӽ� �Ҹ�*/
#define UIM_RESUME        WM_USER + 115 /*UI �簳*/
#define UIM_MOUSEMOVE     WM_USER + 116
#define UIM_MOUSEON       WM_USER + 117
#define UIM_MOUSELEAVE    WM_USER + 118
#define UIM_LIST_SELECT   WM_USER + 300

#define ALL_ZERO { 0,0,0,0 }

/*�ʿ� ��ü ���漱��*/
class UI;
class UISystem;

/**
    @brief UI���� �޼��� �ڵ鷯
    @param pUI     �޼����� ���� UI. WndProc�� HWND���ڿ� ����ϴ�.
    @param Message ���޵� �޼���
    @param wParam  �޼��������� �߰�����. WndProc�� WPARAM
    @param lParam  �޼��������� �߰�����. WndProc�� LPARAM
    @remark Win32API�� WndProc �ݹ��Լ��� ���� ����.
*/
typedef void (*pfnUIHandler)(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam);

/**
    @brief ��ü�� ������Ʈ �ڵ鷯
    @param nTime ms���� ����ð�
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
*/
class UI {
public :
    UISystem*          uiSys;
    ID2D1RenderTarget* pRenderTarget;
    BOOL               Focusable;       /**< ��Ŀ���� ���� �� �ִ� UI�ΰ�?*/
    BOOL               MouseEventCheck; /**< ���콺 ���� ���� üũ ����. �⺻�� TRUE (���콺 �ڵ鸵�� ���ʿ��� UI�� FALSE : UI_Panel���� �ش� UI�� skip)*/
    POSITION           uiPos;           /**< ���̽��κ����� UI ������*/
    eUIMotionState     uiMotionState;   /**< UI�� ��ǻ��� (�������� ���� �̺�Ʈ�� ���� �ϱ� ���� : ex-������� ������ ���콺������� ��� ����)*/
    pfnUIHandler       DefaultHandler;  /**< UI ���� ���� �⺻�޼��� �ڵ鷯*/
    pfnUIHandler       MessageHandler;  /**< UI ���� ���� ����ڸ޼��� �ڵ鷯*/

public:
    UI() {
        uiSys           = 0;
        pRenderTarget   = 0;
        Focusable       = FALSE;
        uiPos           = {0,0,0,0};
        MouseEventCheck = TRUE;
        uiMotionState   = eUIMotionState::eUMS_Null;
        DefaultHandler  = NULL;
        MessageHandler  = NULL;
    }

    ~UI() {}
    virtual BOOL update(unsigned long time) = 0;
    virtual void render() = 0;
    virtual void pause(int nDelay) = 0;
    virtual void resume(int nDelay) = 0;

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