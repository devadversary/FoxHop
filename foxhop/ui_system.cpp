#include "./include/ui_system.hpp"

#include "./include/ui_panel.hpp"
#include "./include/ui_button.hpp"
#include "./include/ui_fraggedline.hpp"
#include "./include/ui_static.hpp"

/**
    @brief UI�ý����� �ʱ�ȭ �Ѵ�.
    @param hWnd UI �ý����� ����� ���������� �ڵ�
    @remark ���������� ��ǿ�����Ʈ ������ ���� 5000~10000�������� �����ϱ� ������
    @n      ������ �� ������Ʈ�� ���� ��, ��ǿ�����Ʈ�� ���� ���ѿ� �ɸ� �� ����.
    @n      ���Ŀ� UI���� ���ѿ� ���� ������ ��ǿ�����Ʈ ������ �������־�� ��.
*/
UISystem::UISystem(HWND hWnd)
{
    WCHAR szProgramPath[MAX_PATH];
    WCHAR szFontPath[MAX_PATH];
    WCHAR szDir[MAX_PATH];
    WCHAR szDrive[8];

    pMainPanel = NULL;
    hBindWnd   = hWnd;

    /*���α׷��� ���� ��ο� �ִ� Ư�� ��Ʈ�� �ε��Ѵ�.*/
    GetModuleFileNameW(NULL, szProgramPath, MAX_PATH);
    _wsplitpath_s(szProgramPath, szDrive, 8, szDir, MAX_PATH, NULL, 0, NULL, 0);
    wsprintfW(szFontPath, L"%s%s%s", szDrive, szDir, L"monoMMM_5.ttf");
    AddFontResourceW(szFontPath);

    /*D2D ȯ�� �ʱ�ȭ �� UI�ý��� �ʱ�ȭ*/
    D2DA_Init(&D2DA, hWnd); /*Direct X ��ü��� ����Ÿ��, ���丮 �� �ʱ�ȭ*/
    TinyTextForm   = D2DA_SetFont(&D2DA, (wchar_t*)UISYSTEM_FONTNAME_DEFAULT, 7.9f, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    SmallTextForm  = D2DA_SetFont(&D2DA, (wchar_t*)UISYSTEM_FONTNAME_DEFAULT, 9.f, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    ButtonTextForm = D2DA_SetFont(&D2DA, (wchar_t*)UISYSTEM_FONTNAME_DEFAULT, 12, DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    MediumTextForm = D2DA_SetFont(&D2DA, (wchar_t*)UISYSTEM_FONTNAME_DEFAULT, 17, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    /*D2D���� �������� ��, �Ǽ���ǥ�踦 ��������� �� �ȼ��� �߽��� �������� �����ؾ��Ѵ�
      ���� ��ǥ�谡 �ƴϹǷ�, �ȼ��� �߽� (0.5 �ȼ��� +) ������ �ƴϸ� ���� �帮��.*/
    D2DA.pRenTarget->SetTransform(D2D1::Matrix3x2F::Translation(0.5f, 0.5f));

    /*2022.06.04 - TODO : ������Ʈ ����Ʈ�� UISystem ���� ����/���� ���� �ʰ�
      ���� UI���� ������ �ʿ��� ��ŭ�� ���� ����ϵ��� ���� ����*/
    ObjPoolBox.Init(5000, 0);
    ObjPoolLine.Init(5000, 0);
    ObjPoolText.Init(10000, 0);
}

UISystem::~UISystem() {}

/**
    @brief ���� �г��� �ʱ�ȭ �Ѵ�.
*/
UI_Panel* UISystem::InitMainPanel(HWND hWnd, pfnUIHandler MainPanelProc)
{
    RECT WindowSize;
    POSITION pos;

    GetClientRect(hWnd, &WindowSize);
    pos.x = 0;
    pos.y = 0;
    pos.width = (float)WindowSize.right;
    pos.height = (float)WindowSize.bottom;
    return new UI_Panel(this, this->D2DA.pRenTarget, MainPanelProc, pos, TRUE);
}

/**
    @brief Ư�� UI�� �޼��� ������
*/
BOOL UISystem::SendUIMessage(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam)
{
    if (!pUI || !pUI->DefaultHandler) return FALSE;
    pUI->DefaultHandler(pUI, Message, wParam, lParam);
    return TRUE;
}

/**
    @brief UI ��ȯó�� (�̱���)
*/
void UISystem::ReleaseUI()
{

}
