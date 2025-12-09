#include "ProgressBar.h"

#if !defined(PBM_SETSTATE) // g++
#define PBM_SETSTATE (WM_USER + 16)
#endif

#if !defined(PBM_GETSTATE) // g++
#define PBM_GETSTATE (WM_USER + 17)
#endif

#if !defined(PBS_SMOOTHREVERSE) // g++
#define PBS_SMOOTHREVERSE 0x10
#endif

sw::ProgressBar::ProgressBar()
    : Minimum(
          Property<uint16_t>::Init(this)
              .Getter([](ProgressBar *self) -> uint16_t {
                  return (uint16_t)self->SendMessageW(PBM_GETRANGE, TRUE, 0);
              })
              .Setter([](ProgressBar *self, uint16_t value) {
                  if (self->Minimum != value) {
                      uint16_t maximum = self->Maximum;
                      self->SendMessageW(PBM_SETRANGE, 0, MAKELPARAM(value, maximum));
                      self->RaisePropertyChanged(&ProgressBar::Minimum);
                  }
              })),

      Maximum(
          Property<uint16_t>::Init(this)
              .Getter([](ProgressBar *self) -> uint16_t {
                  return (uint16_t)self->SendMessageW(PBM_GETRANGE, FALSE, 0);
              })
              .Setter([](ProgressBar *self, uint16_t value) {
                  if (self->Maximum != value) {
                      uint16_t minimum = self->Minimum;
                      self->SendMessageW(PBM_SETRANGE, 0, MAKELPARAM(minimum, value));
                      self->RaisePropertyChanged(&ProgressBar::Maximum);
                  }
              })),

      Value(
          Property<uint16_t>::Init(this)
              .Getter([](ProgressBar *self) -> uint16_t {
                  return (uint16_t)self->SendMessageW(PBM_GETPOS, 0, 0);
              })
              .Setter([](ProgressBar *self, uint16_t value) {
                  if (self->Value != value) {
                      self->SendMessageW(PBM_SETPOS, value, 0);
                      self->RaisePropertyChanged(&ProgressBar::Value);
                  }
              })),

      State(
          Property<ProgressBarState>::Init(this)
              .Getter([](ProgressBar *self) -> ProgressBarState {
                  return (ProgressBarState)self->SendMessageW(PBM_GETSTATE, 0, 0);
              })
              .Setter([](ProgressBar *self, ProgressBarState value) {
                  self->SendMessageW(PBM_SETSTATE, (WPARAM)value, 0);
              })),

      Vertical(
          Property<bool>::Init(this)
              .Getter([](ProgressBar *self) -> bool {
                  return self->GetStyle(PBS_VERTICAL);
              })
              .Setter([](ProgressBar *self, bool value) {
                  auto pos = self->Value.Get();
                  self->SetStyle(PBS_VERTICAL, value);
                  self->SendMessageW(PBM_SETPOS, pos, 0);
              }))
{
    InitControl(PROGRESS_CLASSW, L"", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | PBS_SMOOTH | PBS_SMOOTHREVERSE, 0);
    Rect = sw::Rect(0, 0, 150, 20);
}
