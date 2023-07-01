#include "Control.h"

sw::Control::Control()
    : BackColor(
          // get
          [&]() -> const Color & {
              return this->_backColor;
          },
          // set
          [&](const Color &value) {
              this->_backColor = value;
              this->Redraw();
          }),

      TextColor(
          // get
          [&]() -> const Color & {
              return this->_textColor;
          },
          // set
          [&](const Color &value) {
              this->_textColor = value;
              this->Redraw();
          })
{
    HWND hwnd        = this->Handle;
    HDC hdc          = GetDC(hwnd);
    this->_backColor = GetBkColor(hdc);
    this->_textColor = GetTextColor(hdc);
    ReleaseDC(hwnd, hdc);
}

LRESULT sw::Control::CtlColor(HDC hdc, HWND hwnd)
{
    SetTextColor(hdc, this->_textColor);
    SetBkColor(hdc, this->_backColor);
    return (LRESULT)CreateSolidBrush(this->_backColor);
}
