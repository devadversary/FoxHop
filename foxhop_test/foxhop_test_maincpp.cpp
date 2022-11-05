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

    if (t1 == 0) t1 = (unsigned int)GetTickCount64();
    t2 = t1;
    t1 = (unsigned int)GetTickCount64();
    return distance(t2, t1);
}

void TestButtProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam)
{
    UI_Button* pButton = (UI_Button*)pUI;
    static int tmp = 0;
    wchar_t tmpStr[32];

    switch (Message) {
    case WM_MOUSEWHEEL:
        tmp += GET_WHEEL_DELTA_WPARAM(wParam);
        wsprintf(tmpStr, L"( %d )", tmp);
        pButton->setText(tmpStr, 0);
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
        ButtonList.push_back(pPanel->CreateButton({ 30,30,100,20 }, (wchar_t*)L"TESTBUTT", 0, TestButtProc));
        ButtonList.push_back(pPanel->CreateButton({ 150,30,100,20 }, (wchar_t*)L"TESTBUTT", 100, TestButtProc));
        ButtonList.push_back(pPanel->CreateButton({ 270,30,100,20 }, (wchar_t*)L"TESTBUTT", 200, TestButtProc));
        ButtonList.push_back(pPanel->CreateButton({ 390,30,100,20 }, (wchar_t*)L"TESTBUTT", 300, TestButtProc));
        
        ButtonList.push_back(pPanel->CreateButton({ 30,80,100,20 }, (wchar_t*)L"TESTBUTT", 400, TestButtProc));
        ButtonList.push_back(pPanel->CreateButton({ 150,80,100,20 }, (wchar_t*)L"TESTBUTT", 500, TestButtProc));
        ButtonList.push_back(pPanel->CreateButton({ 270,80,100,20 }, (wchar_t*)L"TESTBUTT", 600, TestButtProc));
        ButtonList.push_back(pPanel->CreateButton({ 390,80,100,20 }, (wchar_t*)L"TESTBUTT", 700, TestButtProc));
        
        ButtonList.push_back(pPanel->CreateButton({ 30,130,100,20 }, (wchar_t*)L"TESTBUTT", 800, TestButtProc));
        ButtonList.push_back(pPanel->CreateButton({ 150,130,100,20 }, (wchar_t*)L"TESTBUTT", 900, TestButtProc));
        ButtonList.push_back(pPanel->CreateButton({ 270,130,100,20 }, (wchar_t*)L"TESTBUTT", 1000, TestButtProc));
        ButtonList.push_back(pPanel->CreateButton({ 390,130,100,20 }, (wchar_t*)L"TESTBUTT", 1100, TestButtProc));
        
        ButtonList.push_back(pPanel->CreateButton({ 30,180,100,20 }, (wchar_t*)L"TESTBUTT", 1200, TestButtProc));
        ButtonList.push_back(pPanel->CreateButton({ 150,180,100,20 }, (wchar_t*)L"TESTBUTT", 1300, TestButtProc));
        ButtonList.push_back(pPanel->CreateButton({ 270,180,100,20 }, (wchar_t*)L"TESTBUTT", 1400, TestButtProc));
        ButtonList.push_back(pPanel->CreateButton({ 390,180,100,20 }, (wchar_t*)L"TESTBUTT", 1500, TestButtProc));
        
        ButtonList.push_back(pPanel->CreateButton({ 30,230,100,20 }, (wchar_t*)L"TESTBUTT", 1600, TestButtProc));
        ButtonList.push_back(pPanel->CreateButton({ 150,230,100,20 }, (wchar_t*)L"TESTBUTT", 1700, TestButtProc));
        ButtonList.push_back(pPanel->CreateButton({ 270,230,100,20 }, (wchar_t*)L"TESTBUTT", 1800, TestButtProc));
        ButtonList.push_back(pPanel->CreateButton({ 390,230,100,20 }, (wchar_t*)L"TESTBUTT", 1900, TestButtProc));
        break;
        
    case WM_RBUTTONDOWN :
        for (UI* pUI : ButtonList){
            pUI->pause(rand()%300);
        }
        break;

    case WM_LBUTTONDOWN :
    {
        int i = 0;
        for (UI* pUI : ButtonList) {
            pUI->resume(i);
            i += 30;
        }
    }
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
    UITheme* pTheme;

    if (pMainPanel) uiSys->SendUIMessage(pMainPanel, Message, wParam, lParam);

    switch (Message) {
    case WM_CREATE:
        uiSys = new UISystem(hWnd);
        pTheme = uiSys->Theme;
        pTheme->ButtonInitMotion = eButtonMotionPattern::eInit_Reload;
        pTheme->ButtonInitPitch = 700;
        pTheme->ButtonPauseMotion = eButtonMotionPattern::ePause_Flick;
        pTheme->ButtonPausePitch = 500;
        pTheme->ButtonFaceColor = { 0.5f,0.5f,0.5f,1 };
        pTheme->ButtonFontColor = { 1,1,1,1 };
        

        SetTimer(hWnd, 666, 15, NULL);
        pMainPanel = uiSys->InitMainPanel(hWnd, MainPanelProc);
        break;

    case WM_TIMER:
        InvalidateRect(hWnd, NULL, FALSE);
        break;

    case WM_PAINT:
        uiSys->D2DA.pRenTarget->BeginDraw();
        //uiSys->D2DA.pRenTarget->Clear({1,1,1,1});
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
