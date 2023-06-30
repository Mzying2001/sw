#pragma once

namespace sw
{
    /**
     * @brief 水平对齐方式
     */
    enum class HorizontalAlignment {
        Center,  // 中心
        Stretch, // 拉伸
        Left,    // 左对齐
        Right,   // 右对齐
    };

    /**
     * @brief 垂直对齐方式
     */
    enum class VerticalAlignment {
        Center,  // 中心
        Stretch, // 拉伸
        Top,     // 顶部对齐
        Bottom,  // 底部对齐
    };

    /**
     * @brief 排列方式
     */
    enum class Orientation {
        Horizontal, // 水平排列
        Vertical,   // 垂直排列
    };
}
