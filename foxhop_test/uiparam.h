#pragma once

#include "../foxhop/include/ui_system.hpp"
#include "../foxhop/include/ui_base.hpp"
#include "../foxhop/include/ui_button.hpp"
#include "../foxhop/include/ui_static.hpp"
#include "../foxhop/include/ui_line.hpp"
#include "../foxhop/include/ui_table.hpp"
#include "../foxhop/include/ui_textinput.hpp"

UI_Static_MotionParam IntroStaticParam;
UI_Static_MotionParam IntroStaticParam2;
UI_Static_MotionParam IntroStaticParam3;
UI_Line_MotionParam IntroLineParam;

UI_Static_MotionParam MainTitleParam;
UI_Static_MotionParam SubTitleParam;

UI_Button_MotionParam ButtonParam;
UI_Table_MotionParam TableParam;
UI_Static_MotionParam LabelParam;
UI_Static_MotionParam StaticParam;
UI_Static_MotionParam TimerParam;
UI_Textinput_MotionParam InputParam;
UI_LineChart_MotionParam ChartParam;
UI_Static_MotionParam DescMainParam;
UI_Static_MotionParam DescSubParam;
UI_Line_MotionParam LineParam;

void UI_ParamSet() {
	IntroStaticParam.ColorBg = { 1, 0, 0, 0.7 };
	IntroStaticParam.ColorFrame = ALL_ZERO;
	IntroStaticParam.ColorText = {1,1,1,1};

	IntroStaticParam.MotionInitBg = eStaticMotionPattern::eInitBg_ExpendRight;
	IntroStaticParam.PitchInitBg = 400;
	IntroStaticParam.MotionPauseBg = eStaticMotionPattern::ePauseBg_CollapseLeft;
	IntroStaticParam.PitchPauseBg = 400;
	IntroStaticParam.DelayPauseBg = 300;

	IntroStaticParam.MotionInitText = eStaticMotionPattern::eInitText_Typing;
	IntroStaticParam.DelayInitText = 500;
	IntroStaticParam.PitchInitText = 500;
	IntroStaticParam.MotionPauseText = eStaticMotionPattern::ePauseText_Flick;
	IntroStaticParam.DelayPauseText = 0;
	IntroStaticParam.PitchPauseText = 220;

	IntroStaticParam2 = IntroStaticParam;
	IntroStaticParam2.ColorBg = ALL_ZERO;
	IntroStaticParam2.MotionInitBg = eStaticMotionPattern::eInitBg_Default;
	IntroStaticParam2.MotionPauseBg = eStaticMotionPattern::ePauseBg_Default;
	IntroStaticParam2.MotionInitText = eStaticMotionPattern::eInitText_Flick;
	IntroStaticParam2.DelayInitText = 0;
	IntroStaticParam2.PitchInitText = 200;
	IntroStaticParam2.MotionPauseText = eStaticMotionPattern::ePauseText_Flick;
	IntroStaticParam2.DelayPauseText = 0;
	IntroStaticParam2.PitchPauseText = 200;

	IntroStaticParam3 = IntroStaticParam2;
	IntroStaticParam3.ColorText = { 1,0.2,0.2,1 };
	IntroStaticParam3.PitchInitText = 220;
	IntroStaticParam3.PitchPauseText = 220;

	MainTitleParam = IntroStaticParam2;
	SubTitleParam = IntroStaticParam2;

	IntroLineParam.ColorLine = { 1,1,1,1 };
	IntroLineParam.ColorNode = { 1,1,1,1 };
	IntroLineParam.PointSize = 6;

	IntroLineParam.MotionInitLine = eLineMotionPattern::eInitLine_Connect;
	IntroLineParam.PitchInitLine = 250;
	IntroLineParam.MotionPauseLine = eLineMotionPattern::ePauseLine_Disconnect;
	IntroLineParam.PitchPauseLine = 250;

	IntroLineParam.MotionInitNode = eLineMotionPattern::eInitNode_Flick;
	IntroLineParam.PitchInitNode = 220;
	IntroLineParam.MotionPauseNode = eLineMotionPattern::ePauseNode_Flick;
	IntroLineParam.PitchPauseNode = 220;

	IntroLineParam.MotionInitLineOrder = eLineMotionPattern::eInitLineOrder_Linear;
	IntroLineParam.GapInitLineOrder = 250;
	IntroLineParam.MotionPauseLineOrder = eLineMotionPattern::ePauseLineOrder_Linear;
	IntroLineParam.GapPauseLineOrder = 250;

	LineParam.ColorLine = { 1,1,1,1 };
	LineParam.ColorNode = { 1,1,1,1 };
	LineParam.PointSize = 6;

	LineParam.MotionInitLine = eLineMotionPattern::eInitLine_Flick;
	LineParam.PitchInitLine = 200;
	LineParam.MotionPauseLine = eLineMotionPattern::ePauseLine_Flick;
	LineParam.PitchPauseLine = 200;

	LineParam.MotionInitNode = eLineMotionPattern::eInitNode_Flick;
	LineParam.PitchInitNode = 220;
	LineParam.MotionPauseNode = eLineMotionPattern::ePauseNode_Flick;
	LineParam.PitchPauseNode = 220;

	LineParam.MotionInitLineOrder = eLineMotionPattern::eInitLineOrder_Rand;
	LineParam.GapInitLineOrder = 200;
	LineParam.RandNoiseInitLineOrder = 200;
	LineParam.MotionPauseLineOrder = eLineMotionPattern::ePauseLineOrder_Rand;
	LineParam.GapPauseLineOrder = 200;
	LineParam.RandNoisePauseLineOrder = 200;

	LineParam.MotionInitNodeOrder = eLineMotionPattern::eInitNodeOrder_Rand;
	LineParam.GapInitNodeOrder = 200;
	LineParam.RandNoiseInitNodeOrder = 200;
	LineParam.MotionPauseNodeOrder = eLineMotionPattern::ePauseNodeOrder_Rand;
	LineParam.GapPauseNodeOrder = 200;
	LineParam.RandNoisePauseNodeOrder = 200;


	ButtonParam.InitMotion = eButtonMotionPattern::eInit_Reload;
	ButtonParam.InitPitch = 700;
	ButtonParam.PauseMotion = eButtonMotionPattern::ePause_Flick;
	ButtonParam.PausePitch = 200;
	ButtonParam.ClickMotion = eButtonMotionPattern::eClick_Flash;
	ButtonParam.ClickPitch = 1000;
	ButtonParam.FaceColor = { 0.7,0,0,0.6 };
	ButtonParam.FrameColor = { 1,0,0,1 };
	ButtonParam.FontColor = { 1,1,1,1 };

	TableParam.MotionInitTableFrame = eTableMotionPattern::eInitTableFrame_ExpendAllDirFlick;
	TableParam.PitchInitTableFrame = 400;
	TableParam.MotionPauseTableFrame = eTableMotionPattern::ePauseTableFrame_Flick;
	TableParam.PitchPauseTableFrame = 150;
	
	TableParam.MotionInitTableHeaderBg = eTableMotionPattern::eInitTableHeaderBg_LinearReverse;
	TableParam.PitchInitTableHeaderBg = 300;
	TableParam.DelayInitTableHeaderBg = 300; /*이전 프레임모션 직후.*/
	TableParam.MotionPauseTableHeaderBg = eTableMotionPattern::ePauseTableHeaderBg_Linear;
	TableParam.PitchPauseTableHeaderBg = 200;

	TableParam.MotionInitTableHeaderText = eTableMotionPattern::eInitTableHeaderText_SlideIn;
	TableParam.PitchInitTableHeaderText = 400;
	TableParam.GapInitTableHeaderText = 100;
	TableParam.DelayInitTableHeaderText = 400;
	TableParam.MotionPauseTableHeaderText = eTableMotionPattern::ePauseTableHeaderText_SlideOut;
	TableParam.PitchPauseTableHeaderText = 400;
	TableParam.GapPauseTableHeaderText = 100;
	TableParam.DelayPauseTableHeaderText = 0;

	TableParam.MotionInitRowSelect = eTableMotionPattern::eInitTableSelect_Decel;
	TableParam.PitchInitRowSelect = 200;
	TableParam.DelayInitRowSelect = 500;
	TableParam.MotionPauseRowSelect = eTableMotionPattern::ePauseTableSelect_Decel;
	TableParam.PitchPauseRowSelect = 200;
	TableParam.DelayPauseRowSelect = 300;

	TableParam.MotionInitRowText = eTableMotionPattern::eInitRowText_Typing;
	TableParam.PitchInitRowText = 200;
	TableParam.GapInitRowText = 50;
	TableParam.MotionPauseRowText = eTableMotionPattern::ePauseRowText_FlickRandom;
	TableParam.PitchPauseRowText = 200;
	TableParam.GapPauseRowText = 50;
	TableParam.RangePauseRowText = 100;

	TableParam.MotionInitTableRowOrder = eTableMotionPattern::eInitTableRowOrder_Random;
	TableParam.DelayInitTableRowOrder = 400;
	TableParam.GapInitTableRowOrder = 10;
	TableParam.RangeInitTableRowOrder = 250;
	TableParam.MotionPauseTableRowOrder = eTableMotionPattern::ePauseTableRowOrder_Random;
	TableParam.RangePauseTableRowOrder = 300;

	TableParam.ColorFrame = { 1,0,0,1 };
	TableParam.ColorHeaderBg = { 0.6, 0, 0, 0.6 };
	TableParam.ColorHeaderText = { 1,1,1,1 };
	TableParam.ColorRowBg1 = { 0,0,0,0 };
	TableParam.ColorRowBg2 = { 1,0,0,0.1 };
	TableParam.ColorRowBgSelect = { 1,1,1,0.8 };
	TableParam.ColorRowText = { 1,1,1,1 };
	TableParam.ColorRowTextSelect = { 0,0,0,1 };

	LabelParam.MotionInitBg = eStaticMotionPattern::eInitBg_ExpendRight;
	LabelParam.PitchInitBg = 300;
	LabelParam.MotionInitText = eStaticMotionPattern::eInitText_Typing;
	LabelParam.DelayInitText = 300;
	LabelParam.PitchInitText = 800;

	LabelParam.MotionPauseBg = eStaticMotionPattern::ePauseBg_CollapseLeft;
	LabelParam.PitchPauseBg = 300;
	LabelParam.DelayPauseBg = 300;
	LabelParam.MotionPauseText = eStaticMotionPattern::ePauseText_Flick;
	LabelParam.PitchPauseText = 150;

	LabelParam.ColorBg = { 1,1,1, 0.65 };
	LabelParam.ColorFrame = { 0,0,0,0 };
	LabelParam.ColorText = { 0,0,0,1 };

	StaticParam.MotionInitFrame = eStaticMotionPattern::eInitFrame_ExpendCenter;
	StaticParam.PitchInitFrame = 400;
	StaticParam.DelayInitFrame = 0;
	StaticParam.MotionPauseFrame = eStaticMotionPattern::ePauseFrame_CollapseCenter;
	StaticParam.PitchPauseFrame = 400;
	StaticParam.DelayPauseFrame = 300;

	StaticParam.MotionInitBg = eStaticMotionPattern::eInitBg_ExpendRight;
	StaticParam.PitchInitBg = 300;
	StaticParam.DelayInitBg = 300;
	StaticParam.MotionPauseBg = eStaticMotionPattern::ePauseBg_CollapseLeft;
	StaticParam.PitchPauseBg = 300;
	StaticParam.DelayPauseBg = 0;

	StaticParam.MotionInitText = eStaticMotionPattern::eInitText_Typing;
	StaticParam.PitchInitText = 200;
	StaticParam.DelayInitText = 400;
	StaticParam.MotionPauseText = eStaticMotionPattern::ePauseText_Flick;
	StaticParam.PitchPauseText = 200;
	StaticParam.DelayPauseText = 0;

	StaticParam.ColorFrame = { 0.8,0,0,0.8 };
	StaticParam.ColorBg = { 0.5,0,0,0.5 };
	StaticParam.ColorText = { 1,1,1,1 };

	DescMainParam.ColorBg = { 0.7, 0, 0, 1 };
	DescMainParam.ColorText = { 1,1,1,1 };
	DescMainParam.ColorFrame = ALL_ZERO;
	DescMainParam.MotionInitBg = eStaticMotionPattern::eInitBg_ExpendRight;
	DescMainParam.PitchInitBg = 300;
	DescMainParam.DelayInitBg = 0;
	DescMainParam.MotionInitText = eStaticMotionPattern::eInitText_Typing;
	DescMainParam.PitchInitText = 300;
	DescMainParam.DelayInitText = 300;
	DescMainParam.MotionPauseBg = eStaticMotionPattern::ePauseBg_CollapseLeft;
	DescMainParam.PitchPauseBg = 300;
	DescMainParam.DelayPauseBg = 0;
	DescMainParam.MotionPauseText = eStaticMotionPattern::ePauseText_Flick;
	DescMainParam.PitchPauseText = 200;
	DescMainParam.DelayPauseText = 300;

	DescSubParam.ColorBg = ALL_ZERO;
	DescSubParam.ColorText = { 1,1,1,1 };
	DescSubParam.ColorFrame = ALL_ZERO;
	DescSubParam.MotionInitText = eStaticMotionPattern::eInitText_Flick;
	DescSubParam.PitchInitText = 200;
	DescSubParam.DelayInitText = 0;
	DescSubParam.MotionPauseText = eStaticMotionPattern::ePauseText_Flick;
	DescSubParam.PitchPauseText = 200;
	DescSubParam.DelayPauseText = 0;

	TimerParam.ColorBg = ALL_ZERO;
	TimerParam.ColorFrame = ALL_ZERO;
	TimerParam.ColorText = {1,1,1,1};
	TimerParam.MotionPauseText = eStaticMotionPattern::ePauseText_Flick;
	TimerParam.PitchPauseText = 200;

	InputParam.MotionInitFrame = eTextinputMotionPattern::eInitFrame_ExpendLR;
	InputParam.PitchInitFrame = 500;
	InputParam.MotionPauseFrame = eTextinputMotionPattern::ePauseFrame_CollapseLR;
	InputParam.PitchPauseFrame = 500;

	InputParam.MotionMoveCaret = eTextinputMotionPattern::eMoveCaret_Decel;

	InputParam.MotionInitText = eTextinputMotionPattern::eInitText_SlideIn;
	InputParam.PitchInitText = 300;
	InputParam.DelayInitText = 300;
	InputParam.MotionPauseText = eTextinputMotionPattern::ePauseText_SlideOut;
	InputParam.PitchPauseText = 300;
	InputParam.DelayPauseText = 0;
	
	InputParam.ColorFrame = { 0.8,0,0,0.8 };
	InputParam.ColorBg = ALL_ZERO;
	InputParam.ColorText = { 1,1,1,1 };
	InputParam.ColorCaret = { 0.8,0.8,0.8,1 };
	InputParam.PitchMoveCaret = 150;

	ChartParam.MotionInitFrame = eLineChartMotionPattern::eInitFrame_Flick;
	ChartParam.DelayInitFrame = 0;
	ChartParam.PitchInitFrame = 200;
	ChartParam.MotionPauseFrame = eLineChartMotionPattern::ePauseFrame_Flick;
	ChartParam.DelayPauseFrame = 500;
	ChartParam.PitchPauseFrame = 200;

	ChartParam.MotionInitChartGuideLine = eLineChartMotionPattern::eInitChartGuideLine_Expend;
	ChartParam.DelayInitChartGuideLine = 0;
	ChartParam.PitchInitChartGuideLine = 500;
	ChartParam.MotionPauseChartGuideLine = eLineChartMotionPattern::ePauseChartGuideLine_Collapse;
	ChartParam.DelayPauseChartGuideLine = 200;
	ChartParam.PitchPauseChartGuideLine = 500;

	ChartParam.MotionInitChartPoint = eLineChartMotionPattern::eInitChartPoint_Flick;
	ChartParam.DelayInitChartPoint = 200;
	ChartParam.PitchInitChartPoint = 200;
	ChartParam.MotionPauseChartPoint = eLineChartMotionPattern::ePauseChartPoint_Flick;
	ChartParam.DelayPauseChartPoint = 200;
	ChartParam.PitchPauseChartPoint = 200;

	ChartParam.MotionInitChartLine = eLineChartMotionPattern::eInitChartLine_Connect;
	ChartParam.DelayInitChartLine = 300;
	ChartParam.PitchInitChartLine = 200;
	ChartParam.MotionPauseChartLine = eLineChartMotionPattern::ePauseChartLine_Disconnect;
	ChartParam.DelayPauseChartLine = 0;
	ChartParam.PitchPauseChartLine = 200;

	ChartParam.MotionInitChartPointDeco = eLineChartMotionPattern::eInitChartPointDeco_Step;
	ChartParam.DelayInitChartPointDeco = 100;
	ChartParam.PitchInitChartPointDeco = 600;
	ChartParam.MotionPauseChartPointDeco = eLineChartMotionPattern::ePauseChartPoint_Default;
	ChartParam.DelayPauseChartPointDeco = 0;
	ChartParam.PitchPauseChartPointDeco = 0;

	ChartParam.MotionInitDataOrder = eLineChartMotionPattern::eInitDataOrder_Linear;
	ChartParam.DelayInitDataOrder = 500;
	ChartParam.GapInitDataOrder = 80;
	ChartParam.MotionPauseDataOrder = eLineChartMotionPattern::ePauseDataOrder_Linear;
	ChartParam.DelayPauseDataOrder = 0;
	ChartParam.GapPauseDataOrder = 80;

	ChartParam.PitchScroll = 500;
	ChartParam.PointDecoMaxRadius = 50;
	ChartParam.PointDecoMinRadius = 15;

	ChartParam.ColorBg = ALL_ZERO;
	ChartParam.ColorFrame = { 1,0,0,1 };
	ChartParam.ColorChartLine = { 1,1,1,1 };
	ChartParam.ColorChartPoint = { 0.9, 0.9, 0.9, 1 };
	ChartParam.ColorChartPointDeco = { 0.9, 0.9, 0.9, 1 };
	ChartParam.ColorGuideLine = { 0.3,0.3,0.3,1 };

}

