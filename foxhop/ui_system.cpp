#include "./include/ui_system.hpp"

UISystem::UISystem()
{
    D2DA = { 0, };
    pUITbl = NULL;
    ButtonTextForm = NULL;
}

UISystem::~UISystem() {}

/**
    @brief UI�ý����� �ʱ�ȭ �Ѵ�.
*/
void UISystem::Init(HWND hWnd, unsigned int nMaxUI)
{
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
    TinyTextForm = D2DA_SetFont(&D2DA, (wchar_t*)UISYSTEM_FONTNAME_DEFAULT, 7.9f, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    SmallTextForm = D2DA_SetFont(&D2DA, (wchar_t*)UISYSTEM_FONTNAME_DEFAULT, 9.f, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    ButtonTextForm = D2DA_SetFont(&D2DA, (wchar_t*)UISYSTEM_FONTNAME_DEFAULT, 12, DWRITE_TEXT_ALIGNMENT_CENTER,  DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    MediumTextForm = D2DA_SetFont(&D2DA, (wchar_t*)UISYSTEM_FONTNAME_DEFAULT, 17, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    pUITbl = (UI**)calloc(nMaxUI, sizeof(UI*));
    nUITblCnt = nMaxUI;
    /*D2D���� �������� ��, �Ǽ���ǥ�踦 ��������� �� �ȼ��� �߽��� �������� �����ؾ��Ѵ�
      ���� ��ǥ�谡 �ƴϹǷ�, �ȼ��� �߽� (0.5 �ȼ��� +) ������ �ƴϸ� ���� �帮��.*/
    D2DA.pRenTarget->SetTransform(D2D1::Matrix3x2F::Translation(0.5f, 0.5f));
    //UIList.Init(nMaxUI);
    BoxPoolStorage.Init(200,50);
    LinePoolStorage.Init(200, 100);
    TextPoolStorage.Init(200, 50);
}
 
/**
    @brief UI ��Ʈ���� �����Ѵ�.
    @param type     ������ UI�� Ÿ�� (��ư, ����ƽ ���)
    @param nID      ������ UI�� ���� ID�� ���� (�� ID�� �ڵ�ó�� ����Ͽ�, Ÿ UI�� ��ſ� Ȱ��)
    @param callback ������ UI���� �ݹ��Լ� ���� (�̺�Ʈ ó�� ��)
    @remark �� �ż��尡 UI�� �����Ҷ��� �� UI���� �����Ǿ��ִ� preInit �ż��带 ���� �⺻ �ʱ�ȭ�� �����Ѵ�
    @n      UI�ý��� �ʱ�ȭ�� �����ߴ� �ִ� UI ������ 200�̶�� nID�� ������ 0-199 �� �Ǿ���Ѵ�.
*/
UI* UISystem::CreateUI(UIType type, unsigned int nID, pfnUIHandler callback)
{
    UI* pUI=0;
    UI_INFO* pInfo;

    if (!pUITbl) return NULL;
    if (nID >= nUITblCnt) return NULL;
    if (pUITbl[nID]) return NULL; /*�̹� �ش� ID�� UI�� ���� �Ǿ��ִ� ���*/
    switch (type) {
    case UIType::eUI_Panel:
        pUI = new UI_Panel;
        pUI->uiType = UIType::eUI_Panel;
        ((UI_Panel*)pUI)->preInit(this, D2DA.pRenTarget, nID, callback);
        break;

    case UIType::eUI_Button:
        pUI = new UI_Button;
        pUI->uiType = UIType::eUI_Button;
        ((UI_Button*)pUI)->preInit(this, D2DA.pRenTarget, nID, callback);
        break;

    case UIType::eUI_DivLine : 
        pUI = new UI_DivisionLine;
        pUI->uiType = UIType::eUI_DivLine;
        ((UI_DivisionLine*)pUI)->preInit(this, D2DA.pRenTarget, nID, callback);
        break;

    case UIType::eUI_Static: 
        pUI = new UI_Static;
        pUI->uiType = UIType::eUI_Static;
        ((UI_Static*)pUI)->preInit(this, D2DA.pRenTarget, nID, callback);
        break;

    case UIType::eUI_List: break;
    case UIType::eUI_Edit: break;
    case UIType::eUI_Scroll: break;
    case UIType::eUI_Progress: break;
    case UIType::eUI_Graph: break;
    default: return NULL;
    }
    pUITbl[nID] = pUI;
    if (!pUI) return NULL;
    /************************************************/
    //pUIID[nID].pUI = pUI;
    return pUI;
}

/**
    @brief Ư�� UI�� �޼��� ������
*/
void UISystem::SendUIMessage(UI* pUI, UINT Message, void* parm)
{
    if (!pUI || !pUI->DefaultHandler) return;
    pUI->DefaultHandler(pUI, Message, parm);
}

/**
    @brief Ư�� UI�� �޼��� ������ (ID������ ������)
*/
void UISystem::SendUIMessageByID(unsigned int nID, UINT Message, void* parm)
{
    UI* pUI;

    pUI = pUITbl[nID];
    if (!pUI || !pUI->DefaultHandler) return;
    pUI->DefaultHandler(pUI, Message, parm);
}

/**
    @brief UI ��ȯó��
*/
void UISystem::ReleaseUI(unsigned int nID)
{
    UI* pUI;

    pUI = pUITbl[nID];
    /*��ȯ �۾� �̱���. delete �� ������*/
    pUITbl[nID] = NULL;
}
