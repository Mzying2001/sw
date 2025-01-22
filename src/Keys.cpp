#include "Keys.h"

sw::KeyFlags::KeyFlags(LPARAM lParam)
{
    this->repeatCount      = (lParam >> 0) & 0xFFFF;
    this->scanCode         = (lParam >> 16) & 0xFF;
    this->isExtendedKey    = (lParam >> 24) & 0x01;
    this->contextCode      = (lParam >> 29) & 0x01;
    this->previousKeyState = (lParam >> 30) & 0x01;
    this->transitionState  = (lParam >> 31) & 0x01;
}
