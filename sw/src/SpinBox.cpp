#include "SpinBox.h"

sw::SpinBox::SpinBox()
    : Minimum(
          // get
          [this]() -> int {
              int result = 0;
              _GetRange32(&result, nullptr);
              return result;
          },
          // set
          [this](const int &value) {
              int max = 0;
              _GetRange32(nullptr, &max);
              _SetRange32(value, max);
          }),

      Maximum(
          // get
          [this]() -> int {
              int result = 0;
              _GetRange32(nullptr, &result);
              return result;
          },
          // set
          [this](const int &value) {
              int min = 0;
              _GetRange32(&min, nullptr);
              _SetRange32(min, value);
          }),

      Value(
          // get
          [this]() -> int {
              return _GetPos32();
          },
          // set
          [this](const int &value) {
              _SetPos32(value);
          }),

      Hexadecimal(
          // get
          [this]() -> bool {
              return ::SendMessageW(_hUpDown, UDM_GETBASE, 0, 0) == 16;
          },
          // set
          [this](const bool &value) {
              WPARAM base = value ? 16 : 10;
              ::SendMessageW(_hUpDown, UDM_SETBASE, base, 0);
          }),

      Increment(
          // get
          [this]() -> uint32_t {
              if (_accels.empty())
                  _InitAccels();
              return _accels[0].nInc;
          },
          // set
          [this](const uint32_t &value) {
              if (_accels.empty()) {
                  _accels.push_back({0, value});
              } else {
                  _accels[0].nInc = value;
              }
              _SetAccel(_accels.size(), &_accels[0]);
          })
{
    InitTextBoxBase(WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | ES_LEFT | ES_AUTOHSCROLL | ES_AUTOVSCROLL, WS_EX_CLIENTEDGE);
    Rect = sw::Rect{0, 0, 100, 24};
    _InitSpinBox();
}

sw::SpinBox &sw::SpinBox::AddAccel(uint32_t seconds, uint32_t increment)
{
    if (seconds <= 0) {
        Increment = increment;
    } else {
        _accels.push_back({seconds, increment});
        _SetAccel(_accels.size(), &_accels[0]);
    }
    return *this;
}

void sw::SpinBox::ClearAccels()
{
    _InitAccels();
}

void sw::SpinBox::OnHandleChanged(HWND hwnd)
{
    _InitSpinBox();
}

bool sw::SpinBox::OnSize(Size newClientSize)
{
    _UpdateUpDownPos();
    return TextBoxBase::OnSize(newClientSize);
}

bool sw::SpinBox::OnKillFocus(HWND hNextFocus)
{
    _SetPos32(_GetPos32());
    return TextBoxBase::OnKillFocus(hNextFocus);
}

void sw::SpinBox::_InitSpinBox()
{
    _InitUpDownControl();
    _SetRange32(0, 100);
    _SetPos32(0);
}

void sw::SpinBox::_InitUpDownControl()
{
    if (_hUpDown != NULL) {
        DestroyWindow(_hUpDown);
        _hUpDown = NULL;
    }

    HWND hwnd = Handle;

    _hUpDown = CreateWindowExW(
        0, UPDOWN_CLASSW, L"",
        WS_CHILD | WS_VISIBLE | UDS_ARROWKEYS | UDS_SETBUDDYINT | UDS_NOTHOUSANDS,
        0, 0, 20, 0,
        hwnd, NULL, App::Instance, NULL);

    _InitAccels();
    _UpdateUpDownPos();

    ::SendMessageW(_hUpDown, UDM_SETBUDDY, reinterpret_cast<WPARAM>(hwnd), 0);
}

void sw::SpinBox::_InitAccels()
{
    _accels.clear();
    _accels.push_back({0, 1});
    _SetAccel(_accels.size(), &_accels[0]);
}

int sw::SpinBox::_GetPos32()
{
    return static_cast<int>(::SendMessageW(_hUpDown, UDM_GETPOS32, 0, 0));
}

void sw::SpinBox::_SetPos32(int pos)
{
    ::SendMessageW(_hUpDown, UDM_SETPOS32, 0, static_cast<LPARAM>(pos));
}

void sw::SpinBox::_GetRange32(int *min, int *max)
{
    ::SendMessageW(_hUpDown, UDM_GETRANGE32, reinterpret_cast<WPARAM>(min), reinterpret_cast<LPARAM>(max));
}

void sw::SpinBox::_SetRange32(int min, int max)
{
    ::SendMessageW(_hUpDown, UDM_SETRANGE32, static_cast<WPARAM>(min), static_cast<LPARAM>(max));
}

void sw::SpinBox::_SetAccel(size_t count, UDACCEL *pAccels)
{
    ::SendMessageW(_hUpDown, UDM_SETACCEL, static_cast<WPARAM>(count), reinterpret_cast<LPARAM>(pAccels));
}

void sw::SpinBox::_UpdateUpDownPos()
{
    RECT rtUpDown;
    GetWindowRect(_hUpDown, &rtUpDown);

    RECT rtClient;
    GetClientRect(Handle, &rtClient);

    if (_upDownWidth != rtUpDown.right - rtUpDown.left) {
        _upDownWidth = rtUpDown.right - rtUpDown.left;
        SendMessageW(EM_SETMARGINS, EC_RIGHTMARGIN, MAKELPARAM(0, _upDownWidth));
    }

    SetWindowPos(
        _hUpDown, NULL, rtClient.right - _upDownWidth, 0,
        _upDownWidth, rtClient.bottom, SWP_NOZORDER | SWP_NOACTIVATE);

    InvalidateRect(_hUpDown, NULL, FALSE);
}
