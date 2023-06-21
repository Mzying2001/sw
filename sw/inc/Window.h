#pragma once

#include "WndBase.h"

namespace sw
{
    class Window : public WndBase
    {
    public:
        static bool PostQuitWhenAllClosed;

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
