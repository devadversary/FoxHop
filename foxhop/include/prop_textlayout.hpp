#pragma once

#include "prop.hpp"
#include <iostream>

/**
    @brief 모션그래픽 텍스트 오브젝트
*/
class PropTextLayout : public PropBase {
public : 
    IDWriteTextLayout*    pLayout;
    std::wstring          Str;         /**< 출력 대상 문자열*/

private:
    ComponentMotion       ComMotionMovement;
    ComponentMotion       ComMotionColor;
    ComponentMotion       ComMotionStrLen;
    ID2D1SolidColorBrush* Brush;        /**< 해당 오브젝트의 색상 브러시*/
    IDWriteFactory*       pDWFactory;
    IDWriteTextFormat*    pTextFmt;     /**< 텍스트가 그려질 형식 지정(폰트, 정렬, 자간 등)*/
    POSITION              InitPos;      /**< 현재 오브젝트의 포지션*/
    D2D1_COLOR_F          InitColor;    /**< 현재 오브젝트의 RGBA 값*/
    POSITION              CurPos;       /**< 현재 오브젝트의 포지션*/
    D2D1_COLOR_F          CurColor;     /**< 현재 오브젝트의 RGBA 값*/
    int                   nStrLen;      /**< 전체 문자 길이*/

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