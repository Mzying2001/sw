#include "TextBox.h"

sw::TextBox::TextBox()
    : ReadOnly(
          // get
          [&]() -> const bool & {
              static bool result;
              result = this->GetStyle(ES_READONLY);
              return result;
          },
          // set
          [&](const bool &value) {
              this->SendMessageW(EM_SETREADONLY, value, NULL);
          }),

      MultiLine(
          // get
          [&]() -> const bool & {
              static bool result;
              result = this->GetStyle(ES_MULTILINE);
              return result;
          },
          // set
          [&](const bool &value) {
              if (this->MultiLine != value) {
                  this->SetStyle(ES_MULTILINE, value);
                  this->ResetHandle();
              }
          }),

      HorizontalContentAlignment(
          // get
          [&]() -> const sw::HorizontalAlignment & {
              static sw::HorizontalAlignment result;
              LONG_PTR style = this->GetStyle();
              if (style & ES_CENTER) {
                  result = sw::HorizontalAlignment::Center;
              } else if (style & ES_RIGHT) {
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
                      this->SetStyle(ES_CENTER | ES_RIGHT, false);
                      break;
                  }
                  case sw::HorizontalAlignment::Center: {
                      LONG_PTR style = this->GetStyle();
                      style &= ~(ES_CENTER | ES_RIGHT);
                      style |= ES_CENTER;
                      this->SetStyle(style);
                      break;
                  }
                  case sw::HorizontalAlignment::Right: {
                      LONG_PTR style = this->GetStyle();
                      style &= ~(ES_CENTER | ES_RIGHT);
                      style |= ES_RIGHT;
                      this->SetStyle(style);
                      break;
                  }
              }
              this->Redraw();
          })
{
    this->InitControl(L"EDIT", L"", WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL | ES_AUTOVSCROLL, WS_EX_CLIENTEDGE);
    this->Rect = sw::Rect(0, 0, 100, 24);
}

std::wstring &sw::TextBox::GetText()
{
    std::wstring &refText = this->WndBase::GetText();

    if (!this->_isTextChanged) {
        return refText;
    }

    HWND hwnd = this->Handle;
    int len   = GetWindowTextLengthW(hwnd);

    if (len > 0) {
        wchar_t *buf = new wchar_t[len + 1];
        GetWindowTextW(hwnd, buf, len + 1);
        refText = buf;
        delete[] buf;
    } else {
        refText = L"";
    }

    this->_isTextChanged = false;
    return refText;
}

void sw::TextBox::OnCommand(int code)
{
    switch (code) {
        case EN_CHANGE: {
            this->_isTextChanged = true;
            break;
        }

        default:
            break;
    }
}
