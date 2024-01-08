#include "Layer.h"
#include <cmath>

sw::Layer::Layer()
    : Layout(
          // get
          [&]() -> LayoutHost *const & {
              return this->_customLayout;
          },
          // set
          [&](LayoutHost *const &value) {
              if (value != nullptr)
                  value->Associate(this);
              this->_customLayout = value;
          }),

      AutoSize(
          // get
          [&]() -> const bool & {
              return this->_autoSize;
          },
          // set
          [&](const bool &value) {
              if (this->_autoSize != value) {
                  this->_autoSize = value;
                  this->NotifyLayoutUpdated();
              }
          }),

      HorizontalScrollBar(
          // get
          [&]() -> const bool & {
              static bool result;
              result = this->GetStyle(WS_HSCROLL);
              return result;
          },
          // set
          [&](const bool &value) {
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
          [&]() -> const bool & {
              static bool result;
              result = this->GetStyle(WS_VSCROLL);
              return result;
          },
          // set
          [&](const bool &value) {
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
          [&]() -> const double & {
              static double result;

              SCROLLINFO info{};
              info.cbSize = sizeof(info);
              info.fMask  = SIF_POS;
              GetScrollInfo(this->Handle, SB_HORZ, &info);

              result = info.nPos * Dip::ScaleX;
              return result;
          },
          // set
          [&](const double &value) {
              SCROLLINFO info{};
              info.cbSize = sizeof(info);
              info.fMask  = SIF_POS;
              info.nPos   = std::lround(value / Dip::ScaleX);
              SetScrollInfo(this->Handle, SB_HORZ, &info, true);

              LayoutHost *layout = this->_GetLayout();

              if (layout != nullptr && !this->_horizontalScrollDisabled && this->HorizontalScrollBar) {
                  this->GetArrangeOffsetX() = -HorizontalScrollPos;
                  layout->Arrange(this->ClientRect);
              }
          }),

      VerticalScrollPos(
          // get
          [&]() -> const double & {
              static double result;

              SCROLLINFO info{};
              info.cbSize = sizeof(info);
              info.fMask  = SIF_POS;
              GetScrollInfo(this->Handle, SB_VERT, &info);

              result = info.nPos * Dip::ScaleY;
              return result;
          },
          // set
          [&](const double &value) {
              SCROLLINFO info{};
              info.cbSize = sizeof(info);
              info.fMask  = SIF_POS;
              info.nPos   = std::lround(value / Dip::ScaleY);
              SetScrollInfo(this->Handle, SB_VERT, &info, true);

              LayoutHost *layout = this->_GetLayout();

              if (layout != nullptr && !this->_verticalScrollDisabled && this->VerticalScrollBar) {
                  this->GetArrangeOffsetY() = -VerticalScrollPos;
                  layout->Arrange(this->ClientRect);
              }
          }),

      HorizontalScrollLimit(
          // get
          [&]() -> const double & {
              static double result;

              if (this->_horizontalScrollDisabled) {
                  result = 0;
                  return result;
              }

              SCROLLINFO info{};
              info.cbSize = sizeof(info);
              info.fMask  = SIF_RANGE | SIF_PAGE;
              GetScrollInfo(this->Handle, SB_HORZ, &info);

              result = (info.nMax - info.nPage + 1) * Dip::ScaleX;
              return result;
          }),

      VerticalScrollLimit(
          // get
          [&]() -> const double & {
              static double result;

              if (this->_verticalScrollDisabled) {
                  result = 0;
                  return result;
              }

              SCROLLINFO info{};
              info.cbSize = sizeof(info);
              info.fMask  = SIF_RANGE | SIF_PAGE;
              GetScrollInfo(this->Handle, SB_VERT, &info);

              result = (info.nMax - info.nPage + 1) * Dip::ScaleY;
              return result;
          })
{
}

sw::LayoutHost *sw::Layer::_GetLayout()
{
    return this->_customLayout != nullptr ? this->_customLayout : this->GetDefaultLayout();
}

void sw::Layer::_MeasureAndArrangeWithoutLayout()
{
    this->GetArrangeOffsetX() = 0;
    this->GetArrangeOffsetY() = 0;

    int childCount = this->GetChildLayoutCount();

    for (int i = 0; i < childCount; ++i) {
        // measure
        UIElement &item = static_cast<UIElement &>(this->GetChildLayoutAt(i));
        item.Measure(Size(INFINITY, INFINITY));
        // arrange
        Size desireSize      = item.GetDesireSize();
        sw::Rect itemRect    = item.Rect;
        Thickness itemMargin = item.Margin;
        item.Arrange(sw::Rect(itemRect.left - itemMargin.left, itemRect.top - itemMargin.top, desireSize.width, desireSize.height));
    }
}

void sw::Layer::_MeasureAndArrangeWithoutResize()
{
    LayoutHost *layout  = this->_GetLayout();
    sw::Size desireSize = this->GetDesireSize();
    sw::Rect clientRect = this->ClientRect;

    // measure
    layout->Measure(clientRect.GetSize());
    this->SetDesireSize(desireSize); // 恢复DesireSize

    // arrange
    layout->Arrange(clientRect);
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
        this->_MeasureAndArrangeWithoutResize();
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
                this->ScrollHorizontal(-20);
                break;
            }
            case ScrollEvent::LineRight: {
                this->ScrollHorizontal(20);
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
                this->ScrollVertical(-20);
                break;
            }
            case ScrollEvent::LineDown: {
                this->ScrollVertical(20);
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
                   (event == SB_THUMBTRACK || event == SB_THUMBPOSITION) ? (pos * Dip::ScaleY) : (0.0));
    return true;
}

bool sw::Layer::OnHorizontalScroll(int event, int pos)
{
    this->OnScroll(ScrollOrientation::Horizontal, (ScrollEvent)event,
                   (event == SB_THUMBTRACK || event == SB_THUMBPOSITION) ? (pos * Dip::ScaleX) : (0.0));
    return true;
}

void sw::Layer::Measure(const Size &availableSize)
{
    LayoutHost *layout = this->_GetLayout();

    // 未设置布局时无法使用自动尺寸
    // 若未使用自动尺寸，则按照普通元素measure
    if (layout == nullptr || !this->_autoSize) {
        this->UIElement::Measure(availableSize);
        return;
    }

    Size measureSize    = availableSize;
    Thickness margin    = this->Margin;
    sw::Rect windowRect = this->Rect;
    sw::Rect clientRect = this->ClientRect;

    // 考虑边框
    measureSize.width -= (windowRect.width - clientRect.width) + margin.left + margin.right;
    measureSize.height -= (windowRect.height - clientRect.height) + margin.top + margin.bottom;

    layout->Measure(measureSize);
    Size desireSize = this->GetDesireSize();

    desireSize.width += (windowRect.width - clientRect.width) + margin.left + margin.right;
    desireSize.height += (windowRect.width - clientRect.width) + margin.top + margin.bottom;
    this->SetDesireSize(desireSize);
}

void sw::Layer::Arrange(const sw::Rect &finalPosition)
{
    this->UIElement::Arrange(finalPosition);

    LayoutHost *layout = this->_GetLayout();

    if (layout == nullptr) {
        // 未设置布局方式，此时需要对子元素进行Measure和Arrange
        this->_MeasureAndArrangeWithoutLayout();
    } else if (!this->_autoSize) {
        // 已设置布局方式，但是AutoSize被取消，此时子元素也未Measure
        this->_MeasureAndArrangeWithoutResize();
    } else {
        // 已设置布局方式且AutoSize为true，此时子元素已Measure，调用Arrange即可
        layout->Arrange(this->ClientRect);
    }

    this->UpdateScrollRange();
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

void sw::Layer::GetHorizontalScrollRange(double &refMin, double &refMax)
{
    double scale = Dip::ScaleX;

    INT nMin = 0, nMax = 0;
    GetScrollRange(this->Handle, SB_HORZ, &nMin, &nMax);

    refMin = nMin * scale;
    refMax = nMax * scale;
}

void sw::Layer::GetVerticalScrollRange(double &refMin, double &refMax)
{
    double scale = Dip::ScaleY;

    INT nMin = 0, nMax = 0;
    GetScrollRange(this->Handle, SB_VERT, &nMin, &nMax);

    refMin = nMin * scale;
    refMax = nMax * scale;
}

void sw::Layer::SetHorizontalScrollRange(double min, double max)
{
    double scale = Dip::ScaleX;

    SCROLLINFO info{};
    info.cbSize = sizeof(info);
    info.fMask  = SIF_RANGE | SIF_PAGE;
    info.nMin   = std::lround(min / scale);
    info.nMax   = std::lround(max / scale);
    info.nPage  = std::lround(this->ClientWidth / scale);

    SetScrollInfo(this->Handle, SB_HORZ, &info, true);
}

void sw::Layer::SetVerticalScrollRange(double min, double max)
{
    double scale = Dip::ScaleY;

    SCROLLINFO info{};
    info.cbSize = sizeof(info);
    info.fMask  = SIF_RANGE | SIF_PAGE;
    info.nMin   = std::lround(min / scale);
    info.nMax   = std::lround(max / scale);
    info.nPage  = std::lround(this->ClientHeight / scale);

    SetScrollInfo(this->Handle, SB_VERT, &info, true);
}

double sw::Layer::GetHorizontalScrollPageSize()
{
    SCROLLINFO info{};
    info.cbSize = sizeof(info);
    info.fMask  = SIF_PAGE;
    GetScrollInfo(this->Handle, SB_HORZ, &info);
    return info.nPage * Dip::ScaleX;
}

double sw::Layer::GetVerticalScrollPageSize()
{
    SCROLLINFO info{};
    info.cbSize = sizeof(info);
    info.fMask  = SIF_PAGE;
    GetScrollInfo(this->Handle, SB_VERT, &info);
    return info.nPage * Dip::ScaleY;
}

void sw::Layer::SetHorizontalScrollPageSize(double pageSize)
{
    SCROLLINFO info{};
    info.cbSize = sizeof(info);
    info.fMask  = SIF_PAGE;
    info.nPage  = std::lround(pageSize / Dip::ScaleX);
    SetScrollInfo(this->Handle, SB_HORZ, &info, true);
}

void sw::Layer::SetVerticalScrollPageSize(double pageSize)
{
    SCROLLINFO info{};
    info.cbSize = sizeof(info);
    info.fMask  = SIF_PAGE;
    info.nPage  = std::lround(pageSize / Dip::ScaleY);
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
            if (-this->GetArrangeOffsetX() > pos) {
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
            if (-this->GetArrangeOffsetY() > pos) {
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
    this->HorizontalScrollPos = this->HorizontalScrollPos + offset;
}

void sw::Layer::ScrollVertical(double offset)
{
    this->VerticalScrollPos = this->VerticalScrollPos + offset;
}
