#include "./include/object_motion_line.hpp"

ObjectMotionLine::ObjectMotionLine() {}

ObjectMotionLine::~ObjectMotionLine() {}

void ObjectMotionLine::ClearMovementMotion()
{
    ComMotionMovement.clearChannel();
}

/**
    @brief  ������Ʈ�� �ʱ� �Ӽ� ����
    @remark ������ �ð����� ȭ�鿡 ������ �ʰ� �Ϸ��� StartColor�� ���İ��� 0���� ����.
*/
void ObjectMotionLine::Init(ID2D1RenderTarget* pRT, POSITION StartPoint, D2D1_COLOR_F StartColor)
{
    if(!Brush) pRT->CreateSolidColorBrush(StartColor, &Brush);
    InitPos = CurPos = StartPoint;
    InitColor = CurColor = StartColor;
    ComMotionMovement.clearChannel();
    ComMotionColor.clearChannel();
}

/**
    @brief  �̵� ����� ä�ο� �߰��Ѵ�.
    @remark ä�ε��� ������ ��⿭�� ����� �� �ִ�.
    @n      � ����� ����ǰ� �� ��. �̾ �ٸ� ����� �����ϴ� ������
    @n      �������� �ִϸ��̼��� ���� �� �� �ִ�.
    @n      �÷� ��� ä�ΰ� �Բ� ���̸� �� �پ��� ����� ���� �� �� �ִ�.
*/
void ObjectMotionLine::addMovementMotion(MOTION_INFO MotionInfo, BOOL bAppend, POSITION StartPos, POSITION EndPos)
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
    @brief  �÷� ����� ä�ο� �߰��Ѵ�.
    @remark ä�ε��� ������ ��⿭�� ����� �� �ִ�.
    @n      � ����� ����ǰ� �� ��. �̾ �ٸ� ����� �����ϴ� ������
    @n      �������� �ִϸ��̼��� ���� �� �� �ִ�.
    @n      �̵� ��� ä�ΰ� �Բ� ���̸� �� �پ��� ����� ���� �� �� �ִ�.
*/
void ObjectMotionLine::addColorMotion(MOTION_INFO MotionInfo, BOOL bAppend, D2D1_COLOR_F StartColor, D2D1_COLOR_F EndColor)
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
    @brief �̹� ������ ������Ʈ�� ���� ��ġ�� �̵�
    @param bCurrent �� ���� TRUE �̸� StartPos�� �����ϰ� ���� ��ġ���� EndPos�� �̵��Ѵ�.
*/
void ObjectMotionLine::SetPos(MOTION_INFO MotionInfo, BOOL bCurrent, POSITION StartPos, POSITION EndPos)
{
    POSITION TmpPos;

    ComMotionMovement.clearChannel();
    if (bCurrent) TmpPos = CurPos;
    else TmpPos = StartPos;
    addMovementMotion(MotionInfo, FALSE, TmpPos, EndPos);
}

/**
    @brief �̹� ������ ������Ʈ�� ���� �������� ����
    @param bCurrent �� ���� TRUE �̸� StartPos�� �����ϰ� ���� ��ġ���� EndPos�� �̵��Ѵ�.
*/
void ObjectMotionLine::SetColor(MOTION_INFO MotionInfo, BOOL bCurrent, D2D1_COLOR_F StartColor, D2D1_COLOR_F EndColor)
{
    D2D1_COLOR_F TmpColor;

    ComMotionColor.clearChannel();
    if (bCurrent) TmpColor = CurColor;
    else TmpColor = StartColor;
    addColorMotion(MotionInfo, FALSE, TmpColor, EndColor);
}

void ObjectMotionLine::setRuntime(int t)
{
    CurPos = InitPos;
    CurColor = InitColor;
    ComMotionMovement.setRuntime(t);
    ComMotionColor.setRuntime(t);
}

/**
    @brief ��� ����� ������Ʈ�Ѵ�
*/
BOOL ObjectMotionLine::allUpdate(unsigned long time)
{
    int nTrue = 0;
    ObjectMotionLine* p = this;

    while (p) {
        nTrue += p->ComMotionMovement.update(time);
        nTrue += p->ComMotionColor.update(time);
        p = p->pNext;
    }
    if (!nTrue) return FALSE;
    return TRUE;
}

/**
    @brief ���� ���� ��ǿ� �°� ������ �Ѵ�
    @remark ������Ʈ���� ���� ��ũ�帮��Ʈ ���¸� ��� ������
    @n      �� �� ������Ʈ���� �� �ż��带 �ѹ��� ȣ�����ָ�
    @n      ��ü ������Ʈ�� ������ �ȴ�. (����� �Լ�ȣ��, ĳ�ù̽� ����)
*/
void ObjectMotionLine::allRender(ID2D1RenderTarget* pRT)
{
    ObjectMotionLine* p = this;
    D2D1_POINT_2F point1, point2;

    while (p) {
        p->Brush->SetColor(p->CurColor);
        point1 = { p->CurPos.x, p->CurPos.y };
        point2 = { p->CurPos.x2, p->CurPos.y2 };
        pRT->DrawLine(point1, point2, p->Brush);
        p = p->pNext;
    }
}
/**
    @brief ���� ������Ʈ�Ѵ�
*/
BOOL ObjectMotionLine::update(unsigned long time)
{
    int nTrue=0;

    nTrue += ComMotionMovement.update(time);
    nTrue += ComMotionColor.update(time);
    if (!nTrue) return FALSE;
    return TRUE;
}

/**
    @brief ���� ���� ��ǿ� �°� ������ �Ѵ�
    @remark ������Ʈ���� ���� ��ũ�帮��Ʈ ���¸� ��� ������
    @n      �� �� ������Ʈ���� �� �ż��带 �ѹ��� ȣ�����ָ�
    @n      ��ü ������Ʈ�� ������ �ȴ�. (����� �Լ�ȣ��, ĳ�ù̽� ����)
*/
void ObjectMotionLine::render(ID2D1RenderTarget* pRT)
{
    D2D1_POINT_2F point1, point2;

    Brush->SetColor(CurColor);
    point1 = { CurPos.x, CurPos.y };
    point2 = { CurPos.x2,CurPos.y2 };
    pRT->DrawLine(point1, point2, Brush);
}
