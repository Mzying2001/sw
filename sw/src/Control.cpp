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
}

bool sw::Control::OnSetCursor(HWND hwnd, int hitTest, int message, bool &useDefaultWndProc)
{
    if (this->_useDefaultCursor) {
        return false;
    }
    ::SetCursor(this->_hCursor);
    useDefaultWndProc = false;
    return true;
}

/*void sw::Control::HandleInitialized(HWND hwnd)
{
    HDC hdc          = GetDC(hwnd);
    this->_backColor = GetBkColor(hdc);
    this->_textColor = GetTextColor(hdc);
    ReleaseDC(hwnd, hdc);
}*/

LRESULT sw::Control::CtlColor(HDC hdc, HWND hwnd)
{
    static HBRUSH hBrush = NULL;

    if (hBrush != NULL) {
        DeleteObject(hBrush);
    }

    hBrush = CreateSolidBrush(this->_backColor);

    SetTextColor(hdc, this->_textColor);
    SetBkColor(hdc, this->_backColor);
    return (LRESULT)hBrush;
}

void sw::Control::SetCursor(HCURSOR hCursor)
{
    this->_hCursor          = hCursor;
    this->_useDefaultCursor = false;
}

void sw::Control::SetCursor(StandardCursor cursor)
{
    this->SetCursor(CursorHelper::GetCursorHandle(cursor));
}

void sw::Control::ResetCursor()
{
    this->_hCursor          = NULL;
    this->_useDefaultCursor = true;
}
