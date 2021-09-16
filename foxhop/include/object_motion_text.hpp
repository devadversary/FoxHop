#pragma once

#include "object.hpp"

/**
    @brief 모션그래픽 텍스트 오브젝트
*/
class ObjectMotionText : public Object{
public :
    ObjectMotionText*     pNext;
    ObjectMotionText*     pPrev;
    BOOL                  bActivate;     /**< 해당 오브젝트의 활성화 여부*/

private:
    ComponentMotion       ComMotionMovement;
    ComponentMotion       ComMotionColor;
    ComponentMotion       ComMotionStrLen;
    ID2D1SolidColorBrush* Brush;        /**< 해당 오브젝트의 색상 브러시*/
    POSITION              InitPos;      /**< 현재 오브젝트의 포지션*/
    D2D1_COLOR_F          InitColor;    /**< 현재 오브젝트의 RGBA 값*/
    int                   InitLen;      /**< 현재 출력중인 문자열 길이*/
    POSITION              CurPos;       /**< 현재 오브젝트의 포지션*/
    D2D1_COLOR_F          CurColor;     /**< 현재 오브젝트의 RGBA 값*/
    float                 CurLen;       /**< 현재 출력중인 문자열 길이 (정수변환해야함)*/
    IDWriteTextFormat*    pTextFmt;     /**< 텍스트가 그려질 형식 지정(폰트, 정렬, 자간 등)*/
    wchar_t*              pStr;         /**< 출력 대상 문자열*/
    int                   nStrLen;      /**< 전체 문자 길이*/

public:
    ObjectMotionText();
    ~ObjectMotionText();
    void Init(ID2D1RenderTarget* pRT, IDWriteTextFormat* pTexFmt, wchar_t* pText, int nTextLen, POSITION StartPos, D2D1_COLOR_F StartColor, int StartLen);
    void addMovementMotion(MOTION_INFO MotionInfo, BOOL bAppend, POSITION StartPos, POSITION EndPos);
    void addColorMotion(MOTION_INFO MotionInfo, BOOL bAppend, D2D1_COLOR_F StartColor, D2D1_COLOR_F EndColor);
    void addLenMotion(MOTION_INFO MotionInfo, BOOL bAppend, int nStartLen, int nEndLen);

    void SetPos(MOTION_INFO MotionInfo, BOOL bCurrent, POSITION StartPos, POSITION EndPos);
    void SetColor(MOTION_INFO MotionInfo, BOOL bCurrent, D2D1_COLOR_F StartColor, D2D1_COLOR_F EndColor);
    void SetText(MOTION_INFO MotionInfo, wchar_t* pText, int nStartTextLen, int nEndTextLen);

    void setRuntime(unsigned long time);
    BOOL allUpdate(unsigned long time);
    void allRender(ID2D1RenderTarget* pRT);
    BOOL update(unsigned long time);
    void render(ID2D1RenderTarget* pRT);
};