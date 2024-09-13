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
        CenterOwner,  // 所有者窗口中心
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
         * @brief 窗口的尺寸限制，当值不大于0时表示不限制
         */
        double _maxWidth = -1, _maxHeight = -1, _minWidth = -1, _minHeight = -1;

        /**
         * @brief 窗口初次启动的位置
         */
        WindowStartupLocation _startupLocation = WindowStartupLocation::Manual;

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
        std::unique_ptr<LayoutHost> _layout;

    public:
        /**
         * @brief 程序的当前活动窗体
         */
        static const ReadOnlyPtrProperty<Window *> ActiveWindow;

        /**
         * @brief 当前已创建的窗口数
         */
        static const ReadOnlyProperty<int> WindowCount;

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
        const PtrProperty<sw::Menu *> Menu;

        /**
         * @brief  窗口是否显示为模态窗口，当调用ShowDialog时该属性值为true，否则为false
         */
        const ReadOnlyProperty<bool> IsModal;

        /**
         * @brief 拥有者窗口
         */
        const PtrProperty<Window *> Owner;

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
         * @brief  接收到WM_DESTROY时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnDestroy() override;

        /**
         * @brief        接收到WM_ERASEBKGND时调用该函数
         * @param result 若已处理该消息则设为非零值，默认值为0
         * @return       若返回true则将result作为消息的返回值，否则使用DefaultWndProc的返回值
         */
        virtual bool OnEraseBackground(int &result) override;

        /**
         * @brief  接收到WM_PAINT时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnPaint() override;

        /**
         * @brief    当OnCommand接收到菜单命令时调用该函数
         * @param id 菜单id
         */
        virtual void OnMenuCommand(int id) override;

        /**
         * @brief 窗口第一次显示时调用该函数
         */
        virtual void OnFirstShow();

        /**
         * @brief 窗口成为前台窗口时调用该函数
         */
        virtual void OnActived();

        /**
         * @brief 窗口成为后台窗口时调用该函数
         */
        virtual void OnInactived();

        /**
         * @brief      接收到WM_DPICHANGED时调用该函数
         * @param dpiX 横向DPI
         * @param dpiY 纵向DPI
         */
        virtual void OnDpiChanged(int dpiX, int dpiY);

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
         * @brief       设置图标
         * @param hIcon 图标句柄
         */
        void SetIcon(HICON hIcon);

        /**
         * @brief 重回窗口的菜单栏
         */
        void DrawMenuBar();

        /**
         * @brief 调整窗口尺寸以适应其内容大小，只对设置了布局方式的顶级窗口有效
         */
        void SizeToContent();

        /**
         * @brief 设置窗口的默认布局方式
         */
        template <typename TLayout>
        typename std::enable_if<std::is_base_of<LayoutHost, TLayout>::value>::type SetLayout()
        {
            this->_layout.reset(new TLayout);
            this->_layout->Associate(this);
            this->NotifyLayoutUpdated();
        }

        /**
         * @brief 取消通过SetLayout设置的布局方式
         */
        template <std::nullptr_t>
        void SetLayout()
        {
            this->_layout.reset(nullptr);
            this->NotifyLayoutUpdated();
        }
    };
}
