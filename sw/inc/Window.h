#pragma once

#include "Color.h"
#include "Cursor.h"
#include "IDialog.h"
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
    class Window : public Layer, public IDialog
    {
    private:
        /**
         * @brief 是否为第一次显示，该字段使StartupLocation生效
         */
        bool _isFirstShow = true;

        /**
         * @brief 窗口初次启动的位置
         */
        WindowStartupLocation _startupLocation = WindowStartupLocation::Manual;

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

        /**
         * @brief 当前窗口是否显示为模态窗口
         */
        bool _isModal = false;

        /**
         * @brief 以模态窗口显示时保存所有者窗口
         */
        HWND _hModalOwner = NULL;

        /**
         * @brief 窗口无边框
         */
        bool _isBorderless = false;

        /**
         * @brief 窗口的对话框结果，ShowDialog返回该值
         */
        int _dialogResult = 0;

    public:
        /**
         * @brief 程序的当前活动窗体
         */
        static const ReadOnlyProperty<Window *> ActiveWindow;

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
         * @brief 窗口顶部的菜单栏
         */
        const Property<sw::Menu *> Menu;

        /**
         * @brief  窗口是否显示为模态窗口，当调用ShowDialog时该属性值为true，否则为false
         */
        const ReadOnlyProperty<bool> IsModal;

        /**
         * @brief 拥有者窗口
         */
        const Property<Window *> Owner;

        /**
         * @brief 窗口是否为分层窗口，即WS_EX_LAYERED样式是否被设置
         */
        const Property<bool> IsLayered;

        /**
         * @brief 窗口的透明度，范围为0.0~1.0
         * @note  只有将IsLayered设为true该属性才生效，初始值为0.0但需手动设置新值后才会生效
         */
        const Property<double> Opacity;

        /**
         * @brief 窗口无边框
         */
        const Property<bool> Borderless;

        /**
         * @brief 窗口的对话框结果，ShowDialog返回该值
         * @note  该属性仅在窗口作为模态对话框显示时有效，默认值为0，该属性一旦被设置则会自动关闭窗口
         */
        const Property<int> DialogResult;

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
         * @param hdc    设备上下文句柄
         * @param result 若已处理该消息则设为非零值，默认值为0
         * @return       若返回true则将result作为消息的返回值，否则使用DefaultWndProc的返回值
         */
        virtual bool OnEraseBackground(HDC hdc, LRESULT &result) override;

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
         * @brief 当MinWidth、MinHeight、MaxWidth或MaxHeight属性更改时调用此函数
         */
        virtual void OnMinMaxSizeChanged() override;

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
         * @brief  尝试将对象转换成Window
         * @return 若函数成功则返回Window指针，否则返回nullptr
         */
        virtual Window *ToWindow() override;

        /**
         * @brief 关闭窗口
         */
        virtual void Close() override;

        /**
         * @brief 显示窗口
         */
        virtual void Show() override;

        /**
         * @brief       将窗口显示为模式对话框
         * @param owner 窗体的所有者，若为nullptr则使用当前活动窗口
         * @return      DialogResult属性的值，若函数失败则返回-1
         * @note        该函数会创建一个新的消息循环并在窗口销毁时退出
         */
        virtual int ShowDialog(Window *owner = nullptr) override;

        /**
         * @brief       将窗口显示为模式对话框
         * @param owner 窗体的所有者，窗体显示期间该窗体的Enabled属性将被设为false，该参数不能设为自己
         * @return      DialogResult属性的值，若函数失败则返回-1
         * @note        该函数会创建一个新的消息循环并在窗口销毁时退出
         */
        virtual int ShowDialog(Window &owner);

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
            this->InvalidateMeasure();
        }

        /**
         * @brief 取消通过SetLayout设置的布局方式
         */
        template <std::nullptr_t>
        void SetLayout()
        {
            this->_layout.reset(nullptr);
            this->InvalidateMeasure();
        }

    private:
        /**
         * @brief      通过窗口句柄获取Window指针
         * @param hwnd 窗口句柄
         * @return     若函数成功则返回对象的指针，否则返回nullptr
         */
        static Window *_GetWindowPtr(HWND hwnd);

        /**
         * @brief      关联窗口句柄与Window对象
         * @param hwnd 窗口句柄
         * @param wnd  与句柄关联的对象
         */
        static void _SetWindowPtr(HWND hwnd, Window &wnd);

        /**
         * @brief DPI更新时调用该函数递归地更新所有子项的字体
         */
        static void _UpdateFontForAllChild(UIElement &element);

        /**
         * @brief  获取窗口默认图标（即当前exe图标）
         * @return 图标句柄
         */
        static HICON _GetWindowDefaultIcon();
    };
}
