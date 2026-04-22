#pragma once

#include "Delegate.h"
#include "WndBase.h"
#include <string>
#include <windows.h>

namespace sw
{
    /**
     * @brief 消息框按钮类型
     */
    enum class MsgBoxButton {
        AbortRetryIgnore    = MB_ABORTRETRYIGNORE,  ///< “中止”、“重试”、“忽略” 三按钮
        CancelRetryContinue = MB_CANCELTRYCONTINUE, ///< “取消”、“重试”、“继续” 三按钮
        Ok                  = MB_OK,                ///< “确定” 单按钮（默认）
        OkCancel            = MB_OKCANCEL,          ///< “确定”、“取消” 双按钮
        RetryCancel         = MB_RETRYCANCEL,       ///< “重试”、“取消” 双按钮
        YesNo               = MB_YESNO,             ///< “是”、“否” 双按钮
        YesNoCancel         = MB_YESNOCANCEL,       ///< “是”、“否”、“取消” 三按钮
    };

    /**
     * @brief 消息框图标类型
     */
    enum class MsgBoxIcon {
        None        = 0,                  ///< 不显示图标
        Warning     = MB_ICONWARNING,     ///< 警告图标（黄色三角形）
        Information = MB_ICONINFORMATION, ///< 信息图标（蓝色字母 i）
        Question    = MB_ICONQUESTION,    ///< 问号图标（微软已不建议使用，推荐改用 Warning）
        Error       = MB_ICONERROR,       ///< 错误图标（红色叉号）
    };

    /**
     * @brief 消息框结果
     */
    enum class MsgBoxResult {
        Abort    = IDABORT,    ///< 用户点击了 “中止” 按钮
        Cancel   = IDCANCEL,   ///< 用户点击了 “取消” 按钮，或按 Esc / 关闭按钮
        Continue = IDCONTINUE, ///< 用户点击了 “继续” 按钮
        Ignore   = IDIGNORE,   ///< 用户点击了 “忽略” 按钮
        No       = IDNO,       ///< 用户点击了 “否” 按钮
        Ok       = IDOK,       ///< 用户点击了 “确定” 按钮
        Retry    = IDRETRY,    ///< 用户点击了 “重试” 按钮
        TryAgain = IDTRYAGAIN, ///< 用户点击了 “重试” 按钮（仅 CancelRetryContinue 组合下产生）
        Yes      = IDYES,      ///< 用户点击了 “是” 按钮
    };

    /**
     * @brief 处理消息框消息的辅助类
     */
    class MsgBoxResultHelper
    {
    private:
        /**
         * @brief 消息框的结果
         */
        MsgBoxResult _result;

    public:
        /**
         * @brief 消息框的结果
         */
        ReadOnlyProperty<MsgBoxResult> Result;

    public:
        /**
         * @brief 构造MsgBoxResultHelper
         */
        MsgBoxResultHelper(MsgBoxResult result);

        /**
         * @brief 隐式转换MsgBoxResult
         */
        operator sw::MsgBoxResult() const noexcept;

        /**
         * @brief 指定按下“确定”按钮时的处理函数
         */
        MsgBoxResultHelper &OnOk(const Action<> &callback);

        /**
         * @brief 指定按下“是”按钮时的处理函数
         */
        MsgBoxResultHelper &OnYes(const Action<> &callback);

        /**
         * @brief 指定按下“否”按钮时的处理函数
         */
        MsgBoxResultHelper &OnNo(const Action<> &callback);

        /**
         * @brief 指定按下“取消”按钮时的处理函数
         */
        MsgBoxResultHelper &OnCancel(const Action<> &callback);

        /**
         * @brief 指定按下“中止”按钮时的处理函数
         */
        MsgBoxResultHelper &OnAbort(const Action<> &callback);

        /**
         * @brief 指定按下“继续”按钮时的处理函数
         */
        MsgBoxResultHelper &OnContinue(const Action<> &callback);

        /**
         * @brief 指定按下“忽略”按钮时的处理函数
         */
        MsgBoxResultHelper &OnIgnore(const Action<> &callback);

        /**
         * @brief 指定按下“重试”按钮时的处理函数
         */
        MsgBoxResultHelper &OnRetry(const Action<> &callback);

        /**
         * @brief 指定按下“重试”按钮时的处理函数
         * @note 只有在按钮类型为CancelRetryContinue并按下“重试”时才触发
         */
        MsgBoxResultHelper &OnTryAgain(const Action<> &callback);

    public:
        /**
         * @brief 指定消息框结果的处理函数
         */
        template <MsgBoxResult RES>
        MsgBoxResultHelper &On(const Action<> &callback)
        {
            if (this->_result == RES)
                if (callback) callback();
            return *this;
        }
    };

    /**
     * @brief 消息框类
     */
    class MsgBox
    {
    public:
        /**
         * @brief 静态类，不允许实例化
         */
        MsgBox() = delete;

    public:
        /**
         * @brief 显示消息框
         */
        static MsgBoxResultHelper Show(const WndBase *owner, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::Ok, MsgBoxIcon icon = MsgBoxIcon::None);

        /**
         * @brief 显示消息框
         */
        static MsgBoxResultHelper Show(const WndBase &owner, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::Ok, MsgBoxIcon icon = MsgBoxIcon::None);

        /**
         * @brief 显示消息框，将当前活动窗体作为Owner
         */
        static MsgBoxResultHelper Show(const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::Ok, MsgBoxIcon icon = MsgBoxIcon::None);

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
