#pragma once

#include "ui_base.hpp"

#define MAX_STATIC_TEXTLEN 256

class UISystem; /*UI시스템 클래스의 전방선언*/

/**
    @brief 스태틱 생성 모션 타입
*/
enum class eStaticMotionInit {
    eMotionDefault = 0,    /**< 모션 없음*/
    eMotionSlideConst,     /**< 등속 슬라이드*/
    eMotionSlideAcc,       /**< 가속 슬라이드*/
    eMotionSlideDec,       /**< 감속 슬라이드*/
    eMotionFlick           /**< 플리커링 모션*/
};

/**
    @brief 버튼 소멸 모션 타입
*/
enum class eStaticMotionPause {
    eMotionDefault = 0, /**< 모션 없음*/
    eMotionFlick        /**< 플리커링 모션*/
};

class UI_Static : public UI {
private:
    ObjectMotionBox*              MBox;
    ObjectMotionText*             MText;
    D2D1_COLOR_F                  FrameColor;     /**< 초기 테두리 색*/
    D2D1_COLOR_F                  FaceColor;      /**< 초기 버튼 전경색*/
    D2D1_COLOR_F                  FontColor;      /**< 초기 텍스트 색*/
    wchar_t                       szText[MAX_STATIC_TEXTLEN];
    int                           nTextLen;
    void InputMotion(int Motion, int nDelay);

public:
    void CreateUI(UISystem* pUISys, ID2D1RenderTarget* pRT,
                  BUTTON_MOTION_SET UIMotionSet, BUTTON_COLOR_SET UIColorSet,
                  pfnUIHandler pfnCallback, POSITION Pos, int nID, wchar_t* pText, int nDelay);
    void pause(int nDelay);
    void resume(int nDelay);
    void Destroy();
    void update(unsigned long time);
    void render();
    
public: /*UI별 옵션 매서드*/
    void setText(wchar_t* pText, int nDelay);
};
