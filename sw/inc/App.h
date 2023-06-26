#pragma once

#include "Property.h"
#include <Windows.h>

namespace sw
{
    class App
    {
    private:
        App();

    public:
        static const ReadOnlyProperty<HINSTANCE> Instance;
        static void MsgLoop();
        static void Quit(int exitCode = 0);
    };
}
