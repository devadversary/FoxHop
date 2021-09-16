#pragma once

#include "ui_base.hpp"
#include "ui_system.hpp"

#define MAX_BUTTONNAME 32 /*버튼의 최대 텍스트 길이*/

class UISystem; /*UI시스템 클래스의 전방선언*/

/**
    @brief 버튼 생성 모션 타입
*/
enum class eButtonMotionInit {
    eMotionReload = 0, /**< 장전 모션*/
    eMotionFlick       /**< 플리커링 모션*/
};

/**
    @brief 버튼 소멸 모션 타입
*/
enum class eButtonMotionPause {
    eMotionFlick = 0       /**< 플리커링 모션*/
};

/**
    @brief 버튼 마우스 오버 모션 타입
*/
enum class eButtonMotionMouseover {
    eMotionFlick = 0       /**< 플리커링 모션*/
};

/**
    @brief 버튼 클릭 모션 타입
*/
enum class eButtonMotionClick {
    eMotionFlash = 0       /**< 점멸 모션*/
};

/**
    @brief 버튼 색상 변경 모션 타입
*/
enum class eButtonMotionColor {
    eMotionFlash = 0       /**< 점멸 모션*/
};

/**
    @brief 버튼 텍스트 모션 타입
*/
enum class eButtonMotionText {
    eMotionDefault = 0       /**< 모션 없음*/
};


/**
    @brief 모션 집합
    @remark 개체의 매서드를 통해 변경 될 수 있음
*/
typedef struct _stButtonMotionSet {
    eButtonMotionInit      Init;      /**< 시작모션*/
    eButtonMotionPause     Pause;     /**< 소멸모션*/
    eButtonMotionMouseover MouseOver; /**< 마우스오버 모션*/
    eButtonMotionClick     Click;     /**< 클릭 모션*/
    eButtonMotionColor     Color;     /**< 색상 변경 모션*/
    eButtonMotionText      Text;      /**< 텍스트 변경 모션*/
}BUTTON_MOTION_SET;

/**
    @brief  버튼 색상 집합
    @remark 개체의 매서드를 통해 변경 될 수 있음
*/
typedef struct _stColorSet {
    D2D1_COLOR_F Frame;     /**< 초기 테두리 색*/
    D2D1_COLOR_F Face;      /**< 초기 버튼 전경색*/
    D2D1_COLOR_F Highlight; /**< 버튼 하이라이트 색*/
    D2D1_COLOR_F Font;      /**< 초기 텍스트 색*/
}BUTTON_COLOR_SET;

/**
    @brief 버튼 생성 팩토리
*/
class UI_ButtonFactory {
private:
    BUTTON_MOTION_SET MotionSet; /**< 모션 집합*/
    BUTTON_COLOR_SET  ColorSet;  /**< 색상 집합*/
};

class UI_Button : public UI {
private:
    ObjectPool<ObjectMotionBox>*  PoolBox;
    ObjectPool<ObjectMotionText>* PoolText;
    BUTTON_MOTION_SET             MotionSet;      /**< 모션 집합*/
    BUTTON_COLOR_SET              ColorSet;       /**< 색상 집합*/
    ObjectMotionBox*              MBoxFace;       /**< 전경 모션 박스*/
    ObjectMotionBox*              MBoxHighlight;  /**< 하이라이트 모션 박스*/
    ObjectMotionText*             MText;          /**< 모션 박스 기본 텍스트*/
    int                           nTextLen;
    D2D1_COLOR_F                  FrameColor;     /**< 초기 테두리 색*/
    D2D1_COLOR_F                  FaceColor;      /**< 초기 버튼 전경색*/
    D2D1_COLOR_F                  HighlightColor; /**< 버튼 하이라이트 색*/
    D2D1_COLOR_F                  FontColor;      /**< 초기 텍스트 색*/
    wchar_t                       szText[MAX_BUTTONNAME];

public: /*반드시 있어야 되는 매서드*/
    UI_Button();
    UI_Button(UISystem* pUISys, ID2D1RenderTarget* pRT, int nID, pfnUIHandler pfnCallback);
    ~UI_Button();
    void preInit(UISystem* pUISys, ID2D1RenderTarget* pRT, int nID, pfnUIHandler pfnCallback);
    void CreateUI(UISystem* pUISys, ID2D1RenderTarget* pRT,
                  BUTTON_MOTION_SET UIMotionSet, BUTTON_COLOR_SET UIColorSet,
                  pfnUIHandler pfnCallback, POSITION Pos, int nID, wchar_t* pText, int nDelay);
    void Init(int Motion, POSITION Pos, D2D1_COLOR_F Color, wchar_t* pText, D2D1_COLOR_F TextColor, int nDelay = 0);
    void InputMotion(int Motion, int nDelay);
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
