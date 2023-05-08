#pragma once

#include "prop.hpp"

/**
    @brief ��Ǳ׷��� ���� ������Ʈ
*/
class PropLine : public PropBase{
private:
    ComponentMotion       ComMotionMovement;
    ComponentMotion       ComMotionColor;
    ID2D1SolidColorBrush* Brush;        /**< �ش� ������Ʈ�� ���� �귯��*/
    POSITION              InitPos;       /**< �ʱ� ������Ʈ�� ������*/
    D2D1_COLOR_F          InitColor;     /**< �ʱ� ������Ʈ�� RGBA ��*/
    POSITION              CurPos;       /**< ���� ������Ʈ�� ������*/
    D2D1_COLOR_F          CurColor;     /**< ���� ������Ʈ�� RGBA ��*/

public :
    PropLine(ID2D1RenderTarget* pRT);
    ~PropLine();
    void Init(POSITION StartPoint, D2D1_COLOR_F StartColor);
    void addMovementMotion(MOTION_INFO MotionInfo, BOOL bAppend, POSITION StartPos, POSITION EndPos);
    void addColorMotion(MOTION_INFO MotionInfo, BOOL bAppend, D2D1_COLOR_F StartColor, D2D1_COLOR_F EndColor);

    void SetPos(MOTION_INFO MotionInfo, BOOL bCurrent, POSITION StartPos, POSITION EndPos);
    void SetColor(MOTION_INFO MotionInfo, BOOL bCurrent, D2D1_COLOR_F StartColor, D2D1_COLOR_F EndColor);

    void setRuntime(int t);
    BOOL update(unsigned long time);
    void render(ID2D1RenderTarget* pRT);
};
