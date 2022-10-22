#include "./include/ui_system.hpp"

#include "./include/ui_panel.hpp"
#include "./include/ui_button.hpp"
#include "./include/ui_fraggedline.hpp"
#include "./include/ui_static.hpp"

/**
    @brief UI시스템을 초기화 한다.
    @param hWnd UI 시스템을 사용할 메인윈도우 핸들
    @param nMaxUI UI생성 한계치 갯수 지정 (0 지정시 기본값 256개)
    @remark 내부적으로 모션오브젝트 갯수를 각각 5000~10000개정도만 생성하기 때문에
    @n      nMaxUI 로 UI갯수 상한을 늘릴경우, 모션오브젝트의 갯수 상한에 걸릴 수 있음.
    @n      추후에 UI갯수 상한에 따른 적절한 모션오브젝트 갯수를 지정해주어야 함.
*/
UISystem::UISystem(HWND hWnd)
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
    TinyTextForm   = D2DA_SetFont(&D2DA, (wchar_t*)UISYSTEM_FONTNAME_DEFAULT, 7.9f, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    SmallTextForm  = D2DA_SetFont(&D2DA, (wchar_t*)UISYSTEM_FONTNAME_DEFAULT, 9.f, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    ButtonTextForm = D2DA_SetFont(&D2DA, (wchar_t*)UISYSTEM_FONTNAME_DEFAULT, 12, DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    MediumTextForm = D2DA_SetFont(&D2DA, (wchar_t*)UISYSTEM_FONTNAME_DEFAULT, 17, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    /*D2D에서 렌더링될 때, 실수좌표계를 사용함으로 각 픽셀의 중심을 기준으로 렌더해야한다
      정수 좌표계가 아니므로, 픽셀의 중심 (0.5 픽셀씩 +) 기준이 아니면 상이 흐리다.*/
    D2DA.pRenTarget->SetTransform(D2D1::Matrix3x2F::Translation(0.5f, 0.5f));

    /*2022.06.04 - TODO : 오브젝트 리스트를 UISystem 에서 생성/관리 하지 않고
      각자 UI마다 스스로 필요한 만큼만 만들어서 사용하도록 변경 예정*/
    ObjPoolBox.Init(5000, 0);
    ObjPoolLine.Init(5000, 0);
    ObjPoolText.Init(10000, 0);

    /*각 UI 팩토리 초기화 (UI_Panel 팩토리는 삭제됨)*/
    pUIButtonFactory = new UI_ButtonFactory(this, D2DA.pRenTarget);
}

UISystem::~UISystem() {}

#if 0
/**
    @brief WndProc 에서 UISystem으로 메세지 라우팅
    @remark WndProc으로 전달되는 WINAPI 본연의 메세지를 전달한다.
*/
void UISystem::MainPanelProc(UINT Message, WPARAM wParam, LPARAM lParam)
{
    switch (Message) {
    case WM_MOUSEMOVE:
        //SendUIMessage(pMainPanel, UIM_MOUSEMOVE, );
        break;
    }
    return;
}
#endif
/**
    @brief 특정 UI에 메세지 보내기
*/
BOOL UISystem::SendUIMessage(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam)
{
    if (!pUI || !pUI->DefaultHandler) return FALSE;
    pUI->DefaultHandler(pUI, Message, wParam, lParam);
    return TRUE;
}

/**
    @brief UI 반환처리 (미구현)
*/
void UISystem::ReleaseUI()
{

}
