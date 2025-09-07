#include "Layer.h"
#include <cmath>

namespace
{
    /**
     * @brief 滚动条滚动一行的距离
     */
    constexpr int _LayerScrollBarLineInterval = 20;
}

sw::Layer::Layer()
    : Layout(
          // get
          [this]() -> LayoutHost * {
              return this->_customLayout;
          },
          // set
          [this](LayoutHost *value) {
              if (value != nullptr)
                  value->Associate(this);
              this->_customLayout = value;
              this->InvalidateMeasure();
          }),

      AutoSize(
          // get
          [this]() -> bool {
              return this->_autoSize;
          },
          // set
          [this](const bool &value) {
              if (this->_autoSize != value) {
                  this->_autoSize = value;
                  this->InvalidateMeasure();
              }
          }),

      HorizontalScrollBar(
          // get
          [this]() -> bool {
              return this->GetStyle(WS_HSCROLL);
          },
          // set
          [this](const bool &value) {
              if (this->HorizontalScrollBar == value) {
                  return;
              }
              if (value) {
                  ShowScrollBar(this->Handle, SB_HORZ, value);
                  this->HorizontalScrollPos = this->HorizontalScrollPos;
              } else {
                  this->HorizontalScrollPos = 0;
                  ShowScrollBar(this->Handle, SB_HORZ, value);
              }
          }),

      VerticalScrollBar(
          // get
          [this]() -> bool {
              return this->GetStyle(WS_VSCROLL);
          },
          // set
          [this](const bool &value) {
              if (this->VerticalScrollBar == value) {
                  return;
              }
              if (value) {
                  ShowScrollBar(this->Handle, SB_VERT, value);
                  this->VerticalScrollPos = this->VerticalScrollPos;
              } else {
                  this->VerticalScrollPos = 0;
                  ShowScrollBar(this->Handle, SB_VERT, value);
              }
          }),

      HorizontalScrollPos(
          // get
          [this]() -> double {
              SCROLLINFO info{};
              info.cbSize = sizeof(info);
              info.fMask  = SIF_POS;
              GetScrollInfo(this->Handle, SB_HORZ, &info);
              return Dip::PxToDipX(info.nPos);
          },
          // set
          [this](const double &value) {
              SCROLLINFO info{};
              info.cbSize = sizeof(info);
              info.fMask  = SIF_POS;
              info.nPos   = Dip::DipToPxX(value);
              SetScrollInfo(this->Handle, SB_HORZ, &info, true);

              LayoutHost *layout = this->_GetLayout();

              if (layout != nullptr && !this->_horizontalScrollDisabled && this->HorizontalScrollBar) {
                  this->GetInternalArrangeOffsetX() = -this->HorizontalScrollPos;
                  this->_MeasureAndArrangeWithoutResize(*layout, this->ClientRect->GetSize());
              }
          }),

      VerticalScrollPos(
          // get
          [this]() -> double {
              SCROLLINFO info{};
              info.cbSize = sizeof(info);
              info.fMask  = SIF_POS;
              GetScrollInfo(this->Handle, SB_VERT, &info);
              return Dip::PxToDipY(info.nPos);
          },
          // set
          [this](const double &value) {
              SCROLLINFO info{};
              info.cbSize = sizeof(info);
              info.fMask  = SIF_POS;
              info.nPos   = Dip::DipToPxY(value);
              SetScrollInfo(this->Handle, SB_VERT, &info, true);

              LayoutHost *layout = this->_GetLayout();

              if (layout != nullptr && !this->_verticalScrollDisabled && this->VerticalScrollBar) {
                  this->GetInternalArrangeOffsetY() = -this->VerticalScrollPos;
                  this->_MeasureAndArrangeWithoutResize(*layout, this->ClientRect->GetSize());
              }
          }),

      HorizontalScrollLimit(
          // get
          [this]() -> double {
              if (this->_horizontalScrollDisabled) {
                  return 0;
              }
              SCROLLINFO info{};
              info.cbSize = sizeof(info);
              info.fMask  = SIF_RANGE | SIF_PAGE;
              GetScrollInfo(this->Handle, SB_HORZ, &info);
              return Dip::PxToDipX(info.nMax - info.nPage + 1);
          }),

      VerticalScrollLimit(
          // get
          [this]() -> double {
              if (this->_verticalScrollDisabled) {
                  return 0;
              }
              SCROLLINFO info{};
              info.cbSize = sizeof(info);
              info.fMask  = SIF_RANGE | SIF_PAGE;
              GetScrollInfo(this->Handle, SB_VERT, &info);
              return Dip::PxToDipY(info.nMax - info.nPage + 1);
          }),

      MouseWheelScrollEnabled(
          // get
          [this]() -> bool {
              return this->_mouseWheelScrollEnabled;
          },
          // set
          [this](const bool &value) {
              this->_mouseWheelScrollEnabled = value;
          })
{
}

