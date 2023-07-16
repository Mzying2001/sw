#include "DockLayout.h"

static sw::DockLayout::DockLayoutTag _GetDockLayoutTag(sw::ILayout &item)
{
    uint32_t tag = item.GetLayoutTag();
    return tag > sw::DockLayout::Bottom ? sw::DockLayout::Left : (sw::DockLayout::DockLayoutTag)tag;
}

void sw::DockLayout::MeasureOverride(Size &availableSize)
{
    // TODO
}

void sw::DockLayout::ArrangeOverride(Size &finalSize)
{
    // TODO
}
