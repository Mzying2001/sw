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

    public:
        /**
         * @brief 初始化组合框
         */
        GroupBox();

    protected:
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

    private:
        /**
         * @brief 更新文本大小
         */
        void _UpdateTextSize();
    };
}
