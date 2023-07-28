FoxHop Motion GUI Library.
==========================
## **Summary**

이미지 클릭시 데모 영상으로 이동합니다.

[![Video Label](https://img.youtube.com/vi/z85mqV4YdK0/maxres2.jpg)](https://youtu.be/z85mqV4YdK0)

Direct2D / C++ / WinAPI 기반의 모션 그래픽 UI 라이브러리 입니다.

영화에서나 보던 화려한 UI를 여러분의 프로젝트에 입혀보세요 !

피드백 적극 환영 / 코드 개선 적극 환영 / 같이 참여해주실분 넘모 고마운~

*****

## **프로젝트 구성**
> **foxhop**
>
> 각종 UI들이 정의된 라이브러리입니다.

<br/>

> **foxhop_test**
>
> FoxHop을 사용한 데모 프로그램 예제입니다.
>
> 시작 프로젝트로 지정 후 실행해 보세요.
>
> 네트워크 패킷을 캡쳐하는 예제이므로, Visual Studio를 관리자 권한으로 실행 후 빌드 해 주세요.

*****

## **Initialize & Usage**

WinAPI의 기본적인 코드 구조 : WinMain - WndProc 꼴로 설명합니다. 

UISystem 클래스를 초기화 하고 메인 패널을 만들어야 합니다.

```cpp
#include <ui_system.hpp>

UISystem* uiSys = NULL;

LRESULT CALLBACK WndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    static UI_Panel* pMainPanel = NULL;
    if (pMainPanel) uiSys->SendUIMessage(pMainPanel, Message, wParam, lParam);

    switch (Message) {
    case WM_CREATE:
        uiSys = new UISystem(hWnd);
        pMainPanel = uiSys->InitMainPanel(hWnd, MainPanelProc);
        break;

    case WM_SIZE:
        uiSys->D2DA.pRenTarget->Resize({LOWORD(lParam),HIWORD(lParam)});
        break;

    case WM_MBUTTONDOWN:
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hWnd, Message, wParam, lParam);
}
```

**UISystem**의 내장함수 **InitMainPanel** 을 이용하여 메인패널을 생성하며, 동시에 기본 핸들러를 지정합니다.

지정된 핸들러는 **WndProc**의 상단에 있는

```cpp
    ...
    if (pMainPanel) uiSys->SendUIMessage(pMainPanel, Message, wParam, lParam);
    ...
```
코드에 의해 **WndProc** 기본 처리 루틴보다 먼저 동작하게 됩니다.

여기서는 *MainPanelProc* 라는 이름으로 핸들러를 만들어 사용 하도록 하겠습니다.

```cpp
UI_Button* pButton = NULL;

void MainPanelProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam)
{
    UI_Panel* pPanel = (UI_Panel*)pUI;
    IDWriteTextFormat* pFmtButton;

    switch (Message) {
    case UIM_CREATE:
        pFmtButton = pUI->uiSys->CreateTextFmt((wchar_t*)L"맑은 고딕", 30,
                                               DWRITE_TEXT_ALIGNMENT_CENTER,
                                               DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

        pButton = new UI_Button(pUI->uiSys, TestButtonProc, { 10, 10, 200, 30 },
                                        pFmtButton, (wchar_t*)L"Hello");

        pPanel->RegisterUI(pButton);
        break;
    }
}
```
*(IDWriteTextFormat와 같은 DirectX의 객체는 직접 사용 하지 않도록 라이브러리 수정 예정)*

UI 생성시 **UIM_CREATE** 메세지가 통지되며, 이는 패널도 마찬가지 입니다.

**UI_Panel** 은 자신 안에 다른 UI 객체를 등록하여 렌더링 및 업데이트하며, 메세지 또한 하위 UI에게 전달 합니다.

위 코드는 버튼 객체를 생성 후, 패널 자기 자신에게 등록하는 작업을 수행 합니다.

버튼 생성시, 버튼의 향후 동작을 기술할 사용자 핸들러를 등록 할 수 있으며, 불필요시 **NULL** 을 넣어도 됩니다.

여기서는 *TestButtonProc* 라는 이름으로 사용 하도록 하겠습니다.

```cpp
void TestButtonProc(UI* pUI, UINT Message, WPARAM wParam, LPARAM lParam)
{
    UI_Button* pButton = static_cast<UI_Button*>(pUI);

    switch (Message) {
    case WM_LBUTTONUP:
        ...
        break;
    }
}
```
버튼을 포함한 각 UI들은 윈도우 메세지를 전달받습니다.

버튼의 경우, 클릭 이벤트를 주로 사용 하게 되므로 WM_LBUTTONUP 등을 이용하여

클릭 되었을 때의 기능을 기술할 수 있습니다.

다른 UI의 멤버함수 **pause** 를 호출해 화면에서 잠시 사라지게 하거나 **resume** 으로 나타나게 할 수 있습니다.

화면 전환을 포함해, 다양한 기능을 구현해 보세요.

간단한 사용 설명서는 여기까지 입니다. 

테스트용 데모 소스를 살펴 보시면서 *(코드가 더럽지만)* 추가적인 기능을 둘러보세요 ! UwU

*****
## **UI List**

> ### UI (ui_base.hpp)
> 모든 UI들의 전신이되는 기초 클래스 입니다.
>
> 모든 UI는 이를 상속하게 되며 일종의 인터페이스를 제공합니다.
>
> UI들은 기본적으로 갖고 있는 핸들러 외에, 사용자 프로시저를 등록하여
>
> 추가적인 기능을 구현 할 수 있습니다. (서브클래싱과 결이 같음)

<br/>

> ### UI_Panel (ui_panel.hpp)
> 보이지 않는 투명한 영역 입니다.
>
> 기본적인 마우스 이벤트, 키보드 이벤트, 포커스 등을 내부 핸들러에서 처리 합니다.
>
> UI를 생성하고 UI_Panel에 등록시켜 사용하며, UI_Panel 자기 자신을 등록 할 수도 있습니다.
>
> 이를 통해 화면 레이아웃을 정리 할 수 있습니다. (보완 필요)

<br/>

> ### UI_Button (ui_button.hpp)
>
> 기본적으로 제공되는 버튼 UI 입니다.
> 
> 마우스 클릭, 마우스 오버 등의 처리는 자체적으로 가지는
>
> 기본 핸들러에 의해 처리되며 (버튼 색상 하이라이트 등의 기본 동작)
>
> 버튼이 눌렸을때 어떤 행위를 할 것인지는, 사용자 핸들러에 구현합니다.

<br/>

> ### UI_Static (ui_static.hpp)
>
> 기본적으로 제공되는 텍스트 스태틱 UI 입니다.
>
> 텍스트를 띄워주며, 원할 때 문구를 바꿀 수도 있습니다.

<br/>

> ### UI_TextInput (ui_textinput.hpp)
>
> 기본적으로 제공되는 입력 UI 입니다.
> 
> 마우스 클릭, 키보드 입력, 커서 이동 등의 처리는 자체적으로 가지는
>
> 기본 핸들러에 의해 처리됩니다. (IME 한글 조합 등의 기본 동작)
>
> 텍스트 입력을 받을 수 있으며, 클릭과 드래그 기능도 구현 되어 있지만
>
> 칸의 크기를 넘어서는 텍스트 입력시 스크롤링 하는 기능은 아직 구현되어 있지 않습니다. (Ctrl + C/V 도 !)

<br/>

> ### UI_Table (ui_table.hpp)
>
> 기본적으로 제공되는, 행과 열을 갖는 데이터 표현 UI 입니다.
>
> 흔히 ListView 라고 부르는 그것과 같습니다.

<br/>

> ### UI_LineChart (ui_linechart.hpp)
>
> 기본적으로 제공되는 꺾은선 그래프 UI 입니다.

이어서 작성중...