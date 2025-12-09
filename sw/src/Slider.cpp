#include "Slider.h"

sw::Slider::Slider()
    : Minimum(
          Property<int>::Init(this)
              .Getter([](Slider *self) -> int {
                  return (int)self->SendMessageW(TBM_GETRANGEMIN, 0, 0);
              })
              .Setter([](Slider *self, int value) {
                  if (self->Minimum != value) {
                      self->SendMessageW(TBM_SETRANGEMIN, TRUE, value);
                      self->RaisePropertyChanged(&Slider::Minimum);
                  }
              })),

      Maximum(
          Property<int>::Init(this)
              .Getter([](Slider *self) -> int {
                  return (int)self->SendMessageW(TBM_GETRANGEMAX, 0, 0);
              })
              .Setter([](Slider *self, int value) {
                  if (self->Maximum != value) {
                      self->SendMessageW(TBM_SETRANGEMAX, TRUE, value);
                      self->RaisePropertyChanged(&Slider::Maximum);
                  }
              })),

      Value(
          Property<int>::Init(this)
              .Getter([](Slider *self) -> int {
                  return (int)self->SendMessageW(TBM_GETPOS, 0, 0);
              })
              .Setter([](Slider *self, int value) {
                  if (self->Value != value) {
                      self->SendMessageW(TBM_SETPOS, TRUE, value);
                      self->OnValueChanged();
                      self->OnEndTrack();
                  }
              })),

      Vertical(
          Property<bool>::Init(this)
              .Getter([](Slider *self) -> bool {
                  return self->GetStyle(TBS_VERT);
              })
              .Setter([](Slider *self, bool value) {
                  self->SetStyle(TBS_VERT, value);
              })),

      ValueTooltips(
          Property<bool>::Init(this)
              .Getter([](Slider *self) -> bool {
                  return self->GetStyle(TBS_TOOLTIPS);
              })
              .Setter([](Slider *self, bool value) {
                  if (self->ValueTooltips != value) {
                      int maximum  = self->Maximum;
                      int minimum  = self->Minimum;
                      int position = self->Value;
                      self->SetStyle(TBS_TOOLTIPS, value);
                      self->ResetHandle();
                      self->SendMessageW(TBM_SETRANGEMIN, FALSE, minimum);
                      self->SendMessageW(TBM_SETRANGEMAX, FALSE, maximum);
                      self->SendMessageW(TBM_SETPOS, TRUE, position);
                  }
              }))
{
    this->InitControl(TRACKBAR_CLASSW, L"", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | TBS_NOTIFYBEFOREMOVE | TBS_DOWNISLEFT, 0);
    this->Rect    = sw::Rect(0, 0, 150, 30);
    this->TabStop = true;
}

bool sw::Slider::OnVerticalScroll(int event, int pos)
{
    switch (event) {
        case TB_BOTTOM:
        case TB_LINEDOWN:
        case TB_LINEUP:
        case TB_PAGEDOWN:
        case TB_PAGEUP:
        case TB_THUMBPOSITION:
        case TB_THUMBTRACK:
        case TB_TOP: {
            this->OnValueChanged();
            break;
        }
        case TB_ENDTRACK: {
            this->OnEndTrack();
            break;
        }
    }
    return true;
}

bool sw::Slider::OnHorizontalScroll(int event, int pos)
{
    switch (event) {
        case TB_BOTTOM:
        case TB_LINEDOWN:
        case TB_LINEUP:
        case TB_PAGEDOWN:
        case TB_PAGEUP:
        case TB_THUMBTRACK:
        case TB_TOP: {
            this->OnValueChanged();
            break;
        }
        case TB_ENDTRACK: {
            this->OnEndTrack();
            break;
        }
    }
    return true;
}

void sw::Slider::OnValueChanged()
{
    this->RaisePropertyChanged(&Slider::Value);
    this->RaiseRoutedEvent(Slider_ValueChanged);
}

void sw::Slider::OnEndTrack()
{
    this->RaiseRoutedEvent(Slider_EndTrack);
}
