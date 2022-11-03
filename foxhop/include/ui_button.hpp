#pragma once

#include "ui_base.hpp"
#include "ui_theme.hpp"

class UISystem; /*UI시스템 클래스의 전방선언*/

/**
    @brief 버튼 클래스
    @remark 생성은 팩토리에서 해준다.
*/
class UI_Button : public UI {
private:
    ObjectMotionBox*  MBoxFace;       /**< 전경 모션 박스*/
    ObjectMotionBox*  MBoxHighlight;  /**< 하이라이트 모션 박스*/
    ObjectMotionText* MText;          /**< 모션 박스 기본 텍스트*/
    int               nTextLen;
    wchar_t           szText[MAX_BUTTONNAME];
    
private: /*2022.04.30 : Devadversary - InputMotion 은 기본적으로 Private 함수여야하지만 그렇게 되면, DefaultButtonProc 함수에서 접근을 못함.*/
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

public: /*UI별 옵션 매서드*/
    void setText(wchar_t* pText, int nDelay);
    void setFaceColor(D2D1_COLOR_F Color, MOTION_INFO miColor);
    void setHighlightColor(D2D1_COLOR_F Color, MOTION_INFO miColor);
};
