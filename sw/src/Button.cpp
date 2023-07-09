#include "Button.h"

#define BUTTONSTYLE_DEFAULT (WS_CHILD | WS_VISIBLE | BS_NOTIFY | BS_PUSHBUTTON)
#define BUTTONSTYLE_FOCUSED (WS_CHILD | WS_VISIBLE | BS_NOTIFY | BS_DEFPUSHBUTTON)

sw::Button::Button()
{
    this->InitButtonBase(L"Button", BUTTONSTYLE_DEFAULT);
    this->Rect = sw::Rect(0, 0, 70, 30);
}

bool sw::Button::OnSetFocus(HWND hPreFocus)
{
    this->SetStyle(BUTTONSTYLE_FOCUSED);
    this->Redraw();
    return this->ButtonBase::OnSetFocus(hPreFocus);
}

bool sw::Button::OnKillFocus(HWND hNextFocus)
{
    this->SetStyle(BUTTONSTYLE_DEFAULT);
    this->Redraw();
    return this->ButtonBase::OnKillFocus(hNextFocus);
}
