#pragma once

#include "prop.hpp"

/**
    @brief ��Ǳ׷��� �ؽ�Ʈ ������Ʈ
*/
class PropText : public PropBase{
private:
    ComponentMotion       ComMotionMovement;
    ComponentMotion       ComMotionColor;
    ComponentMotion       ComMotionStrLen;
    ID2D1SolidColorBrush* Brush;        /**< �ش� ������Ʈ�� ���� �귯��*/
    IDWriteTextFormat*    pTextFmt;     /**< �ؽ�Ʈ�� �׷��� ���� ����(��Ʈ, ����, �ڰ� ��)*/
    POSITION              InitPos;      /**< ���� ������Ʈ�� ������*/
    D2D1_COLOR_F          InitColor;    /**< ���� ������Ʈ�� RGBA ��*/
    POSITION              CurPos;       /**< ���� ������Ʈ�� ������*/
    D2D1_COLOR_F          CurColor;     /**< ���� ������Ʈ�� RGBA ��*/
    float                 CurLen;       /**< ���� ������� ���ڿ� ���� (������ȯ�ؾ���)*/
    wchar_t*              pStr;         /**< ��� ��� ���ڿ�*/
    int                   nStrLen;      /**< ��ü ���� ����*/
    int                   MaxLen;       /**< ���ڿ� �ִ� ����*/

public:
    PropText(ID2D1RenderTarget* pRT, int MaxLength);
    ~PropText();
    void Init(IDWriteTextFormat* pTexFmt, wchar_t* pText, int nTextLen, POSITION StartPos, D2D1_COLOR_F StartColor, int StartLen);
    void addMovementMotion(MOTION_INFO MotionInfo, BOOL bAppend, POSITION StartPos, POSITION EndPos);
    void addColorMotion(MOTION_INFO MotionInfo, BOOL bAppend, D2D1_COLOR_F StartColor, D2D1_COLOR_F EndColor);
    void addLenMotion(MOTION_INFO MotionInfo, BOOL bAppend, int nStartLen, int nEndLen);

    void SetPos(MOTION_INFO MotionInfo, BOOL bCurrent, POSITION StartPos, POSITION EndPos);
    void SetColor(MOTION_INFO MotionInfo, BOOL bCurrent, D2D1_COLOR_F StartColor, D2D1_COLOR_F EndColor);
    void SetLen(MOTION_INFO MotionInfo, BOOL bCurrent, unsigned long StartLen, unsigned long EndLen);
    void SetText(wchar_t* Str);
    BOOL update(unsigned long time);
    void render(ID2D1RenderTarget* pRT);
};