#include "ToolTip.h"

sw::ToolTip::ToolTip()
    : ToolTip(WS_POPUP)
{
}

sw::ToolTip::ToolTip(DWORD style)
    : InitialDelay(
          Property<int>::Init(this)
              .Getter([](ToolTip *self) -> int {
                  return int(self->SendMessageW(TTM_GETDELAYTIME, TTDT_INITIAL, 0));
              })
              .Setter([](ToolTip *self, int value) {
                  if (self->InitialDelay != value) {
                      self->SendMessageW(TTM_SETDELAYTIME, TTDT_AUTOMATIC, static_cast<LPARAM>(value));
                      self->RaisePropertyChanged(&ToolTip::InitialDelay);
                  }
              })),

      ToolTipIcon(
          Property<sw::ToolTipIcon>::Init(this)
              .Getter([](ToolTip *self) -> sw::ToolTipIcon {
                  return self->_icon;
              })
              .Setter([](ToolTip *self, sw::ToolTipIcon value) {
                  if (self->_icon != value) {
                      self->_icon = value;
                      self->_UpdateIconAndTitle();
                      self->RaisePropertyChanged(&ToolTip::ToolTipIcon);
                  }
              })),

      ToolTipTitle(
          Property<std::wstring>::Init(this)
              .Getter([](ToolTip *self) -> std::wstring {
                  return self->_title;
              })
              .Setter([](ToolTip *self, const std::wstring &value) {
                  if (self->_title != value) {
                      self->_title = value;
                      self->_UpdateIconAndTitle();
                      self->RaisePropertyChanged(&ToolTip::ToolTipTitle);
                  }
              })),

      MaxTipWidth(
          Property<double>::Init(this)
              .Getter([](ToolTip *self) -> double {
                  int w = int(self->SendMessageW(TTM_GETMAXTIPWIDTH, 0, 0));
                  return (w == -1) ? -1 : Dip::PxToDipX(w);
              })
              .Setter([](ToolTip *self, double value) {
                  if (self->MaxTipWidth != value) {
                      int w = value < 0 ? -1 : Dip::DipToPxX(value);
                      self->SendMessageW(TTM_SETMAXTIPWIDTH, 0, w);
                      self->RaisePropertyChanged(&ToolTip::MaxTipWidth);
                  }
              }))
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
