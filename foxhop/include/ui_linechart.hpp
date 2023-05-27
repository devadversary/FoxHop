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

    eInitTotalLine_Default,
    ePauseTotalLine_Default,

    eInitDataOrder_Default,
    ePauseDataOrder_Default,

    eInitChartGuideLine_Default,
    ePauseChartGuideLine_Default,

    eInitChartPoint_Default,
    ePauseChartPoint_Default,

    eInitChartLabel_Default,
    ePauseChartLabel_Default,

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

    eLineChartMotionPattern MotionInitTotalLine;
    unsigned long PitchInitTotalLine;
    unsigned long DelayInitTotalLine;
    eLineChartMotionPattern MotionPauseTotalLine;
    unsigned long PitchPauseTotalLine;
    unsigned long DelayPauseTotalLine;

    eLineChartMotionPattern MotionInitChartGuideLine;
    unsigned long PitchInitChartGuideLine;
    unsigned long DelayInitChartGuideLine;
    eLineChartMotionPattern MotionPauseChartGuideLine;
    unsigned long PitchPauseChartGuideLine;
    unsigned long DelayPauseChartGuideLine;

    eLineChartMotionPattern MotionInitChartPoint;
    unsigned long PitchInitChartPoint;
    unsigned long DelayInitChartPoint;
    eLineChartMotionPattern MotionPauseChartPoint;
    unsigned long PitchPauseChartPoint;
    unsigned long DelayPauseChartPoint;

    eLineChartMotionPattern MotionInitChartLabel;
    unsigned long PitchInitChartLabel;
    unsigned long DelayInitChartLabel;
    eLineChartMotionPattern MotionPauseChartLabel;
    unsigned long PitchPauseChartLabel;
    unsigned long DelayPauseChartLabel;

    eLineChartMotionPattern MotionAdjustData;
    unsigned long PitchAdjustData;
    unsigned long DelayAdjustData;

    unsigned long PitchScroll;
    D2D1_COLOR_F ColorFrame;
    D2D1_COLOR_F ColorBg;
    D2D1_COLOR_F ColorChartLine;
    D2D1_COLOR_F ColorChartPoint;
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

        MotionInitTotalLine = eLineChartMotionPattern::eInitTotalLine_Default;
        PitchInitTotalLine = 0;
        DelayInitTotalLine = 0;
        MotionPauseTotalLine = eLineChartMotionPattern::ePauseTotalLine_Default;
        PitchPauseTotalLine = 0;
        DelayPauseTotalLine = 0;

        MotionInitChartGuideLine = eLineChartMotionPattern::eInitChartGuideLine_Default;
        PitchInitChartGuideLine = 0;
        DelayInitChartGuideLine = 0;
        MotionPauseChartGuideLine = eLineChartMotionPattern::ePauseChartGuideLine_Default;
        PitchPauseChartGuideLine = 0;
        DelayPauseChartGuideLine = 0;

        MotionInitChartPoint = eLineChartMotionPattern::eInitChartPoint_Default;
        PitchInitChartPoint = 0;
        DelayInitChartPoint = 0;
        MotionPauseChartPoint = eLineChartMotionPattern::ePauseChartPoint_Default;
        PitchPauseChartPoint = 0;
        DelayPauseChartPoint = 0;

        MotionInitChartLabel = eLineChartMotionPattern::eInitChartLabel_Default;
        PitchInitChartLabel = 0;
        DelayInitChartLabel = 0;
        MotionPauseChartLabel = eLineChartMotionPattern::ePauseChartLabel_Default;
        PitchPauseChartLabel = 0;
        DelayPauseChartLabel = 0;

        MotionAdjustData = eLineChartMotionPattern::eAdjustData_Default;
        PitchAdjustData = 0;
        DelayAdjustData = 0;

        PitchScroll = 150;
        ColorFrame = { 0.f,0.f,0.f,1.f };
        ColorBg = { 1.f, 1.f, 1.f, 1.f };
        ColorChartLine = { 0,0,0,1 };
        ColorChartPoint = { 0,0,0,1 };
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
    unsigned long PointPixelSize;
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
    void AddValue(float value, float max, float min, wchar_t);
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
    PropLine* pGuideLine;
    PropBox*  pBoxPoint;
    PropCircle* pCircle;
    PropText* pLabel;

public:
    void ResumeGuideLine(BOOL bMotion, unsigned long Delay);
    void PauseGuideLine(unsigned long Delay);
    void ResumePoint(BOOL bMotion, unsigned long Delay);
    void PausePoint(unsigned long Delay);
    void ResumeLabel(BOOL bMotion, unsigned long Delay);
    void PauseLabel(unsigned long Delay);

public:
    ChartObject(UI_LineChart* pParent, ID2D1StrokeStyle* Stroke, unsigned long width, unsigned long height);
    ~ChartObject();

    void SetValue(BOOL bMotion, float Value, float Max, float Min);
    BOOL update(unsigned long time);
    void render();
    void pause(unsigned long Delay);
    void resume(unsigned long Delay);
};