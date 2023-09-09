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
        /**
         * @brief 初始化DockPanel
         */
        DockPanel();

        /**
         * @brief 获取指定元素的Dock
         */
        static DockLayout::DockLayoutTag GetDock(UIElement &element);

        /**
         * @brief 设置指定元素的Dock
         */
        static void SetDock(UIElement &element, DockLayout::DockLayoutTag dock);
    };
}