void UI_ParamSet2() {
	ButtonParam.InitMotion = eButtonMotionPattern::eInit_Reload;
	ButtonParam.InitPitch = 700;
	ButtonParam.ClickMotion = eButtonMotionPattern::eClick_Flash;
	ButtonParam.ClickPitch = 1000;
	ButtonParam.FaceColor = { 0.9,0.9,0.9,1 };
	ButtonParam.FrameColor = { 0,0,0,1 };
	ButtonParam.FontColor = { 0,0,0,1 };
	ButtonParam.HighlightColor = { 0,0,0,1 };

	TableParam.MotionInitTableFrame = eTableMotionPattern::eInitTableFrame_ExpendAllDirFlick;
	TableParam.PitchInitTableFrame = 400;
	TableParam.MotionPauseTableFrame = eTableMotionPattern::ePauseTableFrame_ExpendAllDirFlick;
	TableParam.PitchPauseTableFrame = 400;
	
	TableParam.MotionInitTableHeaderBg = eTableMotionPattern::eInitTableHeaderBg_LinearReverse;
	TableParam.PitchInitTableHeaderBg = 300;
	TableParam.DelayInitTableHeaderBg = 300; /*이전 프레임모션 직후.*/
	TableParam.MotionPauseTableHeaderBg = eTableMotionPattern::ePauseTableHeaderBg_Linear;
	TableParam.PitchPauseTableHeaderBg = 200;

	TableParam.MotionInitTableHeaderText = eTableMotionPattern::eInitTableHeaderText_SlideIn;
	TableParam.PitchInitTableHeaderText = 400;
	TableParam.GapInitTableHeaderText = 100;
	TableParam.DelayInitTableHeaderText = 400;
	TableParam.MotionPauseTableHeaderText = eTableMotionPattern::ePauseTableHeaderText_SlideOut;
	TableParam.PitchPauseTableHeaderText = 400;
	TableParam.GapPauseTableHeaderText = 100;
	TableParam.DelayPauseTableHeaderText = 0;

	TableParam.MotionInitRowSelect = eTableMotionPattern::eInitTableSelect_Decel;
	TableParam.PitchInitRowSelect = 200;
	TableParam.DelayInitRowSelect = 500;
	TableParam.MotionPauseRowSelect = eTableMotionPattern::ePauseTableSelect_Decel;
	TableParam.PitchPauseRowSelect = 200;
	TableParam.DelayPauseRowSelect = 300;

	TableParam.MotionInitRowText = eTableMotionPattern::eInitRowText_Typing;
	TableParam.PitchInitRowText = 200;
	TableParam.GapInitRowText = 50;
	TableParam.MotionPauseRowText = eTableMotionPattern::ePauseRowText_FlickRandom;
	TableParam.PitchPauseRowText = 200;
	TableParam.GapPauseRowText = 50;
	TableParam.RangePauseRowText = 100;

	TableParam.MotionInitTableRowOrder = eTableMotionPattern::eInitTableRowOrder_Random;
	TableParam.DelayInitTableRowOrder = 400;
	TableParam.GapInitTableRowOrder = 10;
	TableParam.RangeInitTableRowOrder = 250;
	TableParam.MotionPauseTableRowOrder = eTableMotionPattern::ePauseTableRowOrder_Random;
	TableParam.RangePauseTableRowOrder = 300;

	TableParam.PitchInitRowHighlight = 300;

	TableParam.ColorFrame = { 0,0,0,1 };
	TableParam.ColorHeaderBg = { 0.8, 0.8, 0.8, 1 };
	TableParam.ColorHeaderText = { 0,0,0,1 };
	TableParam.ColorRowBg1 = { 1,1,1,1 };
	TableParam.ColorRowBg2 = { 0.9,0.9,0.9,1 };
	TableParam.ColorRowBgSelect = { 0,0,0,1 };
	TableParam.ColorRowText = { 0,0,0,1 };
	TableParam.ColorRowTextSelect = { 1,1,1,1 };

	LabelParam.MotionInitBg = eStaticMotionPattern::eInitBg_ExpendRight;
	LabelParam.PitchInitBg = 300;
	LabelParam.MotionInitText = eStaticMotionPattern::eInitText_Typing;
	LabelParam.DelayInitText = 300;
	LabelParam.PitchInitText = 300;

	LabelParam.MotionPauseBg = eStaticMotionPattern::ePauseBg_CollapseLeft;
	LabelParam.PitchPauseBg = 300;
	LabelParam.DelayPauseBg = 300;
	LabelParam.MotionPauseText = eStaticMotionPattern::ePauseText_Flick;
	LabelParam.PitchPauseText = 150;

	LabelParam.ColorBg = { 0.25,0.3,0.5,1 };
	LabelParam.ColorFrame = { 0,0,0,0 };
	LabelParam.ColorText = { 1,1,1,1 };

	StaticParam.MotionInitFrame = eStaticMotionPattern::eInitFrame_ExpendCenter;
	StaticParam.PitchInitFrame = 400;
	StaticParam.DelayInitFrame = 0;
	StaticParam.MotionPauseFrame = eStaticMotionPattern::ePauseFrame_CollapseCenter;
	StaticParam.PitchPauseFrame = 400;
	StaticParam.DelayPauseFrame = 300;

	StaticParam.MotionInitBg = eStaticMotionPattern::eInitBg_ExpendRight;
	StaticParam.PitchInitBg = 300;
	StaticParam.DelayInitBg = 300;
	StaticParam.MotionPauseBg = eStaticMotionPattern::ePauseBg_CollapseLeft;
	StaticParam.PitchPauseBg = 300;
	StaticParam.DelayPauseBg = 0;

	StaticParam.MotionInitText = eStaticMotionPattern::eInitText_Typing;
	StaticParam.PitchInitText = 200;
	StaticParam.DelayInitText = 400;
	StaticParam.MotionPauseText = eStaticMotionPattern::ePauseText_Flick;
	StaticParam.PitchPauseText = 200;
	StaticParam.DelayPauseText = 0;

	StaticParam.ColorFrame = { 0,0,0,1 };
	StaticParam.ColorBg = { 0.8,0.8,0.8,1 };
	StaticParam.ColorText = { 0,0,0,1 };

	InputParam.MotionInitFrame = eTextinputMotionPattern::eInitFrame_ExpendLR;
	InputParam.PitchInitFrame = 500;
	InputParam.MotionPauseFrame = eTextinputMotionPattern::ePauseFrame_CollapseLR;
	InputParam.PitchPauseFrame = 500;

	InputParam.MotionMoveCaret = eTextinputMotionPattern::eMoveCaret_Decel;

	InputParam.MotionInitText = eTextinputMotionPattern::eInitText_SlideIn;
	InputParam.PitchInitText = 300;
	InputParam.DelayInitText = 300;
	InputParam.MotionPauseText = eTextinputMotionPattern::ePauseText_SlideOut;
	InputParam.PitchPauseText = 300;
	InputParam.DelayPauseText = 0;
	
	InputParam.ColorFrame = { 0,0,0,1 };
	InputParam.ColorBg = {1,1,1,1};
	InputParam.ColorText = { 0,0,0,1 };
	InputParam.ColorCaret = { 0.3,0.3,0.3,1 };
	InputParam.PitchMoveCaret = 150;
}
