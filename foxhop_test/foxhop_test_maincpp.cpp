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
#include "etc.h"
#include <process.h>
#pragma comment (lib,"ws2_32.lib")
#pragma comment (lib, "../bin/debug/foxhop.lib")
#define CLASSNAME TEXT("TestModule")
#define U(x) (x*60)
TREE hTreeTx;
TREE hTreeRx;
UISystem* uiSys = NULL;
SRWLOCK lock;

UI_Static* pIntroText = NULL; /*2023 6/6 666*/
UI_Static* pIntroText2 = NULL; /*COME BACK*/
UI_Static* pIntroText3 = NULL; /*FOXHOP UI Library*/
UI_Static* pIntroText4 = NULL; /*Prototype*/
UI_Line* pIntroLine = NULL;
UI_Line* pTopLine = NULL;
UI_Static* pTitleMain = NULL;
UI_Static* pTitleSub = NULL;
UI_Line* pBotLine = NULL;
UI_Static* pLabelPacketMon = NULL;
UI_Static* pLabelAnalyiseTx = NULL;
UI_Static* pLabelAnalyiseRx = NULL;
UI_Static* pLabelTimer = NULL;
UI_Table* pTablePacket = NULL;
UI_Table* pTableTx = NULL;
UI_Table* pTableRx = NULL;
UI_Static* pStaticPacketSel = NULL;
UI_Button* pButtonConnInfo = NULL;
UI_Button* pButtonDetailInfo = NULL;
UI_Button* pButtonOutro = NULL;

UI_LineChart* pChart1 = NULL;
UI_Textinput* pInput = NULL;


UI_Button* pPauseButton = NULL;
UI_Button* pResumeButton = NULL;
UI_Static* pTitle1 = NULL;
UI_Static* pTitle2 = NULL;
UI_Table* pTable = NULL;
UI_Table* pTable2 = NULL;
UI_Static* pStatic = NULL;


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

unsigned int thread_packetcapture(void* pTemp)
{
    SOCKET sock;
    SOCKADDR_IN saddr = { 0, };
    unsigned char buffer[12345];
    SOCKADDR_IN peer = {0,};
    int peersize = sizeof(SOCKADDR);
    int rx;
    IPv4* iphdr;

    wchar_t* Num;
    wchar_t* srcip;
    wchar_t* dstip;
    wchar_t* DataLen;
    wchar_t* Protocol;
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
    iphdr = (IPv4*)buffer;
    while(1){
        rx = recvfrom(sock, (char*)buffer, sizeof(buffer), NULL, (SOCKADDR*)&peer, &peersize);
        if (rx <= 0) {
            Sleep(1);
            continue;
        }
        nPacket++;
        wsprintfW(tmpData, L"%06d", nPacket);
        Num = _wcsdup(tmpData);
        mbstowcs(tmpData, inet_ntoa(*(IN_ADDR*)&iphdr->SrcAddr), ARRAYSIZE(tmpData));
        srcip = _wcsdup(tmpData);
        mbstowcs(tmpData, inet_ntoa(*(IN_ADDR*)&iphdr->DstAddr), ARRAYSIZE(tmpData));
        dstip = _wcsdup(tmpData);
        wsprintfW(tmpData, L"%d", rx);
        DataLen = _wcsdup(tmpData);
        switch (iphdr->Protocol) {
        case IPPROTO_TCP: Protocol = _wcsdup(L"TCP"); break;
        case IPPROTO_UDP: Protocol = _wcsdup(L"UDP"); break;
        case IPPROTO_ICMP: Protocol = _wcsdup(L"ICMP"); break;
        case IPPROTO_SCTP: Protocol = _wcsdup(L"SCTP"); break;
        default: Protocol = (wchar_t*)L"???";
        }
        pTablePacket->AddData(TRUE, TRUE, Num, srcip, dstip, DataLen, Protocol);

        pNode = TREE_Search(&hTreeTx, &iphdr->SrcAddr, sizeof(iphdr->SrcAddr));
        if (!pNode) {
            IP_COUNT* pCount = (IP_COUNT*)malloc(sizeof(IP_COUNT));
            wchar_t* CountStr;

            pCount->TableIdx = hTreeTx.nElements;
            pCount->Count = 0;
            pNode = TREE_Input(&hTreeTx, &iphdr->SrcAddr, sizeof(iphdr->SrcAddr), pCount);
            wsprintf(pCount->CountStr, L"%d", pCount->Count);
            pTableTx->AddData(TRUE, FALSE, srcip, pCount->CountStr);
        }
        pInfo = (IP_COUNT*)pNode->pParam;
        pInfo->Count++;
        wsprintf(pInfo->CountStr, L"%d", pInfo->Count);
        pTableTx->EditData(FALSE, pInfo->TableIdx, srcip, pInfo->CountStr);
        pTableTx->HighlightData(pInfo->TableIdx, {0.55,1,0.45,1}); /*초록이*/

        pNode = TREE_Search(&hTreeRx, &iphdr->DstAddr, sizeof(iphdr->DstAddr));
        if (!pNode) {
            IP_COUNT* pCount = (IP_COUNT*)malloc(sizeof(IP_COUNT));
            wchar_t* CountStr;

            pCount->TableIdx = hTreeRx.nElements;
            pCount->Count = 0;
            pNode = TREE_Input(&hTreeRx, &iphdr->DstAddr, sizeof(iphdr->DstAddr), pCount);
            wsprintf(pCount->CountStr, L"%d", pCount->Count);
            pTableRx->AddData(TRUE, FALSE, dstip, pCount->CountStr);
        }
        pInfo = (IP_COUNT*)pNode->pParam;
        pInfo->Count++;
        wsprintf(pInfo->CountStr, L"%d", pInfo->Count);
        pTableRx->EditData(FALSE, pInfo->TableIdx, dstip, pInfo->CountStr);
        pTableRx->HighlightData(pInfo->TableIdx, {0.35,0.35,1,1}); /*파랑이*/

    }
    return 0;
}

