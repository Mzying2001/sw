#pragma once

#include "WndBase.h"
#include <Windows.h>
#include <functional>
#include <string>

namespace sw
{
    typedef std::function<void()> MsgBoxCallback;

    class MsgBox
    {
    public:
        enum MsgBoxButton {
            Ok          = MB_OK,
            YesNo       = MB_YESNO,
            OkCancel    = MB_OKCANCEL,
            YesNoCancel = MB_YESNOCANCEL,
        };

    public:
        int result = -1;

    private:
        MsgBox();
        MsgBox(int result);

    public:
        static MsgBox Show(const WndBase *parent, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = Ok);
        static MsgBox ShowInfo(const WndBase *parent, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = Ok);
        static MsgBox ShowError(const WndBase *parent, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = Ok);
        static MsgBox ShowWarning(const WndBase *parent, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = Ok);
        static MsgBox ShowQuestion(const WndBase *parent, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = YesNo);

    private:
        const MsgBox &On(int btnId, const MsgBoxCallback &callback) const;

    public:
        const MsgBox &OnOk(const MsgBoxCallback &callback) const;
        const MsgBox &OnYes(const MsgBoxCallback &callback) const;
        const MsgBox &OnNo(const MsgBoxCallback &callback) const;
        const MsgBox &OnCancel(const MsgBoxCallback &callback) const;
    };
}
