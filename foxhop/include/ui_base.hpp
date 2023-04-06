#pragma once
#include <windows.h>

#include "object_pool.hpp"
#include "prop_box.hpp"
#include "prop_line.hpp"
#include "prop_text.hpp"

/*기본적으로 WM_XXX 메세지를 사용하되,
  별도로 필요한 UI메세지는 여기서 정의한다.*/
#define UIM_CREATE        WM_USER + 100
#define UIM_DELETE        WM_USER + 101
#define UIM_FOCUS         WM_USER + 109
#define UIM_UNFOCUS       WM_USER + 110
#define UIM_SETPOS        WM_USER + 111
#define UIM_SETCOLOR      WM_USER + 112
#define UIM_SETTEXT       WM_USER + 113
#define UIM_PAUSE         WM_USER + 114 /*UI 임시 소멸*/
#define UIM_RESUME        WM_USER + 115 /*UI 재개*/
#define UIM_MOUSEMOVE     WM_USER + 116
#define UIM_MOUSEON       WM_USER + 117
#define UIM_MOUSELEAVE    WM_USER + 118
#define UIM_LIST_SELECT   WM_USER + 300

#define ALL_ZERO { 0,0,0,0 }

/*필요 개체 전방선언*/
class UI;
class UISystem;

/**
    @brief UI들의 메세지 핸들러
    @param pUI     메세지를 받은 UI. WndProc의 HWND인자와 비슷하다.
    @param Message 전달된 메세지
    @param wParam  메세지에대한 추가정보. WndProc의 WPARAM
    @param lParam  메세지에대한 추가정보. WndProc의 LPARAM
    @remark Win32API의 WndProc 콜백함수와 결이 같다.
*/
typedef void (*pfnUIHandler)(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam);

/**
    @brief 개체별 업데이트 핸들러
    @param nTime ms단위 진행시간
*/
typedef void (*pfnUpdate)(unsigned long nTime);

/**
    @brief 개체별 렌더링 핸들러
*/
typedef void (*pfnRender)();

/**
    @brief UI 모션 진행 상태
*/
enum class eUIMotionState {
    eUMS_Null = 0,       /**< 빈 상태*/
    eUMS_PlayingVisible, /**< 초기화 모션 진행중 (delay 대기도 포함)*/
    eUMS_Visible,        /**< 보임상태 (pause 종료, init 완료 등등)*/
    eUMS_PlayingHide,    /**< 숨김 모션 진행중*/
    eUMS_Hide            /**< 숨김상태 (pause 완료, init 이전 등등)*/
};

/**
    @brief UI 일반화 클래스
*/
class UI {
public :
    UISystem*          uiSys;
    ID2D1RenderTarget* pRenderTarget;
    BOOL               Focusable;       /**< 포커스를 가질 수 있는 UI인가?*/
    BOOL               MouseEventCheck; /**< 마우스 관련 영역 체크 여부. 기본값 TRUE (마우스 핸들링이 불필요한 UI면 FALSE : UI_Panel에서 해당 UI는 skip)*/
    POSITION           uiPos;           /**< 베이스로부터의 UI 포지션*/
    eUIMotionState     uiMotionState;   /**< UI의 모션상태 (적절하지 않은 이벤트를 무시 하기 위함 : ex-생성모션 진행중 마우스오버모션 재생 방지)*/
    pfnUIHandler       DefaultHandler;  /**< UI 마다 가질 기본메세지 핸들러*/
    pfnUIHandler       MessageHandler;  /**< UI 마다 가질 사용자메세지 핸들러*/

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
    @brief 특정 좌표가 사각영역에 포함 되어있는지 확인
    @param pos 검사할 영역
    @param pt  검사할 좌표
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