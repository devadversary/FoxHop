#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <windows.h>
#include <mstcpip.h>
#include "../foxhop/include/alphawindow.hpp"
#include "../foxhop/include/ui_system.hpp"
#include <process.h>
#pragma comment (lib,"ws2_32.lib")
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

UI_Button* pButton = NULL;
UI_Table* pTable = NULL;
UI_Static* pStatic = NULL;

unsigned int thread_test(void* pTemp)
{
    SOCKET sock;
    SOCKADDR_IN saddr = { 0, };
    unsigned char buffer[12345];
    SOCKADDR_IN peer = {0,};
    int peersize = sizeof(SOCKADDR);
    int rx;
    wchar_t* tabledata[3];
    wchar_t tmpData[1234];
    int nPacket = 0;
    char hostname[1024];
    DWORD enable = 1, wrt;
    gethostname(hostname, sizeof(hostname));
    HOSTENT* ent = gethostbyname(hostname);
    memcpy(&saddr.sin_addr, ent->h_addr_list[0], sizeof(IN_ADDR));
    saddr.sin_port = 0;
    saddr.sin_family = AF_INET;

    sock = socket(AF_INET, SOCK_RAW, NULL);
    bind(sock, (SOCKADDR*)&saddr, sizeof(SOCKADDR));
    int err = WSAGetLastError();
    WSAIoctl(sock, SIO_RCVALL, &enable, sizeof(enable), 0 , 0 , &wrt, 0 , 0);
    while(1){
        rx = recvfrom(sock, (char*)buffer, sizeof(buffer), NULL, (SOCKADDR*)&peer, &peersize);
        if (rx <= 0) {
            Sleep(1);
            continue;
        }
        nPacket++;
        wsprintfW(tmpData, L"%06d", nPacket);
        tabledata[0] = _wcsdup(tmpData);
        mbstowcs(tmpData, inet_ntoa(peer.sin_addr), ARRAYSIZE(tmpData));
        tabledata[1] = _wcsdup(tmpData);
        wsprintfW(tmpData, L"%d", rx);
        tabledata[2] = _wcsdup(tmpData);
        pTable->AddData(tabledata, TRUE);
    }
    return 0;
}

void TestButtProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam)
{
    UI_Button* pButton = (UI_Button*)pUI;
    static int tmp = 0;
    wchar_t tmpStr[32];
    FLT_MAX;
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
        pTable->AddData(Data, TRUE);
        wsprintf(tmpStr, L"%d Datas", ttt);
        pButton->setText(tmpStr, 0);
        ttt++;
        break;
    }
    }
}

void TestTableProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam)
{
    UI_Table* pTable = static_cast<UI_Table*>(pUI);
    wchar_t Noti[64];

    switch (Message) {
    case UIM_TABLE_SELECT:
        wsprintfW(Noti, L"Line %d Selected. : %s", (int)lParam, ((wchar_t**)wParam)[0]);
        pStatic->SetText(Noti);
        break;

    case UIM_TABLE_UNSELECT:
        wsprintfW(Noti, L"Line %d Unselected.", (int)lParam);
        pStatic->SetText(Noti);
        break;

    case UIM_FOCUS:
        break;

    case UIM_UNFOCUS:
        break;
    }
}

void MainPanelProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam)
{
    HWND hWnd = pUI->uiSys->hBindWnd;
    UI_Panel* pPanel = (UI_Panel*)pUI;

    wchar_t* ColData[3] = { (wchar_t*)L"No.", (wchar_t*)L"IP", (wchar_t*)L"Length" };
    unsigned int ColWidth[3] = {80,150,120};
    unsigned int ThreadID;

    switch(Message) {
    case UIM_CREATE:
    {
        UI_Button_MotionParam ButtonParam;
        UI_Table_MotionParam TableParam;
        UI_Static_MotionParam StaticParam;

        ButtonParam.InitMotion = eButtonMotionPattern::eInit_Reload;
        ButtonParam.InitPitch = 700;
        ButtonParam.ClickMotion = eButtonMotionPattern::eClick_Flash;
        ButtonParam.ClickPitch = 300;
        ButtonParam.FaceColor = { 0.7,0,0,0.6 };
        ButtonParam.FrameColor = { 1,0,0,1 };
        ButtonParam.FontColor = { 1,1,1,1 };

        TableParam.MotionSelect = eTableMotionPattern::eSelect_Decel;
        TableParam.PitchSelect = 300;
        TableParam.MotionRowText = eTableMotionPattern::eText_Typing;
        TableParam.PitchRowOneText = 200;
        TableParam.PitchRowAllText = 500;
        TableParam.ColorFrame = { 1,0,0,1 };
        TableParam.ColorHeaderBg = { 0.6, 0, 0, 0.6 };
        TableParam.ColorHeaderText = { 1,1,1,1 };
        TableParam.ColorRowBg1 = { 0,0,0,0 };
        TableParam.ColorRowBg2 = { 1,0,0,0.1 };
        TableParam.ColorRowBgSelect = { 1,1,1,0.5 };
        TableParam.ColorRowText = { 1,1,1,1 };
        TableParam.ColorRowTextSelect = { 0,0,0,1 };

        StaticParam.MotionText = eStaticMotionPattern::eText_Flick;
        StaticParam.PitchText = 200;
        StaticParam.ColorBg = { 0.7,0,0,0.4 };
        StaticParam.ColorFont = { 1,1,1,1 };
        StaticParam.ColorFrame = { 1,0,0,1 };

        pButton = new UI_Button(pUI->uiSys, TestButtProc, {10,10,100,20}, (wchar_t*)L"Data Input Test", 0, ButtonParam);
        pTable = new UI_Table(pUI->uiSys, TestTableProc, {10, 40, 590 , 270}, 3, ColData, ColWidth, 30, 20, FALSE, TableParam);
        pStatic = new UI_Static(pUI->uiSys, NULL, {10, 320, 590, 25}, (wchar_t*)L"Done.", StaticParam);
        pPanel->RegisterUI(pButton);
        pPanel->RegisterUI(pTable);
        pPanel->RegisterUI(pStatic);

        _beginthreadex(NULL, NULL, thread_test, 0, 0, &ThreadID);
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
        uiSys->D2DA.pRenTarget->Clear({0,0,0,0.7});
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
    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2), &wsa);
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
