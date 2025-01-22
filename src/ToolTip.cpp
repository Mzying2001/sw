#include "ToolTip.h"

sw::ToolTip::ToolTip()
    : ToolTip(WS_POPUP)
{
}

sw::ToolTip::ToolTip(DWORD style)
    : InitialDelay(
          // get
          [this]() -> int {
              return int(this->SendMessageW(TTM_GETDELAYTIME, TTDT_INITIAL, 0));
          },
          // set
          [this](const int &value) {
              this->SendMessageW(TTM_SETDELAYTIME, TTDT_AUTOMATIC, static_cast<LPARAM>(value));
          }),

      ToolTipIcon(
          // get
          [this]() -> sw::ToolTipIcon {
              return this->_icon;
          },
          // set
          [this](const sw::ToolTipIcon &value) {
              this->_icon = value;
              this->_UpdateIconAndTitle();
          }),

      ToolTipTitle(
          // get
          [this]() -> std::wstring {
              return this->_title;
          },
          // set
          [this](const std::wstring &value) {
              this->_title = value;
              this->_UpdateIconAndTitle();
          }),

      MaxTipWidth(
          // get
          [this]() -> double {
              int w = int(this->SendMessageW(TTM_GETMAXTIPWIDTH, 0, 0));
              return (w == -1) ? -1 : Dip::PxToDipX(w);
          },
          // set
          [this](const double &value) {
              int w = value < 0 ? -1 : Dip::DipToPxX(value);
              this->SendMessageW(TTM_SETMAXTIPWIDTH, 0, w);
          })
{
    this->InitControl(TOOLTIPS_CLASSW, L"", style, 0);
}

bool sw::ToolTip::SetToolTip(HWND hwnd, const std::wstring &tooltip)
{
    TOOLINFOW info{};
    info.cbSize = sizeof(info);
    info.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
    info.hwnd   = hwnd;
    info.uId    = reinterpret_cast<UINT_PTR>(hwnd);

    if (this->SendMessageW(TTM_GETTOOLINFOW, 0, reinterpret_cast<LPARAM>(&info))) {
        info.lpszText = const_cast<LPWSTR>(tooltip.c_str());
        this->SendMessageW(TTM_UPDATETIPTEXTW, 0, reinterpret_cast<LPARAM>(&info));
        return true;
    } else {
        info.lpszText = const_cast<LPWSTR>(tooltip.c_str());
        return this->SendMessageW(TTM_ADDTOOLW, 0, reinterpret_cast<LPARAM>(&info));
    }
}

bool sw::ToolTip::SetToolTip(const WndBase &wnd, const std::wstring &tooltip)
{
    return this->SetToolTip(wnd.Handle, tooltip);
}

void sw::ToolTip::RemoveAll()
{
    TOOLINFOW info{};
    info.cbSize = sizeof(info);
    while (this->SendMessageW(TTM_ENUMTOOLSW, 0, reinterpret_cast<LPARAM>(&info)))
        this->SendMessageW(TTM_DELTOOLW, 0, reinterpret_cast<LPARAM>(&info));
}

void sw::ToolTip::_UpdateIconAndTitle()
{
    this->SendMessageW(TTM_SETTITLEW, static_cast<WPARAM>(this->_icon), reinterpret_cast<LPARAM>(this->_title.c_str()));
}

sw::BallonToolTip::BallonToolTip()
    : ToolTip(WS_POPUP | TTS_BALLOON)
{
}
