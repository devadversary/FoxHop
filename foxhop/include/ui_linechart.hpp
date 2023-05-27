#pragma once

#include "ui_base.hpp"
#include "ui_system.hpp"
#include <vector>

/**
    @brief 스태틱 모션 타입
*/
enum class eLineChartMotionPattern {
    eInitFrame_Default,
    ePauseFrame_Default,

    eInitBg_Default,
    ePauseBg_Default,

    eInitText_Default,
    ePauseText_Default,

    eInitDataOrder_Default,
    ePauseDataOrder_Default,

    eInitChart_Default,
    ePauseChart_Default,

    eAdjustData_Default,
};

class UI_LineChart_MotionParam {
public:
    eLineChartMotionPattern MotionInitBg;
    unsigned long PitchInitBg;
    unsigned long DelayInitBg;
    eLineChartMotionPattern MotionPauseBg;
    unsigned long PitchPauseBg;
    unsigned long DelayPauseBg;

    eLineChartMotionPattern MotionInitFrame;
    unsigned long PitchInitFrame;
    unsigned long DelayInitFrame;
    eLineChartMotionPattern MotionPauseFrame;
    unsigned long PitchPauseFrame;
    unsigned long DelayPauseFrame;

    eLineChartMotionPattern MotionInitText;
    unsigned long PitchInitText;
    unsigned long DelayInitText;
    eLineChartMotionPattern MotionPauseText;
    unsigned long PitchPauseText;
    unsigned long DelayPauseText;

    eLineChartMotionPattern MotionInitDataOrder;
    unsigned long PitchInitDataOrder;
    unsigned long DelayInitDataOrder;
    eLineChartMotionPattern MotionPauseDataOrder;
    unsigned long PitchPauseDataOrder;
    unsigned long DelayPauseDataOrder;

    eLineChartMotionPattern MotionInitChart;
    unsigned long PitchInitChart;
    unsigned long DelayInitChart;
    eLineChartMotionPattern MotionPauseChart;
    unsigned long PitchPauseChart;
    unsigned long DelayPauseChart;

    eLineChartMotionPattern MotionAdjustData;
    unsigned long PitchAdjustData;
    unsigned long DelayAdjustData;

    unsigned long PitchScroll;
    D2D1_COLOR_F ColorFrame;
    D2D1_COLOR_F ColorBg;
    D2D1_COLOR_F ColorChartLine;
    D2D1_COLOR_F ColorGuideLine;

public:
    UI_LineChart_MotionParam() {
        MotionInitBg = eLineChartMotionPattern::eInitBg_Default;
        PitchInitBg = 0;
        DelayInitBg = 0;
        MotionPauseBg = eLineChartMotionPattern::ePauseBg_Default;
        PitchPauseBg = 0;
        DelayPauseBg = 0;

        MotionInitFrame = eLineChartMotionPattern::eInitFrame_Default;
        PitchInitFrame = 0;
        DelayInitFrame = 0;
        MotionPauseFrame = eLineChartMotionPattern::ePauseFrame_Default;
        PitchPauseFrame = 0;
        DelayPauseFrame = 0;

        MotionInitText = eLineChartMotionPattern::eInitText_Default;
        PitchInitText = 0;
        DelayInitText = 0;
        MotionPauseText = eLineChartMotionPattern::ePauseText_Default;
        PitchPauseText = 0;
        DelayPauseText = 0;

        MotionInitDataOrder = eLineChartMotionPattern::eInitDataOrder_Default;
        PitchInitDataOrder = 0;
        DelayInitDataOrder = 0;
        MotionPauseDataOrder = eLineChartMotionPattern::ePauseDataOrder_Default;
        PitchPauseDataOrder = 0;
        DelayPauseDataOrder = 0;

        MotionInitChart = eLineChartMotionPattern::eInitChart_Default;
        PitchInitChart = 0;
        DelayInitChart = 0;
        MotionPauseChart = eLineChartMotionPattern::ePauseChart_Default;
        PitchPauseChart = 0;
        DelayPauseChart = 0;

        MotionAdjustData = eLineChartMotionPattern::eAdjustData_Default;
        PitchAdjustData = 0;
        DelayAdjustData = 0;

        PitchScroll = 150;
        ColorFrame = { 0.f,0.f,0.f,1.f };
        ColorBg = { 1.f, 1.f, 1.f, 1.f };
        ColorChartLine = { 0,0,0,1 };
        ColorGuideLine = { 0.8,0.8,0.8,1 };
    }
};

