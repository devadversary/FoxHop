#pragma once

#include "ui_base.hpp"

class UISystem; /*UI시스템 클래스의 전방선언*/

/**
    @brief 버튼 모션 타입 상수
*/
enum class eButtonMotionPattern {
    eInit_Default = 0,      /**< 생성: 모션 없음*/
    eInit_Reload,           /**< 생성: 장전 모션*/
    eInit_Flick,            /**< 생성: 플리커링 모션*/

    ePause_Default = 0,     /**< 소멸: 모션 없음*/
    ePause_Flick,           /**< 소멸: 플리커링 모션*/

    eMouseover_Default = 0, /**< 마우스오버: 모션 없음*/
    eMouseover_Flick,       /**< 마우스오버: 플리커링 모션*/

    eClick_Default = 0,     /**< 클릭: 모션 없음*/
    eClick_Flash,           /**< 클릭: 점멸 모션*/

    eColor_Default = 0,     /**< 색상변경: 모션 없음*/
    eColor_Flash,           /**< 색상변경: 점멸 모션*/

    eText_Default = 0       /**< 텍스트: 모션 없음*/
};

/**
    @brief 버튼 모션 타입
*/
enum class eButtonMotionType {
    eType_Init = 0,  /**< 초기화 모션*/
    eType_Pause,     /**< 소멸 모션*/
    eType_Mouseover, /**< 마우스 오버 모션 (커서가 떠났을때도 포함)*/
    eType_Click,     /**< 클릭 모션*/
    eType_Color,     /**< 색상 변경 모션*/
    eType_Text       /**< 텍스트 모션*/
};

/**
    @brief 모션 집합
    @remark 개체의 매서드를 통해 변경 될 수 있음
*/
typedef struct _stButtonMotionSet {
    eButtonMotionPattern Init;           /**< 시작모션*/
    unsigned int         InitPitch;      /**< 시작모션 피치*/

    eButtonMotionPattern Pause;          /**< 소멸모션*/
    unsigned int         PausePitch;     /**< 소멸모션 피치*/

    eButtonMotionPattern MouseOver;      /**< 마우스오버 모션*/
    unsigned int         MouseOverPitch; /**< 마우스오버모션 피치*/

    eButtonMotionPattern Click;          /**< 클릭 모션*/
    unsigned int         ClickPitch;     /**< 클릭 피치*/

    eButtonMotionPattern Color;          /**< 색상 변경 모션*/
    unsigned int         ColorPitch;     /**< 색상 변경 피치*/

    eButtonMotionPattern Text;           /**< 텍스트 변경 모션*/
    unsigned int         TextPitch;      /**< 텍스트 변경 피치*/
}BUTTON_MOTION_SET;

/**
    @brief  버튼 색상 집합
    @remark 개체의 매서드를 통해 변경 될 수 있음
*/
typedef struct _stColorSet {
    D2D1_COLOR_F Frame;     /**< 초기 테두리 색*/
    D2D1_COLOR_F Face;      /**< 초기 버튼 전경색*/
    D2D1_COLOR_F Mouseover; /**< 마우스 오버 색*/
    D2D1_COLOR_F Highlight; /**< 버튼 하이라이트 색*/
    D2D1_COLOR_F Font;      /**< 초기 텍스트 색*/
}BUTTON_COLOR_SET;

#define MAX_BUTTONNAME 32 /*버튼의 최대 텍스트 길이*/

/**
    @brief 버튼 클래스
    @remark 생성은 팩토리에서 해준다.
*/
class UI_Button : public UI {
private:
    ObjectMotionBox*              MBoxFace;       /**< 전경 모션 박스*/
    ObjectMotionBox*              MBoxHighlight;  /**< 하이라이트 모션 박스*/
    ObjectMotionText*             MText;          /**< 모션 박스 기본 텍스트*/
    int                           nTextLen;
    wchar_t                       szText[MAX_BUTTONNAME];
    BUTTON_MOTION_SET             MotionSet;      /**< 모션 집합*/
    BUTTON_COLOR_SET              ColorSet;       /**< 색상 집합*/
    
private: /*2022.04.30 : Devadversary - InputMotion 은 기본적으로 Private 함수여야하지만 그렇게 되면, DefaultButtonProc 함수에서 접근을 못함.*/
    void InputMotion(eButtonMotionType MotionType, eButtonMotionPattern Pattern, unsigned int nDelay, unsigned int nPitch, void* param);
    static void DefaultButtonProc(UI* pUI, UINT Message, void* param);

public: /*반드시 있어야 되는 매서드*/
    void CreateUI(UISystem* pUISys, int nID, ID2D1RenderTarget* pRT,
                  BUTTON_MOTION_SET UIMotionSet, BUTTON_COLOR_SET UIColorSet,
                  pfnUIHandler pfnCallback, POSITION Pos, wchar_t* pText, int nDelay);
    
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
