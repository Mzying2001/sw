#include "DockSplitter.h"
#include "Cursor.h"
#include "Utils.h"

sw::DockSplitter::DockSplitter()
{
    _hCurHorz = CursorHelper::GetCursorHandle(StandardCursor::SizeNS);
    _hCurVert = CursorHelper::GetCursorHandle(StandardCursor::SizeWE);
}

void sw::DockSplitter::CancelDrag(bool restoreSize)
{
    if (GetCapture() == Handle) {
        _OnEndDrag(restoreSize);
    }
}

bool sw::DockSplitter::OnMouseLeftButtonDown(Point mousePosition, MouseKey keyState)
{
    if (TBase::OnMouseLeftButtonDown(mousePosition, keyState))
        return true;
    else {
        _OnStartDrag();
        return true;
    }
}

bool sw::DockSplitter::OnMouseLeftButtonUp(Point mousePosition, MouseKey keyState)
{
    if (TBase::OnMouseLeftButtonUp(mousePosition, keyState))
        return true;
    else {
        if (GetCapture() == Handle)
            _OnEndDrag(false);
        return true;
    }
}

bool sw::DockSplitter::OnMouseMove(Point mousePosition, MouseKey keyState)
{
    if (TBase::OnMouseMove(mousePosition, keyState))
        return true;
    else {
        if (GetCapture() == Handle)
            _OnDragMove();
        return true;
    }
}

bool sw::DockSplitter::OnKillFocus(HWND hNextFocus)
{
    if (TBase::OnKillFocus(hNextFocus))
        return true;
    else {
        if (GetCapture() == Handle)
            _OnEndDrag(false);
        return false;
    }
}

bool sw::DockSplitter::OnKeyDown(VirtualKey key, KeyFlags flags)
{
    if (TBase::OnKeyDown(key, flags))
        return true;
    else {
        if (key == VirtualKey::Esc) {
            if (GetCapture() == Handle)
                _OnEndDrag(true);
            return true;
        }
        return false;
    }
}

bool sw::DockSplitter::OnSetCursor(HWND hwnd, HitTestResult hitTest, int message, bool &result)
{
    ::SetCursor(Orientation == Orientation::Horizontal ? _hCurHorz : _hCurVert);
    result = true;
    return true;
}

void sw::DockSplitter::_OnStartDrag()
{
    _UpdateRelatedElement();

    POINT pt;
    GetCursorPos(&pt);
    _initialMousePos = Point(pt);

    if (_relatedElement != nullptr) {
        _initialRelatedElementSize = _relatedElement->Rect->GetSize();
        SetCapture(Handle);
        Focused = true;
    }
}

void sw::DockSplitter::_OnEndDrag(bool restoreSize)
{
    ReleaseCapture();

    if (_relatedElement != nullptr) {
        if (restoreSize)
            _relatedElement->Resize(_initialRelatedElementSize);
        _relatedElement = nullptr;
    }
}

void sw::DockSplitter::_OnDragMove()
{
    if (_relatedElement == nullptr) return;
    if (_relatedElement->Parent != Parent) return;

    POINT pt;
    GetCursorPos(&pt);
    Point currentMousePos = Point(pt);

    auto newSize = _initialRelatedElementSize;
    if (Orientation == Orientation::Vertical) {
        // 水平方向
        double delta =
            currentMousePos.x - _initialMousePos.x;
        switch (LayoutTag.Get()) {
            case DockLayoutTag::Left: {
                newSize.width += delta;
                break;
            }
            case DockLayoutTag::Right: {
                newSize.width -= delta;
                break;
            }
        }
    } else {
        // 垂直方向
        double delta =
            currentMousePos.y - _initialMousePos.y;
        switch (LayoutTag.Get()) {
            case DockLayoutTag::Top: {
                newSize.height += delta;
                break;
            }
            case DockLayoutTag::Bottom: {
                newSize.height -= delta;
                break;
            }
        }
    }

    newSize.width  = Utils::Max(newSize.width, 0.0);
    newSize.height = Utils::Max(newSize.height, 0.0);
    _relatedElement->Resize(newSize);
}

void sw::DockSplitter::_UpdateRelatedElement()
{
    _relatedElement = nullptr;

    UIElement *parent = Parent;
    if (parent == nullptr) return;

    int index       = parent->IndexOf(this);
    _relatedElement = _FindPreviousElement(parent, index - 1, sw::DockLayoutTag(LayoutTag));
}

sw::UIElement *sw::DockSplitter::_FindPreviousElement(UIElement *parent, int startIndex, sw::DockLayoutTag tag)
{
    for (int i = startIndex; i >= 0; --i) {
        auto ele = &parent->GetChildAt(i);
        if (ele->LayoutTag == tag) {
            if (!ele->Visible && ele->CollapseWhenHide)
                return nullptr;
            return ele;
        }
    }
    return nullptr;
}