class ChartObject;

typedef struct _st_chartdata {
    BOOL MotionPlayed;
    BOOL Selected;
    float val;
} CHART_DATA;

class UI_LineChart : public UI {
public:
    UI_LineChart_MotionParam Motion;

private:
    SRWLOCK lock;
    ID2D1StrokeStyle* pStroke;
    ComponentMotion* ScrollComp;
    PropBox* pBoxBg;
    PropBox* pBoxFrame;
    float CurrentScroll;
    long long ScrollPixel;
    unsigned long ViewDataWidth; /*차트 값 하나의 폭*/
    unsigned long ViewDataCnt;
    unsigned long ValidViewDataCnt;
    long long ViewStartIdx; /**< 현재 뷰의 가장 첫 인덱스*/
    float EnsureMin;
    float EnsureMax;
    float IntervalMin;
    float IntervalMax;
    unsigned long SplitCnt;
    float BaseSplit; /**< 50이면, 이 값을 기준으로 100, 50, 25, 12.5 등의 단위로 구간이 분할된다.*/
    std::vector<float> MainData;
    std::vector<PropLine*> PropLines;
    std::vector<ChartObject*> ViewData;

private:
    static void DefaultLineChartProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam);

    void SetScroll(long long TargetScrollPx);

    void ResumeFrame(unsigned long Delay);
    void PauseFrame(unsigned long Delay);
    void ResumeBg(unsigned long Delay);
    void PauseBg(unsigned long Delay);
    void ResumeText(unsigned long Delay);
    void PauseText(unsigned long Delay);
    void ResumeDataOrder(unsigned long Delay);
    void PauseDataOrder(unsigned long Delay);

    long long DataIdx2ViewRowIdx(unsigned long long DataIdx);
    BOOL DataIdxIsInScreen(unsigned long long DataIdx);

public: /*반드시 있어야 되는 매서드*/
    UI_LineChart(UISystem* pUISys, pfnUIHandler pfnCallback, POSITION Pos, IDWriteTextFormat* pTextFmt, unsigned long ChartPixelGap, float EnsureMinVal, float EnsureMaxVal, float BaseSplitVal, unsigned long nSplitCnt, unsigned long Delay, UI_LineChart_MotionParam MotionParam = UI_LineChart_MotionParam());
    ~UI_LineChart();

    void AddValue(float Val, BOOL bAutoScroll);
    void EditValue(unsigned long long DataIdx, float NewVal);

    void resume(int nDelay);
    void pause(int nDelay);
    BOOL update(unsigned long time);
    void render();

public: /*UI별 옵션 매서드*/
    void AddValue(float value, float max, float min);
};

class ChartObject {
public:
    UI_LineChart* pChart;
    ID2D1StrokeStyle* pStroke;
    POSITION Pos;
    long long MainDataIdx; /*이 데이터가 가리키는 MainData 인덱스*/
    float Value;
    float ValueMax;
    float ValueMin;
    PropLine* pLine;
    PropBox*  pBoxPoint;
    PropCircle* pCircle;
    PropText* pText;

public:
    void Resumeline(BOOL bMotion, unsigned long Delay);
    void Pauseline(unsigned long Delay);

public:
    ChartObject(UI_LineChart* pParent, ID2D1StrokeStyle* Stroke, unsigned long width, unsigned long height);
    ~ChartObject();

    void SetValue(BOOL bMotion, float Value, float Max, float Min);
    BOOL update(unsigned long time);
    void render();
    void pause(unsigned long Delay);
    void resume(unsigned long Delay);
};