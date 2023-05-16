#include "./include/prop_textlayout.hpp"

PropTextLayout::PropTextLayout(ID2D1RenderTarget* pRT, IDWriteTextFormat* pTexFmt, IDWriteFactory* pFactory, int Width, int Height)
{
    pRT->CreateSolidColorBrush({ 0,0,0,0 }, &Brush);
    CurColor = { 0,0,0,0 };
    CurPos = { 0,0,0,0 };
    InitColor = { 0,0,0,0 };
    InitPos = { 0,0,0,0 };
    nStrLen = 0;
    pTextFmt = pTexFmt;
    pDWFactory = pFactory;

    Str.assign(L"");
    pDWFactory->CreateTextLayout(Str.c_str(), Str.size(), pTextFmt, (float)Width, (float)Height, &pLayout);
    //pLayout->SetMaxWidth(uiPos.x2);
    //pLayout->SetMaxHeight(uiPos.y2);

}

PropTextLayout::~PropTextLayout()
{
    if (Brush) Brush->Release();
}

/**
    @brief  오브젝트의 초기 속성 셋팅
    @remark 딜레이 시간동안 화면에 나가지 않게 하려면 StartColor의 알파값을 0으로 주자.
*/
void PropTextLayout::Init(POSITION StartPos, D2D1_COLOR_F StartColor)
{
    InitPos = CurPos = StartPos;
    InitColor = CurColor = StartColor;
    ComMotionColor.clearChannel();
    ComMotionMovement.clearChannel();
    ComMotionStrLen.clearChannel();
}

void PropTextLayout::addMovementMotion(MOTION_INFO MotionInfo, BOOL bAppend, POSITION StartPos, POSITION EndPos)
{
    MOTION_PATTERN mc;

    mc = InitMotionPattern(MotionInfo, NULL);
    AddChain(&mc, &CurPos.x, StartPos.x, EndPos.x);
    AddChain(&mc, &CurPos.y, StartPos.y, EndPos.y);
    AddChain(&mc, &CurPos.width, StartPos.width, EndPos.width);
    AddChain(&mc, &CurPos.height, StartPos.height, EndPos.height);
    if (bAppend) ComMotionMovement.appendChannel(mc);
    else ComMotionMovement.addChannel(mc);
}

void PropTextLayout::addColorMotion(MOTION_INFO MotionInfo, BOOL bAppend, D2D1_COLOR_F StartColor, D2D1_COLOR_F EndColor)
{
    MOTION_PATTERN mc;

    mc = InitMotionPattern(MotionInfo, NULL);
    AddChain(&mc, &CurColor.r, StartColor.r, EndColor.r);
    AddChain(&mc, &CurColor.g, StartColor.g, EndColor.g);
    AddChain(&mc, &CurColor.b, StartColor.b, EndColor.b);
    AddChain(&mc, &CurColor.a, StartColor.a, EndColor.a);
    if (bAppend) ComMotionColor.appendChannel(mc);
    else ComMotionColor.addChannel(mc);
}

BOOL PropTextLayout::EraseText(unsigned long IntervalA, unsigned long IntervalB)
{
    int si, ei, distance;
    if (IntervalA == IntervalB) return FALSE;
    si = min(IntervalA, IntervalB);
    ei = max(IntervalA, IntervalB);
    distance = ei - si;
    Str.erase(si, distance);
    UpdateTextLayout();
    return TRUE;
}

void PropTextLayout::InsertChar(unsigned long Idx, wchar_t wch)
{
    Str.insert(Idx, 1, wch);
    UpdateTextLayout();
}

void PropTextLayout::ReplaceChar(unsigned long Idx, wchar_t wch)
{
    Str.replace(Idx, 1, 1, wch);
    UpdateTextLayout();
}


void PropTextLayout::SetPos(MOTION_INFO MotionInfo, BOOL bCurrent, POSITION StartPos, POSITION EndPos)
{
    POSITION TmpPos;

    ComMotionMovement.clearChannel();
    if (bCurrent) TmpPos = CurPos;
    else TmpPos = StartPos;
    addMovementMotion(MotionInfo, FALSE, TmpPos, EndPos);
}

void PropTextLayout::SetColor(MOTION_INFO MotionInfo, BOOL bCurrent, D2D1_COLOR_F StartColor, D2D1_COLOR_F EndColor)
{
    D2D1_COLOR_F TmpColor;

    ComMotionColor.clearChannel();
    if (bCurrent) TmpColor = CurColor;
    else TmpColor = StartColor;
    addColorMotion(MotionInfo, FALSE, TmpColor, EndColor);
}

void PropTextLayout::UpdateTextLayout()
{
    IDWriteTextLayout* pNewLayout;
    IDWriteTextLayout* pOldLayout;

    pDWFactory->CreateTextLayout(Str.c_str(), Str.size(), pTextFmt, InitPos.x2, InitPos.y2, &pNewLayout);
    pOldLayout = pLayout;
    pLayout = pNewLayout;
    pOldLayout->Release();
}

/**
    @brief 상태 업데이트
*/
BOOL PropTextLayout::update(unsigned long time)
{
    BOOL bUpdated = FALSE;
    /*모션 보정*/
    bUpdated |= ComMotionMovement.update(time);
    bUpdated |= ComMotionColor.update(time);
    bUpdated |= ComMotionStrLen.update(time);
    if (!bUpdated) return FALSE;
    return TRUE;
}

/**
    @brief 모션 데이터에 따라 렌더링
*/
void PropTextLayout::render(ID2D1RenderTarget* pRT)
{
    D2D1_RECT_F rect;
    Brush->SetColor(CurColor);
    pRT->DrawTextLayout({ CurPos.x , CurPos.y}, pLayout, Brush);
}