#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <windowsx.h>

#include "../foxhop/include/alphawindow.hpp"
#include "../foxhop/include/ui_base.hpp"
#include "../foxhop/include/ui_system.hpp"
#include "../foxhop/include/ui_button.hpp"
#include "../foxhop/include/ui_static.hpp"
#include "../foxhop/include/ui_fraggedline.hpp"
#include "../foxhop/include/ui_panel.hpp"

#pragma comment (lib, "../bin/Debug/foxhop.lib")

#define CLASSNAME TEXT("Conspiracy")

#define HOTKEY_ID 666
#define SIZE_SIDEBAR_X 300 /*���̵�� ũ�� ����*/
#define SIZE_SIDEBAR_Y 400

/**
    @brief ���ø����̼ǿ��� ����� �������� ������ ����
*/
typedef struct _stCONSPIRACY_APP
{
    UISystem*              UISys;                   /**< UI �Ŵ�����Ʈ �ý��� ��ü*/
    ID2D1HwndRenderTarget* pRenTarget;              /**< ����Ÿ��*/
    WCHAR                  szProgramPath[MAX_PATH]; /**< ���α׷��� ���� ���*/
    int                    nWindowedState;          /**< ������ â ����. 0=���� 1=���̵�� 2=������ȭ�� 3=��üȭ�� */
}CONSPIRACY_APP;

/**
    @biref WM_HOTKEY �޼��� ó�� ��ƾ
*/
void OnHotkey(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    CONSPIRACY_APP* pApp = (CONSPIRACY_APP*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

    if (!pApp) return;
    if (pApp->nWindowedState != 0) pApp->nWindowedState = 0; /*������°� �ƴϸ� �����ش�.*/
#if 0 /*2020.02.25 - Devadversary : ���̵�� ���¿��� ��üȭ�� ��ú���� ��� ��ȯ*/
    else pApp->nWindowedState = 1; /*������¿��ٸ� ���̵�ٷ� ��ȯ�� ǥ��*/
#else
    else pApp->nWindowedState = 3;
#endif
    SendMessage(hWnd, WM_SIZE, NULL, NULL); /*â ũ�� ����*/
}

/**
    @brief WM_SIZE �޼��� ó�� ��ƾ
*/
void OnSize(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    CONSPIRACY_APP* pApp = (CONSPIRACY_APP*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

    if (!pApp) return;
    if (pApp->nWindowedState) {
#if 0 /*2020.02.25 - Devadversary : ���̵�� ���¿��� ��üȭ�� ��ú���� ��� ��ȯ*/
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
    @brief APP��ü�� �ʱ�ȭ �Ѵ�.
*/
static void AppSettingInit(CONSPIRACY_APP* pApp, HWND hWnd)
{
    if (!pApp) return;
    pApp->nWindowedState = 1; /*ù ����� ���̵�� ���·� ǥ��*/
    /*���� �������� ���� ��� ���*/
    GetModuleFileNameW(NULL, pApp->szProgramPath, MAX_PATH);
    pApp->UISys = new UISystem(hWnd); /*UI�ý����� ������ �ִ� UI������ 200����*/
    pApp->pRenTarget = pApp->UISys->D2DA.pRenTarget;
}

/**
    @brief ��ư �⺻ �ڵ鷯
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
    static ID2D1SolidColorBrush* Brush;
    static D2D1::Matrix3x2F Mat;
    static HWND Child;

    switch (Message) {
    case WM_CREATE:
        RegisterHotKey(hWnd, HOTKEY_ID, MOD_ALT | MOD_SHIFT | MOD_NOREPEAT, 'A'); /* ����Ű ��� - ALT + Shift + A : �뽬���� OnOff*/
        pApp = (CONSPIRACY_APP*)calloc(1, sizeof(CONSPIRACY_APP)); /*���α׷��� �ΰ�ΰ� ����� ���� ���ҽ� �Ҵ�*/
        if (!pApp) {
            MessageBoxA(hWnd, "�ֿ� �޸� �Ҵ� ����", "����", MB_OK);
            PostQuitMessage(0);
            break;
        }
        AppSettingInit(pApp, hWnd);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pApp);
        pUISys = pApp->UISys;
        SetTimer(hWnd, 123123, 1, NULL);

        //
        pApp->pRenTarget->CreateSolidColorBrush({1.f, 0.5f, 0.5f, 0.5f}, &Brush);
        Mat = D2D1::Matrix3x2F::Translation(100,100);
        pApp->pRenTarget->SetTransform(&Mat);
        //

        break;

    case WM_HOTKEY:
        OnHotkey(hWnd, Message, wParam, lParam);
        break;

    case WM_SIZE:
        //OnSize(hWnd, Message, wParam, lParam);
    {
        D2D1_SIZE_U size = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        pApp->pRenTarget->Resize(size);
    }
        break;

    case WM_TIMER:
        InvalidateRect(hWnd, NULL, FALSE);
        break;

    case WM_PAINT: {
        pApp->pRenTarget->BeginDraw();
        pApp->pRenTarget->Clear({ 0.f,0.f,0.f,0.0f });
        Brush->SetColor({ 1.f,0.f,0.f,0.33f});
        pApp->pRenTarget->FillEllipse({ {0.f,0.f}, 50.f,50.f }, Brush);
        Brush->SetColor({ 0.f,1.f,0.f,0.33f });
        pApp->pRenTarget->FillEllipse({ {50.f,0.f}, 50.f,50.f }, Brush);
        Brush->SetColor({ 0.f,0.f,1.f,0.33f });
        pApp->pRenTarget->FillEllipse({ {25.f,42.f}, 50.f,50.f }, Brush);
        pApp->pRenTarget->EndDraw();
    }
        break;

    case WM_MOUSEMOVE:
    {
        int x = GET_X_LPARAM(lParam);
        int y = GET_Y_LPARAM(lParam);
        HDC hDC = GetDC(hWnd);
        char buffer[32];

        sprintf(buffer, "%d, %d", x, y);
        TextOutA(hDC, 10, 10, buffer, strlen(buffer));
        DeleteDC(hDC);
        Mat = D2D1::Matrix3x2F::Translation(x, y);

        pApp->pRenTarget->SetTransform(&Mat);

    }
        break;

    case WM_LBUTTONUP:
        //Child = CreateWindowW(CLASSNAME, CLASSNAME, WS_VISIBLE | WS_POPUP, 0, 0, 200, 130, hWnd, NULL, NULL, NULL);
        break;

    case WM_DESTROY:
        UnregisterHotKey(hWnd, HOTKEY_ID);
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hWnd, Message, wParam, lParam);
}

LRESULT CALLBACK WndProcChild()
{
    return 0;
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


    //hWnd = CreateWindow(CLASSNAME, CLASSNAME, WS_POPUP | WS_VISIBLE, 0, 0, 0, 0, NULL, NULL, hInst, NULL);
    hWnd = CreateWindow(CLASSNAME, CLASSNAME, WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0,600, 400, NULL, NULL, hInst, NULL);
    AlphaWindow(hWnd, WINDOWMODE_BLURBEHIND);

    while (GetMessage(&Message, NULL, NULL, NULL)) {
        TranslateMessage(&Message);
        DispatchMessage(&Message);
    }

    return (int)Message.wParam;
}