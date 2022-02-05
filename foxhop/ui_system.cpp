#include "./include/ui_system.hpp"

#include "./include/ui_panel.hpp"
#include "./include/ui_button.hpp"
#include "./include/ui_divisionline.hpp"
#include "./include/ui_static.hpp"

UISystem::UISystem()
{
    D2DA = { 0, };
    pUIArray = NULL;
    ButtonTextForm = NULL;
}

UISystem::~UISystem() {}

/**
    @brief UI�ý����� �ʱ�ȭ �Ѵ�.
    @param hWnd UI �ý����� ����� ���������� �ڵ�
    @param nMaxUI UI���� �Ѱ�ġ ���� ���� (0 ������ �⺻�� 256��)
    @remark ���������� ��ǿ�����Ʈ ������ ���� 5000~10000�������� �����ϱ� ������
    @n      nMaxUI �� UI���� ������ �ø����, ��ǿ�����Ʈ�� ���� ���ѿ� �ɸ� �� ����.
    @n      ���Ŀ� UI���� ���ѿ� ���� ������ ��ǿ�����Ʈ ������ �������־�� ��.
*/
void UISystem::Init(HWND hWnd, unsigned int nMaxUI)
{
    int   nTmpMaxUI;
    WCHAR szProgramPath[MAX_PATH];
    WCHAR szFontPath[MAX_PATH];
    WCHAR szDir[MAX_PATH];
    WCHAR szDrive[8];

    /*���α׷��� ���� ��ο� �ִ� Ư�� ��Ʈ�� �ε��Ѵ�.*/
    GetModuleFileNameW(NULL, szProgramPath, MAX_PATH);
    _wsplitpath_s(szProgramPath, szDrive, 8, szDir, MAX_PATH, NULL, 0, NULL, 0);
    wsprintfW(szFontPath, L"%s%s%s", szDrive, szDir, L"monoMMM_5.ttf");
    AddFontResourceW(szFontPath);
    /*D2D ȯ�� �ʱ�ȭ �� UI�ý��� �ʱ�ȭ*/
    D2DA_Init(&D2DA, hWnd); /*Direct X ��ü��� ����Ÿ��, ���丮 �� �ʱ�ȭ*/
    TinyTextForm   = D2DA_SetFont(&D2DA, (wchar_t*)UISYSTEM_FONTNAME_DEFAULT, 7.9f, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    SmallTextForm  = D2DA_SetFont(&D2DA, (wchar_t*)UISYSTEM_FONTNAME_DEFAULT, 9.f, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    ButtonTextForm = D2DA_SetFont(&D2DA, (wchar_t*)UISYSTEM_FONTNAME_DEFAULT, 12, DWRITE_TEXT_ALIGNMENT_CENTER,  DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    MediumTextForm = D2DA_SetFont(&D2DA, (wchar_t*)UISYSTEM_FONTNAME_DEFAULT, 17, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    nTmpMaxUI = nMaxUI ? nMaxUI : DEFAULT_MAXUI; /*nMaxUI�� 0�̸� �⺻�� 256���� ����*/
    pUIArray = (UI**)calloc(nMaxUI, sizeof(UI*));
    nMaxUICnt = nMaxUI;
    /*D2D���� �������� ��, �Ǽ���ǥ�踦 ��������� �� �ȼ��� �߽��� �������� �����ؾ��Ѵ�
      ���� ��ǥ�谡 �ƴϹǷ�, �ȼ��� �߽� (0.5 �ȼ��� +) ������ �ƴϸ� ���� �帮��.*/
    D2DA.pRenTarget->SetTransform(D2D1::Matrix3x2F::Translation(0.5f, 0.5f));

    ObjPoolBox.Init(5000, 0);
    ObjPoolLine.Init(5000, 0);
    ObjPoolText.Init(10000, 0);

    /*�� UI ���丮 �ʱ�ȭ*/
    pUIButtonFactory = new UI_ButtonFactory; pUIButtonFactory->Init(this, D2DA.pRenTarget);
}
 
/**
    @brief UI ��Ʈ���� �����Ѵ�.
    @param type     ������ UI�� Ÿ�� (��ư, ����ƽ ���)
    @param callback ������ UI���� �ݹ��Լ� ���� (�̺�Ʈ ó�� ��)
    @remark �� �ż��尡 UI�� �����Ҷ��� �� UI���� �����Ǿ��ִ� preInit �ż��带 ���� �⺻ �ʱ�ȭ�� �����Ѵ�
    @n      UI�ý��� �ʱ�ȭ�� �����ߴ� �ִ� UI ������ 200�̶�� nID�� ������ 0-199 �� �Ǿ���Ѵ�.
*/
UI* UISystem::CreateUI(UIType type, POSITION pos, wchar_t* pText, int nDelay, pfnUIHandler callback)
{
    UI* pUI = 0;
    UI_INFO* pInfo;

    if (!pUIArray) return NULL;
    if (nID >= nMaxUICnt) return NULL;
    if (pUIArray[nID]) return NULL; /*�̹� �ش� ID�� UI�� ���� �Ǿ��ִ� ���*/
    switch (type) {
    case UIType::eUI_Panel:
        pUI = new UI_Panel;
        pUI->uiType = UIType::eUI_Panel;
        ((UI_Panel*)pUI)->preInit(this, D2DA.pRenTarget, nID, callback);
        break;

    case UIType::eUI_Button:
        pUI = new UI_Button;
        pUI->uiType = UIType::eUI_Button;
        pUIButtonFactory->CreateUI(nID, pos, pText, nDelay, callback);
        break;

    case UIType::eUI_DivLine : 
        break;

    case UIType::eUI_Static: 
        break;

    case UIType::eUI_List: break;
    case UIType::eUI_Edit: break;
    case UIType::eUI_Scroll: break;
    case UIType::eUI_Progress: break;
    case UIType::eUI_Graph: break;
    default: return NULL;
    }
    pUIArray[nID] = pUI;
    if (!pUI) return NULL;
    /************************************************/
    //pUIID[nID].pUI = pUI;
    return pUI;
}

/**
    @brief Ư�� UI�� �޼��� ������
*/
BOOL UISystem::SendUIMessage(UI* pUI, UINT Message, void* param)
{
    if (!pUI || !pUI->DefaultHandler) return FALSE;
    pUI->DefaultHandler(pUI, Message, param);
    return TRUE;
}

/**
    @brief UI ��ȯó�� (�̱���)
*/
void UISystem::ReleaseUI(unsigned int nID)
{
    UI* pUI;

    pUI = pUIArray[nID];
    /*��ȯ �۾� �̱���. delete �� ������*/
    pUIArray[nID] = NULL;
}
