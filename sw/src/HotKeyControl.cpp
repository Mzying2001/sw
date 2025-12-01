#include "HotKeyControl.h"

sw::HotKeyControl::HotKeyControl()
    : Value(
          Property<HotKey>::Init(this)
              .Getter([](HotKeyControl *self) -> HotKey {
                  return self->_value;
              })
              .Setter([](HotKeyControl *self, const HotKey &value) {
                  if (value.key != self->_value.key && value.modifier != self->_value.modifier) {
                      WORD val = MAKEWORD(value.key, value.modifier);
                      self->SendMessageW(HKM_SETHOTKEY, val, 0);
                      self->_UpdateValue();
                      self->OnValueChanged(self->_value);
                  }
              }))
{
    this->InitControl(HOTKEY_CLASSW, L"", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, 0);
    this->_UpdateValue();
    this->Rect = {0, 0, 100, 24};
}

void sw::HotKeyControl::SetRules(HotKeyCombination invalidComb, HotKeyModifier defaultModifier)
{
    this->SendMessageW(HKM_SETRULES, static_cast<WPARAM>(invalidComb), static_cast<LPARAM>(defaultModifier));
}

void sw::HotKeyControl::OnCommand(int code)
{
    if (code == EN_CHANGE) {
        this->_UpdateValue();
        this->OnValueChanged(this->_value);
    }
}

void sw::HotKeyControl::OnValueChanged(HotKey value)
{
    HotKeyValueChangedEventArgs arg{value.key, value.modifier};
    this->RaiseRoutedEvent(arg);
}

void sw::HotKeyControl::_UpdateValue()
{
    WORD val     = LOWORD(this->SendMessageW(HKM_GETHOTKEY, 0, 0));
    this->_value = {/*key*/ VirtualKey(LOBYTE(val)), /*modifier*/ HotKeyModifier(HIBYTE(val))};
}
