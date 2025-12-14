#pragma once

#include "Panel.h"

namespace sw
{
    /**
     * @brief 组合框
     */
    class GroupBox : public Panel
    {
    private:
        /**
         * @brief 标题文本所需大小
         */
        SIZE _textSize;

        /**
         * @brief 默认布局方式
         */
        std::unique_ptr<LayoutHost> _defaultLayout;

    public:
        /**
         * @brief 初始化组合框
         */
        GroupBox();

    protected:
        /**
         * @brief 获取默认布局对象
         */
        virtual LayoutHost *GetDefaultLayout() override;

        /**
         * @brief      绘制边框
         * @param hdc  绘制设备句柄，可能为NULL
         * @param rect 绘制边框的矩形区域，该函数会减去边框厚度
         * @note       若hdc为NULL则不进行绘制，只更新rect
         */
        virtual void OnDrawBorder(HDC hdc, RECT &rect) override;

        /**
         * @brief Text属性更改时调用此函数
         */
        virtual void OnTextChanged() override;

        /**
         * @brief       字体改变时调用该函数
         * @param hfont 字体句柄
         */
        virtual void FontChanged(HFONT hfont) override;

        /**
         * @brief        设置背景颜色
         * @param color  要设置的颜色
         * @param redraw 是否重绘
         */
        virtual void SetBackColor(Color color, bool redraw) override;

        /**
         * @brief        设置文本颜色
         * @param color  要设置的颜色
         * @param redraw 是否重绘
         */
        virtual void SetTextColor(Color color, bool redraw) override;

    public:
        /**
         * @brief 设置默认布局方式
         */
        template <typename TLayout>
        auto SetLayout()
            -> typename std::enable_if<std::is_base_of<LayoutHost, TLayout>::value>::type
        {
            auto layout = std::make_unique<TLayout>();
            layout->Associate(this);
            _defaultLayout = std::move(layout);
            InvalidateMeasure();
        }

        /**
         * @brief 取消通过SetLayout设置的布局方式
         */
        template <std::nullptr_t>
        void SetLayout()
        {
            _defaultLayout.reset(nullptr);
            InvalidateMeasure();
        }

    private:
        /**
         * @brief 更新文本大小
         */
        void _UpdateTextSize();
    };
}
