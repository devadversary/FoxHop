#pragma once

#include "../foxhop/include/ui_system.hpp"
#include "../foxhop/include/ui_base.hpp"
#include "../foxhop/include/ui_button.hpp"
#include "../foxhop/include/ui_listview.hpp"
#include "../foxhop/include/ui_static.hpp"
#include "../foxhop/include/ui_fraggedline.hpp"
#include "../foxhop/include/ui_table.hpp"
#include "../foxhop/include/ui_textinput.hpp"

UI_Button_MotionParam ButtonParam;
UI_Table_MotionParam TableParam;
UI_Static_MotionParam StaticParam;
UI_Textinput_MotionParam InputParam;

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
	TableParam.MotionInitTableHeaderBg = eTableMotionPattern::eInitTableHeaderBg_LinearReverse;
	TableParam.PitchInitTableHeaderBg = 300;
	TableParam.DelayInitTableHeaderBg = 300; /*이전 프레임모션 직후.*/

	TableParam.MotionInitTableHeaderText = eTableMotionPattern::eInitTableHeaderText_SlideIn;
	TableParam.PitchInitTableHeaderText = 400;
	TableParam.GapInitTableHeaderText = 100;
	TableParam.DelayInitTableHeaderText = 400;

	TableParam.MotionInitRowSelect = eTableMotionPattern::eInitTableSelect_Decel;
	TableParam.PitchInitRowSelect = 200;
	TableParam.DelayInitRowSelect = 500;
	TableParam.MotionInitRowText = eTableMotionPattern::eInitRowText_Typing;
	TableParam.PitchInitRowText = 100;
	TableParam.GapInitRowText = 50;

	TableParam.MotionInitTableRowOrder = eTableMotionPattern::eInitTableRowOrder_Random;
	TableParam.DelayInitTableRowOrder = 400;
	TableParam.GapInitTableRowOrder = 10;
	TableParam.RangeInitTableRowOrder = 250;

	TableParam.MotionPauseTableFrame = eTableMotionPattern::ePauseTableFrame_ExpendAllDirFlick;
	TableParam.PitchPauseTableFrame = 400;

	TableParam.MotionPauseTableHeaderBg = eTableMotionPattern::ePauseTableHeaderBg_Linear;
	TableParam.PitchPauseTableHeaderBg = 200;

	TableParam.MotionPauseTableHeaderText = eTableMotionPattern::ePauseTableHeaderText_SlideOut;
	TableParam.PitchPauseTableHeaderText = 400;
	TableParam.GapPauseTableHeaderText = 100;
	TableParam.DelayPauseTableHeaderText = 0;

	TableParam.MotionPauseRowSelect = eTableMotionPattern::ePauseTableSelect_Decel;
	TableParam.PitchPauseRowSelect = 200;

	TableParam.MotionPauseRowText = eTableMotionPattern::ePauseRowText_FlickRandom;
	TableParam.PitchPauseRowText = 200;
	TableParam.GapPauseRowText = 50;
	TableParam.RangePauseRowText = 100;

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

	StaticParam.MotionText = eStaticMotionPattern::eText_Flick;
	StaticParam.PitchText = 200;
	StaticParam.ColorFrame = { 0.8,0,0,0.5 };
	StaticParam.ColorBg = { 0.7,0,0,0.3 };
	//StaticParam.ColorFrame = { 0,0,0,0 };
	StaticParam.ColorBg = { 0,0,0,0 };
	StaticParam.ColorFont = { 1,1,1,1 };

	InputParam.MotionMoveCaret = eTextinputMotionPattern::eMoveCaret_Decel;
	InputParam.ColorFrame = { 0.8,0,0,0.6 };
	InputParam.ColorBg = ALL_ZERO;
	InputParam.ColorText = { 1,1,1,1 };
	InputParam.ColorCaret = { 0.8,0.8,0.8,1 };
	InputParam.PitchMoveCaret = 150;
}
