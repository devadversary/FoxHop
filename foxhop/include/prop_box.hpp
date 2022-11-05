#pragma once

#include "prop.hpp"

/**
    @brief ��Ǳ׷��� �ڽ� ������Ʈ
*/
class PropBox : public PropBase{
private:
    ComponentMotion       ComMotionMovement;
    ComponentMotion       ComMotionColor;
    ID2D1SolidColorBrush* Brush;        /**< �ش� ������Ʈ�� ���� �귯��*/
    POSITION              InitPos;      /**< ������Ʈ�� �ʱ� ������*/
    POSITION              CurPos;       /**< ���� ������Ʈ�� ������*/
    D2D1_COLOR_F          InitColor;    /**< ������Ʈ�� �ʱ� RGBA ��*/
    D2D1_COLOR_F          CurColor;     /**< ���� ������Ʈ�� RGBA ��*/
    BOOL                  bFill;        /**< ä���� �簢�� or ����� �簢��*/

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