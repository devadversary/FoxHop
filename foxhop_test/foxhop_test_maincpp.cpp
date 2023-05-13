#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <windows.h>
#include <mstcpip.h>
#include "../foxhop/include/alphawindow.hpp"
#include "../foxhop/include/ui_system.hpp"
#include "../foxhop/include/ui_base.hpp"
#include "../foxhop/include/ui_button.hpp"
#include "../foxhop/include/ui_listview.hpp"
#include "../foxhop/include/ui_static.hpp"
#include "../foxhop/include/ui_fraggedline.hpp"
#include "../foxhop/include/ui_table.hpp"
#include "../foxhop/include/ui_textinput.hpp"
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

UI_Button* pPauseButton = NULL;
UI_Button* pResumeButton = NULL;
UI_Table* pTable = NULL;
UI_Static* pStatic = NULL;
UI_Textinput* pInput = NULL;

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

void TestTableProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam)
{
    UI_Table* pTable = static_cast<UI_Table*>(pUI);
    wchar_t Noti[64];

    switch (Message) {
    case UIM_TABLE_SELECT:
        wsprintfW(Noti, L"Line %d Selected. : %s", (int)lParam, ((wchar_t**)wParam)[1]);
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
        UI_Textinput_MotionParam InputParam;

        ButtonParam.InitMotion = eButtonMotionPattern::eInit_Reload;
        ButtonParam.InitPitch = 700;
        ButtonParam.ClickMotion = eButtonMotionPattern::eClick_Flash;
        ButtonParam.ClickPitch = 1000;
        ButtonParam.FaceColor = { 0.7,0,0,0.6 };
        ButtonParam.FrameColor = { 1,0,0,1 };
        ButtonParam.FontColor = { 1,1,1,1 };
        /*
        TableParam.MotionSelect = eTableMotionPattern::eSelect_Decel;
        TableParam.PitchSelect = 300;
        TableParam.MotionRowText = eTableMotionPattern::eViewRowTextInit_Typing;
        TableParam.PitchRowOneText = 200;
        TableParam.PitchRowAllText = 500;
        TableParam.ColorFrame = { 1,0,0,1 };
        TableParam.ColorHeaderBg = { 0.6, 0, 0, 0.6 };
        TableParam.ColorHeaderText = { 1,1,1,1 };
        TableParam.ColorRowBg1 = { 0,0,0,0 };
        TableParam.ColorRowBg2 = { 1,0,0,0.1 };
        TableParam.ColorRowBgSelect = { 1,1,1,0.8 };
        TableParam.ColorRowText = { 1,1,1,1 };
        TableParam.ColorRowTextSelect = { 0,0,0,1 };
        */

        StaticParam.MotionText = eStaticMotionPattern::eText_Flick;
        StaticParam.PitchText = 200;
        //StaticParam.ColorFrame = { 1,0,0,1 };
        //StaticParam.ColorBg = { 0.7,0,0,0.4 };
        StaticParam.ColorFrame = { 0,0,0,0 };
        StaticParam.ColorBg = { 0,0,0,0 };
        StaticParam.ColorFont = { 1,1,1,1 };

        InputParam.MotionCaretMove = eTextinputMotionPattern::eCaretMove_Decel;
        InputParam.ColorFrame = { 0.8,0,0,0.6 };
        InputParam.ColorBg = ALL_ZERO;
        InputParam.ColorFont = { 1,1,1,1 };
        InputParam.PitchCaretMove = 150;

        pPauseButton = new UI_Button(pUI->uiSys, NULL, {10,10,100,20}, (wchar_t*)L"UI Pause", 0, ButtonParam);
        pResumeButton = new UI_Button(pUI->uiSys, NULL, {120,10,100,20}, (wchar_t*)L"UI Resume", 200, ButtonParam);
        pTable = new UI_Table(pUI->uiSys, TestTableProc, {10, 40, 590 , 270}, 3, ColData, ColWidth, 30, 20, FALSE, TableParam);
        pStatic = new UI_Static(pUI->uiSys, NULL, {10, 320, 590, 25}, (wchar_t*)L"Done.", StaticParam);
        pInput = new UI_Textinput(pUI->uiSys, NULL, { 10, 355, 590, 150 }, pUI->uiSys->CreateTextFmt((wchar_t*)L"Consolas", 15, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_NEAR), InputParam);
        pPanel->RegisterUI(pPauseButton);
        pPanel->RegisterUI(pResumeButton);
        pPanel->RegisterUI(pTable);
        pPanel->RegisterUI(pStatic);
        pPanel->RegisterUI(pInput);

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
    hWnd = CreateWindow(CLASSNAME, CLASSNAME, WS_OVERLAPPEDWINDOW, 0, 0, 630, 550, NULL, NULL, hInst, NULL);
    AlphaWindow(hWnd, WINDOWMODE_TRANSPARENT);
    ShowWindow(hWnd, TRUE);
    while (GetMessage(&Message, NULL, NULL, NULL)) {
        TranslateMessage(&Message);
        DispatchMessage(&Message);
    }
    return (int)Message.wParam;
}
