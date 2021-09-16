#include "./include/ui_system.hpp"

UISystem::UISystem()
{
    D2DA = { 0, };
    pUITbl = NULL;
    ButtonTextForm = NULL;
}

UISystem::~UISystem() {}

/**
    @brief UI시스템을 초기화 한다.
*/
void UISystem::Init(HWND hWnd, unsigned int nMaxUI)
{
    WCHAR szProgramPath[MAX_PATH];
    WCHAR szFontPath[MAX_PATH];
    WCHAR szDir[MAX_PATH];
    WCHAR szDrive[8];

    /*프로그램과 같은 경로에 있는 특정 폰트를 로드한다.*/
    GetModuleFileNameW(NULL, szProgramPath, MAX_PATH);
    _wsplitpath_s(szProgramPath, szDrive, 8, szDir, MAX_PATH, NULL, 0, NULL, 0);
    wsprintfW(szFontPath, L"%s%s%s", szDrive, szDir, L"monoMMM_5.ttf");
    AddFontResourceW(szFontPath);
    /*D2D 환경 초기화 및 UI시스템 초기화*/
    D2DA_Init(&D2DA, hWnd); /*Direct X 개체들과 렌더타겟, 팩토리 등 초기화*/
    TinyTextForm = D2DA_SetFont(&D2DA, (wchar_t*)UISYSTEM_FONTNAME_DEFAULT, 7.9f, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    SmallTextForm = D2DA_SetFont(&D2DA, (wchar_t*)UISYSTEM_FONTNAME_DEFAULT, 9.f, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    ButtonTextForm = D2DA_SetFont(&D2DA, (wchar_t*)UISYSTEM_FONTNAME_DEFAULT, 12, DWRITE_TEXT_ALIGNMENT_CENTER,  DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    MediumTextForm = D2DA_SetFont(&D2DA, (wchar_t*)UISYSTEM_FONTNAME_DEFAULT, 17, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    pUITbl = (UI**)calloc(nMaxUI, sizeof(UI*));
    nUITblCnt = nMaxUI;
    /*D2D에서 렌더링될 때, 실수좌표계를 사용함으로 각 픽셀의 중심을 기준으로 렌더해야한다
      정수 좌표계가 아니므로, 픽셀의 중심 (0.5 픽셀씩 +) 기준이 아니면 상이 흐리다.*/
    D2DA.pRenTarget->SetTransform(D2D1::Matrix3x2F::Translation(0.5f, 0.5f));
    //UIList.Init(nMaxUI);
    BoxPoolStorage.Init(200,50);
    LinePoolStorage.Init(200, 100);
    TextPoolStorage.Init(200, 50);
}
 
/**
    @brief UI 컨트롤을 생성한다.
    @param type     생성할 UI의 타입 (버튼, 스태틱 등등)
    @param nID      생성될 UI의 고유 ID를 지정 (이 ID는 핸들처럼 사용하여, 타 UI와 통신에 활용)
    @param callback 생성된 UI만의 콜백함수 지정 (이벤트 처리 등)
    @remark 이 매서드가 UI를 생성할때는 각 UI마다 구현되어있는 preInit 매서드를 통해 기본 초기화를 진행한다
    @n      UI시스템 초기화시 지정했던 최대 UI 갯수가 200이라면 nID의 범위는 0-199 가 되어야한다.
*/
UI* UISystem::CreateUI(UIType type, unsigned int nID, pfnUIHandler callback)
{
    UI* pUI=0;
    UI_INFO* pInfo;

    if (!pUITbl) return NULL;
    if (nID >= nUITblCnt) return NULL;
    if (pUITbl[nID]) return NULL; /*이미 해당 ID에 UI가 매핑 되어있는 경우*/
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
    @brief 특정 UI에 메세지 보내기
*/
void UISystem::SendUIMessage(UI* pUI, UINT Message, void* parm)
{
    if (!pUI || !pUI->DefaultHandler) return;
    pUI->DefaultHandler(pUI, Message, parm);
}

/**
    @brief 특정 UI에 메세지 보내기 (ID값으로 보내기)
*/
void UISystem::SendUIMessageByID(unsigned int nID, UINT Message, void* parm)
{
    UI* pUI;

    pUI = pUITbl[nID];
    if (!pUI || !pUI->DefaultHandler) return;
    pUI->DefaultHandler(pUI, Message, parm);
}

/**
    @brief UI 반환처리
*/
void UISystem::ReleaseUI(unsigned int nID)
{
    UI* pUI;

    pUI = pUITbl[nID];
    /*반환 작업 미구현. delete 꼭 해주자*/
    pUITbl[nID] = NULL;
}
