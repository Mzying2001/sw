#include "TextBoxBase.h"

sw::TextBoxBase::TextBoxBase()
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
}

void sw::TextBoxBase::InitTextBoxBase(DWORD dwStyle, DWORD dwExStyle)
{
    this->InitControl(L"EDIT", L"", dwStyle, dwExStyle);
}

std::wstring &sw::TextBoxBase::GetText()
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

void sw::TextBoxBase::OnCommand(int code)
{
    switch (code) {
        case EN_CHANGE: {
            this->OnTextChanged();
            break;
        }

        default:
            break;
    }
}

void sw::TextBoxBase::OnTextChanged()
{
    this->_isTextChanged = true;
}

void sw::TextBoxBase::Select(int start, int length)
{
    this->SendMessageW(EM_SETSEL, start, start + length);
}

void sw::TextBoxBase::SelectAll()
{
    this->SendMessageW(EM_SETSEL, 0, -1);
}
