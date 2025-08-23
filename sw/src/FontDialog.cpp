#include "FontDialog.h"

sw::FontDialog::FontDialog()
    : Flags(
          // get
          [this]() -> FontDialogFlags {
              return static_cast<FontDialogFlags>(_cf.Flags);
          },
          // set
          [this](const FontDialogFlags &value) {
              _cf.Flags = static_cast<DWORD>(value);
          }),

      Font(
          // get
          [this]() -> sw::Font {
              return _font;
          },
          // set
          [this](const sw::Font &value) {
              _font = value;
          }),

      FontName(
          // get
          [this]() -> std::wstring {
              return _font.name;
          },
          // set
          [this](const std::wstring &value) {
              _font.name = value;
          }),

      FontSize(
          // get
          [this]() -> double {
              return _font.size;
          },
          // set
          [this](const double &value) {
              _font.size = value;
          }),

      FontWeight(
          // get
          [this]() -> sw::FontWeight {
              return _font.weight;
          },
          // set
          [this](const sw::FontWeight &value) {
              _font.weight = value;
          }),

      ShowEffects(
          // get
          [this]() -> bool {
              return (Flags & FontDialogFlags::Effects) == FontDialogFlags::Effects;
          },
          // set
          [this](const bool &value) {
              if (value) {
                  Flags |= FontDialogFlags::Effects;
              } else {
                  Flags &= ~FontDialogFlags::Effects;
              }
          }),

      SelectedColor(
          // get
          [this]() -> Color {
              return _cf.rgbColors;
          },
          // set
          [this](const Color &value) {
              _cf.rgbColors = value;
          })
{
    _font           = Font::GetDefaultFont();
    _cf.lStructSize = sizeof(CHOOSEFONTW);
    _cf.Flags       = DWORD(FontDialogFlags::InitToLogFontStruct | FontDialogFlags::Effects);
}

void sw::FontDialog::Close()
{
}

void sw::FontDialog::Show()
{
}

int sw::FontDialog::ShowDialog(Window *owner)
{
    int result = 0;

    if (owner == nullptr) {
        owner = Window::ActiveWindow;
    }

    HWND hOwner    = owner ? owner->Handle.Get() : NULL;
    auto pCF       = GetChooseFontStruct();
    pCF->hwndOwner = hOwner;

    LOGFONTW logFont = _font;
    pCF->lpLogFont   = &logFont;

    if (result = ChooseFontW(pCF)) {
        _font = logFont;
    }
    return result;
}

int sw::FontDialog::ShowDialog(Window &owner)
{
    return ShowDialog(&owner);
}

CHOOSEFONTW *sw::FontDialog::GetChooseFontStruct()
{
    return &_cf;
}
