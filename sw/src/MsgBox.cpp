#include "MsgBox.h"

sw::MsgBox::MsgBox(int result)
    : result(result)
{
}

sw::MsgBox sw::MsgBox::Show(const WndBase *parent, const std::wstring &text, const std::wstring &caption, MsgBoxButton button)
{
    HWND hwnd = parent == NULL ? NULL : parent->Handle;
    return MessageBoxW(hwnd, text.c_str(), caption.c_str(), button);
}

sw::MsgBox sw::MsgBox::ShowInfo(const WndBase *parent, const std::wstring &text, const std::wstring &caption, MsgBoxButton button)
{
    HWND hwnd = parent == NULL ? NULL : parent->Handle;
    return MessageBoxW(hwnd, text.c_str(), caption.c_str(), button | MB_ICONINFORMATION);
}

sw::MsgBox sw::MsgBox::ShowError(const WndBase *parent, const std::wstring &text, const std::wstring &caption, MsgBoxButton button)
{
    HWND hwnd = parent == NULL ? NULL : parent->Handle;
    return MessageBoxW(hwnd, text.c_str(), caption.c_str(), button | MB_ICONERROR);
}

sw::MsgBox sw::MsgBox::ShowWarning(const WndBase *parent, const std::wstring &text, const std::wstring &caption, MsgBoxButton button)
{
    HWND hwnd = parent == NULL ? NULL : parent->Handle;
    return MessageBoxW(hwnd, text.c_str(), caption.c_str(), button | MB_ICONWARNING);
}

sw::MsgBox sw::MsgBox::ShowQuestion(const WndBase *parent, const std::wstring &text, const std::wstring &caption, MsgBoxButton button)
{
    HWND hwnd = parent == NULL ? NULL : parent->Handle;
    return MessageBoxW(hwnd, text.c_str(), caption.c_str(), button | MB_ICONQUESTION);
}

const sw::MsgBox &sw::MsgBox::On(int btnId, const MsgBoxCallback &callback) const
{
    if (this->result == btnId && callback)
        callback();
    return *this;
}

const sw::MsgBox &sw::MsgBox::OnOk(const MsgBoxCallback &callback) const
{
    return this->On(IDOK, callback);
}

const sw::MsgBox &sw::MsgBox::OnYes(const MsgBoxCallback &callback) const
{
    return this->On(IDYES, callback);
}

const sw::MsgBox &sw::MsgBox::OnNo(const MsgBoxCallback &callback) const
{
    return this->On(IDNO, callback);
}

const sw::MsgBox &sw::MsgBox::OnCancel(const MsgBoxCallback &callback) const
{
    return this->On(IDCANCEL, callback);
}
