#include "Panel.h"

sw::Panel::Panel()
{
    // STATIC控件默认没有响应滚动条操作，故使用BUTTON
    this->InitControl(L"BUTTON", NULL, WS_CHILD | WS_VISIBLE, WS_EX_NOACTIVATE);
    this->HorizontalAlignment = HorizontalAlignment::Stretch;
    this->VerticalAlignment   = VerticalAlignment::Stretch;
}

bool sw::Panel::OnPaint()
{
    PAINTSTRUCT ps;
    HWND hwnd = this->Handle;
    HDC hdc   = BeginPaint(hwnd, &ps);

    RECT clientRect;
    GetClientRect(hwnd, &clientRect);

    HBRUSH hBrush = CreateSolidBrush(this->BackColor.Get());
    FillRect(hdc, &clientRect, hBrush);

    DeleteObject(hBrush);
    EndPaint(hwnd, &ps);
    return true;
}

bool sw::Panel::OnKeyDown(VirtualKey key, KeyFlags flags)
{
    this->UIElement::OnKeyDown(key, flags);
    return true;
}

bool sw::Panel::OnKeyUp(VirtualKey key, KeyFlags flags)
{
    this->UIElement::OnKeyUp(key, flags);
    return true;
}

bool sw::Panel::OnMouseMove(Point mousePosition, MouseKey keyState)
{
    this->UIElement::OnMouseMove(mousePosition, keyState);
    return true;
}

bool sw::Panel::OnMouseLeave()
{
    this->UIElement::OnMouseLeave();
    return true;
}

bool sw::Panel::OnMouseLeftButtonDown(Point mousePosition, MouseKey keyState)
{
    this->UIElement::OnMouseLeftButtonDown(mousePosition, keyState);
    return true;
}

bool sw::Panel::OnMouseLeftButtonUp(Point mousePosition, MouseKey keyState)
{
    this->UIElement::OnMouseLeftButtonUp(mousePosition, keyState);
    return true;
}

bool sw::Panel::OnMouseLeftButtonDoubleClick(Point mousePosition, MouseKey keyState)
{
    /*this->WndBase::OnMouseLeftButtonDoubleClick(mousePosition, keyState);*/
    return true;
}
