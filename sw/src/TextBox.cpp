#include "TextBox.h"

sw::TextBox::TextBox()
    : AutoWrap(
          Property<bool>::Init(this)
              .Getter([](TextBox *self) -> bool {
                  return self->_autoWrap;
              })
              .Setter([](TextBox *self, bool value) {
                  if (self->_autoWrap == value) {
                      return;
                  }
                  self->_autoWrap = value;
                  if (self->MultiLine && self->GetStyle(ES_AUTOHSCROLL) == value) {
                      self->SetStyle(ES_AUTOHSCROLL, !value);
                      self->ResetHandle();
                  }
              })),

      MultiLine(
          Property<bool>::Init(this)
              .Getter([](TextBox *self) -> bool {
                  return self->GetStyle(ES_MULTILINE);
              })
              .Setter([](TextBox *self, bool value) {
                  if (self->MultiLine != value) {
                      self->SetStyle(ES_MULTILINE, value);
                      self->SetStyle(ES_AUTOHSCROLL, !(value && self->_autoWrap));
                      self->ResetHandle();
                  }
              })),

      HorizontalScrollBar(
          Property<bool>::Init(this)
              .Getter([](TextBox *self) -> bool {
                  return self->GetStyle(WS_HSCROLL);
              })
              .Setter([](TextBox *self, bool value) {
                  if (self->HorizontalScrollBar != value) {
                      self->SetStyle(WS_HSCROLL, value);
                      self->ResetHandle();
                  }
              })),

      VerticalScrollBar(
          Property<bool>::Init(this)
              .Getter([](TextBox *self) -> bool {
                  return self->GetStyle(WS_VSCROLL);
              })
              .Setter([](TextBox *self, bool value) {
                  if (self->VerticalScrollBar != value) {
                      self->SetStyle(WS_VSCROLL, value);
                      self->ResetHandle();
                  }
              }))
{
    this->InitTextBoxBase(WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | ES_LEFT | ES_AUTOHSCROLL | ES_AUTOVSCROLL, WS_EX_CLIENTEDGE);
    this->Rect = sw::Rect(0, 0, 100, 24);
}
