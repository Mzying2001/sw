#include "Slider.h"

sw::Slider::Slider()
{
    this->InitControl(TRACKBAR_CLASSW, L"", WS_CHILD | WS_VISIBLE, 0);
    this->Rect = sw::Rect(0, 0, 150, 30);
}
