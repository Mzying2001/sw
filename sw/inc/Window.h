#pragma once

#include "WndBase.h"

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

    public:
        static bool PostQuitWhenAllClosed;
        const Property<WindowState> State;

    public:
        Window();

    protected:
        virtual LRESULT WndProc(const ProcMsg &refMsg);
        virtual bool OnCreate();
        virtual bool OnDestroy();
        virtual bool OnPaint();

    public:
        using WndBase::Show;
        virtual void Show();
    };
}
