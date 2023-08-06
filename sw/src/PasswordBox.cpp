#include "PasswordBox.h"

sw::PasswordBox::PasswordBox()
    : PasswordChar(
          // get
          [&]() -> const wchar_t & {
              static wchar_t result;
              result = (wchar_t)this->SendMessageW(EM_GETPASSWORDCHAR, NULL, NULL);
              return result;
          },
          // set
          [&](const wchar_t &value) {
              this->SendMessageW(EM_SETPASSWORDCHAR, value, NULL);
          })
{
    this->InitTextBoxBase(WS_CHILD | WS_VISIBLE | ES_PASSWORD | ES_LEFT | ES_AUTOHSCROLL, WS_EX_CLIENTEDGE);
    this->Rect = sw::Rect(0, 0, 100, 24);
}