unsigned int thread_update_render(void* pTemp)
{
    UI_Panel* pMainPanel;
    pMainPanel = uiSys->pMainPanel;

    while (1) {
        uiSys->D2DA.pRenTarget->BeginDraw();
        uiSys->D2DA.pRenTarget->Clear({ 0,0,0,0.8 });
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
    while (1) {
        Sleep(1);
        GetSystemTime;
        //pLabelTimer->SetText();
    }
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
    while (1) {
        GetSystemTimes(&idleTime, &kernelTime, &userTime);
        idle = Filetime2long64(idleTime);
        kernel = Filetime2long64(kernelTime);
        user = Filetime2long64(userTime);

        runtime = (kernel - prevkernel) + (user - prevuser);
        totaltime = (idle - previdle) + runtime;

        percent = (((float)runtime - (float)(idle - previdle)) / (float)totaltime) * 100.f;

        //pChart1->AddValue(percent, (wchar_t*)L"", TRUE);

        previdle = idle;
        prevkernel = kernel;
        prevuser = user;

        Sleep(1000);
    }
    return 0;
}



unsigned int thread_scene_intro(void* pTemp)
{
    unsigned int ThreadID;

    Sleep(1400);
    pIntroLine->resume(0);
    pIntroText->resume(600);
    pIntroText2->resume(1800);
    Sleep(2000);
    pIntroText->pause(1500);
    pIntroText2->pause(2000);
    pIntroText3->resume(2500);
    pIntroText4->resume(2800);
    Sleep(4000);
    pIntroLine->pause(0);
    pIntroText3->pause(800);
    pIntroText4->pause(1100);

    pTopLine->resume(0);
    pBotLine->resume(0);
    pTitleMain->resume(1000);
    pTitleSub->resume(1600);

    pLabelPacketMon->resume(1200);
    pLabelAnalyiseTx->resume(1200);
    pLabelAnalyiseRx->resume(1200);
    pTablePacket->resume(400);
    pStaticPacketSel->resume(600);
    pTableTx->resume(700);
    pTableRx->resume(1000);

    pButtonConnInfo->resume(2000);
    pButtonDetailInfo->resume(2200);
    pButtonOutro->resume(2400);
    pLabelTimer->resume(500);

    Sleep(1500);
    _beginthreadex(NULL, NULL, thread_sysmon, 0, 0, &ThreadID);
    _beginthreadex(NULL, NULL, thread_packetcapture, 0, 0, &ThreadID);
    return 0;
}


void TestTableProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam)
{
    UI_Table* pTable = static_cast<UI_Table*>(pUI);
    wchar_t Noti[64];

    switch (Message) {
    case UIM_TABLE_SELECT:
        wsprintfW(Noti, L"Line %d Selected. : %s -> %s", (int)lParam, ((wchar_t**)wParam)[1], ((wchar_t**)wParam)[2]);
        pStaticPacketSel->SetText(Noti);
        break;

    case UIM_TABLE_UNSELECT:
        wsprintfW(Noti, L"Line %d Unselected.", (int)lParam);
        pStaticPacketSel->SetText(Noti);
        break;

    case UIM_FOCUS:
        break;

    case UIM_UNFOCUS:
        break;
    }
}

void TestConnInfoButtonProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam)
{
    UI_Button* pButton = static_cast<UI_Button*>(pUI);

    switch (Message) {
    case WM_LBUTTONUP:
        break;
    }
}

void TestDetailInfoButtonProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam)
{
    UI_Button* pButton = static_cast<UI_Button*>(pUI);

    switch (Message) {
    case WM_LBUTTONUP:
        break;
    }
}

void TestOutroButtonProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam)
{
    UI_Button* pButton = static_cast<UI_Button*>(pUI);

    switch (Message) {
    case WM_LBUTTONUP:
        break;
    }
}

void MainPanelProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam)
{
    HWND hWnd = pUI->uiSys->hBindWnd;
    UI_Panel* pPanel = (UI_Panel*)pUI;

    wchar_t* PacketDumpCols[5] = { (wchar_t*)L"No.", (wchar_t*)L"Source IP", (wchar_t*)L"Destination IP",(wchar_t*)L"Length", (wchar_t*)L"Protocol" };
    wchar_t* AnalyseCol[2] = { (wchar_t*)L"Address", (wchar_t*)L"Count"};
    unsigned int PacketDumpColWIdth[5] = {80, 150, 150, 100, 100};
    unsigned int AnalyseColWidth[3] = {150, 80};
    unsigned int ThreadID;
    IDWriteTextFormat* pFmtIntro;
    IDWriteTextFormat* pFmtTitle;
    IDWriteTextFormat* pFmtTimer;
    IDWriteTextFormat* pFmtTitleSub;
    IDWriteTextFormat* pFmtButton;
    static POSITION IntroLineVertex[] = { {480, 420,}, {1440, 420,}, {1440, 660,}, {480, 660,} };
    static POSITION TopLineVertex[] = { {U(1),U(2),}, {U(5),U(2),}, {U(7),U(2),}, {U(20),U(2),}, {U(28),U(2),}, {U(31),U(2),} };
    static POSITION BotLineVertex[] = { {U(31),U(16),},{U(24),U(16),},{U(19),U(16),}, {U(1),U(16),} };
    //static POSITION BotLineVertex[] = { {480, 420, 0, 0}, {1440, 420, 0, 0}, {1440, 660, 0, 0}, {480, 660, 0, 0} };
    switch (Message) {
    case UIM_CREATE:
        UI_ParamSet();
        TREE_Init(&hTreeTx);
        TREE_Init(&hTreeRx);
        pFmtIntro = pUI->uiSys->CreateTextFmt((wchar_t*)L"Agency FB", 80, DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        pFmtTitle = pUI->uiSys->CreateTextFmt((wchar_t*)L"Agency FB", 80, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_FAR);
        pFmtTitleSub = pUI->uiSys->CreateTextFmt((wchar_t*)L"Agency FB", 40, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        pFmtTimer = pUI->uiSys->CreateTextFmt((wchar_t*)L"Agency FB", 50, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        pFmtButton = pUI->uiSys->CreateTextFmt((wchar_t*)L"Agency FB", 30, DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

        //pFmt = pUI->uiSys->CreateTextFmt((wchar_t*)L"monoMMM_5", 19, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        pIntroText = new UI_Static(pUI->uiSys, NULL, {U(8), U(7), U(16), U(2)}, pFmtIntro, (wchar_t*)L"2 0 2 3 / 6 / 6  -  6 6 6", IntroStaticParam);
        pIntroText2 = new UI_Static(pUI->uiSys, NULL, { U(8), U(9), U(16), U(2) }, pFmtIntro, (wchar_t*)L"C O M E   B A C K", IntroStaticParam3);
        pIntroText3 = new UI_Static(pUI->uiSys, NULL, { U(8), U(7), U(16), U(2) }, pFmtIntro, (wchar_t*)L"F O X H O P - L I B R A R Y", IntroStaticParam2);
        pIntroText4 = new UI_Static(pUI->uiSys, NULL, { U(8), U(9), U(16), U(2) }, pFmtIntro, (wchar_t*)L"p r o t o t y p e", IntroStaticParam2);
        pIntroLine = new UI_Line(pUI->uiSys, IntroLineVertex, ARRAYSIZE(IntroLineVertex), NULL, TRUE, IntroLineParam);

        pTitleMain = new UI_Static(pUI->uiSys, NULL, { U(1), U(0), U(5), U(2) }, pFmtTitle, (wchar_t*)L"Foxhop.lib", MainTitleParam);
        pTitleSub = new UI_Static(pUI->uiSys, NULL, { U(5.5), U(1), U(3), U(1) }, pFmtTitleSub, (wchar_t*)L"Demo play", SubTitleParam);
        pTopLine = new UI_Line(pUI->uiSys, TopLineVertex, ARRAYSIZE(TopLineVertex), NULL, TRUE, LineParam);
        pBotLine = new UI_Line(pUI->uiSys, BotLineVertex, ARRAYSIZE(BotLineVertex), NULL, TRUE, LineParam);
        
        pLabelPacketMon = new UI_Static(pUI->uiSys, NULL, { U(1), U(2.5), U(19), U(1)}, pFmtTitleSub, (wchar_t*)L"DATA FLOW MONITORING .", LabelParam);
        pTablePacket = new UI_Table(pUI->uiSys, TestTableProc, {U(1),U(3.5),U(19),U(11)}, ARRAYSIZE(PacketDumpCols), PacketDumpCols, PacketDumpColWIdth, 30, 20, FALSE, TableParam);
        pStaticPacketSel = new UI_Static(pUI->uiSys, NULL, { U(1), U(14.5), U(19), U(0.5) }, pUI->uiSys->MediumTextForm, (wchar_t*)L"Done.", StaticParam);

        pLabelAnalyiseTx = new UI_Static(pUI->uiSys, NULL, { U(21), U(2.5), U(10), U(1)}, pFmtTitleSub, (wchar_t*)L"TX Analyze .", LabelParam);
        pTableTx = new UI_Table(pUI->uiSys, NULL, { U(21), U(3.5), U(10), U(5) }, ARRAYSIZE(AnalyseCol), AnalyseCol, AnalyseColWidth, 30, 20, FALSE, TableParam);

        pLabelAnalyiseRx = new UI_Static(pUI->uiSys, NULL, { U(21), U(9), U(10), U(1) }, pFmtTitleSub, (wchar_t*)L"RX Analyze .", LabelParam);
        pTableRx = new UI_Table(pUI->uiSys, NULL, { U(21), U(10), U(10), U(5) }, ARRAYSIZE(AnalyseCol), AnalyseCol, AnalyseColWidth, 30, 20, FALSE, TableParam);

        pButtonConnInfo = new UI_Button(pUI->uiSys, TestConnInfoButtonProc, { U(1), U(16.5), U(4), U(1)}, pFmtButton, (wchar_t*)L"Connection Analyse", 0, ButtonParam);
        pButtonDetailInfo = new UI_Button(pUI->uiSys, TestDetailInfoButtonProc, { U(6), U(16.5), U(4), U(1) }, pFmtButton, (wchar_t*)L"Detail Information", 0, ButtonParam);
        pButtonOutro = new UI_Button(pUI->uiSys, TestOutroButtonProc, { U(11), U(16.5), U(4), U(1) }, pFmtButton, (wchar_t*)L"Finale", 0, ButtonParam);
        pLabelTimer = new UI_Static(pUI->uiSys, NULL, { U(23), U(16), U(8), U(2) }, pFmtTimer, (wchar_t*)L"", TimerParam);
        pPanel->RegisterUI(pIntroText); /*인트로용 임시 UI*/
        pPanel->RegisterUI(pIntroText2);
        pPanel->RegisterUI(pIntroText3);
        pPanel->RegisterUI(pIntroText4);
        pPanel->RegisterUI(pIntroLine);

        pPanel->RegisterUI(pTitleMain); /*메인 UI*/
        pPanel->RegisterUI(pTitleSub);
        pPanel->RegisterUI(pTopLine);
        pPanel->RegisterUI(pBotLine);
        
        pPanel->RegisterUI(pLabelPacketMon);
        pPanel->RegisterUI(pTablePacket);
        pPanel->RegisterUI(pStaticPacketSel);
        pPanel->RegisterUI(pLabelAnalyiseTx);
        pPanel->RegisterUI(pTableTx);
        pPanel->RegisterUI(pLabelAnalyiseRx);
        pPanel->RegisterUI(pTableRx);

        pPanel->RegisterUI(pButtonConnInfo);
        pPanel->RegisterUI(pButtonDetailInfo);
        pPanel->RegisterUI(pButtonOutro);
        pPanel->RegisterUI(pLabelTimer);



        _beginthreadex(NULL, NULL, thread_scene_intro, 0, 0, &ThreadID);
        _beginthreadex(NULL, NULL, thread_update_render, 0, 0, &ThreadID);
#if 0
        pFmt = pUI->uiSys->CreateTextFmt((wchar_t*)L"Agency FB", 30, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        pPauseButton = new UI_Button(pUI->uiSys, TestPauseButtonProc, { 10,10,100,20 }, (wchar_t*)L"UI Pause", 500, ButtonParam);
        pResumeButton = new UI_Button(pUI->uiSys, TestResumeButtonProc, { 120,10,100,20 }, (wchar_t*)L"UI Resume", 800, ButtonParam);
        pTitle1 = new UI_Static(pUI->uiSys, NULL, { 10, 40, 450, 40 }, pFmt, (wchar_t*)L"PACKET MONITORING", TitleParam);
        pTitle2 = new UI_Static(pUI->uiSys, NULL, { 470, 40, 230, 40 }, pFmt, (wchar_t*)L"CONNECTION ANALYZE", TitleParam);
        pTable = new UI_Table(pUI->uiSys, TestTableProc, { 10, 90, 450 , 520 }, 3, PacketDumpCols, PacketDumpColWIdth, 30, 20, FALSE, TableParam);
        pStatic = new UI_Static(pUI->uiSys, NULL, { 10, 620, 450, 25 }, pUI->uiSys->MediumTextForm, (wchar_t*)L"Done.", StaticParam);
        pInput = new UI_Textinput(pUI->uiSys, NULL, { 10, 655, 450, 150 }, pUI->uiSys->CreateTextFmt((wchar_t*)L"Consolas", 15, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_NEAR), InputParam);
        pTable2 = new UI_Table(pUI->uiSys, NULL, { 470, 90, 230 , 715 }, 2, AnalyseCol, AnalyseColWidth, 30, 25, FALSE, TableParam);
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
        pPauseButton->resume(700);
        pResumeButton->resume(1000);
        pTable->resume(0);
        pStatic->resume(200);
        pTitle1->resume(500);
        pTitle2->resume(700);
        pInput->resume(300);
        pTable2->resume(300);
        pChart1->resume(500);
#endif
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
    //hWnd = CreateWindow(CLASSNAME, CLASSNAME, WS_OVERLAPPEDWINDOW, 0, 0, 1920, 1080, NULL, NULL, hInst, NULL);
    hWnd = CreateWindow(CLASSNAME, CLASSNAME, WS_POPUP, 0, 0, 1920, 1080, NULL, NULL, hInst, NULL);
    AlphaWindow(hWnd, WINDOWMODE_TRANSPARENT);
    ShowWindow(hWnd, TRUE);
    InitializeSRWLock(&lock);
    while (GetMessage(&Message, NULL, NULL, NULL)) {
        TranslateMessage(&Message);
        DispatchMessage(&Message);
    }
    return (int)Message.wParam;
}
