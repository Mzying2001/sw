#pragma once

#include "CanvasLayout.h"
#include "Panel.h"

namespace sw
{
    /**
     * @brief 一种可以为子元素设置绝对位置的面板，与普通Panel不同的是Canvas支持自动滚动条
     */
    class Canvas : public Panel
    {
    private:
        /**
         * @brief 默认布局对象
         */
        CanvasLayout _canvasLayout = CanvasLayout();

    public:
        /**
         * @brief 初始化Canvas
         */
        Canvas();

        /**
         * @brief 获取指定元素的布局标记
         */
        static CanvasLayoutTag GetCanvasLayoutTag(UIElement &element);

        /**
         * @brief 给指定元素设置布局标记
         */
        static void SetCanvasLayoutTag(UIElement &element, const CanvasLayoutTag &tag);

    protected:
        /**
         * @brief 获取默认布局对象
         */
        virtual LayoutHost *GetDefaultLayout() override;
    };
}
