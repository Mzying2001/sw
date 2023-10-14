#pragma once

#include "Panel.h"
#include "WrapLayout.h"

namespace sw
{
    /**
     * @brief 自动换行面板
     */
    class WrapPanel : public Panel
    {
    private:
        /**
         * @brief 默认布局对象
         */
        WrapLayout _wrapLayout = WrapLayout();

    public:
        /**
         * @brief 排列方式
         */
        const Property<sw::Orientation> Orientation;

    public:
        /**
         * @brief 初始化WrapPanel
         */
        WrapPanel();

    protected:
        /**
         * @brief 获取默认布局对象
         */
        virtual LayoutHost *GetDefaultLayout() override;
    };
}
