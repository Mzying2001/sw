#pragma once

#include <string>
#include <Windows.h>
#include "Property.h"
#include "ProcMsg.h"
#include "Rect.h"

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
        const Property<Rect> Rect;
        const Property<int> Left;
        const Property<int> Top;
        const Property<unsigned int> Width;
        const Property<unsigned int> Height;
        const Property<std::wstring> Text;

    protected:
        WndBase();
        void InitWndBase(
            DWORD dwExStyle, LPCWSTR lpWindowName, DWORD dwStyle,
            int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu);
        virtual LRESULT WndProc(const ProcMsg &refMsg);
        virtual bool OnCreate();
        virtual bool OnDestroy();
        virtual bool OnPaint();

    public:
        void Show(int nCmdShow);
        void Close();
    };
}
