#pragma once

#include "Control.h"
#include <CommCtrl.h>

namespace sw
{
    /**
     * @brief 状态栏
     */
    class StatusBar : public Control
    {
    public:
        /**
         * @brief 是否在控件右下角显示大小调整手柄，修改该属性会重置当前控件
         */
        const Property<bool> SizingGrip;

        /**
         * @brief 获取当前控件中的分区数
         */
        const ReadOnlyProperty<int> PartsCount;

        /**
         * @brief 指定控件是否使用Unicode编码
         */
        const Property<bool> UseUnicode;

    public:
        /**
         * @brief 初始化状态栏
         */
        StatusBar();

        /**
         * @brief       设置控件中的分区信息
         * @param parts 每个部分的宽度，用-1表示延伸到最右边，不能多于256个
         * @return      若函数成功则返回true，否则返回false
         */
        bool SetParts(std::initializer_list<double> parts);

        /**
         * @brief       获取指定部分的文本
         * @param index 指定部分的索引
         * @param out   获取到的文本
         * @return      若函数成功则返回true，否则返回false
         */
        bool GetTextAt(uint8_t index, std::wstring &out);

        /**
         * @brief       设置指定部分的文本
         * @param index 指定部分的索引
         * @param text  要设置的文本
         * @return      若函数成功则返回true，否则返回false
         */
        bool SetTextAt(uint8_t index, const std::wstring &text);

        /**
         * @brief       获取指定部分的位置与尺寸
         * @param index 指定部分的索引
         * @param out   获取到的Rect
         * @return      若函数成功则返回true，否则返回false
         */
        bool GetRectAt(uint8_t index, sw::Rect &out);

    protected:
        /**
         * @brief        设置背景颜色
         * @param color  要设置的颜色
         * @param redraw 是否重绘
         */
        virtual void SetBackColor(Color color, bool redraw) override;
    };
}
