#include "PasswordBox.h"

sw::PasswordBox::PasswordBox()
    : PasswordChar(
          // get
          [this]() -> wchar_t {
              return (wchar_t)this->SendMessageW(EM_GETPASSWORDCHAR, 0, 0);
          },
          // set
          [this](const wchar_t &value) {
              this->SendMessageW(EM_SETPASSWORDCHAR, value, 0);
          })
{
    this->InitTextBoxBase(WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | ES_PASSWORD | ES_LEFT | ES_AUTOHSCROLL, WS_EX_CLIENTEDGE);
    this->Rect = sw::Rect(0, 0, 100, 24);
}
