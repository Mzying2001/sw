#include "StaticControl.h"

sw::StaticControl::StaticControl()
    : Notify(
          // get
          [&]() -> const bool & {
              static bool result;
              result = this->GetStyle(SS_NOTIFY);
              return result;
          },
          // set
          [&](const bool &value) {
              this->SetStyle(SS_NOTIFY, value);
          })
{
    this->InitControl(L"STATIC", L"", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, 0);
}
