#include "./include/d2dassist.hpp"

/**
    @brief D2D ��ý�Ʈ �ʱ�ȭ
    @param pContext �ʱ�ȭ�� ���ؽ�Ʈ
    @param hWnd     ��� ������ �ڵ�
*/
BOOL D2DA_Init(D2DA_CONTEXT* pContext, HWND hWnd)
{
    RECT rect;
    D2D1_RENDER_TARGET_PROPERTIES rtp;
    D2D1_HWND_RENDER_TARGET_PROPERTIES rhtp;
    D2D1_CREATION_PROPERTIES cp;
    D3D_FEATURE_LEVEL d3dFL[] = { D3D_FEATURE_LEVEL_9_1 };

    if (!pContext) return FALSE;
    do {
        if (S_OK != CoInitializeEx(NULL, COINIT_APARTMENTTHREADED)) break;

        /*d3d11 ����̽��� �����Ѵ�*/
        if (S_OK != D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, 0, D3D11_CREATE_DEVICE_BGRA_SUPPORT, d3dFL, 1, D3D11_SDK_VERSION, &pContext->pD3D11, 0, &pContext->pD3D11DC)) break;

        /*d3d11�κ��� DXGI ����̽��� ���´�*/
        if (S_OK != pContext->pD3D11->QueryInterface(__uuidof(IDXGIDevice), (void**)&pContext->pDXGIDev)) break;

        /*���� DXGI�κ��� d2d�� �����Ѵ�*/
        cp.threadingMode = D2D1_THREADING_MODE_SINGLE_THREADED;
        cp.debugLevel = D2D1_DEBUG_LEVEL_NONE;
        cp.options = D2D1_DEVICE_CONTEXT_OPTIONS_NONE;
        if (S_OK != D2D1CreateDevice(pContext->pDXGIDev, &cp, &pContext->pD2D)) break;

        /*d2d���丮�� �����Ѵ�*/
        if (S_OK != D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pContext->pFactory)) break;

        /*��Ʈ ����� ���� DirectWrite���丮�� �����Ѵ�*/
        if (S_OK != DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)&pContext->pDWFactory)) break;

        /*d2d���丮�� hWnd�� ���� ����Ÿ���� ���´�*/
        rtp.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
        rtp.pixelFormat = { DXGI_FORMAT_B8G8R8A8_UNORM ,D2D1_ALPHA_MODE_PREMULTIPLIED };
        rtp.dpiX = 0;
        rtp.dpiY = 0;
        rtp.usage = D2D1_RENDER_TARGET_USAGE_NONE;
        rtp.minLevel = D2D1_FEATURE_LEVEL_DEFAULT;
        GetClientRect(hWnd, &rect);
        rhtp.hwnd = hWnd;
        rhtp.pixelSize = { (UINT32)rect.right, (UINT32)rect.bottom };
        rhtp.presentOptions = D2D1_PRESENT_OPTIONS_NONE;
        if (S_OK != pContext->pFactory->CreateHwndRenderTarget(&rtp, &rhtp, &pContext->pRenTarget)) break;

        return TRUE;
    } while (FALSE);

    if (pContext->pDWFactory) pContext->pDWFactory->Release();
    if (pContext->pDXGIDev)   pContext->pDXGIDev->Release();
    if (pContext->pD3D11)     pContext->pD3D11->Release();
    if (pContext->pD3D11DC)   pContext->pD3D11DC->Release();
    if (pContext->pD2D)       pContext->pD2D->Release();
    if (pContext->pFactory)   pContext->pFactory->Release();
    CoUninitialize();
    return FALSE;
}

/**
    @brief D2D���� ����� ��Ʈ�� �����Ѵ�.
*/
IDWriteTextFormat* D2DA_SetFont(D2DA_CONTEXT* pContext, wchar_t* szFontName, float fFontSize,
                                DWRITE_TEXT_ALIGNMENT TextAlign, DWRITE_PARAGRAPH_ALIGNMENT ParagraphAlign)
{
    IDWriteTextFormat* pTextFmt;

    if (!pContext || !pContext->pDWFactory) return NULL;
    if (pContext->pDWFormat) pContext->pDWFormat->Release(); /*�̹� ���� ��Ʈ�� ������ ���� �� �����*/
    if (S_OK != pContext->pDWFactory->CreateTextFormat(szFontName, 0, DWRITE_FONT_WEIGHT_MEDIUM, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fFontSize, L"en-us", &pTextFmt)) return FALSE;
    pTextFmt->SetTextAlignment(TextAlign); /*�� ���� ���� (��� ���� ��)*/
    /*ex : DWRITE_TEXT_ALIGNMENT_LEADING*/
    pTextFmt->SetParagraphAlignment(ParagraphAlign); /*�ؽ�Ʈ ��� ������ ���� (�ϴ� ����, ��� ���� ��)*/
    /*ex : DWRITE_PARAGRAPH_ALIGNMENT_CENTER*/
    return pTextFmt;
}