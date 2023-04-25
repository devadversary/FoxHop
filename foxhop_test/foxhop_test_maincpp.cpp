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
    static UI_Table* pTable = NULL;

    static std::list<UI*> ButtonList;
    wchar_t* ColData[3] = { (wchar_t*)L"hello", (wchar_t*)L"hhh", (wchar_t*)L"adad" };
    switch(Message) {
    case UIM_CREATE:
    {
        int i, k, delay=0;
        for (k = 10; k < 350; k+=30) {
            for (i = 10; i < 600; i += 40) {
                UI_Button* pButton = new UI_Button(pUI->uiSys, TestButtProc, {(float)i,(float)k,30,20}, (wchar_t*)L"TE", delay);
                pPanel->RegisterUI(pButton);
                ButtonList.push_back(pButton);
                delay += 10;
            }
        }

        pTable = new UI_Table(pUI->uiSys, NULL, {10, 610, 600 , 250}, 3,
                              ColData, NULL, 30, 20, FALSE);
        pPanel->RegisterUI(pTable);
    }
        break;

    case WM_RBUTTONDOWN :
        for (UI* pUI : ButtonList){
            pUI->pause(rand()%300);
        }
        break;

    case WM_LBUTTONDOWN :
    {
        int i = 0;
        wchar_t* TmpData[3];
        for (UI* pUI : ButtonList) {
            pUI->resume(i);
            i += 5;
        }
        TmpData[0] = _wcsdup(L"test");
        TmpData[1] = _wcsdup(L"test22");
        TmpData[2] = _wcsdup(L"test333");
        pTable->AddData(TmpData, FALSE);
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
