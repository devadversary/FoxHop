#pragma once

#include "prop.hpp"

/**
    @brief 모션그래픽 박스 오브젝트
*/
class PropBox : public PropBase{
private:
    ComponentMotion       ComMotionMovement;
    ComponentMotion       ComMotionColor;
    ID2D1SolidColorBrush* Brush;        /**< 해당 오브젝트의 색상 브러시*/
    POSITION              InitPos;      /**< 오브젝트의 초기 포지션*/
    POSITION              CurPos;       /**< 현재 오브젝트의 포지션*/
    D2D1_COLOR_F          InitColor;    /**< 오브젝트의 초기 RGBA 값*/
    D2D1_COLOR_F          CurColor;     /**< 현재 오브젝트의 RGBA 값*/
    BOOL                  bFill;        /**< 채워진 사각형 or 비워진 사각형*/

public:
    PropBox();
    ~PropBox();
    void ClearMovementMotion();
    void ClearColorMotion();
    void Init(ID2D1RenderTarget* pRT, POSITION StartPos, D2D1_COLOR_F StartColor, BOOL Fill = TRUE);
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