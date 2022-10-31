#include <windows.h>
#include "../foxhop/include/alphawindow.hpp"
#include "../foxhop/include/ui_system.hpp"

#pragma comment (lib, "../bin/debug/foxhop.lib")
#define CLASSNAME TEXT("TestModule")

/**
    @brief 차이 구하기
*/
inline unsigned long distance(unsigned long a, unsigned long b)
{
    if (a > b) return a - b;
    return b - a;
}

/**
    @brief 호출 할 때마다 이전 호출된 시간과의 시간차를 리턴한다.
    @remark 당연하게도, 첫 호출은 0일수도 있다.
*/
unsigned int GetElapse()
{
    static unsigned int t1 = 0;
    unsigned int t2;

    if (t1 == 0) t1 = GetTickCount64();
    t2 = t1;
    t1 = GetTickCount64();
    return distance(t2, t1);
}

void TestButtProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam)
{
    UI_Button* pButton = (UI_Button*)pUI;

    switch (Message) {
    case WM_MOUSEMOVE:
    {
        wchar_t ButtonName[32];
        wsprintf(ButtonName, L"[%d,%d]", GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        pButton->setText(ButtonName, 0);
        break;
    }

    case WM_LBUTTONDOWN:
    {
        wchar_t sss[512];
        wsprintf(sss, L"%x", pUI);
        MessageBox(pButton->uiSys->hBindWnd, sss, L"UwU", MB_ICONWARNING);
    }
        break;

    case WM_MOUSEWHEEL:
        MessageBox(pButton->uiSys->hBindWnd, L"이거 스끼린데 ~", L"UwU", MB_ICONWARNING);
        break;
    }
}

void MainPanelProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam)
{
    HWND hWnd = pUI->uiSys->hBindWnd;
    UI_Panel* pPanel = (UI_Panel*)pUI;
    static std::list<UI*> ButtonList;

    switch(Message) {
    case UIM_CREATE:
        ButtonList.push_back(pPanel->CreateUI(UIType::eUI_Button, { 10,10,100,20 }, (wchar_t*)L"TESTBUTT", 0, TestButtProc));
        ButtonList.push_back(pPanel->CreateUI(UIType::eUI_Button, { 10,30,100,20 }, (wchar_t*)L"TESTBUTT", 100, TestButtProc));
        ButtonList.push_back(pPanel->CreateUI(UIType::eUI_Button, { 10,50,100,20 }, (wchar_t*)L"TESTBUTT", 200, TestButtProc));
        ButtonList.push_back(pPanel->CreateUI(UIType::eUI_Button, { 10,70,100,20 }, (wchar_t*)L"TESTBUTT", 300, TestButtProc));
        ButtonList.push_back(pPanel->CreateUI(UIType::eUI_Button, { 10,90,100,20 }, (wchar_t*)L"TESTBUTT", 400, TestButtProc));
        ButtonList.push_back(pPanel->CreateUI(UIType::eUI_Button, { 10,110,100,20 }, (wchar_t*)L"TESTBUTT", 500, TestButtProc));
        break;

    case WM_IME_COMPOSITION:
    {
        HDC hDC;
        wchar_t str[100];

        hDC = GetDC(hWnd);
        str[0] = wParam;
        str[1] = NULL;
        TextOutW(hDC, 10, 30, str, wcslen(str));
        ReleaseDC(hWnd, hDC);
        break;
    }

    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    static UISystem* uiSys = NULL;
    static UI_Panel* pMainPanel = NULL;
    BUTTON_MOTION_SET bms;

    if (pMainPanel) uiSys->SendUIMessage(pMainPanel, Message, wParam, lParam);

    switch (Message) {
    case WM_CREATE:
        uiSys = new UISystem(hWnd);
        uiSys->pUIButtonFactory->GetCurrentMotionSet(&bms);
        bms.Init = eButtonMotionPattern::eInit_Flick;
        bms.InitPitch = 300;
        uiSys->pUIButtonFactory->SetCurrentMotionSet(&bms);
        SetTimer(hWnd, 666, 15, NULL);
        pMainPanel = uiSys->InitMainPanel(hWnd, MainPanelProc);
        break;

    case WM_TIMER:
        InvalidateRect(hWnd, NULL, FALSE);
        break;

    case WM_PAINT:
        uiSys->D2DA.pRenTarget->BeginDraw();
        uiSys->D2DA.pRenTarget->Clear({0,0,0,0.7});       
        pMainPanel->update(GetElapse());
        pMainPanel->render();
        uiSys->D2DA.pRenTarget->EndDraw();
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
    wc.hbrBackground = CreateSolidBrush(RGB(255, 255, 255));
    wc.hCursor = LoadCursor(hInst, IDC_ARROW);
    wc.hIcon = LoadIcon(hInst, IDI_APPLICATION);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpszMenuName = NULL;
    wc.cbClsExtra = NULL;
    wc.cbWndExtra = NULL;
    RegisterClass(&wc);
    hWnd = CreateWindow(CLASSNAME, CLASSNAME, WS_OVERLAPPEDWINDOW, 0, 0, 600, 400, NULL, NULL, hInst, NULL);
    AlphaWindow(hWnd, WINDOWMODE_TRANSPARENT);
    ShowWindow(hWnd, TRUE);
    while (GetMessage(&Message, NULL, NULL, NULL)) {
        TranslateMessage(&Message);
        DispatchMessage(&Message);
    }
    return (int)Message.wParam;
}
