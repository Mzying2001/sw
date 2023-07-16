#pragma once

#include "Point.h"
#include "RoutedEvent.h"
#include "Size.h"

namespace sw
{
    struct RoutedEventArgs; // RoutedEvent.h

    /**
     * @brief 尺寸改变事件参数类型
     */
    struct SizeChangedEventArgs : RoutedEventArgs {
        Size newClientSize; // 用户区的新尺寸
        SizeChangedEventArgs(const Size &newClientSize);
    };

    /**
     * @brief 位置改变事件参数类型
     */
    struct PositionChangedEventArgs : RoutedEventArgs {
        Point newClientPosition; // 移动后用户区左上角的位置
        PositionChangedEventArgs(const Point &newClientPosition);
    };

    /**
     * @brief Text属性改变事件参数类型
     */
    struct TextChangedEventArgs : RoutedEventArgs {
        const wchar_t *newText; // 改变后的Text
        TextChangedEventArgs(const wchar_t *newText);
    };

    /**
     * @brief 窗口正在关闭事件参数类型
     */
    struct WindowClosingEventArgs : RoutedEventArgs {
        bool cancel = false; // 是否取消本次关闭
        WindowClosingEventArgs();
    };
}
