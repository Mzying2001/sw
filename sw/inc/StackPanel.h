#pragma once

#include "Panel.h"
#include "StackLayout.h"

namespace sw
{
    class StackPanel : public Panel
    {
    private:
        StackLayout _stackLayout = StackLayout();

    public:
        /**
         * @brief 排列方式
         */
        const Property<sw::Orientation> Orientation;

    public:
        StackPanel();
    };
}
