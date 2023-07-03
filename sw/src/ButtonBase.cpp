#include "ButtonBase.h"

sw::ButtonBase::ButtonBase()
{
}

void sw::ButtonBase::InitButtonBase(LPCWSTR lpWindowName, DWORD dwStyle)
{
    this->InitControl(L"BUTTON", lpWindowName, dwStyle);
}

void sw::ButtonBase::ParentReceivedCommand(int code)
{
    // TODO
}
