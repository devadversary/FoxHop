#pragma once

#include "prop.hpp"
#include <iostream>

/**
    @brief ��Ǳ׷��� �ؽ�Ʈ ������Ʈ
*/
class PropTextLayout : public PropBase {
public : 
    IDWriteTextLayout*    pLayout;
    std::wstring          Str;         /**< ��� ��� ���ڿ�*/

private:
    ComponentMotion       ComMotionMovement;
    ComponentMotion       ComMotionColor;
    ComponentMotion       ComMotionStrLen;
    ID2D1SolidColorBrush* Brush;        /**< �ش� ������Ʈ�� ���� �귯��*/
    IDWriteFactory*       pDWFactory;
    IDWriteTextFormat*    pTextFmt;     /**< �ؽ�Ʈ�� �׷��� ���� ����(��Ʈ, ����, �ڰ� ��)*/
    POSITION              InitPos;      /**< ���� ������Ʈ�� ������*/
    D2D1_COLOR_F          InitColor;    /**< ���� ������Ʈ�� RGBA ��*/
    POSITION              CurPos;       /**< ���� ������Ʈ�� ������*/
    D2D1_COLOR_F          CurColor;     /**< ���� ������Ʈ�� RGBA ��*/
    int                   nStrLen;      /**< ��ü ���� ����*/

private:
    void UpdateTextLayout();

public:
    PropTextLayout(ID2D1RenderTarget* pRT, IDWriteTextFormat* pTexFmt, IDWriteFactory* pFactory, int Width, int Height);
    ~PropTextLayout();
    void Init(POSITION StartPos, D2D1_COLOR_F StartColor);
    void addMovementMotion(MOTION_INFO MotionInfo, BOOL bAppend, POSITION StartPos, POSITION EndPos);
    void addColorMotion(MOTION_INFO MotionInfo, BOOL bAppend, D2D1_COLOR_F StartColor, D2D1_COLOR_F EndColor);

    BOOL EraseText(unsigned long IntervalA, unsigned long IntervalB);
    void InsertChar(unsigned long Idx, wchar_t wch);
    void ReplaceChar(unsigned long Idx, wchar_t wch);
    void SetPos(MOTION_INFO MotionInfo, BOOL bCurrent, POSITION StartPos, POSITION EndPos);
    void SetColor(MOTION_INFO MotionInfo, BOOL bCurrent, D2D1_COLOR_F StartColor, D2D1_COLOR_F EndColor);

    BOOL update(unsigned long time);
    void render(ID2D1RenderTarget* pRT);
};