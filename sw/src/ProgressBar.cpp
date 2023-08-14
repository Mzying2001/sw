#include "ProgressBar.h"

sw::ProgressBar::ProgressBar()
{
    this->InitControl(PROGRESS_CLASSW, L"", WS_CHILD | WS_VISIBLE | PBS_SMOOTH, 0);
    this->Rect = sw::Rect(0, 0, 150, 24);
}
