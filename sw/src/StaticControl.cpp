#include "StaticControl.h"

sw::StaticControl::StaticControl()
{
    this->InitControl(L"STATIC", L"", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, 0);
}
