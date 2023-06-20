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
        virtual void OnDestroy();
        virtual void OnPaint();

    public:
        using WndBase::Show;
        virtual void Show();
    };
}
