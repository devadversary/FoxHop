#pragma once

#include "ui_base.hpp"

class UISystem; /*UI시스템 클래스의 전방선언*/

/**
    @brief 버튼 생성 모션 타입
*/
enum class eButtonMotionInit {
    eMotionDefault = 0, /**< 모션 없음*/
    eMotionReload,      /**< 장전 모션*/
    eMotionFlick        /**< 플리커링 모션*/
};

/**
    @brief 버튼 소멸 모션 타입
*/
enum class eButtonMotionPause {
    eMotionDefault = 0, /**< 모션 없음*/
    eMotionFlick        /**< 플리커링 모션*/
};

/**
    @brief 버튼 마우스 오버 모션 타입
*/
enum class eButtonMotionMouseover {
    eMotionDefault = 0, /**< 모션 없음*/
    eMotionFlick        /**< 플리커링 모션*/
};

/**
    @brief 버튼 클릭 모션 타입
*/
enum class eButtonMotionClick {
    eMotionDefault = 0, /**< 모션 없음*/
    eMotionFlash        /**< 점멸 모션*/
};

/**
    @brief 버튼 색상 변경 모션 타입
*/
enum class eButtonMotionColor {
    eMotionDefault = 0, /**< 모션 없음*/
    eMotionFlash        /**< 점멸 모션*/
};

/**
    @brief 버튼 텍스트 모션 타입
*/
enum class eButtonMotionText {
    eMotionDefault = 0 /**< 모션 없음*/
};

/**
    @brief 버튼 모션 타입
*/
enum class eButtonMotionType {
    eMotionType_Init = 0,   /**< 초기화 모션*/
    eMotionType_Pause,      /**< 소멸 모션*/
    eMotionType_Mouseover,  /**< 마우스 오버 모션*/
    eMotionType_Click,      /**< 클릭 모션*/
    eMotionType_Color,      /**< 색상 변경 모션*/
    eMotionType_Text        /**< 텍스트 모션*/
};

/**
    @brief 모션 집합
    @remark 개체의 매서드를 통해 변경 될 수 있음
*/
typedef struct _stButtonMotionSet {
    eButtonMotionInit      Init;           /**< 시작모션*/
    unsigned int           InitPitch;      /**< 시작모션 피치*/

    eButtonMotionPause     Pause;          /**< 소멸모션*/
    unsigned int           PausePitch;     /**< 소멸모션 피치*/

    eButtonMotionMouseover MouseOver;      /**< 마우스오버 모션*/
    unsigned int           MouseOverPitch; /**< 마우스오버모션 피치*/

    eButtonMotionClick     Click;          /**< 클릭 모션*/
    unsigned int           ClickPitch;     /**< 클릭 피치*/

    eButtonMotionColor     Color;          /**< 색상 변경 모션*/
    unsigned int           ColorPitch;     /**< 색상 변경 피치*/

    eButtonMotionText      Text;           /**< 텍스트 변경 모션*/
    unsigned int           TextPitch;      /**< 텍스트 변경 피치*/
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

#define MAX_BUTTONNAME 32 /*버튼의 최대 텍스트 길이*/

/**
    @brief 버튼 클래스
    @remark 생성은 왠만하면 팩토리에서 해준다.
*/
class UI_Button : public UI {
private:
    ObjectMotionBox*              MBoxFace;       /**< 전경 모션 박스*/
    ObjectMotionBox*              MBoxHighlight;  /**< 하이라이트 모션 박스*/
    ObjectMotionText*             MText;          /**< 모션 박스 기본 텍스트*/
    int                           nTextLen;
    D2D1_COLOR_F                  FrameColor;     /**< 초기 테두리 색*/
    D2D1_COLOR_F                  FaceColor;      /**< 초기 버튼 전경색*/
    D2D1_COLOR_F                  HighlightColor; /**< 버튼 하이라이트 색*/
    D2D1_COLOR_F                  FontColor;      /**< 초기 텍스트 색*/
    wchar_t                       szText[MAX_BUTTONNAME];
    void InputMotion(eButtonMotionInit Motion, int nDelay);

public:
    BUTTON_MOTION_SET             MotionSet;      /**< 모션 집합*/
    BUTTON_COLOR_SET              ColorSet;       /**< 색상 집합*/

public: /*반드시 있어야 되는 매서드*/
    void CreateUI(UISystem* pUISys, ID2D1RenderTarget* pRT,
                  BUTTON_MOTION_SET UIMotionSet, BUTTON_COLOR_SET UIColorSet,
                  pfnUIHandler pfnCallback, POSITION Pos, int nID, wchar_t* pText, int nDelay);
    
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

/**
    @brief 버튼 생성 팩토리
*/
class UI_ButtonFactory {
private:
    BUTTON_MOTION_SET  MotionSet;     /**< 모션 집합*/
    BUTTON_COLOR_SET   ColorSet;      /**< 색상 집합*/
    UISystem*          pUISys;        /**< UI시스템 인스턴스*/
    ID2D1RenderTarget* pRenderTarget; /**< 렌더링 타겟*/

public:
    void Init(UISystem* pUISystem, ID2D1RenderTarget* pRT);
    void GetCurrentMotionSet(BUTTON_MOTION_SET* pMotionSet);
    void SetCurrentMotionSet(BUTTON_MOTION_SET* pMotionSet);
    void GetCurrentColorSet(BUTTON_COLOR_SET* pColorSet);
    void SetCurrentColorSet(BUTTON_COLOR_SET* pColorSet);
    UI*  CreateUI(int nID, POSITION Pos, wchar_t* pText, int nDelay, pfnUIHandler pfnCallback);
};
