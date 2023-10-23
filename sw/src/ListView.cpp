#include "ListView.h"

sw::ListView::ListView()
{
    this->InitControl(WC_LISTVIEWW, L"", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_BORDER | LVS_REPORT, 0);
    this->Rect    = sw::Rect(0, 0, 200, 200);
    this->TabStop = true;
}

// TODO:
