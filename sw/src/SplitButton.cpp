#include "SplitButton.h"

sw::SplitButton::SplitButton()
{
    UpdateButtonStyle(false);
}

bool sw::SplitButton::ShowDropDownMenu()
{
    sw::Rect clientRect = ClientRect;

    return ShowContextMenu(
        PointToScreen({clientRect.width, clientRect.height}),
        HorizontalAlignment::Right, VerticalAlignment::Top);
}

void sw::SplitButton::OnDropDown(NMBCDROPDOWN *pInfo)
{
    SplitButtonDropDownEventArgs args;
    RaiseRoutedEvent(args);

    if (!args.cancel) {
        ShowDropDownMenu();
    }
}

void sw::SplitButton::UpdateButtonStyle(bool focused)
{
    auto basestyle = GetStyle() & ~(BS_PUSHBUTTON | BS_DEFPUSHBUTTON | BS_SPLITBUTTON | BS_DEFSPLITBUTTON);
    SetStyle(focused ? (basestyle | BS_DEFSPLITBUTTON) : (basestyle | BS_SPLITBUTTON));
}

bool sw::SplitButton::OnNotified(NMHDR *pNMHDR, LRESULT &result)
{
    if (pNMHDR->code == BCN_DROPDOWN) {
        OnDropDown(reinterpret_cast<NMBCDROPDOWN *>(pNMHDR));
        result = 0;
        return true;
    } else {
        return Button::OnNotified(pNMHDR, result);
    }
}

bool sw::SplitButton::OnContextMenu(bool isKeyboardMsg, const Point &mousePosition)
{
    if (ContextMenu == nullptr) {
        return false;
    }

    ShowContextMenuEventArgs args(isKeyboardMsg, mousePosition);
    RaiseRoutedEvent(args);

    if (!args.cancel) {
        ShowDropDownMenu();
    }
    return true;
}
