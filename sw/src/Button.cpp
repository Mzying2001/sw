#include "Button.h"

static constexpr DWORD _ButtonStyle_Default = WS_CHILD | WS_VISIBLE | BS_NOTIFY | BS_PUSHBUTTON;
static constexpr DWORD _ButtonStyle_Focused = WS_CHILD | WS_VISIBLE | BS_NOTIFY | BS_DEFPUSHBUTTON;

sw::Button::Button()
{
    this->InitButtonBase(L"Button", _ButtonStyle_Default, 0);
    this->Rect = sw::Rect(0, 0, 70, 30);
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
