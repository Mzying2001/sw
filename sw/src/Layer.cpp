#include "Layer.h"
#include <cmath>

sw::Layer::Layer()
    : Layout(
          // get
          [&]() -> LayoutHost *const & {
              return this->_layout;
          },
          // set
          [&](LayoutHost *const &value) {
              if (value != nullptr)
                  value->Associate(this);
              this->_layout = value;
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
              ShowScrollBar(this->Handle, SB_HORZ, value);
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
              ShowScrollBar(this->Handle, SB_VERT, value);
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

              if (!this->IsUsingAbsoluteLayout() && !this->_horizontalScrollDisabled) {
                  this->GetArrangeOffsetX() = -HorizontalScrollPos;
                  this->GetLayoutHost().Arrange(this->ClientRect);
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

              if (!this->IsUsingAbsoluteLayout() && !this->_verticalScrollDisabled) {
                  this->GetArrangeOffsetY() = -VerticalScrollPos;
                  this->GetLayoutHost().Arrange(this->ClientRect);
              }
          })
{
    this->_defaultLayout.Associate(this);
}

sw::LayoutHost &sw::Layer::GetLayoutHost()
{
    return this->_layout == nullptr ? this->_defaultLayout : *this->_layout;
}

void sw::Layer::UpdateLayout()
{
    if (!this->_layoutDisabled) {
        sw::Rect clientRect = this->ClientRect;
        this->GetLayoutHost().Measure(Size(clientRect.width, clientRect.height));
        this->GetLayoutHost().Arrange(clientRect);

        this->UpdateScrollRange();

        this->Redraw();
    }
}

bool sw::Layer::OnVerticalScroll(int event, int pos)
{
    if (event == SB_THUMBTRACK) {
        this->VerticalScrollPos = pos * Dip::ScaleY;
        this->Redraw();
    }
    return true;
}

bool sw::Layer::OnHorizontalScroll(int event, int pos)
{
    if (event == SB_THUMBTRACK) {
        this->HorizontalScrollPos = pos * Dip::ScaleX;
        this->Redraw();
    }
    return true;
}

void sw::Layer::Measure(const Size &availableSize)
{
    Size measureSize    = availableSize;
    Thickness margin    = this->Margin;
    sw::Rect windowRect = this->Rect;
    sw::Rect clientRect = this->ClientRect;

    // 考虑边框
    measureSize.width -= (windowRect.width - clientRect.width) + margin.left + margin.right;
    measureSize.height -= (windowRect.height - clientRect.height) + margin.top + margin.bottom;

    this->GetLayoutHost().Measure(measureSize);

    Size desireSize = this->GetDesireSize();

    if (std::isnan(desireSize.width) || std::isnan(desireSize.height)) {
        // AbsoluteLayout特殊处理：用nan表示按照普通控件处理
        this->UIElement::Measure(availableSize);
    } else {
        // 考虑边框
        desireSize.width += (windowRect.width - clientRect.width) + margin.left + margin.right;
        desireSize.height += (windowRect.width - clientRect.width) + margin.top + margin.bottom;
        this->SetDesireSize(desireSize);
    }
}

void sw::Layer::Arrange(const sw::Rect &finalPosition)
{
    this->UIElement::Arrange(finalPosition);
    this->GetLayoutHost().Arrange(this->ClientRect);

    this->UpdateScrollRange();
}

bool sw::Layer::IsUsingAbsoluteLayout()
{
    return this->_layout == nullptr || dynamic_cast<AbsoluteLayout *>(this->_layout) != nullptr;
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

    SetScrollInfo(this->Handle, SB_HORZ, &info, false);
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

    SetScrollInfo(this->Handle, SB_VERT, &info, false);
}

void sw::Layer::UpdateScrollRange()
{
    if (this->IsUsingAbsoluteLayout()) {
        return; // 当使用绝对布局时不更新
    }

    if (this->HorizontalScrollBar) {
        double childRightmost = this->GetChildRightmost(true);

        if (childRightmost > this->ClientWidth) {
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

        if (childBottommost > this->ClientHeight) {
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
