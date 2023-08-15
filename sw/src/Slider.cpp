#include "Slider.h"

sw::Slider::Slider()
    : Minimum(
          // get
          [&]() -> const int & {
              static int result;
              result = (int)this->SendMessageW(TBM_GETRANGEMIN, 0, 0);
              return result;
          },
          // set
          [&](const int &value) {
              this->SendMessageW(TBM_SETRANGEMIN, TRUE, value);
          }),

      Maximum(
          // get
          [&]() -> const int & {
              static int result;
              result = (int)this->SendMessageW(TBM_GETRANGEMAX, 0, 0);
              return result;
          },
          // set
          [&](const int &value) {
              this->SendMessageW(TBM_SETRANGEMAX, TRUE, value);
          }),

      Value(
          // get
          [&]() -> const int & {
              static int result;
              result = (int)this->SendMessageW(TBM_GETPOS, 0, 0);
              return result;
          },
          // set
          [&](const int &value) {
              this->SendMessageW(TBM_SETPOS, TRUE, value);
          }),

      Vertical(
          // get
          [&]() -> const bool & {
              static bool result;
              result = this->GetStyle(TBS_VERT);
              return result;
          },
          // set
          [&](const bool &value) {
              this->SetStyle(TBS_VERT, value);
          }),

      ValueTooltips(
          // get
          [&]() -> const bool & {
              static bool result;
              result = this->GetStyle(TBS_TOOLTIPS);
              return result;
          },
          // set
          [&](const bool &value) {
              if (this->ValueTooltips != value) {
                  int maximum  = this->Maximum;
                  int minimum  = this->Minimum;
                  int position = this->Value;
                  this->SetStyle(TBS_TOOLTIPS, value);
                  this->ResetHandle();
                  this->SendMessageW(TBM_SETRANGEMIN, FALSE, minimum);
                  this->SendMessageW(TBM_SETRANGEMAX, FALSE, maximum);
                  this->SendMessageW(TBM_SETPOS, TRUE, position);
              }
          })
{
    this->InitControl(TRACKBAR_CLASSW, L"", WS_CHILD | WS_VISIBLE | TBS_DOWNISLEFT, 0);
    this->Rect = sw::Rect(0, 0, 150, 30);
}
