#include "PasswordBox.h"

sw::PasswordBox::PasswordBox()
    : PasswordChar(
          // get
          [&]() -> const wchar_t & {
              static wchar_t result;
              result = (wchar_t)this->SendMessageW(EM_GETPASSWORDCHAR, 0, 0);
              return result;
          },
          // set
          [&](const wchar_t &value) {
              this->SendMessageW(EM_SETPASSWORDCHAR, value, 0);
          })
{
    this->InitTextBoxBase(WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | ES_PASSWORD | ES_LEFT | ES_AUTOHSCROLL, WS_EX_CLIENTEDGE);
    this->Rect = sw::Rect(0, 0, 100, 24);
}
