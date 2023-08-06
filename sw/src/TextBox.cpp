#include "TextBox.h"

sw::TextBox::TextBox()
    : MultiLine(
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
          })
{
    this->InitTextBoxBase(WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL | ES_AUTOVSCROLL, WS_EX_CLIENTEDGE);
    this->Rect = sw::Rect(0, 0, 100, 24);
}
