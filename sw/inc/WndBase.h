#pragma once

#include "App.h"
#include "Dip.h"
#include "Font.h"
#include "ICtlColor.h"
#include "Keys.h"
#include "Point.h"
#include "ProcMsg.h"
#include "Property.h"
#include "Rect.h"
#include "Size.h"
#include "WndMsg.h"
#include <Windows.h>
#include <string>
#include <type_traits>
#include <windowsx.h>

namespace sw
{
    class WndBase
    {
    private:
        /**
         * @brief 窗口过程函数，调用对象的WndProc
         */
        static LRESULT CALLBACK _WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    private:
        /**
         * @brief 窗口句柄
         */
        HWND _hwnd = NULL;

        /**
         * @brief 字体句柄
         */
        HFONT _hfont = NULL;

        /**
         * @brief 字体信息
         */
        sw::Font _font;

        /**
         * @brief 窗口的位置和尺寸
         */
        sw::Rect _rect = sw::Rect();

        /**
         * @brief 窗口标题或文本
         */
        std::wstring _text = L"";

        /**
         * @brief 控件是否拥有焦点
         */
        bool _focused = false;

        /**
         * @brief 窗口是否已销毁
         */
        bool _isDestroyed = false;

        /**
         * @brief 当前对象是控件时该函数指针指向控件原本的WndProc
         */
        WNDPROC _controlOldWndProc = NULL;

    public:
        /**
         * @brief 窗口句柄
         */
        const ReadOnlyProperty<HWND> Handle;

        /**
         * @brief 字体
         */
        const Property<sw::Font> Font;

        /**
         * @brief 字体名称
         */
        const Property<std::wstring> FontName;

        /**
         * @brief 字体大小
         */
        const Property<double> FontSize;

        /**
         * @brief 字体粗细
         */
        const Property<sw::FontWeight> FontWeight;

        /**
         * @brief 位置和尺寸
         */
        const Property<sw::Rect> Rect;

        /**
         * @brief 左边
         */
        const Property<double> Left;

        /**
         * @brief 顶边
         */
        const Property<double> Top;

        /**
         * @brief 宽度
         */
        const Property<double> Width;

        /**
         * @brief 高度
         */
        const Property<double> Height;

        /**
         * @brief 用户区尺寸
         */
        const ReadOnlyProperty<sw::Rect> ClientRect;

        /**
         * @brief 用户区宽度
         */
        const ReadOnlyProperty<double> ClientWidth;

        /**
         * @brief 用户区高度
         */
        const ReadOnlyProperty<double> ClientHeight;

        /**
         * @brief 窗口或控件是否可用
         */
        const Property<bool> Enabled;

        /**
         * @brief 窗口或控件是否可见
         */
        const Property<bool> Visible;

        /**
         * @brief 窗口标题或控件文本
         */
        const Property<std::wstring> Text;

        /**
         * @brief 窗口是否拥有焦点
         */
        const Property<bool> Focused;

        /**
         * @brief 父窗口
         */
        const ReadOnlyProperty<WndBase *> Parent;

        /**
         * @brief 是否已销毁，当该值为true时不应该继续使用当前对象
         */
        const ReadOnlyProperty<bool> IsDestroyed;

    protected:
        WndBase();
        ~WndBase();

        WndBase(const WndBase &)            = delete; // 删除拷贝构造函数
        WndBase(WndBase &&)                 = delete; // 删除移动构造函数
        WndBase &operator=(const WndBase &) = delete; // 删除拷贝赋值运算符
        WndBase &operator=(WndBase &&)      = delete; // 删除移动赋值运算符

        /**
         * @brief 初始化为窗口，该函数会调用CreateWindowExW
         */
        void InitWindow(LPCWSTR lpWindowName, DWORD dwStyle, DWORD dwExStyle);

        /**
         * @brief 初始化为控件，该函数会调用CreateWindowExW
         */
        void InitControl(LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, DWORD dwExStyle);

        /**
         * @brief 获取窗口样式
         */
        LONG_PTR GetStyle();

        /**
         * @brief 设置窗口样式
         */
        void SetStyle(LONG_PTR style);

        /**
         * @brief 获取窗口的某个样式
         */
        bool GetStyle(LONG_PTR style);

        /**
         * @brief 设置窗口的某个样式
         */
        void SetStyle(LONG_PTR style, bool value);

        /**
         * @brief 获取扩展窗口样式
         */
        LONG_PTR GetExtendedStyle();

        /**
         * @brief 设置扩展窗口样式
         */
        void SetExtendedStyle(LONG_PTR style);

        /**
         * @brief 获取窗口的某个扩展样式
         */
        bool GetExtendedStyle(LONG_PTR style);

        /**
         * @brief 设置窗口的某个扩展样式
         */
        void SetExtendedStyle(LONG_PTR style, bool value);

        /**
         * @brief 获取字体句柄
         */
        HFONT GetFontHandle();

