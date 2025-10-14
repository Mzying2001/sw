#include "Splitter.h"
#include "Utils.h"

namespace
{
    /**
     * @brief 分隔条的窗口类名
     */
    constexpr wchar_t _SplitterClassName[] = L"sw::Splitter";
}

sw::Splitter::Splitter()
    : Orientation(
          // get
          [this]() -> sw::Orientation {
              return _orientation;
          },
          // set
          [this](const sw::Orientation &value) {
              if (_orientation != value) {
                  _orientation = value;
                  value == Orientation::Horizontal
                      ? SetAlignment(HorizontalAlignment::Stretch, VerticalAlignment::Center)
                      : SetAlignment(HorizontalAlignment::Center, VerticalAlignment::Stretch);
              }
          }),

      DrawSplitterLine(
          // get
          [this]() -> bool {
              return _drawSplitterLine;
          },
          // set
          [this](const bool &value) {
              if (_drawSplitterLine != value) {
                  _drawSplitterLine = value;
                  Redraw();
              }
          })
{
    static thread_local ATOM splitterClsAtom = 0;

    if (splitterClsAtom == 0) {
        WNDCLASSEXW wc{};
        wc.cbSize        = sizeof(wc);
        wc.hInstance     = App::Instance;
        wc.lpfnWndProc   = DefWindowProcW;
        wc.lpszClassName = _SplitterClassName;
        wc.hCursor       = CursorHelper::GetCursorHandle(StandardCursor::Arrow);
        splitterClsAtom  = RegisterClassExW(&wc);
    }

    InitControl(_SplitterClassName, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, WS_EX_NOACTIVATE);

    Rect        = sw::Rect{0, 0, 10, 10};
    Transparent = true;
    SetAlignment(HorizontalAlignment::Stretch, VerticalAlignment::Center);
}

bool sw::Splitter::OnPaint()
{
    PAINTSTRUCT ps;

    HWND hwnd = Handle;
    HDC hdc   = BeginPaint(hwnd, &ps);

    RECT rect;
    GetClientRect(hwnd, &rect);

    HBRUSH hBrush = CreateSolidBrush(GetRealBackColor());
    FillRect(hdc, &rect, hBrush);

    if (_drawSplitterLine) {
        if (_orientation == sw::Orientation::Horizontal) {
            // 在中间绘制横向分隔条
            rect.top += Utils::Max(0L, (rect.bottom - rect.top) / 2 - 1);
            DrawEdge(hdc, &rect, EDGE_ETCHED, BF_TOP);
        } else {
            // 在中间绘制纵向分隔条
            rect.left += Utils::Max(0L, (rect.right - rect.left) / 2 - 1);
            DrawEdge(hdc, &rect, EDGE_ETCHED, BF_LEFT);
        }
    }

    DeleteObject(hBrush);
    EndPaint(hwnd, &ps);
    return true;
}

bool sw::Splitter::OnSize(const Size &newClientSize)
{
    InvalidateRect(Handle, NULL, FALSE);
    return UIElement::OnSize(newClientSize);
}
