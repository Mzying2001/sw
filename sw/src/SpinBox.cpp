#include "SpinBox.h"
#include "App.h"

sw::SpinBox::SpinBox()
    : Minimum(
          Property<int>::Init(this)
              .Getter([](SpinBox *self) -> int {
                  int result = 0;
                  self->_GetRange32(&result, nullptr);
                  return result;
              })
              .Setter([](SpinBox *self, int value) {
                  if (self->Minimum != value) {
                      int max = 0;
                      self->_GetRange32(nullptr, &max);
                      self->_SetRange32(value, max);
                      self->RaisePropertyChanged(&SpinBox::Minimum);
                  }
              })),

      Maximum(
          Property<int>::Init(this)
              .Getter([](SpinBox *self) -> int {
                  int result = 0;
                  self->_GetRange32(nullptr, &result);
                  return result;
              })
              .Setter([](SpinBox *self, int value) {
                  if (self->Maximum != value) {
                      int min = 0;
                      self->_GetRange32(&min, nullptr);
                      self->_SetRange32(min, value);
                      self->RaisePropertyChanged(&SpinBox::Maximum);
                  }
              })),

      Value(
          Property<int>::Init(this)
              .Getter<&SpinBox::_GetPos32>()
              .Setter<&SpinBox::_SetPos32>()),

      Hexadecimal(
          Property<bool>::Init(this)
              .Getter([](SpinBox *self) -> bool {
                  return ::SendMessageW(self->_hUpDown, UDM_GETBASE, 0, 0) == 16;
              })
              .Setter([](SpinBox *self, bool value) {
                  if (self->Hexadecimal != value) {
                      WPARAM base = value ? 16 : 10;
                      ::SendMessageW(self->_hUpDown, UDM_SETBASE, base, 0);
                      self->RaisePropertyChanged(&SpinBox::Hexadecimal);
                  }
              })),

      Increment(
          Property<uint32_t>::Init(this)
              .Getter([](SpinBox *self) -> uint32_t {
                  if (self->_accels.Count() == 0)
                      self->_accels.Refresh();
                  return self->_accels.GetAt(0).nInc;
              })
              .Setter([](SpinBox *self, uint32_t value) {
                  if (self->_accels.Count() == 0 ||
                      self->_accels.GetAt(0).nSec != 0) {
                      self->_accels.Insert(0, {0, value});
                  } else {
                      auto inc = self->_accels.GetAt(0).nInc;
                      if (inc != value) {
                          self->_accels.SetAt(0, {0, value});
                      }
                  }
              })),

      Accelerations(
          Property<ObservableCollection<UDACCEL> *>::Init(this)
              .Getter([](SpinBox *self) -> ObservableCollection<UDACCEL> * {
                  return &self->_accels;
              }))
{
    InitTextBoxBase(WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | ES_LEFT | ES_AUTOHSCROLL | ES_AUTOVSCROLL, WS_EX_CLIENTEDGE);

    _accels.CollectionChanged +=
        NotifyCollectionChangedEventHandler(*this, &SpinBox::_AccelerationCollectionChangedHandler);

    Rect = sw::Rect{0, 0, 100, 24};
    _InitSpinBox();
}

void sw::SpinBox::OnTextChanged()
{
    RaisePropertyChanged(&SpinBox::Value);
    TextBoxBase::OnTextChanged();
}

void sw::SpinBox::OnHandleChanged(HWND hwnd)
{
    _InitSpinBox();
    TextBoxBase::OnHandleChanged(hwnd);
}

bool sw::SpinBox::OnMove(const Point &newClientPosition)
{
    _UpdateUpDownPos();
    return TextBoxBase::OnMove(newClientPosition);
}

bool sw::SpinBox::OnSize(const Size &newClientSize)
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

    _UpdateUpDownPos();
    _accels.Refresh();

    ::SendMessageW(_hUpDown, UDM_SETBUDDY, reinterpret_cast<WPARAM>(hwnd), 0);
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

void sw::SpinBox::_AccelerationCollectionChangedHandler(
    INotifyCollectionChanged &sender, NotifyCollectionChangedEventArgs &args)
{
    bool incChanged = false;

    switch (args.action) {
        case NotifyCollectionChangedAction::Add:
        case NotifyCollectionChangedAction::Remove:
        case NotifyCollectionChangedAction::Replace:
            if (args.index == 0)
                incChanged = true;
            break;

        case NotifyCollectionChangedAction::Move:
            if (args.index == 0 || args.oldIndex == 0)
                incChanged = true;
            break;

        case NotifyCollectionChangedAction::Reset:
            incChanged = true;
            break;
    }

    auto &vec = _accels.GetInternalVector();

    if (vec.size() == 0 || vec[0].nSec != 0) {
        incChanged = true;
        vec.insert(vec.begin(), {0, 1});
    }

    _SetAccel(vec.size(), &vec[0]);

    if (incChanged) {
        RaisePropertyChanged(&SpinBox::Increment);
    }
}
