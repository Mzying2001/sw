#include "Label.h"
#include "Utils.h"
#include <cmath>

sw::Label::Label()
    : HorizontalContentAlignment(
          // get
          [&]() -> const sw::HorizontalAlignment & {
              static sw::HorizontalAlignment result;
              LONG_PTR style = this->GetStyle();
              if (style & SS_CENTER) {
                  result = sw::HorizontalAlignment::Center;
              } else if (style & SS_RIGHT) {
                  result = sw::HorizontalAlignment::Right;
              } else {
                  result = sw::HorizontalAlignment::Left;
              }
              return result;
          },
          // set
          [&](const sw::HorizontalAlignment &value) {
              switch (value) {
                  case sw::HorizontalAlignment::Left: {
                      this->SetStyle(SS_CENTER | SS_RIGHT, false);
                      break;
                  }
                  case sw::HorizontalAlignment::Center: {
                      LONG_PTR style = this->GetStyle();
                      style &= ~(SS_CENTER | SS_RIGHT);
                      style |= SS_CENTER;
                      this->SetStyle(style);
                      break;
                  }
                  case sw::HorizontalAlignment::Right: {
                      LONG_PTR style = this->GetStyle();
                      style &= ~(SS_CENTER | SS_RIGHT);
                      style |= SS_RIGHT;
                      this->SetStyle(style);
                      break;
                  }
              }
              this->Redraw();
          }),

      VerticalContentAlignment(
          // get
          [&]() -> const sw::VerticalAlignment & {
              static sw::VerticalAlignment result;
              result = this->GetStyle(SS_CENTERIMAGE) ? sw::VerticalAlignment::Center : sw::VerticalAlignment::Top;
              return result;
          },
          // set
          [&](const sw::VerticalAlignment &value) {
              this->SetStyle(SS_CENTERIMAGE, value == sw::VerticalAlignment::Center);
          }),

      TextTrimming(
          // get
          [&]() -> const sw::TextTrimming & {
              static sw::TextTrimming result;
              LONG_PTR style = this->GetStyle();
              if ((style & SS_WORDELLIPSIS) == SS_WORDELLIPSIS) {
                  result = sw::TextTrimming::WordEllipsis;
              } else if (style & SS_ENDELLIPSIS) {
                  result = sw::TextTrimming::EndEllipsis;
              } else {
                  result = sw::TextTrimming::None;
              }
              return result;
          },
          // set
          [&](const sw::TextTrimming &value) {
              switch (value) {
                  case sw::TextTrimming::None: {
                      this->SetStyle(SS_WORDELLIPSIS, false);
                      break;
                  }
                  case sw::TextTrimming::WordEllipsis: {
                      this->SetStyle(SS_WORDELLIPSIS, true);
                      break;
                  }
                  case sw::TextTrimming::EndEllipsis: {
                      LONG_PTR style = this->GetStyle();
                      style &= ~SS_WORDELLIPSIS;
                      style |= SS_ENDELLIPSIS;
                      this->SetStyle(style);
                      break;
                  }
              }
              this->Redraw();
          }),

      AutoWrap(
          // get
          [&]() -> const bool & {
              static bool result;
              result = this->GetStyle(SS_EDITCONTROL);
              return result;
          },
          // set
          [&](const bool &value) {
              this->SetStyle(SS_EDITCONTROL, value);
          }),

      AutoSize(
          // get
          [&]() -> const bool & {
              return this->_autoSize;
          },
          // set
          [&](const bool &value) {
              this->_autoSize = value;
              if (value) {
                  this->NotifyLayoutUpdated();
              }
          })
{
    this->InitControl(L"STATIC", L"Label", WS_CHILD | WS_VISIBLE);
    this->_UpdateTextSize();
    this->_ResizeToTextSize();
}

void sw::Label::_UpdateTextSize()
{
    HWND hwnd = this->Handle;
    HDC hdc   = GetDC(hwnd);

    SelectObject(hdc, this->GetFontHandle());

    RECT rect{};
    const std::wstring &text = this->Text;
    DrawTextW(hdc, text.c_str(), (int)text.size(), &rect, DT_CALCRECT);

    sw::Rect textRect = rect;
    this->_textSize   = Size(textRect.width, textRect.height);

    ReleaseDC(hwnd, hdc);
}

void sw::Label::_ResizeToTextSize()
{
    sw::Rect rect = this->Rect;
    rect.width    = this->_textSize.width;
    rect.height   = this->_textSize.height;
    this->Rect    = rect;
}

void sw::Label::OnTextChanged(const std::wstring &newText)
{
    this->UIElement::OnTextChanged(newText);
    this->_UpdateTextSize();

    if (this->_autoSize) {
        this->NotifyLayoutUpdated();
    }
}

void sw::Label::Measure(const Size &availableSize)
{
    if (!this->_autoSize) {
        this->UIElement::Measure(availableSize);
        return;
    }

    Thickness margin = this->Margin;

    Size desireSize(
        this->_textSize.width + margin.left + margin.right,
        this->_textSize.height + margin.top + margin.bottom);

    if (availableSize.width < desireSize.width) {

        if (this->TextTrimming.Get() != sw::TextTrimming::None) {
            desireSize.width = availableSize.width;

        } else if (this->AutoWrap) {
            HWND hwnd = this->Handle;
            HDC hdc   = GetDC(hwnd);

            SelectObject(hdc, this->GetFontHandle());

            double scaleX = Dip::ScaleX;
            double scaleY = Dip::ScaleY;
            RECT rect{0, 0, Utils::Max(0L, std::lround((availableSize.width - margin.left - margin.right) / scaleX)), 0};

            const std::wstring &text = this->Text;
            DrawTextW(hdc, text.c_str(), (int)text.size(), &rect, DT_CALCRECT | DT_WORDBREAK);

            desireSize.width  = availableSize.width;
            desireSize.height = (rect.bottom - rect.top) * scaleY + margin.top + margin.bottom;

            ReleaseDC(hwnd, hdc);
        }
    }

    this->SetDesireSize(desireSize);
}
