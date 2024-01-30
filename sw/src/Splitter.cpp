#include "Splitter.h"
#include "Utils.h"

sw::Splitter::Splitter()
    : Orientation(
          // get
          [&]() -> const sw::Orientation & {
              return this->_orientation;
          },
          // set
          [&](const sw::Orientation &value) {
              if (this->_orientation != value) {
                  this->_orientation = value;
                  value == Orientation::Horizontal
                      ? this->SetAlignment(HorizontalAlignment::Stretch, VerticalAlignment::Center)
                      : this->SetAlignment(HorizontalAlignment::Center, VerticalAlignment::Stretch);
              }
          })
{
    this->Rect        = sw::Rect{0, 0, 10, 10};
    this->Transparent = true;
    this->SetAlignment(HorizontalAlignment::Stretch, VerticalAlignment::Center);
}

bool sw::Splitter::OnPaint()
{
    PAINTSTRUCT ps;

    HWND hwnd = this->Handle;
    HDC hdc   = BeginPaint(hwnd, &ps);

    RECT rect;
    GetClientRect(hwnd, &rect);

    HBRUSH hBrush = CreateSolidBrush(this->GetRealBackColor());
    FillRect(hdc, &rect, hBrush);

    if (this->_orientation == sw::Orientation::Horizontal) {
        // 在中间绘制横向分隔条
        rect.top += Utils::Max(0L, (rect.bottom - rect.top) / 2 - 1);
        DrawEdge(hdc, &rect, EDGE_ETCHED, BF_TOP);
    } else {
        // 在中间绘制纵向分隔条
        rect.left += Utils::Max(0L, (rect.right - rect.left) / 2 - 1);
        DrawEdge(hdc, &rect, EDGE_ETCHED, BF_LEFT);
    }

    DeleteObject(hBrush);
    EndPaint(hwnd, &ps);
    return true;
}

bool sw::Splitter::OnSize(Size newClientSize)
{
    InvalidateRect(this->Handle, NULL, FALSE);
    return this->UIElement::OnSize(newClientSize);
}
