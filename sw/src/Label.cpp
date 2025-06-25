#include "Label.h"
#include "Utils.h"
#include <cmath>

sw::Label::Label()
    : HorizontalContentAlignment(
          // get
          [this]() -> sw::HorizontalAlignment {
              DWORD style = this->GetStyle();
              if (style & SS_CENTER) {
                  return sw::HorizontalAlignment::Center;
              } else if (style & SS_RIGHT) {
                  return sw::HorizontalAlignment::Right;
              } else {
                  return sw::HorizontalAlignment::Left;
              }
          },
          // set
          [this](const sw::HorizontalAlignment &value) {
              switch (value) {
                  case sw::HorizontalAlignment::Left: {
                      this->SetStyle(SS_CENTER | SS_RIGHT, false);
                      break;
                  }
                  case sw::HorizontalAlignment::Center: {
                      DWORD style = this->GetStyle();
                      style &= ~(SS_CENTER | SS_RIGHT);
                      style |= SS_CENTER;
                      this->SetStyle(style);
                      break;
                  }
                  case sw::HorizontalAlignment::Right: {
                      DWORD style = this->GetStyle();
                      style &= ~(SS_CENTER | SS_RIGHT);
                      style |= SS_RIGHT;
                      this->SetStyle(style);
                      break;
                  }
                  default: {
                      break;
                  }
              }
              this->Redraw();
          }),

      VerticalContentAlignment(
          // get
          [this]() -> sw::VerticalAlignment {
              return this->GetStyle(SS_CENTERIMAGE) ? sw::VerticalAlignment::Center : sw::VerticalAlignment::Top;
          },
          // set
          [this](const sw::VerticalAlignment &value) {
              this->SetStyle(SS_CENTERIMAGE, value == sw::VerticalAlignment::Center);
          }),

      TextTrimming(
          // get
          [this]() -> sw::TextTrimming {
              DWORD style = this->GetStyle();
              if ((style & SS_WORDELLIPSIS) == SS_WORDELLIPSIS) {
                  return sw::TextTrimming::WordEllipsis;
              } else if (style & SS_ENDELLIPSIS) {
                  return sw::TextTrimming::EndEllipsis;
              } else {
                  return sw::TextTrimming::None;
              }
          },
          // set
          [this](const sw::TextTrimming &value) {
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
                      DWORD style = this->GetStyle();
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
          [this]() -> bool {
              return this->GetStyle(SS_EDITCONTROL);
          },
          // set
          [this](const bool &value) {
              this->SetStyle(SS_EDITCONTROL, value);
          }),

      AutoSize(
          // get
          [this]() -> bool {
              return this->_autoSize;
          },
          // set
          [this](const bool &value) {
              if (value) {
                  this->_autoSize = true;
                  this->LayoutUpdateCondition |= sw::LayoutUpdateCondition::TextChanged | sw::LayoutUpdateCondition::FontChanged;
                  this->NotifyLayoutUpdated();
              } else {
                  this->_autoSize = false;
                  this->LayoutUpdateCondition &= ~(sw::LayoutUpdateCondition::TextChanged | sw::LayoutUpdateCondition::FontChanged);
              }
          })
{
    this->SetText(L"Label");
    this->_UpdateTextSize();
    this->_ResizeToTextSize();
    this->Transparent      = true;
    this->InheritTextColor = true;
    this->LayoutUpdateCondition |= sw::LayoutUpdateCondition::TextChanged | sw::LayoutUpdateCondition::FontChanged;
}

void sw::Label::_UpdateTextSize()
{
    HWND hwnd = this->Handle;
    HDC hdc   = GetDC(hwnd);

    SelectObject(hdc, this->GetFontHandle());

    RECT rect{};
    std::wstring &text = this->GetText();
    DrawTextW(hdc, text.c_str(), (int)text.size(), &rect, DT_CALCRECT);

    sw::Rect textRect = rect;
    this->_textSize   = Size(textRect.width, textRect.height);

    ReleaseDC(hwnd, hdc);
}

void sw::Label::_ResizeToTextSize()
{
    this->Resize(this->_textSize);
}

bool sw::Label::OnSize(Size newClientSize)
{
    this->Redraw();
    return StaticControl::OnSize(newClientSize);
}

void sw::Label::OnTextChanged()
{
    this->_UpdateTextSize();
    this->Control::OnTextChanged();
}

void sw::Label::FontChanged(HFONT hfont)
{
    this->_UpdateTextSize();
    this->Control::FontChanged(hfont);
}

sw::Size sw::Label::MeasureOverride(const Size &availableSize)
{
    if (!this->_autoSize) {
        return this->UIElement::MeasureOverride(availableSize);
    }

    Size desireSize = this->_textSize;
    if (availableSize.width < desireSize.width) {

        if (this->TextTrimming.Get() != sw::TextTrimming::None) {
            desireSize.width = availableSize.width;

        } else if (this->AutoWrap) {
            HWND hwnd = this->Handle;
            HDC hdc   = GetDC(hwnd);

            SelectObject(hdc, this->GetFontHandle());

            std::wstring &text = this->GetText();
            RECT rect{0, 0, Utils::Max(0, Dip::DipToPxX(availableSize.width)), 0};
            DrawTextW(hdc, text.c_str(), (int)text.size(), &rect, DT_CALCRECT | DT_WORDBREAK);

            desireSize.width  = availableSize.width;
            desireSize.height = Dip::PxToDipY(rect.bottom - rect.top);
            ReleaseDC(hwnd, hdc);
        }
    }
    return desireSize;
}
