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

UI_Table* pTable = NULL;

void TestButtProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam)
{
    UI_Button* pButton = (UI_Button*)pUI;
    static int tmp = 0;
    wchar_t tmpStr[32];

    switch (Message) {
    case WM_LBUTTONDOWN:
    {
        int i = 0;
        wchar_t TmpData[3][32];
        wchar_t* Data[3];
        static int ttt = 1;

        wsprintfW(TmpData[0], L"%s%d", L"TestData", ttt);
        wsprintfW(TmpData[1], L"%s%d", L"TestData", ttt);
        wsprintfW(TmpData[2], L"%s%d", L"TestData", ttt);
        Data[0] = _wcsdup(TmpData[0]);
        Data[1] = _wcsdup(TmpData[1]);
        Data[2] = _wcsdup(TmpData[2]);
        pTable->AddData(Data, FALSE);
        wsprintf(tmpStr, L"%d Datas", ttt);
        pButton->setText(tmpStr, 0);
        ttt++;
        break;
    }
    }
}

void MainPanelProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam)
{
    HWND hWnd = pUI->uiSys->hBindWnd;
    UI_Panel* pPanel = (UI_Panel*)pUI;

    wchar_t* ColData[3] = { (wchar_t*)L"Data #1", (wchar_t*)L"Data #2", (wchar_t*)L"Desc" };
    unsigned int ColWidth[3] = {150,120,120};
    switch(Message) {
    case UIM_CREATE:
    {
        UI_Button* pButton = new UI_Button(pUI->uiSys, TestButtProc, {10,10,100,20}, (wchar_t*)L"Data Input Test", 0);
        pTable = new UI_Table(pUI->uiSys, NULL, {10, 40, 590 , 270}, 3, ColData, ColWidth, 30, 20, FALSE);

        pPanel->RegisterUI(pButton);
        pPanel->RegisterUI(pTable);
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

    if (pMainPanel) uiSys->SendUIMessage(pMainPanel, Message, wParam, lParam);

    switch (Message) {
    case WM_CREATE:
        uiSys = new UISystem(hWnd);        
        SetTimer(hWnd, 666, 10, NULL);
        pMainPanel = uiSys->InitMainPanel(hWnd, MainPanelProc);
        break;

    case WM_TIMER:
        InvalidateRect(hWnd, NULL, FALSE);
        break;

    case WM_SIZE:
        uiSys->D2DA.pRenTarget->Resize({LOWORD(lParam),HIWORD(lParam)});
        break;

    case WM_PAINT:
        uiSys->D2DA.pRenTarget->BeginDraw();
        uiSys->D2DA.pRenTarget->Clear({1,1,1,1});
        //uiSys->D2DA.pRenTarget->Clear({1,1,1,0.9});
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
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpszMenuName = NULL;
    wc.cbClsExtra = NULL;
    wc.cbWndExtra = NULL;
    RegisterClass(&wc);
    hWnd = CreateWindow(CLASSNAME, CLASSNAME, WS_OVERLAPPEDWINDOW, 0, 0, 630, 400, NULL, NULL, hInst, NULL);
    AlphaWindow(hWnd, WINDOWMODE_TRANSPARENT);
    ShowWindow(hWnd, TRUE);
    while (GetMessage(&Message, NULL, NULL, NULL)) {
        TranslateMessage(&Message);
        DispatchMessage(&Message);
    }
    return (int)Message.wParam;
}
