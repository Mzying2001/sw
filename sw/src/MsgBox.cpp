#include "MsgBox.h"

sw::MsgBox::MsgBox(MsgBoxResult result)
    : result(result)
{
}

sw::MsgBox sw::MsgBox::Show(const WndBase *owner, const std::wstring &text, const std::wstring &caption, MsgBoxButton button)
{
    HWND hwnd = owner == NULL ? NULL : owner->Handle;
    return (MsgBoxResult)MessageBoxW(hwnd, text.c_str(), caption.c_str(), (UINT)button);
}

sw::MsgBox sw::MsgBox::ShowInfo(const WndBase *owner, const std::wstring &text, const std::wstring &caption, MsgBoxButton button)
{
    HWND hwnd = owner == NULL ? NULL : owner->Handle;
    return (MsgBoxResult)MessageBoxW(hwnd, text.c_str(), caption.c_str(), (UINT)button | MB_ICONINFORMATION);
}

sw::MsgBox sw::MsgBox::ShowError(const WndBase *owner, const std::wstring &text, const std::wstring &caption, MsgBoxButton button)
{
    HWND hwnd = owner == NULL ? NULL : owner->Handle;
    return (MsgBoxResult)MessageBoxW(hwnd, text.c_str(), caption.c_str(), (UINT)button | MB_ICONERROR);
}

sw::MsgBox sw::MsgBox::ShowWarning(const WndBase *owner, const std::wstring &text, const std::wstring &caption, MsgBoxButton button)
{
    HWND hwnd = owner == NULL ? NULL : owner->Handle;
    return (MsgBoxResult)MessageBoxW(hwnd, text.c_str(), caption.c_str(), (UINT)button | MB_ICONWARNING);
}

sw::MsgBox sw::MsgBox::ShowQuestion(const WndBase *owner, const std::wstring &text, const std::wstring &caption, MsgBoxButton button)
{
    HWND hwnd = owner == NULL ? NULL : owner->Handle;
    return (MsgBoxResult)MessageBoxW(hwnd, text.c_str(), caption.c_str(), (UINT)button | MB_ICONQUESTION);
}

const sw::MsgBox &sw::MsgBox::On(MsgBoxResult result, const MsgBoxCallback &callback) const
{
    if (this->result == result && callback)
        callback();
    return *this;
}

const sw::MsgBox &sw::MsgBox::OnOk(const MsgBoxCallback &callback) const
{
    return this->On(MsgBoxResult::Ok, callback);
}

const sw::MsgBox &sw::MsgBox::OnYes(const MsgBoxCallback &callback) const
{
    return this->On(MsgBoxResult::Yes, callback);
}

const sw::MsgBox &sw::MsgBox::OnNo(const MsgBoxCallback &callback) const
{
    return this->On(MsgBoxResult::No, callback);
}

const sw::MsgBox &sw::MsgBox::OnCancel(const MsgBoxCallback &callback) const
{
    return this->On(MsgBoxResult::Cancel, callback);
}
