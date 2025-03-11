#include "Button.h"

static constexpr DWORD _ButtonStyle_Default = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | BS_NOTIFY | BS_PUSHBUTTON;
static constexpr DWORD _ButtonStyle_Focused = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | BS_NOTIFY | BS_DEFPUSHBUTTON;

sw::Button::Button()
{
    this->InitButtonBase(L"Button", _ButtonStyle_Default, 0);
    this->Rect = sw::Rect(0, 0, 70, 30);
}

void sw::Button::OnDrawFocusRect(HDC hdc)
{
    HWND hwnd = this->Handle;

    RECT rect;
    GetClientRect(hwnd, &rect);

    rect.left += 3;
    rect.top += 3;
    rect.right -= 3;
    rect.bottom -= 3;

    DrawFocusRect(hdc, &rect);
}

bool sw::Button::OnSetFocus(HWND hPreFocus)
{
    this->SetStyle(_ButtonStyle_Focused);
    this->Redraw();
    return this->ButtonBase::OnSetFocus(hPreFocus);
}

bool sw::Button::OnKillFocus(HWND hNextFocus)
{
    this->SetStyle(_ButtonStyle_Default);
    this->Redraw();
    return this->ButtonBase::OnKillFocus(hNextFocus);
}

bool sw::Button::OnKeyDown(VirtualKey key, KeyFlags flags)
{
    bool result = this->UIElement::OnKeyDown(key, flags);

    if (key == VirtualKey::Enter) {
        this->OnClicked();
    }

    return result;
}
