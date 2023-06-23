#pragma once

#include <string>
#include <Windows.h>
#include "Property.h"
#include "ProcMsg.h"
#include "Rect.h"
#include "App.h"
#include "Dpi.h"

namespace sw
{
    class WndBase
    {
    private:
        static LRESULT CALLBACK _WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    protected:
        static const PCWSTR WndBaseClassName;
        static LRESULT DefaultWndProc(const ProcMsg &refMsg);

    private:
        HWND _hwnd;
        std::wstring _text;

    public:
        const ReadOnlyProperty<HWND> Handle;
        const Property<sw::Rect> Rect;
        const Property<double> Left;
        const Property<double> Top;
        const Property<double> Width;
        const Property<double> Height;
        const ReadOnlyProperty<sw::Rect> ClientRect;
        const ReadOnlyProperty<double> ClientWidth;
        const ReadOnlyProperty<double> ClientHeight;
        const Property<bool> Enabled;
        const Property<bool> Visible;
        const Property<std::wstring> Text;

    protected:
        WndBase();
        void InitWndBase(DWORD dwExStyle, LPCWSTR lpWindowName, DWORD dwStyle, HWND hWndParent, HMENU hMenu);
        virtual LRESULT WndProc(const ProcMsg &refMsg);
        virtual bool OnCreate();
        virtual bool OnDestroy();
        virtual bool OnPaint();
        virtual bool OnMove(double newLeft, double newTop);
        virtual bool OnSize(double newClientWidth, double newClientHeight);

    public:
        void Show(int nCmdShow);
        void Close();
    };
}
