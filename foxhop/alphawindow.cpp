#include "./include/alphawindow.hpp"

static int AW_GetWinVer()
{
	if (IsWindows10OrGreater()) return 10;
	if (IsWindows8OrGreater()) return 8;
	if (IsWindows7OrGreater()) return 7;
	return FALSE;
}

/**
	@brief  투명/반투명한 윈도우로 변환시켜준다.
	@remark 팝업윈도우에서만 잘 동작한다.
*/
int AlphaWindow(HWND hWnd, int mode)
{
	WINCOMPATTRDATA data = { 0, };
	ACCENTPOLICY pol;
	DWORD ver;
	SetWindowCompositionAttribute SWCA;
	DWM_BLURBEHIND dwm = { 0, };
	HMODULE hMod;

	ver = AW_GetWinVer(); /*윈도우 버전마다 구현이 다르므로*/
	if (ver < 8) /*윈7 이하*/
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
	case WINDOWMODE_TRANSPARENT: /*완전투명 윈도우*/
		pol.nAccentState = ACCENT_ENABLE_TRANSPARENTGRADIENT;
		pol.nFlags = 2;
		pol.nColor = 0x00000000; /*ABGR*/
		break;

	case WINDOWMODE_BLURBEHIND: /*뒷면 블러링되어 비치는 윈도우*/
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