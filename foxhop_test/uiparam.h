#pragma once

#include "../foxhop/include/ui_system.hpp"
#include "../foxhop/include/ui_base.hpp"
#include "../foxhop/include/ui_button.hpp"
#include "../foxhop/include/ui_listview.hpp"
#include "../foxhop/include/ui_static.hpp"
#include "../foxhop/include/ui_line.hpp"
#include "../foxhop/include/ui_table.hpp"
#include "../foxhop/include/ui_textinput.hpp"

UI_Button_MotionParam ButtonParam;
UI_Table_MotionParam TableParam;
UI_Static_MotionParam TitleParam;
UI_Static_MotionParam StaticParam;
UI_Textinput_MotionParam InputParam;
UI_LineChart_MotionParam ChartParam;

void UI_ParamSet() {
	ButtonParam.InitMotion = eButtonMotionPattern::eInit_Reload;
	ButtonParam.InitPitch = 700;
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
	//TableParam.MotionInitRowText = eTableMotionPattern::eInitRowText_Default;
	//TableParam.PitchInitRowText = 0;
	//TableParam.GapInitRowText = 0;
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

	TitleParam.MotionInitBg = eStaticMotionPattern::eInitBg_ExpendRight;
	TitleParam.PitchInitBg = 300;
	TitleParam.MotionInitText = eStaticMotionPattern::eInitText_Typing;
	TitleParam.DelayInitText = 300;
	TitleParam.PitchInitText = 800;

	TitleParam.MotionPauseBg = eStaticMotionPattern::ePauseBg_CollapseLeft;
	TitleParam.PitchPauseBg = 300;
	TitleParam.DelayPauseBg = 300;
	TitleParam.MotionPauseText = eStaticMotionPattern::ePauseText_Flick;
	TitleParam.PitchPauseText = 150;

	TitleParam.ColorBg = { 0.3, 0.3, 0.3, 0.5 };
	TitleParam.ColorFrame = { 0,0,0,0 };
	TitleParam.ColorText = { 1,1,1,1 };

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
	StaticParam.ColorBg = { 0.7,0,0,0.5 };
	StaticParam.ColorText = { 1,1,1,1 };

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


	ChartParam.MotionInitChartGuideLine = eLineChartMotionPattern::eInitChartGuideLine_Expend;
	ChartParam.DelayInitChartGuideLine = 0;
	ChartParam.PitchInitChartGuideLine = 300;
	ChartParam.MotionPauseChartGuideLine = eLineChartMotionPattern::eInitChartGuideLine_Expend;
	ChartParam.DelayPauseChartGuideLine = 0;
	ChartParam.PitchPauseChartGuideLine = 300;

	ChartParam.ColorBg = ALL_ZERO;
	ChartParam.ColorFrame = { 1,0,0,1 };
	ChartParam.ColorChartLine = { 1,1,1,1 };
	ChartParam.ColorChartPoint = { 0.9, 0.9, 0.9, 1 };
	ChartParam.ColorGuideLine = { 0.25,0.25,0.25,1 };

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

	TitleParam.MotionInitBg = eStaticMotionPattern::eInitBg_ExpendRight;
	TitleParam.PitchInitBg = 300;
	TitleParam.MotionInitText = eStaticMotionPattern::eInitText_Typing;
	TitleParam.DelayInitText = 300;
	TitleParam.PitchInitText = 300;

	TitleParam.MotionPauseBg = eStaticMotionPattern::ePauseBg_CollapseLeft;
	TitleParam.PitchPauseBg = 300;
	TitleParam.DelayPauseBg = 300;
	TitleParam.MotionPauseText = eStaticMotionPattern::ePauseText_Flick;
	TitleParam.PitchPauseText = 150;

	TitleParam.ColorBg = { 0.25,0.3,0.5,1 };
	TitleParam.ColorFrame = { 0,0,0,0 };
	TitleParam.ColorText = { 1,1,1,1 };

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