sw::Layer::~Layer()
{
}

void sw::Layer::UpdateLayout()
{
    if (this->_layoutDisabled) {
        return;
    }

    LayoutHost *layout = this->_GetLayout();

    if (layout == nullptr) {
        this->_MeasureAndArrangeWithoutLayout();
    } else {
        this->_MeasureAndArrangeWithoutResize(*layout, this->ClientRect->GetSize());
    }

    this->UpdateScrollRange();
    // this->Redraw();
}

sw::LayoutHost *sw::Layer::GetDefaultLayout()
{
    return nullptr;
}

void sw::Layer::OnScroll(ScrollOrientation scrollbar, ScrollEvent event, double pos)
{
    ScrollingEventArgs args(scrollbar, event, pos);
    this->RaiseRoutedEvent(args);

    if (args.cancel) {
        return;
    }

    if (scrollbar == ScrollOrientation::Horizontal) {
        // 水平滚动条
        switch (event) {
            case ScrollEvent::ThubmTrack: {
                this->HorizontalScrollPos = pos;
                break;
            }
            case ScrollEvent::Left: {
                this->ScrollToLeft();
                break;
            }
            case ScrollEvent::Right: {
                this->ScrollToRight();
                break;
            }
            case ScrollEvent::PageLeft: {
                this->ScrollHorizontal(-this->GetHorizontalScrollPageSize());
                break;
            }
            case ScrollEvent::PageRight: {
                this->ScrollHorizontal(this->GetHorizontalScrollPageSize());
                break;
            }
            case ScrollEvent::LineLeft: {
                this->ScrollHorizontal(-_LayerScrollBarLineInterval);
                break;
            }
            case ScrollEvent::LineRight: {
                this->ScrollHorizontal(_LayerScrollBarLineInterval);
                break;
            }
            default: {
                break;
            }
        }
    } else {
        // 垂直滚动条
        switch (event) {
            case ScrollEvent::ThubmTrack: {
                this->VerticalScrollPos = pos;
                break;
            }
            case ScrollEvent::Bottom: {
                this->ScrollToBottom();
                break;
            }
            case ScrollEvent::Top: {
                this->ScrollToTop();
                break;
            }
            case ScrollEvent::PageUp: {
                this->ScrollVertical(-this->GetVerticalScrollPageSize());
                break;
            }
            case ScrollEvent::PageDown: {
                this->ScrollVertical(this->GetVerticalScrollPageSize());
                break;
            }
            case ScrollEvent::LineUp: {
                this->ScrollVertical(-_LayerScrollBarLineInterval);
                break;
            }
            case ScrollEvent::LineDown: {
                this->ScrollVertical(_LayerScrollBarLineInterval);
                break;
            }
            default: {
                break;
            }
        }
    }
}

bool sw::Layer::OnVerticalScroll(int event, int pos)
{
    this->OnScroll(ScrollOrientation::Vertical, (ScrollEvent)event,
                   (event == SB_THUMBTRACK || event == SB_THUMBPOSITION) ? Dip::PxToDipY(pos) : (0.0));
    return true;
}

bool sw::Layer::OnHorizontalScroll(int event, int pos)
{
    this->OnScroll(ScrollOrientation::Horizontal, (ScrollEvent)event,
                   (event == SB_THUMBTRACK || event == SB_THUMBPOSITION) ? Dip::PxToDipX(pos) : (0.0));
    return true;
}

sw::Size sw::Layer::MeasureOverride(const Size &availableSize)
{
    LayoutHost *layout = this->_GetLayout();

    // 未设置布局时无法使用自动尺寸
    // 若未使用自动尺寸，则按照普通元素measure
    if (layout == nullptr || !this->_autoSize) {
        return this->UIElement::MeasureOverride(availableSize);
    }

    return layout->MeasureOverride(availableSize);
}

void sw::Layer::ArrangeOverride(const Size &finalSize)
{
    LayoutHost *layout = this->_GetLayout();

    if (layout == nullptr) {
        // 未设置布局方式，此时需要对子元素进行Measure和Arrange
        this->_MeasureAndArrangeWithoutLayout();
    } else if (!this->_autoSize) {
        // 已设置布局方式，但是AutoSize被取消，此时子元素也未Measure
        this->_MeasureAndArrangeWithoutResize(*layout, finalSize);
    } else {
        // 已设置布局方式且AutoSize为true，此时子元素已Measure，调用Arrange即可
        layout->ArrangeOverride(finalSize);
    }

    this->UpdateScrollRange();
}

