#include "ButtonBase.h"

sw::ButtonBase::ButtonBase()
{
}

void sw::ButtonBase::InitButtonBase(LPCWSTR lpWindowName, DWORD dwStyle, DWORD dwExStyle)
{
    this->InitControl(L"BUTTON", lpWindowName, dwStyle, dwExStyle);
}

void sw::ButtonBase::OnCommand(int code)
{
    switch (code) {
        case BN_CLICKED:
            this->OnClicked();
            break;

        case BN_DOUBLECLICKED:
            this->OnDoubleClicked();
            break;

        default:
            break;
    }
}

void sw::ButtonBase::OnClicked()
{
    this->RaiseRoutedEvent(ButtonBase_Clicked);
}

void sw::ButtonBase::OnDoubleClicked()
{
    this->RaiseRoutedEvent(ButtonBase_DoubleClicked);
}
