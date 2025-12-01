#include "ColorDialog.h"

namespace
{
    /**
     * @brief 默认自定义颜色数组
     */
    COLORREF _defaultCustomColors[16] = {
        RGB(255, 255, 255),
        RGB(255, 255, 255),
        RGB(255, 255, 255),
        RGB(255, 255, 255),
        RGB(255, 255, 255),
        RGB(255, 255, 255),
        RGB(255, 255, 255),
        RGB(255, 255, 255),
        RGB(255, 255, 255),
        RGB(255, 255, 255),
        RGB(255, 255, 255),
        RGB(255, 255, 255),
        RGB(255, 255, 255),
        RGB(255, 255, 255),
        RGB(255, 255, 255),
        RGB(255, 255, 255),
    };
}

sw::ColorDialog::ColorDialog()
    : Flags(
          Property<ColorDialogFlags>::Init(this)
              .Getter([](ColorDialog *self) -> ColorDialogFlags {
                  return static_cast<ColorDialogFlags>(self->_cc.Flags);
              })
              .Setter([](ColorDialog *self, ColorDialogFlags value) {
                  self->_cc.Flags = static_cast<DWORD>(value);
              })),

      SelectedColor(
          Property<Color>::Init(this)
              .Getter([](ColorDialog *self) -> Color {
                  return self->_cc.rgbResult;
              })
              .Setter([](ColorDialog *self, const Color &value) {
                  self->_cc.rgbResult = value;
              })),

      FullOpen(
          Property<bool>::Init(this)
              .Getter([](ColorDialog *self) -> bool {
                  return (self->Flags & ColorDialogFlags::FullOpen) == ColorDialogFlags::FullOpen;
              })
              .Setter([](ColorDialog *self, bool value) {
                  if (value) {
                      self->Flags |= ColorDialogFlags::FullOpen;
                  } else {
                      self->Flags &= ~ColorDialogFlags::FullOpen;
                  }
              })),

      CustomColors(
          Property<COLORREF *>::Init(this)
              .Getter([](ColorDialog *self) -> COLORREF * {
                  return self->_cc.lpCustColors;
              })
              .Setter([](ColorDialog *self, COLORREF *value) {
                  self->_cc.lpCustColors = value ? value : _defaultCustomColors;
              }))
{
    _cc.lStructSize  = sizeof(CHOOSECOLORW);
    _cc.Flags        = DWORD(ColorDialogFlags::RgbInit);
    _cc.rgbResult    = Color(KnownColor::Black);
    _cc.lpCustColors = _defaultCustomColors;
}

void sw::ColorDialog::Close()
{
}

void sw::ColorDialog::Show()
{
}

int sw::ColorDialog::ShowDialog(Window *owner)
{
    if (owner == nullptr) {
        owner = Window::ActiveWindow;
    }

    HWND hOwner    = owner ? owner->Handle.Get() : NULL;
    auto pCC       = GetChooseColorStruct();
    pCC->hwndOwner = hOwner;

    return ChooseColorW(pCC);
}

int sw::ColorDialog::ShowDialog(Window &owner)
{
    return ShowDialog(&owner);
}

CHOOSECOLORW *sw::ColorDialog::GetChooseColorStruct()
{
    return &_cc;
}
