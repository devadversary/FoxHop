#include "./include/alphawindow.hpp"

static int AW_GetWinVer()
{
	if (IsWindows10OrGreater()) return 10;
	if (IsWindows8OrGreater()) return 8;
	if (IsWindows7OrGreater()) return 7;
	return FALSE;
}

/**
	@brief  ����/�������� ������� ��ȯ�����ش�.
	@remark �˾������쿡���� �� �����Ѵ�.
*/
int AlphaWindow(HWND hWnd, int mode)
{
	WINCOMPATTRDATA data = { 0, };
	ACCENTPOLICY pol;
	DWORD ver;
	SetWindowCompositionAttribute SWCA;
	DWM_BLURBEHIND dwm = { 0, };
	HMODULE hMod;

	ver = AW_GetWinVer(); /*������ �������� ������ �ٸ��Ƿ�*/
	if (ver < 8) /*��7 ����*/
	{
		dwm.dwFlags = DWM_BB_ENABLE;
		dwm.fEnable = TRUE;
		DwmEnableBlurBehindWindow(hWnd, &dwm);
		return WINDOWMODE_ERROR_VERSION_UNAVAILABLE;
	}
	hMod = LoadLibrary(TEXT("user32.dll"));
	if (!hMod) return WINDOWMODE_ERROR_LIBRARY_FAIL;

	SWCA = (SetWindowCompositionAttribute)GetProcAddress(hMod, "SetWindowCompositionAttribute");
	if (!SWCA)
	{
		FreeLibrary(hMod);
		return WINDOWMODE_ERROR_CODE_FAIL;
	}
	memset(&pol, 0, sizeof(pol));
	switch (mode)
	{
	case WINDOWMODE_TRANSPARENT: /*�������� ������*/
		pol.nAccentState = ACCENT_ENABLE_TRANSPARENTGRADIENT;
		pol.nFlags = 2;
		pol.nColor = 0x00000000; /*ABGR*/
		break;

	case WINDOWMODE_BLURBEHIND: /*�޸� �����Ǿ� ��ġ�� ������*/
		pol.nAccentState = ACCENT_ENABLE_BLURBEHIND;
		break;
	}
	data.attribute = WCA_ACCENT_POLICY;
	data.pData = &pol;
	data.dataSize = sizeof(pol);

	if (SWCA(hWnd, &data))
	{
		FreeLibrary(hMod);
		return WINDOWMODE_ERROR_EXCUTE_FAIL;
	}
	FreeLibrary(hMod);
	return 0;
}