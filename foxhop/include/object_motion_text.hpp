#pragma once

#include "object.hpp"

/**
    @brief ��Ǳ׷��� �ؽ�Ʈ ������Ʈ
*/
class ObjectMotionText : public Object{
public :
    ObjectMotionText*     pNext;
    ObjectMotionText*     pPrev;
    BOOL                  bActivate;     /**< �ش� ������Ʈ�� Ȱ��ȭ ����*/

private:
    ComponentMotion       ComMotionMovement;
    ComponentMotion       ComMotionColor;
    ComponentMotion       ComMotionStrLen;
    ID2D1SolidColorBrush* Brush;        /**< �ش� ������Ʈ�� ���� �귯��*/
    POSITION              InitPos;      /**< ���� ������Ʈ�� ������*/
    D2D1_COLOR_F          InitColor;    /**< ���� ������Ʈ�� RGBA ��*/
    int                   InitLen;      /**< ���� ������� ���ڿ� ����*/
    POSITION              CurPos;       /**< ���� ������Ʈ�� ������*/
    D2D1_COLOR_F          CurColor;     /**< ���� ������Ʈ�� RGBA ��*/
    float                 CurLen;       /**< ���� ������� ���ڿ� ���� (������ȯ�ؾ���)*/
    IDWriteTextFormat*    pTextFmt;     /**< �ؽ�Ʈ�� �׷��� ���� ����(��Ʈ, ����, �ڰ� ��)*/
    wchar_t*              pStr;         /**< ��� ��� ���ڿ�*/
    int                   nStrLen;      /**< ��ü ���� ����*/

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