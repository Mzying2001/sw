#pragma once

#include "DockLayout.h"
#include "Panel.h"

namespace sw
{
    class DockPanel : public Panel
    {
    private:
        DockLayout _dockLayout = DockLayout();

    public:
        /**
         * @brief 最后一个子元素是否填充剩余空间
         */
        const Property<bool> LastChildFill;

    public:
        DockPanel();
    };
}
