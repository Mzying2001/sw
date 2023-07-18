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
        DockPanel();
    };
}
