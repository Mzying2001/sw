#pragma once

#include "Color.h"
#include "Cursor.h"
#include "Layer.h"
#include "Menu.h"
#include "Screen.h"
#include "UIElement.h"

namespace sw
{
    /**
     * @brief 窗口状态
     */
    enum class WindowState {
        Normal,    // 普通窗口
        Minimized, // 最小化窗口
        Maximized, // 最大化窗口
    };

    /**
     * @brief 窗口启动位置
     */
    enum class WindowStartupLocation {
        Manual,       // 使用系统默认或手动设置
        CenterScreen, // 屏幕中心
        CenterOwner,  // 所有者窗口中心，只在ShowDialog时有效
    };

    /**
     * @brief 窗口
     */
    class Window : public Layer
    {
    private:
        /**
         * @brief 是否为第一次显示，该字段使StartupLocation生效
         */
        bool _isFirstShow = true;

        /**
         * @brief 窗口背景颜色
         */
        Color _backColor = Color::White;

        /**
         * @brief 窗口的尺寸限制，当值不大于0时表示不限制
         */
        double _maxWidth = -1, _maxHeight = -1, _minWidth = -1, _minHeight = -1;

        /**
         * @brief 窗口初次启动的位置
         */
        WindowStartupLocation _startupLocation = WindowStartupLocation::Manual;

        /**
         * @brief 鼠标句柄，窗口初始化时会赋值
         */
        HCURSOR _hCursor;

        /**
         * @brief 以模态窗口显示时保存所有者窗口，非模态时该值始终为nullptr
         */
        Window *_modalOwner = nullptr;

        /**
         * @brief 窗口顶部菜单
         */
        sw::Menu *_menu = nullptr;

        /**
         * @brief 窗口在失去焦点前保存句柄，用于窗口重新获得焦点时保留原先焦点控件的焦点
         */
        HWND _hPrevFocused = NULL;

        /**
         * @brief 窗口的默认布局方式
         */
        std::shared_ptr<LayoutHost> _layout;

    public:
        /**
         * @brief 程序的当前活动窗体
         */
        static const ReadOnlyProperty<Window *> ActiveWindow;

        /**
         * @brief 窗口初次启动的位置
         */
        const Property<WindowStartupLocation> StartupLocation;

        /**
         * @brief 窗口状态
         */
        const Property<WindowState> State;

        /**
         * @brief 窗口是否可调整大小
         */
        const Property<bool> SizeBox;

        /**
         * @brief 最大化按钮是否可用
         */
        const Property<bool> MaximizeBox;

        /**
         * @brief 最小化按钮是否可用
         */
        const Property<bool> MinimizeBox;

        /**
         * @brief 窗口是否置顶
         */
        const Property<bool> Topmost;

        /**
         * @brief 是否显示为ToolWindow (窄边框)
         */
        const Property<bool> ToolWindow;

        /**
         * @brief 背景颜色
         */
        const Property<Color> BackColor;

        /**
         * @brief 最大宽度，当值不大于0时表示不限制
         */
        const Property<double> MaxWidth;

        /**
         * @brief 最大高度，当值不大于0时表示不限制
         */
        const Property<double> MaxHeight;

        /**
         * @brief 最小宽度，当值不大于0时表示不限制
         */
        const Property<double> MinWidth;

        /**
         * @brief 最小高度，当值不大于0时表示不限制
         */
        const Property<double> MinHeight;

        /**
         * @brief 窗口顶部的菜单栏
         */
        const Property<sw::Menu *> Menu;

    public:
        /**
         * @brief 初始化窗口
         */
        Window();

    protected:
        /**
         * @brief 对WndProc的封装
         */
        virtual LRESULT WndProc(const ProcMsg &refMsg) override;

        /**
         * @brief 获取默认布局对象
         */
        virtual LayoutHost *GetDefaultLayout() override;

        /**
         * @brief  接收到WM_CLOSE时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnClose() override;

        /**
         * @brief  接收到WM_CREATE时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnCreate() override;

        /**
         * @brief  接收到WM_DESTROY时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnDestroy() override;

        /**
         * @brief  接收到WM_PAINT时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnPaint() override;

        /**
         * @brief               接收到WM_MOUSEMOVE时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseMove(Point mousePosition, MouseKey keyState) override;

        /**
         * @brief    当OnCommand接收到菜单命令时调用该函数
         * @param id 菜单id
         */
        virtual void OnMenuCommand(int id) override;

        /**
         * @brief 窗口成为前台窗口时调用该函数
         */
        virtual void OnActived();

        /**
         * @brief 窗口成为后台窗口时调用该函数
         */
        virtual void OnInactived();

    public:
        /**
         * @brief 显示窗口
         */
        void Show();

        /**
         * @brief       将窗体显示为模式对话框
         * @param owner 窗体的所有者，窗体显示期间该窗体的Enabled属性将被设为false，该参数不能设为自己
         */
        void ShowDialog(Window &owner);

        /**
         * @brief         设置鼠标样式
         * @param hCursor 鼠标句柄
         */
        void SetCursor(HCURSOR hCursor);

        /**
         * @brief        设置鼠标样式
         * @param cursor 鼠标样式
         */
        void SetCursor(StandardCursor cursor);

        /**
         * @brief       设置图标
         * @param hIcon 图标句柄
         */
        void SetIcon(HICON hIcon);

        /**
         * @brief 重回窗口的菜单栏
         */
        void DrawMenuBar();

        /**
         * @brief  窗口是否显示为模态窗口
         * @return 当调用ShowDialog时该函数返回true，否则返回false
         */
        bool IsModal();

        /**
         * @brief 设置窗口的默认布局方式
         */
        template <
            typename TLayout,
            typename std::enable_if<std::is_base_of<LayoutHost, TLayout>::value, int>::type = 0>
        void SetLayout()
        {
            auto layout = std::make_shared<TLayout>();
            layout->Associate(this);
            this->_layout = layout;
            this->NotifyLayoutUpdated();
        }

        /**
         * @brief 取消通过SetLayout设置的布局方式
         */
        template <nullptr_t>
        void SetLayout()
        {
            this->_layout = nullptr;
            this->NotifyLayoutUpdated();
        }
    };
}
