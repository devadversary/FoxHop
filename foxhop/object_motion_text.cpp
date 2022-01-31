#include "./include/object_motion_text.hpp"

ObjectMotionText::ObjectMotionText() {}

ObjectMotionText::~ObjectMotionText() {}

/**
    @brief  ������Ʈ�� �ʱ� �Ӽ� ����
    @remark ������ �ð����� ȭ�鿡 ������ �ʰ� �Ϸ��� StartColor�� ���İ��� 0���� ����.
*/
void ObjectMotionText::Init(ID2D1RenderTarget* pRT, IDWriteTextFormat* pTexFmt, wchar_t* pText, int nTextLen, POSITION StartPos, D2D1_COLOR_F StartColor, int StartLen)
{
    pRT->CreateSolidColorBrush(StartColor, &Brush);
    pTextFmt = pTexFmt; /*��Ʈ ��� ���� ��ü���� ����*/
    InitPos = CurPos = StartPos;
    InitColor = CurColor = StartColor;
    InitLen = CurLen = StartLen;
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
    AddChain(&mc, &CurLen, nStartLen, nEndLen);
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
    @brief ���ڿ��� �����ϰ� ���� ���� ����� �־��ش�
    @remark ����� �ؽ�Ʈ�� �ٷ� ���� �ϱ� ������ �������� ���� bCurrent ���ڰ� ����
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
    @brief ���� ������Ʈ
*/
BOOL ObjectMotionText::update(unsigned long time)
{
    int nTrue = 0;
    /*��� ����*/
    nTrue += ComMotionMovement.update(time);
    nTrue += ComMotionColor.update(time);
    nTrue += ComMotionStrLen.update(time);
    if (!nTrue) return FALSE;
    return TRUE;
}

/**
    @brief ��� �����Ϳ� ���� ������
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
    @brief ���� ������Ʈ
*/
BOOL ObjectMotionText::allUpdate(unsigned long time)
{
    int nTrue = 0;
    ObjectMotionText* p = this;

    /*��� ����*/
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
    @brief ��� �����Ϳ� ���� ������
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