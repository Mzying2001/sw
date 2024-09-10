#include "MsgBox.h"
#include "Window.h"

sw::MsgBoxResultHelper::MsgBoxResultHelper(MsgBoxResult result)
    : result(result)
{
}

sw::MsgBoxResultHelper::operator sw::MsgBoxResult() const
{
    return this->result;
}

sw::MsgBoxResultHelper &sw::MsgBoxResultHelper::OnOk(const MsgBoxCallback &callback)
{
    return this->On<MsgBoxResult::Ok>(callback);
}

sw::MsgBoxResultHelper &sw::MsgBoxResultHelper::OnYes(const MsgBoxCallback &callback)
{
    return this->On<MsgBoxResult::Yes>(callback);
}

sw::MsgBoxResultHelper &sw::MsgBoxResultHelper::OnNo(const MsgBoxCallback &callback)
{
    return this->On<MsgBoxResult::No>(callback);
}

sw::MsgBoxResultHelper &sw::MsgBoxResultHelper::OnCancel(const MsgBoxCallback &callback)
{
    return this->On<MsgBoxResult::Cancel>(callback);
}

sw::MsgBoxResultHelper sw::MsgBox::Show(const WndBase *owner, const std::wstring &text, const std::wstring &caption, MsgBoxButton button)
{
    HWND hwnd = (owner == nullptr) ? reinterpret_cast<HWND>(NULL) : owner->Handle;
    return (MsgBoxResult)MessageBoxW(hwnd, text.c_str(), caption.c_str(), (UINT)button);
}

sw::MsgBoxResultHelper sw::MsgBox::Show(const WndBase &owner, const std::wstring &text, const std::wstring &caption, MsgBoxButton button)
{
    return MsgBox::Show(&owner, text, caption, button);
}

sw::MsgBoxResultHelper sw::MsgBox::Show(const std::wstring &text, const std::wstring &caption, MsgBoxButton button)
{
    return MsgBox::Show(Window::ActiveWindow, text, caption, button);
}

sw::MsgBoxResultHelper sw::MsgBox::ShowInfo(const WndBase *owner, const std::wstring &text, const std::wstring &caption, MsgBoxButton button)
{
    HWND hwnd = (owner == nullptr) ? reinterpret_cast<HWND>(NULL) : owner->Handle;
    return (MsgBoxResult)MessageBoxW(hwnd, text.c_str(), caption.c_str(), (UINT)button | MB_ICONINFORMATION);
}

sw::MsgBoxResultHelper sw::MsgBox::ShowInfo(const WndBase &owner, const std::wstring &text, const std::wstring &caption, MsgBoxButton button)
{
    return MsgBox::ShowInfo(&owner, text, caption, button);
}

sw::MsgBoxResultHelper sw::MsgBox::ShowInfo(const std::wstring &text, const std::wstring &caption, MsgBoxButton button)
{
    return MsgBox::ShowInfo(Window::ActiveWindow, text, caption, button);
}

sw::MsgBoxResultHelper sw::MsgBox::ShowError(const WndBase *owner, const std::wstring &text, const std::wstring &caption, MsgBoxButton button)
{
    HWND hwnd = (owner == nullptr) ? reinterpret_cast<HWND>(NULL) : owner->Handle;
    return (MsgBoxResult)MessageBoxW(hwnd, text.c_str(), caption.c_str(), (UINT)button | MB_ICONERROR);
}

sw::MsgBoxResultHelper sw::MsgBox::ShowError(const WndBase &owner, const std::wstring &text, const std::wstring &caption, MsgBoxButton button)
{
    return MsgBox::ShowError(&owner, text, caption, button);
}

sw::MsgBoxResultHelper sw::MsgBox::ShowError(const std::wstring &text, const std::wstring &caption, MsgBoxButton button)
{
    return MsgBox::ShowError(Window::ActiveWindow, text, caption, button);
}

sw::MsgBoxResultHelper sw::MsgBox::ShowWarning(const WndBase *owner, const std::wstring &text, const std::wstring &caption, MsgBoxButton button)
{
    HWND hwnd = (owner == nullptr) ? reinterpret_cast<HWND>(NULL) : owner->Handle;
    return (MsgBoxResult)MessageBoxW(hwnd, text.c_str(), caption.c_str(), (UINT)button | MB_ICONWARNING);
}

sw::MsgBoxResultHelper sw::MsgBox::ShowWarning(const WndBase &owner, const std::wstring &text, const std::wstring &caption, MsgBoxButton button)
{
    return MsgBox::ShowWarning(&owner, text, caption, button);
}

sw::MsgBoxResultHelper sw::MsgBox::ShowWarning(const std::wstring &text, const std::wstring &caption, MsgBoxButton button)
{
    return MsgBox::ShowWarning(Window::ActiveWindow, text, caption, button);
}

sw::MsgBoxResultHelper sw::MsgBox::ShowQuestion(const WndBase *owner, const std::wstring &text, const std::wstring &caption, MsgBoxButton button)
{
    HWND hwnd = (owner == nullptr) ? reinterpret_cast<HWND>(NULL) : owner->Handle;
    return (MsgBoxResult)MessageBoxW(hwnd, text.c_str(), caption.c_str(), (UINT)button | MB_ICONQUESTION);
}

sw::MsgBoxResultHelper sw::MsgBox::ShowQuestion(const WndBase &owner, const std::wstring &text, const std::wstring &caption, MsgBoxButton button)
{
    return MsgBox::ShowQuestion(&owner, text, caption, button);
}

sw::MsgBoxResultHelper sw::MsgBox::ShowQuestion(const std::wstring &text, const std::wstring &caption, MsgBoxButton button)
{
    return MsgBox::ShowQuestion(Window::ActiveWindow, text, caption, button);
}
