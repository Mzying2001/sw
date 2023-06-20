#include "Rect.h"

sw::Rect::Rect()
    : Rect(0, 0, 0, 0)
{
}

sw::Rect::Rect(int left, int top, unsigned int width, unsigned int height)
    : left(left), top(top), width(width), height(height)
{
}

sw::Rect::Rect(const RECT &rect)
    : Rect(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top)
{
}

sw::Rect::operator RECT()
{
    RECT rect{};
    rect.left   = this->left;
    rect.top    = this->top;
    rect.right  = this->left + this->width;
    rect.bottom = this->top + this->height;
    return rect;
}