bool sw::Layer::RequestBringIntoView(const sw::Rect &screenRect)
{
    bool handled = false;

    sw::Point pos = this->PointFromScreen(screenRect.GetPos());
    sw::Rect rect = {pos.x, pos.y, screenRect.width, screenRect.height};

    sw::Rect clientRect = this->ClientRect;

    if (this->VerticalScrollBar) {
        double curPos = this->VerticalScrollPos;
        if (rect.top < curPos) {
            this->VerticalScrollPos = rect.top;
        } else if (rect.top + rect.height > curPos + clientRect.height) {
            if (rect.height >= clientRect.height) {
                this->VerticalScrollPos = rect.top;
            } else {
                this->VerticalScrollPos = rect.top + rect.height - clientRect.height;
            }
        }
        handled = true;
    }

    if (this->HorizontalScrollBar) {
        double curPos = this->HorizontalScrollPos;
        if (rect.left < curPos) {
            this->HorizontalScrollPos = rect.left;
        } else if (rect.left + rect.width > curPos + clientRect.width) {
            if (rect.width >= clientRect.width) {
                this->HorizontalScrollPos = rect.left;
            } else {
                this->HorizontalScrollPos = rect.left + rect.width - clientRect.width;
            }
        }
        handled = true;
    }

    return handled;
}

bool sw::Layer::OnRoutedEvent(RoutedEventArgs &eventArgs, const RoutedEventHandler &handler)
{
    if (handler) {
        handler(*this, eventArgs);
    }
    if (eventArgs.handled) {
        return true;
    }

    if (eventArgs.eventType == UIElement_MouseWheel && this->_mouseWheelScrollEnabled) {
        auto &wheelArgs = static_cast<MouseWheelEventArgs &>(eventArgs);
        bool shiftDown  = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
        double offset   = -std::copysign(_LayerScrollBarLineInterval, wheelArgs.wheelDelta);
        if (shiftDown) {
            if (this->HorizontalScrollBar) {
                this->ScrollHorizontal(offset);
                eventArgs.handled = true;
            }
        } else {
            if (this->VerticalScrollBar) {
                this->ScrollVertical(offset);
                eventArgs.handled = true;
            }
        }
    }
    return true;
}

void sw::Layer::DisableLayout()
{
    this->_layoutDisabled = true;
}

void sw::Layer::EnableLayout()
{
    this->_layoutDisabled = false;
    this->UpdateLayout();
}

bool sw::Layer::IsLayoutDisabled()
{
    return this->_layoutDisabled;
}

void sw::Layer::GetHorizontalScrollRange(double &refMin, double &refMax)
{
    INT nMin = 0, nMax = 0;
    GetScrollRange(this->Handle, SB_HORZ, &nMin, &nMax);

    refMin = Dip::PxToDipX(nMin);
    refMax = Dip::PxToDipX(nMax);
}

void sw::Layer::GetVerticalScrollRange(double &refMin, double &refMax)
{
    INT nMin = 0, nMax = 0;
    GetScrollRange(this->Handle, SB_VERT, &nMin, &nMax);

    refMin = Dip::PxToDipY(nMin);
    refMax = Dip::PxToDipY(nMax);
}

void sw::Layer::SetHorizontalScrollRange(double min, double max)
{
    SCROLLINFO info{};
    info.cbSize = sizeof(info);
    info.fMask  = SIF_RANGE | SIF_PAGE;
    info.nMin   = Dip::DipToPxX(min);
    info.nMax   = Dip::DipToPxX(max);
    info.nPage  = Dip::DipToPxX(this->ClientWidth);

    SetScrollInfo(this->Handle, SB_HORZ, &info, true);
}

void sw::Layer::SetVerticalScrollRange(double min, double max)
{
    SCROLLINFO info{};
    info.cbSize = sizeof(info);
    info.fMask  = SIF_RANGE | SIF_PAGE;
    info.nMin   = Dip::DipToPxY(min);
    info.nMax   = Dip::DipToPxY(max);
    info.nPage  = Dip::DipToPxY(this->ClientHeight);

    SetScrollInfo(this->Handle, SB_VERT, &info, true);
}

double sw::Layer::GetHorizontalScrollPageSize()
{
    SCROLLINFO info{};
    info.cbSize = sizeof(info);
    info.fMask  = SIF_PAGE;
    GetScrollInfo(this->Handle, SB_HORZ, &info);
    return Dip::PxToDipX(info.nPage);
}

double sw::Layer::GetVerticalScrollPageSize()
{
    SCROLLINFO info{};
    info.cbSize = sizeof(info);
    info.fMask  = SIF_PAGE;
    GetScrollInfo(this->Handle, SB_VERT, &info);
    return Dip::PxToDipY(info.nPage);
}

