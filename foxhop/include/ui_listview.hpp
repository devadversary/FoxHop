#pragma once

#include "ui_base.hpp"
#include <vector>

class UISystem; /*UI시스템 클래스의 전방선언*/

/**
    @brief 리스트뷰가 관리할 행 데이터
*/
typedef struct _st_ListviewRow
{
    bool  bSelected;   /**< 행 선택 여부*/
    void* pDataStruct; /**< 임의의 사용자 데이터 구조체 */
} LISTVIEW_ROW;

#if 0
/**
    @brief 리스트뷰 행을 나타낼 하위 클래스.
    @remark 유저 커스텀 리스트뷰를 만들때 이 클래스를 상속받아 구현한다.
    @n      다양한 모습의 리스트뷰를 렌더링 하기 위한 매개체.
    @n      UI_ListviewBase에 의해 유한개로 관리된다. (한 화면에 보이는 갯수 + 1개)
*/
class UI_LvField {
public :
    int Height; /**< 행 높이*/
    int* width;
public :
    UI_LvField(int nColCnt);
    ~UI_LvField();
    virtual BOOL update(unsigned long time) = 0;
    virtual void render() = 0;
    virtual void pause(int nDelay) = 0;
    virtual void resume(int nDelay) = 0;
};

/**
    @brief 기본 제공되는 문자열 위주의 클래식한 리스트뷰 행
*/
class UI_LvFieldString : public UI_LvField {
public :
};
#endif

/**
    @brief 삽입 삭제를 지원하는 리스트뷰 클래스
    @n     UI_Field 타입을 넘겨주자.
*/
class UI_ListviewBase : public UI {
private:
    BOOL                      MultiSelectMode; /**< FALSE:단일 선택 / TRUE:여러줄 선택 모드*/
    std::vector<LISTVIEW_ROW> MainDataPool;
    /*
    ViewStartPtr;
    ViewDataPool;
    */

private:
    //void InputMotion(eListviewAction Action, UITheme* Theme, unsigned int nDelay, void* param);
    static void DefaultListviewProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam);

public:
    UI_ListviewBase(UISystem* pUISys, pfnUIHandler pfnCallback, POSITION Pos, int RowHeight, BOOL MultiSelect);
    ~UI_ListviewBase() {};

    void pause(int nDelay);
    void resume(int nDelay);
    void Destroy();
    BOOL update(unsigned long time);
    void render();

public: /*UI별 옵션 매서드*/
    void AddRow(void* pUserData);
    void DelRow(int index);
    void DelSelectedRow();
    //void MoveLine(int index);
    void EnsureRow(int index);
};
