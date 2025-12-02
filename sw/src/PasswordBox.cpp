#include "PasswordBox.h"

sw::PasswordBox::PasswordBox()
    : PasswordChar(
          Property<wchar_t>::Init(this)
              .Getter([](PasswordBox *self) -> wchar_t {
                  return (wchar_t)self->SendMessageW(EM_GETPASSWORDCHAR, 0, 0);
              })
              .Setter([](PasswordBox *self, wchar_t value) {
                  self->SendMessageW(EM_SETPASSWORDCHAR, value, 0);
              }))
{
    this->InitTextBoxBase(WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | ES_PASSWORD | ES_LEFT | ES_AUTOHSCROLL, WS_EX_CLIENTEDGE);
    this->Rect = sw::Rect(0, 0, 100, 24);
}
