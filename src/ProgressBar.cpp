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
          // get
          [this]() -> uint16_t {
              return (uint16_t)this->SendMessageW(PBM_GETRANGE, TRUE, 0);
          },
          // set
          [this](const uint16_t &value) {
              uint16_t maximum = this->Maximum;
              this->SendMessageW(PBM_SETRANGE, 0, MAKELPARAM(value, maximum));
          }),

      Maximum(
          // get
          [this]() -> uint16_t {
              return (uint16_t)this->SendMessageW(PBM_GETRANGE, FALSE, 0);
          },
          // set
          [this](const uint16_t &value) {
              uint16_t minimum = this->Minimum;
              this->SendMessageW(PBM_SETRANGE, 0, MAKELPARAM(minimum, value));
          }),

      Value(
          // get
          [this]() -> uint16_t {
              return (uint16_t)this->SendMessageW(PBM_GETPOS, 0, 0);
          },
          // set
          [this](const uint16_t &value) {
              this->SendMessageW(PBM_SETPOS, value, 0);
          }),

      State(
          // get
          [this]() -> ProgressBarState {
              return (ProgressBarState)this->SendMessageW(PBM_GETSTATE, 0, 0);
          },
          // set
          [this](const ProgressBarState &value) {
              this->SendMessageW(PBM_SETSTATE, (WPARAM)value, 0);
          }),

      Vertical(
          // get
          [this]() -> bool {
              return this->GetStyle(PBS_VERTICAL);
          },
          // set
          [this](const bool &value) {
              auto pos = this->Value.Get();
              this->SetStyle(PBS_VERTICAL, value);
              this->Value = pos;
          })
{
    this->InitControl(PROGRESS_CLASSW, L"", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | PBS_SMOOTH | PBS_SMOOTHREVERSE, 0);
    this->Rect = sw::Rect(0, 0, 150, 20);
}
