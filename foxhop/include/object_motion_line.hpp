#pragma once

#include "object.hpp"

/**
    @brief 모션그래픽 직선 오브젝트
*/
class ObjectMotionLine : public Object{
public :
    ObjectMotionLine*     pNext;
    ObjectMotionLine*     pPrev;
    BOOL                  bActivate;    /**< 해당 오브젝트의 활성화 여부*/

private:
    ComponentMotion       ComMotionMovement;
    ComponentMotion       ComMotionColor;
    ID2D1SolidColorBrush* Brush;        /**< 해당 오브젝트의 색상 브러시*/
    POSITION              InitPos;       /**< 초기 오브젝트의 포지션*/
    D2D1_COLOR_F          InitColor;     /**< 초기 오브젝트의 RGBA 값*/
    POSITION              CurPos;       /**< 현재 오브젝트의 포지션*/
    D2D1_COLOR_F          CurColor;     /**< 현재 오브젝트의 RGBA 값*/

public :
    ObjectMotionLine();
    ~ObjectMotionLine();
    void ClearMovementMotion();
    void Init(ID2D1RenderTarget* pRT, POSITION StartPoint, D2D1_COLOR_F StartColor);
    void addMovementMotion(MOTION_INFO MotionInfo, BOOL bAppend, POSITION StartPos, POSITION EndPos);
    void addColorMotion(MOTION_INFO MotionInfo, BOOL bAppend, D2D1_COLOR_F StartColor, D2D1_COLOR_F EndColor);

    void SetPos(MOTION_INFO MotionInfo, BOOL bCurrent, POSITION StartPos, POSITION EndPos);
    void SetColor(MOTION_INFO MotionInfo, BOOL bCurrent, D2D1_COLOR_F StartColor, D2D1_COLOR_F EndColor);

    void setRuntime(int t);
    BOOL allUpdate(unsigned long time);
    void allRender(ID2D1RenderTarget* pRT);
    BOOL update(unsigned long time);
    void render(ID2D1RenderTarget* pRT);
};
