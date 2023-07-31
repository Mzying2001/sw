#include "ButtonBase.h"

sw::ButtonBase::ButtonBase()
{
}

void sw::ButtonBase::InitButtonBase(LPCWSTR lpWindowName, DWORD dwStyle)
{
    this->InitControl(L"BUTTON", lpWindowName, dwStyle);
}

void sw::ButtonBase::OnCommand(int code)
{
    switch (code) {
        case BN_CLICKED:
            this->RaiseRoutedEvent(ButtonBase_Clicked);
            break;

        case BN_DOUBLECLICKED:
            this->RaiseRoutedEvent(ButtonBase_DoubleClicked);
            break;

        default:
            break;
    }
}
