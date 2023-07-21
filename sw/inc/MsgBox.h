#pragma once

#include "WndBase.h"
#include <Windows.h>
#include <functional>
#include <string>

namespace sw
{
    /**
     * @brief 消息框回调
     */
    typedef std::function<void()> MsgBoxCallback;

    /**
     * @brief 消息框按钮类型
     */
    enum class MsgBoxButton {
        Ok          = MB_OK,
        OkCancel    = MB_OKCANCEL,
        YesNoCancel = MB_YESNOCANCEL,
        YesNo       = MB_YESNO,
    };

    /**
     * @brief 消息框结果
     */
    enum class MsgBoxResult {
        Ok     = IDOK,
        Cancel = IDCANCEL,
        Yes    = IDYES,
        No     = IDNO,
    };

    /**
     * @brief 消息框类
     */
    class MsgBox
    {
    public:
        MsgBoxResult result;

    private:
        MsgBox() = delete;
        MsgBox(MsgBoxResult result);

    public:
        static MsgBox Show(const WndBase *owner, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::Ok);
        static MsgBox Show(const WndBase &owner, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::Ok);

        static MsgBox ShowInfo(const WndBase *owner, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::Ok);
        static MsgBox ShowInfo(const WndBase &owner, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::Ok);

        static MsgBox ShowError(const WndBase *owner, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::Ok);
        static MsgBox ShowError(const WndBase &owner, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::Ok);

        static MsgBox ShowWarning(const WndBase *owner, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::Ok);
        static MsgBox ShowWarning(const WndBase &owner, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::Ok);

        static MsgBox ShowQuestion(const WndBase *owner, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::YesNo);
        static MsgBox ShowQuestion(const WndBase &owner, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::YesNo);

    private:
        const MsgBox &On(MsgBoxResult result, const MsgBoxCallback &callback) const;

    public:
        const MsgBox &OnOk(const MsgBoxCallback &callback) const;
        const MsgBox &OnYes(const MsgBoxCallback &callback) const;
        const MsgBox &OnNo(const MsgBoxCallback &callback) const;
        const MsgBox &OnCancel(const MsgBoxCallback &callback) const;
    };
}
