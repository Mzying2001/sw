#include "Button.h"

sw::Button::Button()
{
    this->InitButtonBase(L"Button", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | BS_NOTIFY | BS_PUSHBUTTON, 0);
    this->Rect = sw::Rect(0, 0, 70, 30);
}

void sw::Button::UpdateButtonStyle(bool focused)
{
    this->SetStyle(BS_DEFPUSHBUTTON, focused); // BS_PUSHBUTTON == 0
}

void sw::Button::OnDrawFocusRect(HDC hdc)
{
    HWND hwnd = this->Handle;

    RECT rect;
    GetClientRect(hwnd, &rect);
    InflateRect(&rect, -3, -3);
    DrawFocusRect(hdc, &rect);
}

bool sw::Button::OnSetFocus(HWND hPreFocus)
{
    this->UpdateButtonStyle(true);
    this->Redraw();
    return this->ButtonBase::OnSetFocus(hPreFocus);
}

bool sw::Button::OnKillFocus(HWND hNextFocus)
{
    this->UpdateButtonStyle(false);
    this->Redraw();
    return this->ButtonBase::OnKillFocus(hNextFocus);
}

bool sw::Button::OnKeyDown(VirtualKey key, const KeyFlags &flags)
{
    bool result = this->UIElement::OnKeyDown(key, flags);

    if (!result && key == VirtualKey::Enter) {
        this->OnClicked();
    }
    return result;
}
