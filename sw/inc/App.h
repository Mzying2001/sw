#pragma once

#include <Windows.h>
#include "Property.h"

namespace sw
{
    class App
    {
    private:
        App();

    public:
        static const ReadOnlyProperty<HINSTANCE> Instance;
        static void MsgLoop();
    };
}
