#pragma once
#include <windows.h>
#include <windowsx.h>
#include <dcommon.h>
#include <dxgi.h>
#include <dxgi1_2.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <d2d1.h>
#include <d2d1_1.h>
#include <dwrite.h>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

/**
    @brief D2D ��ý�Ʈ�� ����ϱ� ���� �����̳�
*/
typedef struct _d2da_context
{
    ID3D11Device* pD3D11;     /**< D3D11 ����̽�*/
    ID3D11DeviceContext* pD3D11DC;   /**< D3D11�� DC*/
    IDXGIDevice* pDXGIDev;   /**< �� DXGI ��ü*/
    ID2D1Device* pD2D;       /**< D2D ��ġ*/
    ID2D1Factory* pFactory;   /**< D2D ���丮*/
    ID2D1HwndRenderTarget* pRenTarget; /**< �׷��� ���� Ÿ��*/
    IDWriteFactory* pDWFactory; /**< �ؽ�Ʈ�� ������ �� ���丮*/
    IDWriteTextFormat* pDWFormat;  /**< �ؽ�Ʈ�� �׷��� ���� ����*/
}D2DA_CONTEXT;

/**
    @brief D2D ��ý�Ʈ �ʱ�ȭ
    @param pContext �ʱ�ȭ�� ���ؽ�Ʈ
    @param hWnd     ��� ������ �ڵ�
*/
BOOL D2DA_Init(D2DA_CONTEXT* pContext, HWND hWnd);

/**
    @brief D2D���� ����� ��Ʈ�� �����Ѵ�.
*/
IDWriteTextFormat* D2DA_SetFont(D2DA_CONTEXT* pContext, wchar_t* szFontName, float fFontSize,
                                DWRITE_TEXT_ALIGNMENT TextAlign, DWRITE_PARAGRAPH_ALIGNMENT ParagraphAlign);