        /**
         * @brief 调用默认的WndProc，对于窗口则调用DefWindowProcW，控件则调用_controlOldWndProc
         */
        LRESULT DefaultWndProc(const ProcMsg &refMsg);

        /**
         * @brief 对WndProc的封装
         */
        virtual LRESULT WndProc(const ProcMsg &refMsg);

        /**
         * @brief 更新_text字段
         */
        void UpdateText();

        /**
         * @brief  获取窗口文本
         * @return _text字段
         */
        virtual std::wstring &GetText();

        /**
         * @brief       调用SetWindowTextW设置窗口文本
         * @param value 要设置的文本
         */
        virtual void SetText(const std::wstring &value);

        /**
         * @brief  接收到WM_CREATE时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnCreate();

        /**
         * @brief  接收到WM_CLOSE时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnClose();

        /**
         * @brief  接收到WM_DESTROY时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnDestroy();

        /**
         * @brief  接收到WM_PAINT时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnPaint();

        /**
         * @brief                   接收到WM_MOVE时调用该函数
         * @param newClientPosition 移动后用户区左上角的位置
         * @return                  若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMove(Point newClientPosition);

        /**
         * @brief               接收到WM_SIZE时调用该函数
         * @param newClientSize 改变后的用户区尺寸
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSize(Size newClientSize);

        /**
         * @brief Text属性更改时调用此函数
         */
        virtual void OnTextChanged();

        /**
         * @brief            接收到WM_SETFOCUS时调用该函数
         * @param hPrevFocus 丢失焦点的hwnd，可能为NULL
         * @return           若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSetFocus(HWND hPrevFocus);

        /**
         * @brief            接收到WM_KILLFOCUS时调用该函数
         * @param hNextFocus 接收到焦点的hwnd，可能为NULL
         * @return           若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnKillFocus(HWND hNextFocus);

        /**
         * @brief               接收到WM_MOUSEMOVE时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseMove(Point mousePosition, MouseKey keyState);

        /**
         * @brief  接收到WM_MOUSELEAVE时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseLeave();

        /**
         * @brief               接收到WM_MOUSEWHEEL时调用该函数
         * @param wheelDelta    滚轮滚动的距离，为120的倍数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseWheel(int wheelDelta, Point mousePosition, MouseKey keyState);

        /**
         * @brief               接收到WM_LBUTTONDOWN时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseLeftButtonDown(Point mousePosition, MouseKey keyState);

        /**
         * @brief               接收到WM_LBUTTONUP时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseLeftButtonUp(Point mousePosition, MouseKey keyState);

        /**
         * @brief               接收到WM_LBUTTONDBLCLK时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseLeftButtonDoubleClick(Point mousePosition, MouseKey keyState);

        /**
         * @brief               接收到WM_RBUTTONDOWN时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseRightButtonDown(Point mousePosition, MouseKey keyState);

        /**
         * @brief               接收到WM_RBUTTONUP时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseRightButtonUp(Point mousePosition, MouseKey keyState);

        /**
         * @brief               接收到WM_RBUTTONDBLCLK时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseRightButtonDoubleClick(Point mousePosition, MouseKey keyState);

        /**
         * @brief               接收到WM_MBUTTONDOWN时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseMiddleButtonDown(Point mousePosition, MouseKey keyState);

        /**
         * @brief               接收到WM_MBUTTONUP时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseMiddleButtonUp(Point mousePosition, MouseKey keyState);

        /**
         * @brief               接收到WM_MBUTTONDBLCLK时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseMiddleButtonDoubleClick(Point mousePosition, MouseKey keyState);

        /**
         * @brief       接收到WM_CHAR时调用该函数
         * @param ch    按键的字符代码
         * @param flags 附加信息
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnChar(wchar_t ch, KeyFlags flags);

        /**
         * @brief       接收到WM_DEADCHAR时调用该函数
         * @param ch    按键的字符代码
         * @param flags 附加信息
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnDeadChar(wchar_t ch, KeyFlags flags);

        /**
         * @brief       接收到WM_KEYDOWN时调用该函数
         * @param key   虚拟按键
         * @param flags 附加信息
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnKeyDown(VirtualKey key, KeyFlags flags);

        /**
         * @brief       接收到WM_KEYUP时调用该函数
         * @param key   虚拟按键
         * @param flags 附加信息
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnKeyUp(VirtualKey key, KeyFlags flags);

        /**
         * @brief       接收到WM_SYSCHAR时调用该函数
         * @param ch    按键的字符代码
         * @param flags 附加信息
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSysChar(wchar_t ch, KeyFlags flags);

        /**
         * @brief       接收到WM_SYSDEADCHAR时调用该函数
         * @param ch    按键的字符代码
         * @param flags 附加信息
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSysDeadChar(wchar_t ch, KeyFlags flags);

        /**
         * @brief       接收到WM_SYSKEYDOWN时调用该函数
         * @param key   虚拟按键
         * @param flags 附加信息
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSysKeyDown(VirtualKey key, KeyFlags flags);

        /**
         * @brief       接收到WM_SYSKEYUP时调用该函数
         * @param key   虚拟按键
         * @param flags 附加信息
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSysKeyUp(VirtualKey key, KeyFlags flags);

        /**
         * @brief Visible属性改变时调用此函数
         */
        virtual void VisibleChanged(bool newVisible);

