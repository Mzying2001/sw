#pragma once

#include "Alignment.h"
#include "ContextMenu.h"
#include "WndBase.h"
#include <shellapi.h>

namespace sw
{
    class NotifyIcon; // 前向声明

    /**
     * @brief 通知图标鼠标事件处理函数类型
     * @note  第一个参数为触发事件的NotifyIcon对象引用，第二个参数为鼠标位置
     * @note  返回值表示是否已处理该事件，若返回true则表示事件已被处理，框架将不再执行默认处理逻辑
     */
    using NotifyIconMouseEventHandler = Delegate<bool(NotifyIcon &, const Point &)>;

    /**
     * @brief 系统托盘通知图标
     */
    class NotifyIcon : public WndBase
    {
    private:
        /**
         * @brief 基类
         */
        using TBase = WndBase;

        /**
         * @brief 通知图标数据
         */
        NOTIFYICONDATAW _nid{};

        /**
         * @brief 右键菜单
         */
        sw::ContextMenu *_contextMenu = nullptr;

    public:
        /**
         * @brief 图标
         */
        const Property<HICON> Icon;

        /**
         * @brief 图标的提示文本
         */
        const Property<std::wstring> ToolTip;

        /**
         * @brief 图标是否可见
         */
        const Property<bool> Visible;

        /**
         * @brief 右键菜单
         */
        const Property<sw::ContextMenu *> ContextMenu;

        /**
         * @brief 图标在屏幕上的位置和尺寸
         */
        const ReadOnlyProperty<sw::Rect> Rect;

        /**
         * @brief 当图标被单击时触发该事件
         */
        NotifyIconMouseEventHandler ClickedHandler;

        /**
         * @brief 当图标被双击时触发该事件
         */
        NotifyIconMouseEventHandler DoubleClickedHandler;

        /**
         * @brief 当图标被右键单击时触发该事件
         */
        NotifyIconMouseEventHandler ContextMenuHandler;

    public:
        /**
         * @brief 初始化通知图标
         */
        NotifyIcon();

        /**
         * @brief 析构函数
         */
        ~NotifyIcon();

    protected:
        /**
         * @brief 避免隐藏基类的OnContextMenu函数
         */
        using TBase::OnContextMenu;

        /**
         * @brief 对WndProc的封装
         */
        virtual LRESULT WndProc(const ProcMsg &refMsg) override;

        /**
         * @brief    当WM_COMMAND接收到菜单命令时调用该函数
         * @param id 菜单id
         */
        virtual void OnMenuCommand(int id) override;

        /**
         * @brief 处理通知图标消息
         */
        virtual void OnNotyfyIconMessage(WPARAM wParam, LPARAM lParam);

        /**
         * @brief          鼠标单击图标时调用该函数
         * @param mousePos 鼠标位置
         */
        virtual void OnClicked(const Point &mousePos);

        /**
         * @brief          鼠标双击图标时调用该函数
         * @param mousePos 鼠标位置
         */
        virtual void OnDoubleClicked(const Point &mousePos);

        /**
         * @brief          鼠标右键单击图标时调用该函数
         * @param mousePos 鼠标位置
         */
        virtual void OnContextMenu(const Point &mousePos);

        /**
         * @brief 获取通知图标数据
         */
        NOTIFYICONDATAW &GetNotifyIconData();

    public:
        /**
         * @brief 显示通知图标
         */
        bool Show();

        /**
         * @brief 隐藏通知图标
         */
        bool Hide();

        /**
         * @brief 销毁通知图标
         * @note  调用该函数后不应继续使用当前对象
         */
        void Destroy();

        /**
         * @brief       弹出上下文菜单
         * @param point 弹出菜单在屏幕中的位置
         * @param horz  菜单的水平方向对齐方式
         * @param vert  菜单的垂直方向对齐方式
         * @return      若函数成功则返回true，否则返回false
         */
        bool ShowContextMenu(
            const Point &point,
            sw::HorizontalAlignment horz = sw::HorizontalAlignment::Left,
            sw::VerticalAlignment vert   = sw::VerticalAlignment::Bottom);

    private:
        /**
         * @brief 调用Shell_NotifyIcon函数
         */
        bool _ShellNotifyIcon(DWORD dwMessage);

        /**
         * @brief 添加图标
         */
        bool _AddIcon();

        /**
         * @brief 删除图标
         */
        bool _DeleteIcon();

        /**
         * @brief 修改图标
         */
        bool _ModifyIcon();

        /**
         * @brief 设置图标状态
         */
        bool _ModifyState(DWORD dwState, DWORD dwStateMask);

        /**
         * @brief 获取默认图标
         */
        HICON _GetDefaultIcon();
    };
}
