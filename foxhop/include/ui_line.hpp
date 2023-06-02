#pragma once

#include "ui_base.hpp"
#include "ui_system.hpp"
#include <vector>

#define UISTATIC_MAX_TEXTLEN 1024

/**
    @brief 스태틱 모션 타입
*/
enum class eLineMotionPattern {
    eInitLine_Default,
    eInitLine_Flick,
    eInitLine_Connect,
    ePauseLine_Default,
    ePauseLine_Flick,
    ePauseLine_Disconnect,

    eInitNode_Default,
    eInitNode_Flick,
    ePauseNode_Default,
    ePauseNode_Flick,

    eInitLineOrder_Default,
    eInitLineOrder_Linear,
    eInitLineOrder_Rand,
    ePauseLineOrder_Default,
    ePauseLineOrder_Linear,
    ePauseLineOrder_Rand,

    eInitNodeOrder_Default,
    eInitNodeOrder_Linear,
    eInitNodeOrder_Rand,
    ePauseNodeOrder_Default,
    ePauseNodeOrder_Linear,
    ePauseNodeOrder_Rand,
};

class UI_Line_MotionParam {
public:
    eLineMotionPattern   MotionInitLine;
    unsigned long        PitchInitLine;
    unsigned long        DelayInitLine;
    eLineMotionPattern   MotionPauseLine;
    unsigned long        PitchPauseLine;
    unsigned long        DelayPauseLine;

    eLineMotionPattern   MotionInitNode;
    unsigned long        PitchInitNode;
    unsigned long        DelayInitNode;
    eLineMotionPattern   MotionPauseNode;
    unsigned long        PitchPauseNode;
    unsigned long        DelayPauseNode;

    eLineMotionPattern   MotionInitLineOrder;
    unsigned long        PitchInitLineOrder;
    unsigned long        DelayInitLineOrder;
    unsigned long        GapInitLineOrder;
    unsigned long        RandNoiseInitLineOrder;
    eLineMotionPattern   MotionPauseLineOrder;
    unsigned long        PitchPauseLineOrder;
    unsigned long        DelayPauseLineOrder;
    unsigned long        GapPauseLineOrder;
    unsigned long        RandNoisePauseLineOrder;

    eLineMotionPattern   MotionInitNodeOrder;
    unsigned long        PitchInitNodeOrder;
    unsigned long        DelayInitNodeOrder;
    unsigned long        GapInitNodeOrder;
    unsigned long        RandNoiseInitNodeOrder;
    eLineMotionPattern   MotionPauseNodeOrder;
    unsigned long        PitchPauseNodeOrder;
    unsigned long        DelayPauseNodeOrder;
    unsigned long        GapPauseNodeOrder;
    unsigned long        RandNoisePauseNodeOrder;

    unsigned long        PointSize;
    D2D1_COLOR_F ColorLine;
    D2D1_COLOR_F ColorNode;

public:
    UI_Line_MotionParam() {
        MotionInitLine = eLineMotionPattern::eInitLine_Default;
        PitchInitLine = 0;
        DelayInitLine = 0;
        MotionPauseLine = eLineMotionPattern::ePauseLine_Default;;
        PitchPauseLine = 0;
        DelayPauseLine = 0;

        MotionInitNode = eLineMotionPattern::eInitNode_Default;
        PitchInitNode = 0;
        DelayInitNode = 0;
        MotionPauseNode = eLineMotionPattern::ePauseNode_Default;;
        PitchPauseNode = 0;
        DelayPauseNode = 0;

        MotionInitLineOrder = eLineMotionPattern::eInitLineOrder_Default;
        PitchInitLineOrder = 0;
        DelayInitLineOrder = 0;
        GapInitLineOrder = 0;
        RandNoiseInitLineOrder = 0;
        MotionPauseLineOrder = eLineMotionPattern::ePauseLineOrder_Default;
        PitchPauseLineOrder = 0;
        DelayPauseLineOrder = 0;
        GapPauseLineOrder = 0;
        RandNoisePauseLineOrder = 0;

        MotionInitNodeOrder = eLineMotionPattern::eInitNodeOrder_Default;
        PitchInitNodeOrder = 0;
        DelayInitNodeOrder = 0;
        GapInitNodeOrder = 0;
        RandNoiseInitNodeOrder = 0;
        MotionPauseNodeOrder = eLineMotionPattern::ePauseNodeOrder_Default;
        PitchPauseNodeOrder = 0;
        DelayPauseNodeOrder = 0;
        GapPauseNodeOrder = 0;
        RandNoisePauseNodeOrder = 0;

        PointSize = 4;
        ColorLine = { 0,0,0,1 };
        ColorNode = { 0,0,0,1 };
    }
};

class UI_Line : public UI {
public:
    UI_Line_MotionParam Motion;
    std::vector<POSITION> Points;

private:
    SRWLOCK lock;
    std::vector<PropLine*> pLines;
    std::vector<PropBox*> pNodes;
    ID2D1StrokeStyle* Stroke;
    BOOL Closure;
    unsigned long PointCnt;

private:
    void ResumeLine(unsigned long PointIdx, unsigned long Delay);
    void PauseLine(unsigned long PointIdx, unsigned long Delay);
    void ResumeNode(unsigned long PointIdx, unsigned long Delay);
    void PauseNode(unsigned long PointIdx, unsigned long Delay);
    void ResumeLineOrder(unsigned long Delay);
    void PauseLineOrder(unsigned long Delay);
    void ResumeNodeOrder(unsigned long Delay);
    void PauseNodeOrder(unsigned long Delay);

public: /*반드시 있어야 되는 매서드*/
    /**
        @brief 장식용 선분을 생성한다
        @param pPoints 선을 이을 점 집합. POSITION 구조체에서 x, y 멤버만 사용한다.
    */
    UI_Line(UISystem* pUISys, POSITION* pPoints, size_t nPointCnt, ID2D1StrokeStyle* pLineStyle = NULL, BOOL bClosure = FALSE, UI_Line_MotionParam MotionParam = UI_Line_MotionParam());
    ~UI_Line();
    void pause(int nDelay);
    void resume(int nDelay);
    BOOL update(unsigned long time);
    void render();
};
