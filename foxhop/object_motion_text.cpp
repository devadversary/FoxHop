#include "./include/object_motion_text.hpp"

ObjectMotionText::ObjectMotionText()
{
    Brush = 0;
    CurColor = {0,0,0,0};
    CurPos = { 0,0,0,0 };
    InitLen = 0;
    CurLen = 0;
    InitColor = { 0,0,0,0 };
    InitPos = { 0,0,0,0 };
    bActivate = 0;
    nStrLen = 0;
    pNext = 0;
    pPrev = 0;
    pStr = NULL;
    pTextFmt = NULL;
}

ObjectMotionText::~ObjectMotionText() {}

/**
    @brief  오브젝트의 초기 속성 셋팅
    @remark 딜레이 시간동안 화면에 나가지 않게 하려면 StartColor의 알파값을 0으로 주자.
*/
void ObjectMotionText::Init(ID2D1RenderTarget* pRT, IDWriteTextFormat* pTexFmt, wchar_t* pText, int nTextLen, POSITION StartPos, D2D1_COLOR_F StartColor, int StartLen)
{
    pRT->CreateSolidColorBrush(StartColor, &Brush);
    pTextFmt = pTexFmt; /*폰트 출력 정보 개체별로 지정*/
    InitPos = CurPos = StartPos;
    InitColor = CurColor = StartColor;
    InitLen = StartLen;
    CurLen = (float)StartLen;
    pStr = pText;
    nStrLen = nTextLen;
    ComMotionColor.clearChannel();
    ComMotionMovement.clearChannel();
    ComMotionStrLen.clearChannel();
}

void ObjectMotionText::addMovementMotion(MOTION_INFO MotionInfo, BOOL bAppend, POSITION StartPos, POSITION EndPos)
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

void ObjectMotionText::addColorMotion(MOTION_INFO MotionInfo, BOOL bAppend, D2D1_COLOR_F StartColor, D2D1_COLOR_F EndColor)
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

void ObjectMotionText::addLenMotion(MOTION_INFO MotionInfo, BOOL bAppend, int nStartLen, int nEndLen)
{
    MOTION_PATTERN mc;

    mc = InitMotionPattern(MotionInfo, NULL);
    AddChain(&mc, &CurLen, (float)nStartLen, (float)nEndLen);
    if (bAppend) ComMotionStrLen.appendChannel(mc);
    else ComMotionStrLen.addChannel(mc);
}

void ObjectMotionText::SetPos(MOTION_INFO MotionInfo, BOOL bCurrent, POSITION StartPos, POSITION EndPos)
{
    POSITION TmpPos;

    ComMotionMovement.clearChannel();
    if (bCurrent) TmpPos = CurPos;
    else TmpPos = StartPos;
    addMovementMotion(MotionInfo, FALSE, TmpPos, EndPos);
}

void ObjectMotionText::SetColor(MOTION_INFO MotionInfo, BOOL bCurrent, D2D1_COLOR_F StartColor, D2D1_COLOR_F EndColor)
{
    D2D1_COLOR_F TmpColor;

    ComMotionColor.clearChannel();
    if (bCurrent) TmpColor = CurColor;
    else TmpColor = StartColor;
    addColorMotion(MotionInfo, FALSE, TmpColor, EndColor);
}

/**
    @brief 문자열을 셋팅하고 길이 조절 모션을 넣어준다
    @remark 변경된 텍스트를 바로 적용 하기 때문에 길이조절 한정 bCurrent 인자가 없다
*/
void ObjectMotionText::SetText(MOTION_INFO MotionInfo, wchar_t* pText, int nStartTextLen, int nEndTextLen)
{
    ComMotionStrLen.clearChannel();
    addLenMotion(MotionInfo, FALSE, nStartTextLen, nEndTextLen);
}

void ObjectMotionText::setRuntime(unsigned long time)
{
    CurPos = InitPos;
    CurColor = InitColor;
    CurLen = (float)InitLen;
    ComMotionMovement.setRuntime(time);
    ComMotionColor.setRuntime(time);
    ComMotionStrLen.setRuntime(time);
}

/**
    @brief 상태 업데이트
*/
BOOL ObjectMotionText::update(unsigned long time)
{
    int nTrue = 0;
    /*모션 보정*/
    nTrue += ComMotionMovement.update(time);
    nTrue += ComMotionColor.update(time);
    nTrue += ComMotionStrLen.update(time);
    if (!nTrue) return FALSE;
    return TRUE;
}

/**
    @brief 모션 데이터에 따라 렌더링
*/
void ObjectMotionText::render(ID2D1RenderTarget* pRT)
{
    D2D1_RECT_F rect;

    Brush->SetColor(CurColor);
    rect = { (float)CurPos.x,
                (float)CurPos.y,
                (float)CurPos.x + CurPos.width,
                (float)CurPos.y + CurPos.height };
    pRT->DrawTextW(pStr, (UINT32)CurLen, pTextFmt, rect, Brush);
}

/**
    @brief 상태 업데이트
*/
BOOL ObjectMotionText::allUpdate(unsigned long time)
{
    int nTrue = 0;
    ObjectMotionText* p = this;

    /*모션 보정*/
    while (p) {
        nTrue += ComMotionMovement.update(time);
        nTrue += ComMotionColor.update(time);
        nTrue += ComMotionStrLen.update(time);
        p = p->pNext;
    }
    if (!nTrue) return FALSE;
    return TRUE;
}

/**
    @brief 모션 데이터에 따라 렌더링
*/
void ObjectMotionText::allRender(ID2D1RenderTarget* pRT)
{
    ObjectMotionText* p = this;
    D2D1_RECT_F rect;

    while(p){
        p->Brush->SetColor(CurColor);
        rect = { (float)p->CurPos.x,
                 (float)p->CurPos.y,
                 (float)p->CurPos.x + p->CurPos.width,
                 (float)p->CurPos.y + p->CurPos.height };
        pRT->DrawTextW(p->pStr, (UINT32)p->CurLen, p->pTextFmt, rect, p->Brush );
        p = p->pNext;
    }
}