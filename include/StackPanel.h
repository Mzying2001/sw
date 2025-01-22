#pragma once

#include "Panel.h"
#include "StackLayout.h"

namespace sw
{
    /**
     * @brief 堆叠面板
     */
    class StackPanel : public Panel
    {
    private:
        /**
         * @brief 默认布局对象
         */
        StackLayout _stackLayout = StackLayout();

    public:
        /**
         * @brief 排列方式
         */
        const Property<sw::Orientation> Orientation;

    public:
        /**
         * @brief 初始化StackPanel
         */
        StackPanel();

    protected:
        /**
         * @brief 获取默认布局对象
         */
        virtual LayoutHost *GetDefaultLayout() override;
    };
}
