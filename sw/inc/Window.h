#pragma once

#include "WndBase.h"
#include "Screen.h"

namespace sw
{
    class Window : public WndBase
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

    public:
        static bool PostQuitWhenAllClosed;
        const Property<WindowState> State;
        const Property<bool> SizeBox;
        const Property<bool> MaximizeBox;
        const Property<bool> MinimizeBox;
        WindowStartupLocation StartupLocation = Manual;

    public:
        Window();

    private:
        bool GetStyle(LONG_PTR style);
        void SetStyle(LONG_PTR style, bool value);

    protected:
        virtual LRESULT WndProc(const ProcMsg &refMsg);
        virtual bool OnCreate();
        virtual bool OnDestroy();
        virtual bool OnPaint();

    public:
        void Show(int nCmdShow);
        virtual void Show();
    };
}
