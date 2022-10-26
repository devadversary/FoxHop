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
    @brief D2D 어시스트를 사용하기 위한 컨테이너
*/
typedef struct _d2da_context
{
    ID3D11Device* pD3D11;     /**< D3D11 디바이스*/
    ID3D11DeviceContext* pD3D11DC;   /**< D3D11의 DC*/
    IDXGIDevice* pDXGIDev;   /**< 얻어낸 DXGI 개체*/
    ID2D1Device* pD2D;       /**< D2D 장치*/
    ID2D1Factory* pFactory;   /**< D2D 팩토리*/
    ID2D1HwndRenderTarget* pRenTarget; /**< 그려질 렌더 타겟*/
    IDWriteFactory* pDWFactory; /**< 텍스트를 렌더링 할 팩토리*/
    IDWriteTextFormat* pDWFormat;  /**< 텍스트가 그려질 형식 지정*/
}D2DA_CONTEXT;

/**
    @brief D2D 어시스트 초기화
    @param pContext 초기화될 컨텍스트
    @param hWnd     대상 윈도우 핸들
*/
BOOL D2DA_Init(D2DA_CONTEXT* pContext, HWND hWnd);

/**
    @brief D2D에서 사용할 폰트를 설정한다.
*/
IDWriteTextFormat* D2DA_SetFont(D2DA_CONTEXT* pContext, wchar_t* szFontName, float fFontSize,
                                DWRITE_TEXT_ALIGNMENT TextAlign, DWRITE_PARAGRAPH_ALIGNMENT ParagraphAlign);
