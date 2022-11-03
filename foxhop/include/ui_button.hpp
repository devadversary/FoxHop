#pragma once

#include "ui_base.hpp"
#include "ui_theme.hpp"

class UISystem; /*UI�ý��� Ŭ������ ���漱��*/

/**
    @brief ��ư Ŭ����
    @remark ������ ���丮���� ���ش�.
*/
class UI_Button : public UI {
private:
    ObjectMotionBox*  MBoxFace;       /**< ���� ��� �ڽ�*/
    ObjectMotionBox*  MBoxHighlight;  /**< ���̶���Ʈ ��� �ڽ�*/
    ObjectMotionText* MText;          /**< ��� �ڽ� �⺻ �ؽ�Ʈ*/
    int               nTextLen;
    wchar_t           szText[MAX_BUTTONNAME];
    
private: /*2022.04.30 : Devadversary - InputMotion �� �⺻������ Private �Լ����������� �׷��� �Ǹ�, DefaultButtonProc �Լ����� ������ ����.*/
    void InputMotion(eButtonAction Action, UITheme* Theme, unsigned int nDelay, void* param);
    static void DefaultButtonProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam);

public:
    UI_Button(UISystem* pUISys, pfnUIHandler pfnCallback, POSITION Pos, wchar_t* pText, int nDelay);
    ~UI_Button() {};

    void pause(int nDelay);
    void resume(int nDelay);
    void Destroy();
    BOOL update(unsigned long time);
    void render();

public: /*UI�� �ɼ� �ż���*/
    void setText(wchar_t* pText, int nDelay);
    void setFaceColor(D2D1_COLOR_F Color, MOTION_INFO miColor);
    void setHighlightColor(D2D1_COLOR_F Color, MOTION_INFO miColor);
};
