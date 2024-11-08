#pragma once

#include "App.h"
#include "Cursor.h"
#include "Dip.h"
#include "Font.h"
#include "HitTestResult.h"
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
    class Control; // Control.h

    /**
     * @brief 表示一个Windows窗口，是所有窗口和控件的基类
     */
    class WndBase
    {
        // 部分控件可能会改变HWND，设为友元类向Control类暴露_hwnd字段
        friend class Control;

    private:
        /**
         * @brief 用于判断给定指针是否为指向WndBase的指针
         */
        const uint32_t _check;

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
        sw::Rect _rect{};

        /**
         * @brief 窗口标题或文本
         */
        std::wstring _text{};

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
        const ReadOnlyPtrProperty<WndBase *> Parent;

        /**
         * @brief 是否已销毁，当该值为true时不应该继续使用当前对象
         */
        const ReadOnlyProperty<bool> IsDestroyed;

        /**
         * @brief 是否接受拖放文件
         */
        const Property<bool> AcceptFiles;

    protected:
        /**
         * @brief 初始化WndBase
         */
        WndBase();

        WndBase(const WndBase &)            = delete; // 删除拷贝构造函数
        WndBase(WndBase &&)                 = delete; // 删除移动构造函数
        WndBase &operator=(const WndBase &) = delete; // 删除拷贝赋值运算符
        WndBase &operator=(WndBase &&)      = delete; // 删除移动赋值运算符

    public:
        /**
         * @brief 析构函数，这里用纯虚函数使该类成为抽象类
         */
        virtual ~WndBase() = 0;

        /**
         * @brief 判断两个WndBase是否为同一实例
         */
        bool operator==(const WndBase &other) const;

        /**
         * @brief 判断两个WndBase是否为不同实例
         */
        bool operator!=(const WndBase &other) const;

    protected:
        /**
         * @brief 初始化为窗口，该函数会调用CreateWindowExW
         */
        void InitWindow(LPCWSTR lpWindowName, DWORD dwStyle, DWORD dwExStyle);

        /**
         * @brief 初始化为控件，该函数会调用CreateWindowExW
         */
        void InitControl(LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, DWORD dwExStyle);

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
         * @brief 在OnPaint函数完成之后调用该函数
         */
        virtual void OnEndPaint();

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
         * @brief         接收到WM_SETCURSOR消息时调用该函数
         * @param hwnd    鼠标所在窗口的句柄
         * @param hitTest hit-test的结果，详见WM_NCHITTEST消息的返回值
         * @param message 触发该事件的鼠标消息，如WM_MOUSEMOVE
         * @param result  消息的返回值，默认为false
         * @return        若返回true则将result作为消息的返回值，否则使用DefaultWndProc的返回值
         */
        virtual bool OnSetCursor(HWND hwnd, HitTestResult hitTest, int message, bool &result);

        /**
         * @brief               接收到WM_CONTEXTMENU后调用目标控件的该函数
         * @param isKeyboardMsg 消息是否由按下快捷键（Shift+F10、VK_APPS）产生
         * @param mousePosition 鼠标在屏幕中的位置
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnContextMenu(bool isKeyboardMsg, Point mousePosition);

        /**
         * @brief        接收到WM_NOTIFY后调用该函数
         * @param pNMHDR 包含有关通知消息的信息
         * @param result 函数返回值为true时将该值作为消息的返回值，默认值为0
         * @return       若已处理该消息则返回true，否则调用发出通知控件的OnNotified函数，依据其返回值判断是否调用DefaultWndProc
         */
        virtual bool OnNotify(NMHDR *pNMHDR, LRESULT &result);

        /**
         * @brief        父窗口接收到WM_NOTIFY后且父窗口OnNotify函数返回false时调用发出通知控件的该函数
         * @param pNMHDR 包含有关通知消息的信息
         * @param result 函数返回值为true时将该值作为消息的返回值
         * @return       若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnNotified(NMHDR *pNMHDR, LRESULT &result);

        /**
         * @brief       接收到WM_VSCROLL时调用目标控件的该函数
         * @param event 事件类型，即消息wParam的低字
         * @param pos   当前滚动条的位置，仅当event为SB_THUMBPOSITION或SB_THUMBTRACK时有效
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnVerticalScroll(int event, int pos);

        /**
         * @brief       接收到WM_HSCROLL时调用目标控件的该函数
         * @param event 事件类型，即消息wParam的低字
         * @param pos   当前滚动条的位置，仅当event为SB_THUMBPOSITION或SB_THUMBTRACK时有效
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnHorizontalScroll(int event, int pos);

        /**
         * @brief          接收到WM_ENABLE时调用该函数
         * @param newValue Enabled的新值
         * @return         若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnEnabledChanged(bool newValue);

        /**
         * @brief           接收到WM_CTLCOLORxxx时调用该函数
         * @param pControl  消息相关的控件
         * @param hdc       控件的显示上下文句柄
         * @param hRetBrush 要返回的画笔
         * @return          若返回true则将hRetBrush作为消息的返回值，否则使用DefaultWndProc的返回值
         */
        virtual bool OnCtlColor(WndBase *pControl, HDC hdc, HBRUSH &hRetBrush);

        /**
         * @brief           父窗口接收到WM_CTLCOLORxxx时调用对应控件的该函数
         * @param hdc       控件的显示上下文句柄
         * @param hRetBrush 要返回的画笔
         * @return          若返回true则将hRetBrush作为消息的返回值，否则使用DefaultWndProc的返回值
         */
        virtual bool OnColor(HDC hdc, HBRUSH &hRetBrush);

        /**
         * @brief           接收到WM_NCHITTEST后调用该函数
         * @param testPoint 要测试的点在屏幕中的位置
         * @param result    测试的结果，默认为调用DefaultWndProc的结果
         */
        virtual void OnNcHitTest(const Point &testPoint, HitTestResult &result);

        /**
         * @brief        接收到WM_ERASEBKGND时调用该函数
         * @param result 若已处理该消息则设为非零值，默认值为0
         * @return       若返回true则将result作为消息的返回值，否则使用DefaultWndProc的返回值
         */
        virtual bool OnEraseBackground(int &result);

        /**
         * @brief           接收到WM_DRAWITEM时调用该函数
         * @param id        控件的标识符，若消息是通过菜单发送的则此参数为零
         * @param pDrawItem 包含有关要绘制的项和所需绘图类型的信息的结构体指针
         * @return          若已处理该消息则返回true，否则调用发出通知控件的OnDrawItemSelf函数，依据其返回值判断是否调用DefaultWndProc
         */
        virtual bool OnDrawItem(int id, DRAWITEMSTRUCT *pDrawItem);

        /**
         * @brief           父窗口接收到WM_DRAWITEM后且父窗口OnDrawItem函数返回false时调用发出通知控件的该函数
         * @param pDrawItem 包含有关要绘制的项和所需绘图类型的信息的结构体指针
         * @return          若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnDrawItemSelf(DRAWITEMSTRUCT *pDrawItem);

        /**
         * @brief          接收到WM_MEASUREITEM时调用该函数
         * @param id       控件的标识符，若消息是通过菜单发送的则此参数为零
         * @param pMeasure 包含有关要绘制的项的信息的结构体指针
         * @return         若已处理该消息则返回true，否则调用发出通知控件的OnMeasureItemSelf函数，依据其返回值判断是否调用DefaultWndProc
         */
        virtual bool OnMeasureItem(int id, MEASUREITEMSTRUCT *pMeasure);

        /**
         * @brief          父窗口接收到WM_MEASUREITEM后且父窗口OnMeasureItem函数返回false时调用发出通知控件的该函数
         * @param pMeasure 包含有关要绘制的项的信息的结构体指针
         * @return         若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMeasureItemSelf(MEASUREITEMSTRUCT *pMeasure);

        /**
         * @brief       接收到WM_DROPFILES时调用该函数
         * @param hDrop 描述拖入文件的句柄
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnDropFiles(HDROP hDrop);

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
         * @brief 获取字体句柄
         */
        HFONT GetFontHandle();

        /**
         * @brief              重画
         * @param erase        是否擦除旧的背景
         * @param updateWindow 是否调用UpdateWindow
         */
        void Redraw(bool erase = false, bool updateWindow = false);

        /**
         * @brief 判断当前对象是否是控件
         */
        bool IsControl();

        /**
         * @brief 判断当前对象在界面中是否可视，与Visible属性不同的是该函数返回值会受父窗口的影响
         */
        bool IsVisible();

        /**
         * @brief 获取窗口样式
         */
        DWORD GetStyle();

        /**
         * @brief 设置窗口样式
         */
        void SetStyle(DWORD style);

        /**
         * @brief      判断窗口是否设有指定样式
         * @param mask 样式的位掩码，可以是多个样式
         */
        bool GetStyle(DWORD mask);

        /**
         * @brief       打开或关闭指定的样式
         * @param mask  样式的位掩码，可以是多个样式
         * @param value 是否启用指定的样式
         */
        void SetStyle(DWORD mask, bool value);

        /**
         * @brief 获取扩展窗口样式
         */
        DWORD GetExtendedStyle();

        /**
         * @brief 设置扩展窗口样式
         */
        void SetExtendedStyle(DWORD style);

        /**
         * @brief      判断窗口是否设有指定扩展样式
         * @param mask 扩展样式的位掩码，可以是多个扩展样式
         */
        bool GetExtendedStyle(DWORD mask);

        /**
         * @brief       打开或关闭指定的扩展样式
         * @param mask  扩展样式的位掩码，可以是多个扩展样式
         * @param value 是否启用指定的扩展样式
         */
        void SetExtendedStyle(DWORD mask, bool value);

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
         * @brief 发送消息（ASCII）
         */
        LRESULT SendMessageA(UINT uMsg, WPARAM wParam, LPARAM lParam);

        /**
         * @brief 发送消息（UNICODE）
         */
        LRESULT SendMessageW(UINT uMsg, WPARAM wParam, LPARAM lParam);

        /**
         * @brief           测试指定点在窗口的哪一部分
         * @param testPoint 要测试的点在屏幕中的位置
         */
        HitTestResult NcHitTest(const Point &testPoint);

    private:
        /**
         * @brief 窗口过程函数，调用对象的WndProc
         */
        static LRESULT CALLBACK _WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

        /**
         * @brief      关联窗口句柄与WndBase对象
         * @param hwnd 窗口句柄
         * @param wnd  与句柄关联的对象
         */
        static void _SetWndBase(HWND hwnd, WndBase &wnd);

    public:
        /**
         * @brief      通过窗口句柄获取WndBase
         * @param hwnd 窗口句柄
         * @return     若函数成功则返回对象的指针，否则返回nullptr
         */
        static WndBase *GetWndBase(HWND hwnd);
    };
}