void sw::Layer::SetHorizontalScrollPageSize(double pageSize)
{
    SCROLLINFO info{};
    info.cbSize = sizeof(info);
    info.fMask  = SIF_PAGE;
    info.nPage  = Dip::DipToPxX(pageSize);
    SetScrollInfo(this->Handle, SB_HORZ, &info, true);
}

void sw::Layer::SetVerticalScrollPageSize(double pageSize)
{
    SCROLLINFO info{};
    info.cbSize = sizeof(info);
    info.fMask  = SIF_PAGE;
    info.nPage  = Dip::DipToPxY(pageSize);
    SetScrollInfo(this->Handle, SB_VERT, &info, true);
}

void sw::Layer::UpdateScrollRange()
{
    if (this->_GetLayout() == nullptr) {
        // 当未设置布局方式时滚动条和控件位置需要手动设置
        // 将以下俩字段设为false确保xxxScrollLimit属性在未设置布局方式时仍可用
        this->_horizontalScrollDisabled = false;
        this->_verticalScrollDisabled   = false;
        return;
    }

    if (this->HorizontalScrollBar) {
        double childRightmost = this->GetChildRightmost(true);

        if (int(childRightmost - this->ClientWidth) > 0) {
            this->_horizontalScrollDisabled = false;
            EnableScrollBar(this->Handle, SB_HORZ, ESB_ENABLE_BOTH);
            this->SetHorizontalScrollRange(0, childRightmost);

            // 当尺寸改变时确保子元素位置与滚动条同步
            double pos = this->HorizontalScrollPos;
            if (-this->GetInternalArrangeOffsetX() > pos) {
                this->HorizontalScrollPos = pos;
            }

        } else {
            this->HorizontalScrollPos = 0;
            EnableScrollBar(this->Handle, SB_HORZ, ESB_DISABLE_BOTH);
            this->_horizontalScrollDisabled = true;
        }
    }

    if (this->VerticalScrollBar) {
        double childBottommost = this->GetChildBottommost(true);

        if (int(childBottommost - this->ClientHeight) > 0) {
            this->_verticalScrollDisabled = false;
            EnableScrollBar(this->Handle, SB_VERT, ESB_ENABLE_BOTH);
            this->SetVerticalScrollRange(0, childBottommost);

            // 当尺寸改变时确保子元素位置与滚动条同步
            double pos = this->VerticalScrollPos;
            if (-this->GetInternalArrangeOffsetY() > pos) {
                this->VerticalScrollPos = pos;
            }

        } else {
            this->VerticalScrollPos = 0;
            EnableScrollBar(this->Handle, SB_VERT, ESB_DISABLE_BOTH);
            this->_verticalScrollDisabled = true;
        }
    }
}

void sw::Layer::ScrollToTop()
{
    this->VerticalScrollPos = 0;
}

void sw::Layer::ScrollToBottom()
{
    this->VerticalScrollPos = this->VerticalScrollLimit;
}

void sw::Layer::ScrollToLeft()
{
    this->HorizontalScrollPos = 0;
}

void sw::Layer::ScrollToRight()
{
    this->HorizontalScrollPos = this->HorizontalScrollLimit;
}

void sw::Layer::ScrollHorizontal(double offset)
{
    this->HorizontalScrollPos += offset;
}

void sw::Layer::ScrollVertical(double offset)
{
    this->VerticalScrollPos += offset;
}

sw::LayoutHost *sw::Layer::_GetLayout()
{
    auto layout = (this->_customLayout != nullptr) ? this->_customLayout : this->GetDefaultLayout();
    return (layout != nullptr && layout->IsAssociated(this)) ? layout : nullptr;
}

void sw::Layer::_MeasureAndArrangeWithoutLayout()
{
    this->GetInternalArrangeOffsetX() = 0;
    this->GetInternalArrangeOffsetY() = 0;

    int childCount = this->GetChildLayoutCount();

    for (int i = 0; i < childCount; ++i) {
        // measure
        UIElement &item = static_cast<UIElement &>(this->GetChildLayoutAt(i));
        item.Measure(Size{INFINITY, INFINITY});
        // arrange
        Size desireSize      = item.GetDesireSize();
        sw::Rect itemRect    = item.Rect;
        Thickness itemMargin = item.Margin;
        item.Arrange(sw::Rect{itemRect.left - itemMargin.left, itemRect.top - itemMargin.top, desireSize.width, desireSize.height});
    }
}

void sw::Layer::_MeasureAndArrangeWithoutResize(LayoutHost &layout, const Size &clientSize)
{
    if (layout.IsAssociated(this)) {
        layout.MeasureOverride(clientSize);
        layout.ArrangeOverride(clientSize);
    }
}
