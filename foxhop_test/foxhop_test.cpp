#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <windowsx.h>

#include "../foxhop/include/alphawindow.hpp"
#include "../foxhop/include/ui_base.hpp"
#include "../foxhop/include/ui_system.hpp"
#include "../foxhop/include/ui_button.hpp"
#include "../foxhop/include/ui_static.hpp"
#include "../foxhop/include/ui_divisionline.hpp"
#include "../foxhop/include/ui_panel.hpp"

#define CLASSNAME TEXT("Conspiracy")

#define HOTKEY_ID 666
#define SIZE_SIDEBAR_X 300 /*사이드바 크기 설정*/
#define SIZE_SIDEBAR_Y 400

/**
    @brief 애플리케이션에서 사용할 전역적인 데이터 모음
*/
typedef struct _stCONSPIRACY_APP
{
    UISystem               UISys;                   /**< UI 매니지먼트 시스템 객체*/
    ID2D1HwndRenderTarget* pRenTarget;              /**< 렌더타겟*/
    WCHAR                  szProgramPath[MAX_PATH]; /**< 프로그램의 실행 경로*/
    int                    nWindowedState;          /**< 윈도우 창 상태. 0=숨김 1=사이드바 2=윈도우화면 3=전체화면 */
}CONSPIRACY_APP;

/**
    @biref WM_HOTKEY 메세지 처리 루틴
*/
void OnHotkey(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    CONSPIRACY_APP* pApp = (CONSPIRACY_APP*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

    if (!pApp) return;
    if (pApp->nWindowedState != 0) pApp->nWindowedState = 0; /*숨김상태가 아니면 숨겨준다.*/
#if 0 /*2020.02.25 - Devadversary : 사이드바 형태에서 전체화면 대시보드로 방식 전환*/
    else pApp->nWindowedState = 1; /*숨김상태였다면 사이드바로 전환후 표시*/
#else
    else pApp->nWindowedState = 3;
#endif
    SendMessage(hWnd, WM_SIZE, NULL, NULL); /*창 크기 조정*/
}

/**
    @brief WM_SIZE 메세지 처리 루틴
*/
void OnSize(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    CONSPIRACY_APP* pApp = (CONSPIRACY_APP*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

    if (!pApp) return;
    if (pApp->nWindowedState) {
#if 0 /*2020.02.25 - Devadversary : 사이드바 형태에서 전체화면 대시보드로 방식 전환*/
        SetWindowPos(hWnd, HWND_TOPMOST, pApp->ScX - SIZE_SIDEBAR_X, pApp->ScY - SIZE_SIDEBAR_Y, SIZE_SIDEBAR_X, SIZE_SIDEBAR_Y, NULL);
#else
        GetSystemMetrics(SM_CXSCREEN);
        SetWindowPos(hWnd, HWND_TOP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), NULL);
#endif
        ShowWindow(hWnd, SW_SHOW);
    }
    else ShowWindow(hWnd, SW_HIDE);
}

/**
    @brief CONSPIRACY_APP객체를 초기화 한다.
*/
static void AppSettingInit(CONSPIRACY_APP* pApp, HWND hWnd)
{
    if (!pApp) return;
    pApp->nWindowedState = 1; /*첫 실행시 사이드바 형태로 표시*/
    /*현재 실행중인 파일 경로 얻기*/
    GetModuleFileNameW(NULL, pApp->szProgramPath, MAX_PATH);
    pApp->UISys.Init(hWnd, 256); /*UI시스템이 관리할 최대 UI갯수는 200개만*/
    pApp->pRenTarget = pApp->UISys.D2DA.pRenTarget;
}

/**
    @brief 버튼 기본 핸들러
*/
void UI_Handler_Button(UI* pUI, UINT Message, void* param)
{
    UI_Button* pButton = (UI_Button*)pUI;
    MOTION_INFO miColor;

    switch (Message) {
    case UIM_MOUSEON:
        miColor.formular = eMotionForm::eMotion_Pulse1;
        miColor.nDelay = 0;
        miColor.nPitch = 200;
        pButton->setHighlightColor({ 1.f,1.f,1.f,0.5f }, miColor);
        break;

    case UIM_MOUSELEAVE:
        miColor.formular = eMotionForm::eMotion_Pulse1;
        miColor.nDelay = 0;
        miColor.nPitch = 200;
        pButton->setHighlightColor({ 0,0,0,0 }, miColor);
        break;
    }
}

void UIHandler_MainPanel(UI* pUI, UINT Message, void* param)
{

}

LRESULT CALLBACK WndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    static CONSPIRACY_APP* pApp = 0;
    static unsigned long t = 0;
    unsigned int elapsed;
    ID2D1HwndRenderTarget* pRT;
    RECT rt; POINT pt;
    static UI_Panel* pMainPanel;
    UISystem* pUISys;

    switch (Message) {
    case WM_CREATE:
        RegisterHotKey(hWnd, HOTKEY_ID, MOD_ALT | MOD_SHIFT | MOD_NOREPEAT, 'A'); /* 단축키 등록 - ALT + Shift + A : 대쉬보드 OnOff*/
        pApp = (CONSPIRACY_APP*)calloc(1, sizeof(CONSPIRACY_APP)); /*프로그램이 두고두고 사용할 공용 리소스 할당*/
        if (!pApp) {
            MessageBoxA(hWnd, "주요 메모리 할당 실패", "에러", MB_OK);
            PostQuitMessage(0);
            break;
        }
        AppSettingInit(pApp, hWnd);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pApp);

        pUISys = &pApp->UISys;

        /*생성만 한다고 지정한 프로시저가 호출되지는 않음 Init을 별도로 해줄때 호출됨*/
        //pMainPanel = (UI_Panel*)pUISys->CreateUI(UIType::eUI_Panel, 0, UIHandler_MainPanel);
        //rt = { 0, 0, 1920, 1080 };
        //pMainPanel->Init({ (float)rt.left, (float)rt.top, (float)rt.right, (float)rt.bottom }, 0);
        SetTimer(hWnd, 123123, 1, NULL);
        break;

    case WM_HOTKEY:
        OnHotkey(hWnd, Message, wParam, lParam);
        break;

    case WM_SIZE:
        break;

    case WM_TIMER:
        InvalidateRect(hWnd, NULL, FALSE);
        break;

    case WM_PAINT:
        break;

    case WM_DESTROY:
        UnregisterHotKey(hWnd, HOTKEY_ID);
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hWnd, Message, wParam, lParam);
}

int __stdcall WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmdLine, int nShowCmd)
{
    WNDCLASS wc = { 0, };
    HWND hWnd;
    MSG Message;

    wc.hInstance = hInst;
    wc.lpszClassName = CLASSNAME;
    wc.lpfnWndProc = WndProc;
    wc.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpszMenuName = NULL;
    wc.cbClsExtra = NULL;
    wc.cbWndExtra = NULL;
    RegisterClass(&wc);

    hWnd = CreateWindow(CLASSNAME, CLASSNAME, WS_POPUP, 0, 0, 0, 0, NULL, NULL, hInst, NULL);
    AlphaWindow(hWnd, WINDOWMODE_BLURBEHIND);

    while (GetMessage(&Message, NULL, NULL, NULL)) {
        TranslateMessage(&Message);
        DispatchMessage(&Message);
    }

    return (int)Message.wParam;
}