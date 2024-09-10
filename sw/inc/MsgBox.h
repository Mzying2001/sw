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
    using MsgBoxCallback = std::function<void()>;

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
     * @brief 处理消息框消息的帮助类
     */
    struct MsgBoxResultHelper {
        /**
         * @brief 消息框的结果
         */
        MsgBoxResult result;

        /**
         * @brief 构造MsgBoxResultHelper
         */
        MsgBoxResultHelper(MsgBoxResult result);

        /**
         * @brief 隐式转换MsgBoxResult
         */
        operator sw::MsgBoxResult() const;

        /**
         * @brief 指定按下“确定”按钮时的处理函数
         */
        MsgBoxResultHelper &OnOk(const MsgBoxCallback &callback);

        /**
         * @brief 指定按下“是”按钮时的处理函数
         */
        MsgBoxResultHelper &OnYes(const MsgBoxCallback &callback);

        /**
         * @brief 指定按下“否”按钮时的处理函数
         */
        MsgBoxResultHelper &OnNo(const MsgBoxCallback &callback);

        /**
         * @brief 指定按下“取消”按钮时的处理函数
         */
        MsgBoxResultHelper &OnCancel(const MsgBoxCallback &callback);

        /**
         * @brief 指定消息框结果的处理函数
         */
        template <MsgBoxResult RES>
        MsgBoxResultHelper &On(const MsgBoxCallback &callback)
        {
            if (this->result == RES && callback) {
                callback();
            }
            return *this;
        }
    };

    /**
     * @brief 消息框类
     */
    class MsgBox
    {
    private:
        MsgBox() = delete;

    public:
        /**
         * @brief 显示一个不含图标的消息框
         */
        static MsgBoxResultHelper Show(const WndBase *owner, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::Ok);

        /**
         * @brief 显示一个不含图标的消息框
         */
        static MsgBoxResultHelper Show(const WndBase &owner, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::Ok);

        /**
         * @brief 显示一个不含图标的消息框，将当前活动窗体作为Owner
         */
        static MsgBoxResultHelper Show(const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::Ok);

        /**
         * @brief 显示一个含“消息”图标的消息框
         */
        static MsgBoxResultHelper ShowInfo(const WndBase *owner, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::Ok);

        /**
         * @brief 显示一个含“消息”图标的消息框
         */
        static MsgBoxResultHelper ShowInfo(const WndBase &owner, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::Ok);

        /**
         * @brief 显示一个含“消息”图标的消息框，将当前活动窗体作为Owner
         */
        static MsgBoxResultHelper ShowInfo(const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::Ok);

        /**
         * @brief 显示一个含“错误”图标的消息框
         */
        static MsgBoxResultHelper ShowError(const WndBase *owner, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::Ok);

        /**
         * @brief 显示一个含“错误”图标的消息框
         */
        static MsgBoxResultHelper ShowError(const WndBase &owner, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::Ok);

        /**
         * @brief 显示一个含“错误”图标的消息框，将当前活动窗体作为Owner
         */
        static MsgBoxResultHelper ShowError(const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::Ok);

        /**
         * @brief 显示一个含“警告”图标的消息框
         */
        static MsgBoxResultHelper ShowWarning(const WndBase *owner, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::Ok);

        /**
         * @brief 显示一个含“警告”图标的消息框
         */
        static MsgBoxResultHelper ShowWarning(const WndBase &owner, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::Ok);

        /**
         * @brief 显示一个含“警告”图标的消息框，将当前活动窗体作为Owner
         */
        static MsgBoxResultHelper ShowWarning(const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::Ok);

        /**
         * @brief 显示一个含“问题”图标的消息框
         */
        static MsgBoxResultHelper ShowQuestion(const WndBase *owner, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::YesNo);

        /**
         * @brief 显示一个含“问题”图标的消息框
         */
        static MsgBoxResultHelper ShowQuestion(const WndBase &owner, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::YesNo);

        /**
         * @brief 显示一个含“问题”图标的消息框，将当前活动窗体作为Owner
         */
        static MsgBoxResultHelper ShowQuestion(const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::YesNo);
    };
}
