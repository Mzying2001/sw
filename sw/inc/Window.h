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
            SystemDefault,
            CenterScreen,
        };

    private:
        bool _isFirstShow = true;

    public:
        static bool PostQuitWhenAllClosed;
        const Property<WindowState> State;
        WindowStartupLocation StartupLocation = CenterScreen;

    public:
        Window();

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
