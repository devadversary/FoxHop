#pragma once

#include "ui_base.hpp"

class UISystem; /*UI시스템 클래스의 전방선언*/
enum class UIType;

#define MAX_SUB_PANEL_CNT 32 /*최대 하위 패널 갯수 (휴면 상태 패널 포함)*/
#define MAX_SUB_UI_CNT    32 /*최대 하위 UI 갯수 (휴면 상태 UI 포함)*/

typedef struct _st_ui_info
{
    UIType type; /**< 이 UI의 실제 타입 (버튼인지 리스트인지 등)*/
    UI*    pUI;  /**< UI의 인스턴스*/
} UI_INFO;

/**
    @brief 패널 UI 클래스
    @remark 패널 UI는 타 컨트롤 클래스와는 다르게
    @n      자신이 가지고있는 하위 패널의 리스트와
    @n      컨트롤, 모션오브젝트들의 리스트를 갖고 있어야 한다.
    @n      마우스 오버, 포커스, 키 입력 등 사용자 입력을 처리하기 위해서는
    @n      이 리스트가 필요하기 때문이다.
    @n      버튼, 체크박스, 리스트, 스태틱 등의 컨트롤들은 스스로가
    @n      마우스 입력, 클릭에 대해 반응할 수 없다.
*/
class UI_Panel : public UI {
private:
    POSITION             PanelPos;                     /**< 패널의 절대 좌표*/
    int                  PanelDelay;                   /**< 패널의 업데이트 딜레이*/
    int                  nTimeSum;                     /**< 총 지난 시간. 상황에 따라서 이 값은 스스로 바꿀수 있다*/
    std::list<UI_Panel>  PanelList;                    /**< 패널이 가진 하위 패널의 리스트*/
    std::list<UI>        UIList;                       /**< 패널이 가진 하위 UI의 리스트*/
    D2D1::Matrix3x2F     transform;                    /**< 해당 패널의 변환행렬*/
    UI* pTmpUI;                       /**< 마우스 이벤트 핸들링을 위한 임시변수*/

public:
    UI_Panel() {};
    UI_Panel(UISystem* pUISys, ID2D1RenderTarget* pRT, int nID, pfnUIHandler pfnCallback);
    ~UI_Panel();
    void Init(POSITION Pos, int nDelay = 0);
    void pause(int nDelay);
    void resume(int nDelay);
    void Destroy();
    void update(unsigned long time);
    void render();

public: /*UI별 옵션 매서드*/
    UI*  CreateUI(UIType type, unsigned int nID, POSITION pos, wchar_t* pText, int nDelay, pfnUIHandler callback);
    void DefaultMouseHandler(POINT pt, UINT Message, void* param);
};
