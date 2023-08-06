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