        /**
         * @brief  设置父窗口
         * @return 设置是否成功
         */
        virtual bool SetParent(WndBase *parent);

        /**
         * @brief           父窗口改变时调用此函数
         * @param newParent 新的父窗口
         */
        virtual void ParentChanged(WndBase *newParent);

        /**
         * @brief      当父窗口接收到控件的WM_COMMAND时调用该函数
         * @param code 通知代码
         */
        virtual void OnCommand(int code);

        /**
         * @brief          当WM_COMMAND接收到控件命令时调用该函数
         * @param pControl 控件对象指针
         * @param code     通知代码
         * @param id       控件id
         */
        virtual void OnControlCommand(WndBase *pControl, int code, int id);

        /**
         * @brief    当WM_COMMAND接收到菜单命令时调用该函数
         * @param id 菜单id
         */
        virtual void OnMenuCommand(int id);

        /**
         * @brief    当WM_COMMAND接收到快捷键命令时调用该函数
         * @param id 快捷键id
         */
        virtual void OnAcceleratorCommand(int id);

        /**
         * @brief      窗口句柄初始化完成
         * @param hwnd 窗口句柄
         */
        virtual void HandleInitialized(HWND hwnd);

        /**
         * @brief       字体改变时调用该函数
         * @param hfont 字体句柄
         */
        virtual void FontChanged(HFONT hfont);

        /**
         * @brief                   接收到WM_SETCURSOR消息时调用该函数
         * @param hwnd              鼠标所在窗口的句柄
         * @param hitTest           hit-test的结果，详见WM_NCHITTEST消息的返回值
         * @param message           触发该事件的鼠标消息，如WM_MOUSEMOVE
         * @param useDefaultWndProc 是否调用DefaultWndProc并将其返回值作为当前消息的返回值，默认为true
         * @return                  当useDefaultWndProc为false时使用该值作为消息的返回值，表示是否已处理该消息
         */
        virtual bool OnSetCursor(HWND hwnd, int hitTest, int message, bool &useDefaultWndProc);

        /**
         * @brief               接收到WM_CONTEXTMENU后调用目标控件的该函数
         * @param isKeyboardMsg 消息是否由按下快捷键（Shift+F10、VK_APPS）产生
         * @param mousePosition 鼠标在屏幕中的位置
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnContextMenu(bool isKeyboardMsg, Point mousePosition);

        /**
         * @brief 接收到WM_NOTIFY后调用该函数
         */
        virtual void OnNotify(NMHDR *pNMHDR);

        /**
         * @brief 父窗口接收到WM_NOTIFY后调用发出通知控件的该函数
         */
        virtual void OnNotified(NMHDR *pNMHDR);

    public:
        /**
         * @brief 该函数调用ShowWindow
         */
        void Show(int nCmdShow);

        /**
         * @brief 发送关闭消息
         */
        void Close();

        /**
         * @brief 该函数调用UpdateWindow
         */
        void Update();

        /**
         * @brief 更新字体
         */
        void UpdateFont();

        /**
         * @brief       重画
         * @param erase 是否擦除旧的背景
         */
        void Redraw(bool erase = false);

        /**
         * @brief 判断当前对象是否是控件
         */
        bool IsControl();

        /**
         * @brief 将窗口置于 Z 顺序的顶部
         */
        void MoveToTop();

        /**
         * @brief 将窗口置于 Z 顺序的底部
         */
        void MoveToBottom();

        /**
         * @brief       获取用户区点在屏幕上点的位置
         * @param point 用户区坐标
         * @return      该点在屏幕上的坐标
         */
        Point PointToScreen(const Point &point);

        /**
         * @brief             获取屏幕上点在当前用户区点的位置
         * @param screenPoint 屏幕上点的坐标
         * @return            该点在用户区的坐标
         */
        Point PointFromScreen(const Point &screenPoint);

        /**
         * @brief 发送消息
         */
        LRESULT SendMessageW(UINT uMsg, WPARAM wParam, LPARAM lParam);

        /**
         * @brief      通过窗口句柄获取WndBase
         * @param hwnd 窗口句柄
         * @return     若函数成功则返回对象的指针，否则返回nullptr
         */
        static WndBase *GetWndBase(HWND hwnd);

        /**
         * @brief 重载==运算符，判断是否为同一个引用
         */
        friend bool operator==(const WndBase &left, const WndBase &right)
        {
            return &left == &right;
        }

        /**
         * @brief 重载!=运算符，判断是否为不同引用
         */
        friend bool operator!=(const WndBase &left, const WndBase &right)
        {
            return &left != &right;
        }
    };
}
