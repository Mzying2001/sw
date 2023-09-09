#pragma once

#include <Windows.h>

namespace sw
{
    /**
     * @brief 滚动条方向
     */
    enum class ScrollOrientation {
        Horizontal, // 水平滚动条
        Vertical,   // 垂直滚动条
    };

    /**
     * @brief 滚动条事件
     */
    enum class ScrollEvent {
        LineUp        = SB_LINEUP,        // Scrolls one line up.
        LineLeft      = SB_LINELEFT,      // Scrolls left by one unit.
        LineDown      = SB_LINEDOWN,      // Scrolls one line down.
        LineRight     = SB_LINERIGHT,     // Scrolls right by one unit.
        PageUp        = SB_PAGEUP,        // Scrolls one page up.
        PageLeft      = SB_PAGELEFT,      // Scrolls left by the width of the window.
        PageDown      = SB_PAGEDOWN,      // Scrolls one page down.
        PageRight     = SB_PAGERIGHT,     // Scrolls right by the width of the window.
        ThumbPosition = SB_THUMBPOSITION, // The user has dragged the scroll box (thumb) and released the mouse button. The HIWORD indicates the position of the scroll box at the end of the drag operation.
        ThubmTrack    = SB_THUMBTRACK,    // The user is dragging the scroll box. This message is sent repeatedly until the user releases the mouse button. The HIWORD indicates the position that the scroll box has been dragged to.
        Top           = SB_TOP,           // Scrolls to the upper left.
        Left          = SB_LEFT,          // Scrolls to the upper left.
        Bottom        = SB_BOTTOM,        // Scrolls to the lower right.
        Right         = SB_RIGHT,         // Scrolls to the lower right.
        EndScroll     = SB_ENDSCROLL,     // Ends scroll.
    };
}
