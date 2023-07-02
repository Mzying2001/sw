#pragma once

#include "Panel.h"
#include "WrapLayout.h"

namespace sw
{
    class WrapPanel : public Panel
    {
    private:
        WrapLayout _wrapLayout = WrapLayout();

    public:
        /**
         * @brief 排列方式
         */
        const Property<sw::Orientation> Orientation;

    public:
        WrapPanel();
    };
}
