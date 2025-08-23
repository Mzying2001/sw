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
          // get
          [this]() -> ColorDialogFlags {
              return static_cast<ColorDialogFlags>(_cc.Flags);
          },
          // set
          [this](const ColorDialogFlags &value) {
              _cc.Flags = static_cast<DWORD>(value);
          }),

      SelectedColor(
          // get
          [this]() -> Color {
              return _cc.rgbResult;
          },
          // set
          [this](const Color &value) {
              _cc.rgbResult = value;
          }),

      FullOpen(
          // get
          [this]() -> bool {
              return (Flags & ColorDialogFlags::FullOpen) == ColorDialogFlags::FullOpen;
          },
          // set
          [this](const bool &value) {
              if (value) {
                  Flags |= ColorDialogFlags::FullOpen;
              } else {
                  Flags &= ~ColorDialogFlags::FullOpen;
              }
          }),

      CustomColors(
          // get
          [this]() -> COLORREF * {
              return _cc.lpCustColors;
          },
          // set
          [this](COLORREF *value) {
              _cc.lpCustColors = value ? value : _defaultCustomColors;
          })
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
