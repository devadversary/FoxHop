#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <windows.h>
#include <mstcpip.h>
#include "../foxhop/include/alphawindow.hpp"
#include "../foxhop/include/ui_system.hpp"
#include "../foxhop/include/ui_base.hpp"
#include "../foxhop/include/ui_button.hpp"
#include "../foxhop/include/ui_static.hpp"
#include "../foxhop/include/ui_line.hpp"
#include "../foxhop/include/ui_table.hpp"
#include "../foxhop/include/ui_textinput.hpp"
#include "../foxhop/include/ui_linechart.hpp"
#include "tree.hpp"
#include "uiparam.h"
#include <process.h>
#pragma comment (lib,"ws2_32.lib")
#pragma comment (lib, "../bin/debug/foxhop.lib")
#define CLASSNAME TEXT("TestModule")

TREE hTree;
UISystem* uiSys = NULL;
UI_Button* pPauseButton = NULL;
UI_Button* pResumeButton = NULL;
UI_Static* pTitle1 = NULL;
UI_Static* pTitle2 = NULL;
UI_Table* pTable = NULL;
UI_Table* pTable2 = NULL;
UI_Static* pStatic = NULL;
UI_Textinput* pInput = NULL;
UI_LineChart* pChart1 = NULL;


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

typedef struct _st_ipcount
{
    unsigned long long TableIdx;
    unsigned long long Count;
    wchar_t CountStr[16];
}IP_COUNT;

unsigned int thread_test(void* pTemp)
{
    SOCKET sock;
    SOCKADDR_IN saddr = { 0, };
    unsigned char buffer[12345];
    SOCKADDR_IN peer = {0,};
    int peersize = sizeof(SOCKADDR);
    int rx;

    wchar_t* Num;
    wchar_t* ipStr;
    wchar_t* DataLen;
    wchar_t tmpData[1234];
    int nPacket = 0;
    char hostname[1024];
    DWORD enable = 1, wrt;
    NODE_TREE* pNode;
    IP_COUNT* pInfo;

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
        Num = _wcsdup(tmpData);
        mbstowcs(tmpData, inet_ntoa(peer.sin_addr), ARRAYSIZE(tmpData));
        ipStr = _wcsdup(tmpData);
        wsprintfW(tmpData, L"%d", rx);
        DataLen = _wcsdup(tmpData);
        pTable->AddData(TRUE, TRUE, Num, ipStr, DataLen);

        pNode = TREE_Search(&hTree, &peer.sin_addr, sizeof(peer.sin_addr));
        if (!pNode) {
            IP_COUNT* pCount = (IP_COUNT*)malloc(sizeof(IP_COUNT));
            wchar_t* CountStr;

            pCount->TableIdx = hTree.nElements;
            pCount->Count = 0;
            pNode = TREE_Input(&hTree, &peer.sin_addr, sizeof(peer.sin_addr), pCount);
            wsprintf(pCount->CountStr, L"%d", pCount->Count);
            pTable2->AddData(TRUE, FALSE, ipStr, pCount->CountStr);
        }
        pInfo = (IP_COUNT*)pNode->pParam;
        pInfo->Count++;
        wsprintf(pInfo->CountStr, L"%d", pInfo->Count);
        pTable2->EditData(FALSE, pInfo->TableIdx, ipStr, pInfo->CountStr);
        pTable2->HighlightData(pInfo->TableIdx, {0.55,1,0.45,1});

    }
    return 0;
}

unsigned int thread_update_render(void* pTemp)
{
    UI_Panel* pMainPanel;
    pMainPanel = uiSys->pMainPanel;

    while (1) {
        uiSys->D2DA.pRenTarget->BeginDraw();
        uiSys->D2DA.pRenTarget->Clear({ 0,0,0,0.7 });
        //uiSys->D2DA.pRenTarget->Clear({ 1,1,1,1 });
        pMainPanel->update(GetElapse());
        pMainPanel->render();
        uiSys->D2DA.pRenTarget->EndDraw();
        Sleep(1);
    }
    return 0;
}

unsigned long long Filetime2long64(const FILETIME& fileTime)
{
    ULARGE_INTEGER largeInteger;
    largeInteger.LowPart = fileTime.dwLowDateTime;
    largeInteger.HighPart = fileTime.dwHighDateTime;
    return largeInteger.QuadPart;
}

unsigned int thread_sysmon(void* pTemp)
{
    UI_Panel* pMainPanel;
    pMainPanel = uiSys->pMainPanel;
    FILETIME idleTime, kernelTime, userTime;
    unsigned long long idle, kernel, user;
    unsigned long long previdle, prevkernel, prevuser;
    unsigned long long runtime, totaltime;
    float percent;

    GetSystemTimes(&idleTime, &kernelTime, &userTime);
    previdle = Filetime2long64(idleTime);
    prevkernel = Filetime2long64(kernelTime);
    prevuser = Filetime2long64(userTime);
    Sleep(1000);
    //pChart1->AddValue(98, (wchar_t*)L"", TRUE);
    while (1) {
        GetSystemTimes(&idleTime, &kernelTime, &userTime);
        idle = Filetime2long64(idleTime);
        kernel = Filetime2long64(kernelTime);
        user = Filetime2long64(userTime);

        runtime = (kernel - prevkernel) + (user - prevuser);
        totaltime = (idle - previdle) + runtime;

        percent = (((float)runtime - (float)(idle - previdle)) / (float)totaltime) * 100.f;

        pChart1->AddValue(percent, (wchar_t*)L"", TRUE);

        previdle = idle;
        prevkernel = kernel;
        prevuser = user;

        Sleep(1000);
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

void TestPauseButtonProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam)
{
    UI_Button* pButton = static_cast<UI_Button*>(pUI);

    switch (Message) {
    case WM_LBUTTONUP:
        pTable->pause(0);
        pStatic->pause(200);
        pTitle1->pause(500);
        pTitle2->pause(700);
        pInput->pause(300);
        pTable2->pause(300);
        pChart1->pause(500);
        break;
    }
}

void TestResumeButtonProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam)
{
    UI_Button* pButton = static_cast<UI_Button*>(pUI);

    switch (Message) {
    case WM_LBUTTONUP:
        pTable->resume(0);
        pStatic->resume(200);
        pTitle1->resume(500);
        pTitle2->resume(700);
        pInput->resume(300);
        pTable2->resume(300);
        pChart1->resume(500);
        break;
    }
}

void MainPanelProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam)
{
    HWND hWnd = pUI->uiSys->hBindWnd;
    UI_Panel* pPanel = (UI_Panel*)pUI;

    wchar_t* ColData[3] = { (wchar_t*)L"No.", (wchar_t*)L"IP", (wchar_t*)L"Length" };
    wchar_t* ColData2[2] = { (wchar_t*)L"IP", (wchar_t*)L"Count"};
    unsigned int ColWidth[3] = {80,150,120};
    unsigned int ColWidth2[3] = {150, 80};
    unsigned int ThreadID;
    IDWriteTextFormat* pFmt;

    switch (Message) {
    case UIM_CREATE:
        UI_ParamSet();
        TREE_Init(&hTree);
        pFmt = pUI->uiSys->CreateTextFmt((wchar_t*)L"Agency FB", 30, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        //pFmt = pUI->uiSys->CreateTextFmt((wchar_t*)L"monoMMM_5", 19, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        pPauseButton = new UI_Button(pUI->uiSys, TestPauseButtonProc, { 10,10,100,20 }, (wchar_t*)L"UI Pause", 500, ButtonParam);
        pResumeButton = new UI_Button(pUI->uiSys, TestResumeButtonProc, { 120,10,100,20 }, (wchar_t*)L"UI Resume", 800, ButtonParam);
        pTitle1 = new UI_Static(pUI->uiSys, NULL, { 10, 40, 450, 40 }, pFmt, (wchar_t*)L"PACKET MONITORING", TitleParam);
        pTitle2 = new UI_Static(pUI->uiSys, NULL, { 470, 40, 230, 40 }, pFmt, (wchar_t*)L"CONNECTION ANALYZE", TitleParam);
        pTable = new UI_Table(pUI->uiSys, TestTableProc, { 10, 90, 450 , 520 }, 3, ColData, ColWidth, 30, 20, FALSE, TableParam);
        pStatic = new UI_Static(pUI->uiSys, NULL, { 10, 620, 450, 25 }, pUI->uiSys->MediumTextForm, (wchar_t*)L"Done.", StaticParam);
        pInput = new UI_Textinput(pUI->uiSys, NULL, { 10, 655, 450, 150 }, pUI->uiSys->CreateTextFmt((wchar_t*)L"Consolas", 15, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_NEAR), InputParam);
        pTable2 = new UI_Table(pUI->uiSys, NULL, { 470, 90, 230 , 715 }, 2, ColData2, ColWidth2, 30, 25, FALSE, TableParam);
        pChart1 = new UI_LineChart(pUI->uiSys, NULL, {710, 90, 700, 200}, pUI->uiSys->SmallTextForm, 50, 0, 100, 50, 5, 0, ChartParam);
        pPanel->RegisterUI(pPauseButton);
        pPanel->RegisterUI(pResumeButton);
        pPanel->RegisterUI(pTitle1);
        pPanel->RegisterUI(pTitle2);
        pPanel->RegisterUI(pTable);
        pPanel->RegisterUI(pTable2);
        pPanel->RegisterUI(pStatic);
        pPanel->RegisterUI(pInput);
        pPanel->RegisterUI(pChart1);
        _beginthreadex(NULL, NULL, thread_sysmon, 0, 0, &ThreadID);
        _beginthreadex(NULL, NULL, thread_test, 0, 0, &ThreadID);
        _beginthreadex(NULL, NULL, thread_update_render, 0, 0, &ThreadID);
        break;
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    static UI_Panel* pMainPanel = NULL;
    static unsigned int ThreadID;
    if (pMainPanel) uiSys->SendUIMessage(pMainPanel, Message, wParam, lParam);

    switch (Message) {
    case WM_CREATE:
        uiSys = new UISystem(hWnd);        
        pMainPanel = uiSys->InitMainPanel(hWnd, MainPanelProc);
        break;

    case WM_SIZE:
        uiSys->D2DA.pRenTarget->Resize({LOWORD(lParam),HIWORD(lParam)});
        break;

    case WM_MBUTTONDOWN:
        pChart1->AddValue(15.f, (wchar_t*)L"asd", TRUE);
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
    //hWnd = CreateWindow(CLASSNAME, CLASSNAME, WS_POPUP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), NULL, NULL, hInst, NULL);
    hWnd = CreateWindow(CLASSNAME, CLASSNAME, WS_OVERLAPPEDWINDOW, 0, 0, 1920, 1080, NULL, NULL, hInst, NULL);
    AlphaWindow(hWnd, WINDOWMODE_TRANSPARENT);
    ShowWindow(hWnd, TRUE);
    while (GetMessage(&Message, NULL, NULL, NULL)) {
        TranslateMessage(&Message);
        DispatchMessage(&Message);
    }
    return (int)Message.wParam;
}
