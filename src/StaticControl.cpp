#include "StaticControl.h"

sw::StaticControl::StaticControl()
    : Notify(
          // get
          [this]() -> bool {
              return this->GetStyle(SS_NOTIFY);
          },
          // set
          [this](const bool &value) {
              this->SetStyle(SS_NOTIFY, value);
          })
{
    this->InitControl(L"STATIC", L"", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, 0);
}
