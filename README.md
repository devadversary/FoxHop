FoxHop Motion GUI Library.
==========================
## Summary

이미지 클릭시 데모 영상으로 이동합니다.

[![Video Label](https://img.youtube.com/vi/z85mqV4YdK0/maxres2.jpg)](https://youtu.be/z85mqV4YdK0)

Direct2D / C++ / WinAPI 기반의 모션 그래픽 UI 라이브러리 입니다.

영화에서나 보던 화려한 UI를 여러분의 프로젝트에 입혀보세요 !

피드백 적극 환영 / 코드 개선 적극 환영 / 같이 참여해주실분 넘모 고마운~

*****
## Initialize & Usage
UISystem 클래스를 초기화 하고 메인 패널을 만들어야 합니다.

```cpp
#include <ui_system.hpp>

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
이어서 작성중...

*****
## UI List

> ### UI (ui_base.hpp)
> 모든 UI들의 전신이되는 기초 클래스 입니다.
>
> 모든 UI는 이를 상속하게 되며 일종의 인터페이스를 제공합니다.
>
> UI들은 기본적으로 갖고 있는 핸들러 외에, 사용자 프로시저를 등록하여
>
> 추가적인 기능을 구현 할 수 있습니다. (서브클래싱과 결이 같음)

> ### UI_Button (ui_button.hpp)
>
> 기본적으로 제공되는 버튼 UI 입니다.
> 
> 마우스 클릭, 마우스 오버 등의 처리는 자체적으로 가지는
>
> 기본 핸들러에 의해 처리되며 (버튼 색상 하이라이트 등의 기본 동작)
>
> 버튼이 눌렸을때 어떤 행위를 할 것인지는, 사용자 핸들러에 구현합니다.

이어서 작성중...