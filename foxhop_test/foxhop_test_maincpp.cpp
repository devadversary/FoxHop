#include <windows.h>
#include "../foxhop/include/ui_system.hpp"

#pragma comment (lib, "../bin/debug/foxhop.lib")
#define CLASSNAME TEXT("TestModule")

void MainPanelProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam)
{

}

void TestButtProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam)
{
    
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    static UISystem* uiSys;
    static UI_Panel* pMainPanel;
 

    switch (Message) {
    case WM_CREATE:
        uiSys = new UISystem(hWnd);
        pMainPanel = uiSys->InitMainPanel(hWnd, MainPanelProc);
        pMainPanel->CreateUI(UIType::eUI_Button, { 10,10,100,20 }, (wchar_t*)L"TESTBUTT", 1000, TestButtProc);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hWnd, Message, wParam, lParam);
}



int __stdcall WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmdLine, int nShowCmd)
{
    WNDCLASS wc;
    HWND hWnd;
    MSG Message;

    wc.hInstance = hInst;
    wc.lpszClassName = CLASSNAME;
    wc.lpfnWndProc = WndProc;
    wc.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
    wc.hCursor = LoadCursor(hInst, IDC_ARROW);
    wc.hIcon = LoadIcon(hInst, IDI_APPLICATION);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpszMenuName = NULL;
    wc.cbClsExtra = NULL;
    wc.cbWndExtra = NULL;
    RegisterClass(&wc);
    hWnd = CreateWindow(CLASSNAME, CLASSNAME, WS_OVERLAPPEDWINDOW, 0, 0, 600, 400, NULL, NULL, hInst, NULL);
    ShowWindow(hWnd, TRUE);
    while (GetMessage(&Message, NULL, NULL, NULL)) {
        TranslateMessage(&Message);
        DispatchMessage(&Message);
    }
    return (int)Message.wParam;
}
