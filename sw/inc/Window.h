#pragma once

#include "UIElement.h"
#include "Screen.h"
#include "Color.h"

namespace sw
{
    class Window : public UIElement
    {
    public:
        enum WindowState {
            Normal,
            Minimized,
            Maximized,
        };
        enum WindowStartupLocation {
            Manual,
            CenterScreen,
        };

    private:
        bool _isFirstShow = true;
        Color _background  = Color::White;

    public:
        static bool PostQuitWhenAllClosed;
        const Property<WindowState> State;
        const Property<bool> SizeBox;
        const Property<bool> MaximizeBox;
        const Property<bool> MinimizeBox;
        const Property<bool> Topmost;
        const Property<bool> ToolWindow;
        const Property<Color> Background;
        WindowStartupLocation StartupLocation = Manual;

    public:
        Window();

    private:
        bool GetStyle(LONG_PTR style);
        void SetStyle(LONG_PTR style, bool value);
        bool GetExtendedStyle(LONG_PTR style);
        void SetExtendedStyle(LONG_PTR style, bool value);

    protected:
        virtual LRESULT WndProc(const ProcMsg &refMsg);
        virtual bool OnClose();
        virtual bool OnCreate();
        virtual bool OnDestroy();
        virtual bool OnPaint();

    public:
        void Show();
    };
}
