#include "./include/prop_circle.hpp"

PropCircle::PropCircle(ID2D1RenderTarget* pRT)
{
    pRT->CreateSolidColorBrush({ 0,0,0,0 }, &Brush);
    InitPos = { 0,0,0,0 };
    CurPos = { 0,0,0,0 };
    InitColor = { 0,0,0,0 };
    CurColor = { 0,0,0,0 };
    bFill = TRUE;
}

PropCircle::~PropCircle()
{
    if (Brush) Brush->Release();
}

/**
    @brief  ������Ʈ�� �ʱ� �Ӽ� ����
    @remark ������ �ð����� ȭ�鿡 ������ �ʰ� �Ϸ��� StartColor�� ���İ��� 0���� ����.
*/
void PropCircle::Init(POSITION StartPos, D2D1_COLOR_F StartColor, BOOL Fill)
{
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
void PropCircle::addMovementMotion(MOTION_INFO MotionInfo, BOOL bAppend, POSITION StartPos, POSITION EndPos)
{
    MOTION_PATTERN mc;

    mc = InitMotionPattern(MotionInfo, NULL);
    AddChain(&mc, (float*)&CurPos.x, StartPos.x, EndPos.x);
    AddChain(&mc, (float*)&CurPos.y, StartPos.y, EndPos.y);
    AddChain(&mc, (float*)&CurPos.r, StartPos.r, EndPos.r);
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
void PropCircle::addColorMotion(MOTION_INFO MotionInfo, BOOL bAppend, D2D1_COLOR_F StartColor, D2D1_COLOR_F EndColor)
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
void PropCircle::SetPos(MOTION_INFO MotionInfo, BOOL bCurrent, POSITION StartPos, POSITION EndPos)
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
void PropCircle::SetColor(MOTION_INFO MotionInfo, BOOL bCurrent, D2D1_COLOR_F StartColor, D2D1_COLOR_F EndColor)
{
    D2D1_COLOR_F TmpColor;

    ComMotionColor.clearChannel();
    if (bCurrent) TmpColor = CurColor;
    else TmpColor = StartColor;
    addColorMotion(MotionInfo, FALSE, TmpColor, EndColor);
}

void PropCircle::setRuntime(int t)
{
    CurPos = InitPos;
    CurColor = InitColor;
    ComMotionMovement.setRuntime(t);
    ComMotionColor.setRuntime(t);
}

/**
    @brief  ���� ��ü ������Ʈ
    @return ���̻� ������Ʈ �� ���� ������ FALSE
*/
BOOL PropCircle::update(unsigned long time)
{
    BOOL bUpdated = FALSE;

    bUpdated |= ComMotionMovement.update(time);
    bUpdated |= ComMotionColor.update(time);
    if (!bUpdated) return FALSE;
    return TRUE;
}

/**
    @brief ���� ��ü ������
*/
void PropCircle::render(ID2D1RenderTarget* pRT)
{
    D2D1_RECT_F rect;

    Brush->SetColor(CurColor);

    if (bFill)
        pRT->FillEllipse({{CurPos.x, CurPos.y}, CurPos.r, CurPos.r}, Brush);
    else
        pRT->DrawEllipse({{CurPos.x, CurPos.y}, CurPos.r, CurPos.r}, Brush);
}
