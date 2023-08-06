#include "TextBox.h"

sw::TextBox::TextBox()
    : AutoWrap(
          // get
          [&]() -> const bool & {
              return this->_autoWrap;
          },
          // set
          [&](const bool &value) {
              if (this->_autoWrap == value) {
                  return;
              }
              this->_autoWrap = value;
              if (this->MultiLine && this->GetStyle(ES_AUTOHSCROLL) == value) {
                  this->SetStyle(ES_AUTOHSCROLL, !value);
                  this->ResetHandle();
              }
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
                  this->SetStyle(ES_AUTOHSCROLL, !(value && this->_autoWrap));
                  this->ResetHandle();
              }
          }),

      HorizontalScrollBar(
          // get
          [&]() -> const bool & {
              static bool result;
              result = this->GetStyle(WS_HSCROLL);
              return result;
          },
          // set
          [&](const bool &value) {
              if (this->HorizontalScrollBar != value) {
                  this->SetStyle(WS_HSCROLL, value);
                  this->ResetHandle();
              }
          }),

      VerticalScrollBar(
          // get
          [&]() -> const bool & {
              static bool result;
              result = this->GetStyle(WS_VSCROLL);
              return result;
          },
          // set
          [&](const bool &value) {
              if (this->VerticalScrollBar != value) {
                  this->SetStyle(WS_VSCROLL, value);
                  this->ResetHandle();
              }
          })
{
    this->InitTextBoxBase(WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL | ES_AUTOVSCROLL, WS_EX_CLIENTEDGE);
    this->Rect = sw::Rect(0, 0, 100, 24);
}
