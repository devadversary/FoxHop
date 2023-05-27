#include "./include/prop_line.hpp"

PropLine::PropLine(ID2D1RenderTarget* pRT, ID2D1StrokeStyle* pStrokeStyle)
{
    pRT->CreateSolidColorBrush({0,0,0,0}, &Brush);
    InitPos = {0,0,0,0};
    InitColor = { 0,0,0,0 };
    CurPos = { 0,0,0,0 };
    CurColor = { 0,0,0,0 };
    pStroke = pStrokeStyle;
}

PropLine::~PropLine()
{
    if (Brush) Brush->Release();
}

/**
    @brief  오브젝트의 초기 속성 셋팅
    @remark 딜레이 시간동안 화면에 나가지 않게 하려면 StartColor의 알파값을 0으로 주자.
*/
void PropLine::Init(POSITION StartPoint, D2D1_COLOR_F StartColor)
{
    InitPos = CurPos = StartPoint;
    InitColor = CurColor = StartColor;
    ComMotionMovement.clearChannel();
    ComMotionColor.clearChannel();
}

/**
    @brief  이동 모션을 채널에 추가한다.
    @remark 채널들을 일종의 대기열로 사용할 수 있다.
    @n      어떤 모션이 진행되고 난 후. 이어서 다른 모션을 진행하는 식으로
    @n      복합적인 애니메이션을 구현 할 수 있다.
    @n      컬러 모션 채널과 함께 섞이면 더 다양한 모션을 구현 할 수 있다.
*/
void PropLine::addMovementMotion(MOTION_INFO MotionInfo, BOOL bAppend, POSITION StartPos, POSITION EndPos)
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
    @brief  컬러 모션을 채널에 추가한다.
    @remark 채널들을 일종의 대기열로 사용할 수 있다.
    @n      어떤 모션이 진행되고 난 후. 이어서 다른 모션을 진행하는 식으로
    @n      복합적인 애니메이션을 구현 할 수 있다.
    @n      이동 모션 채널과 함께 섞이면 더 다양한 모션을 구현 할 수 있다.
*/
void PropLine::addColorMotion(MOTION_INFO MotionInfo, BOOL bAppend, D2D1_COLOR_F StartColor, D2D1_COLOR_F EndColor)
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
    @brief 이미 생성된 오브젝트를 지정 위치로 이동
    @param bCurrent 이 값이 TRUE 이면 StartPos를 무시하고 현재 위치부터 EndPos로 이동한다.
*/
void PropLine::SetPos(MOTION_INFO MotionInfo, BOOL bCurrent, POSITION StartPos, POSITION EndPos)
{
    POSITION TmpPos;

    ComMotionMovement.clearChannel();
    if (bCurrent) TmpPos = CurPos;
    else TmpPos = StartPos;
    addMovementMotion(MotionInfo, FALSE, TmpPos, EndPos);
}

/**
    @brief 이미 생성된 오브젝트를 지정 색상으로 변경
    @param bCurrent 이 값이 TRUE 이면 StartPos를 무시하고 현재 위치부터 EndPos로 이동한다.
*/
void PropLine::SetColor(MOTION_INFO MotionInfo, BOOL bCurrent, D2D1_COLOR_F StartColor, D2D1_COLOR_F EndColor)
{
    D2D1_COLOR_F TmpColor;

    ComMotionColor.clearChannel();
    if (bCurrent) TmpColor = CurColor;
    else TmpColor = StartColor;
    addColorMotion(MotionInfo, FALSE, TmpColor, EndColor);
}

/**
    @brief 개별 업데이트한다
*/
BOOL PropLine::update(unsigned long time)
{
    int nTrue=0;

    nTrue += ComMotionMovement.update(time);
    nTrue += ComMotionColor.update(time);
    if (!nTrue) return FALSE;
    return TRUE;
}

/**
    @brief 현재 진행 모션에 맞게 렌더링 한다
    @remark 오브젝트들이 서로 링크드리스트 형태를 띠기 때문에
    @n      맨 앞 오브젝트에서 이 매서드를 한번만 호출해주면
    @n      전체 오브젝트가 렌더링 된다. (빈번한 함수호출, 캐시미스 방지)
*/
void PropLine::render(ID2D1RenderTarget* pRT)
{
    D2D1_POINT_2F point1, point2;

    Brush->SetColor(CurColor);
    point1 = { CurPos.x, CurPos.y };
    point2 = { CurPos.x2,CurPos.y2 };
    pRT->DrawLine(point1, point2, Brush, 1.0f, pStroke);
}
