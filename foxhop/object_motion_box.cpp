#include "./include/object_motion_box.hpp"

ObjectMotionBox::ObjectMotionBox() {}

ObjectMotionBox::~ObjectMotionBox() {}

void ObjectMotionBox::ClearMovementMotion()
{
    ComMotionMovement.clearChannel();
}

void ObjectMotionBox::ClearColorMotion()
{
    ComMotionColor.clearChannel();
}

/**
    @brief  ������Ʈ�� �ʱ� �Ӽ� ����
    @remark ������ �ð����� ȭ�鿡 ������ �ʰ� �Ϸ��� StartColor�� ���İ��� 0���� ����.
*/
void ObjectMotionBox::Init(ID2D1RenderTarget* pRT, POSITION StartPos, D2D1_COLOR_F StartColor, BOOL Fill)
{
    if (!Brush)pRT->CreateSolidColorBrush(StartColor, &Brush);
    InitPos = StartPos;
    InitColor = StartColor;
    CurPos = StartPos;
    CurColor = StartColor;
    bFill = Fill;
    ComMotionMovement.clearChannel();
    ComMotionColor.clearChannel();
}

/**
    @brief  �̵� ����� ä�ο� �߰��Ѵ�. (���� ����� ������ Ȱ��)
    @remark ä�ε��� ������ ��⿭�� ����� �� �ִ�.
    @n      � ����� ����ǰ� �� ��. �̾ �ٸ� ����� �����ϴ� ������
    @n      �������� �ִϸ��̼��� ���� �� �� �ִ�.
    @n      �÷� ��� ä�ΰ� �Բ� ���̸� �� �پ��� ����� ���� �� �� �ִ�.
*/
void ObjectMotionBox::addMovementMotion(MOTION_INFO MotionInfo, BOOL bAppend, POSITION StartPos, POSITION EndPos)
{
    MOTION_PATTERN mc;

    mc = InitMotionPattern(MotionInfo, NULL);
    AddChain(&mc, (float*)&CurPos.x, StartPos.x, EndPos.x);
    AddChain(&mc, (float*)&CurPos.y, StartPos.y, EndPos.y);
    AddChain(&mc, (float*)&CurPos.width, StartPos.width, EndPos.width);
    AddChain(&mc, (float*)&CurPos.height, StartPos.height, EndPos.height);
    if (bAppend) ComMotionMovement.appendChannel(mc);
    else ComMotionMovement.addChannel(mc);
}

/**
    @brief  �÷� ����� ä�ο� �߰��Ѵ�. (���� ����� ������ Ȱ��)
    @remark ä�ε��� ������ ��⿭�� ����� �� �ִ�.
    @n      � ����� ����ǰ� �� ��. �̾ �ٸ� ����� �����ϴ� ������
    @n      �������� �ִϸ��̼��� ���� �� �� �ִ�.
    @n      �̵� ��� ä�ΰ� �Բ� ���̸� �� �پ��� ����� ���� �� �� �ִ�.
*/
void ObjectMotionBox::addColorMotion(MOTION_INFO MotionInfo, BOOL bAppend, D2D1_COLOR_F StartColor, D2D1_COLOR_F EndColor)
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

/**
    @brief �̹� ������ ������Ʈ�� ���� ��ġ�� �̵� (���� ����϶� Ȱ��)
    @param bCurrent �� ���� TRUE �̸� StartPos�� �����ϰ� ���� ��ġ���� EndPos�� �̵��Ѵ�.
*/
void ObjectMotionBox::SetPos(MOTION_INFO MotionInfo, BOOL bCurrent, POSITION StartPos, POSITION EndPos)
{
    POSITION TmpPos;

    ComMotionMovement.clearChannel();
    if (bCurrent) TmpPos = CurPos;
    else TmpPos = StartPos;
    addMovementMotion(MotionInfo, FALSE, TmpPos, EndPos);
}

/**
    @brief �̹� ������ ������Ʈ�� ���� �������� ���� (���� ����϶� Ȱ��)
    @param bCurrent �� ���� TRUE �̸� StartPos�� �����ϰ� ���� ��ġ���� EndPos�� �̵��Ѵ�.
*/
void ObjectMotionBox::SetColor(MOTION_INFO MotionInfo, BOOL bCurrent, D2D1_COLOR_F StartColor, D2D1_COLOR_F EndColor)
{
    D2D1_COLOR_F TmpColor;

    ComMotionColor.clearChannel();
    if (bCurrent) TmpColor = CurColor;
    else TmpColor = StartColor;
    addColorMotion(MotionInfo, FALSE, TmpColor, EndColor);
}

void ObjectMotionBox::setRuntime(int t)
{
    CurPos = InitPos;
    CurColor = InitColor;
    ComMotionMovement.setRuntime(t);
    ComMotionColor.setRuntime(t);
}

/**
    @brief ��� ����� ������Ʈ�Ѵ�
*/
BOOL ObjectMotionBox::allUpdate(unsigned long time)
{
    int nTrue = 0;
    ObjectMotionBox* p = this;

    while (p) {
        nTrue += p->ComMotionMovement.update(time);
        nTrue += p->ComMotionColor.update(time);
        p = p->pNext;
    }
    if (!nTrue) return FALSE;
    return TRUE;
}

/**
    @brief  ���� ���� ��ǿ� �°� ������ �Ѵ�
    @remark ������Ʈ���� ���� ��ũ�帮��Ʈ ���¸� ��� ������
    @n      �� �� ������Ʈ���� �� �ż��带 �ѹ��� ȣ�����ָ�
    @n      ��ü ������Ʈ�� ������ �ȴ�. (����� �Լ�ȣ��, ĳ�ù̽� ����)
*/
void ObjectMotionBox::allRender(ID2D1RenderTarget* pRT)
{
    ObjectMotionBox* p = this;
    D2D1_RECT_F rect;

    while(p){
        p->Brush->SetColor(p->CurColor);
        rect = { (float)p->CurPos.x,
                 (float)p->CurPos.y,
                 (float)p->CurPos.x + p->CurPos.width,
                 (float)p->CurPos.y + p->CurPos.height };
        if(bFill)
            pRT->FillRectangle(rect, p->Brush);
        else 
            pRT->DrawRectangle(rect, p->Brush);
        p = p->pNext;
    }
}

/**
    @brief  ���� ��ü ������Ʈ
    @return ���̻� ������Ʈ �� ���� ������ FALSE
*/
BOOL ObjectMotionBox::update(unsigned long time)
{
    int nTrue = 0;

    nTrue += ComMotionMovement.update(time);
    nTrue += ComMotionColor.update(time);
    if (!nTrue) return FALSE;
    return TRUE;
}

/**
    @brief ���� ��ü ������
*/
void ObjectMotionBox::render(ID2D1RenderTarget* pRT)
{
    D2D1_RECT_F rect;

    Brush->SetColor(CurColor);
    rect = { CurPos.x,
             CurPos.y,
             CurPos.x + CurPos.width,
             CurPos.y + CurPos.height };
    if (bFill)
        pRT->FillRectangle(rect, Brush);
    else
        pRT->DrawRectangle(rect, Brush);
}
