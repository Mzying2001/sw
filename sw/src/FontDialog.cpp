#include "FontDialog.h"

sw::FontDialog::FontDialog()
    : Flags(
          Property<FontDialogFlags>::Init(this)
              .Getter([](FontDialog *self) -> FontDialogFlags {
                  return static_cast<FontDialogFlags>(self->_cf.Flags);
              })
              .Setter([](FontDialog *self, FontDialogFlags value) {
                  self->_cf.Flags = static_cast<DWORD>(value);
              })),

      Font(
          Property<sw::Font>::Init(this)
              .Getter([](FontDialog *self) -> sw::Font {
                  return self->_font;
              })
              .Setter([](FontDialog *self, const sw::Font &value) {
                  self->_font = value;
              })),

      FontName(
          Property<std::wstring>::Init(this)
              .Getter([](FontDialog *self) -> std::wstring {
                  return self->_font.name;
              })
              .Setter([](FontDialog *self, const std::wstring &value) {
                  self->_font.name = value;
              })),

      FontSize(
          Property<double>::Init(this)
              .Getter([](FontDialog *self) -> double {
                  return self->_font.size;
              })
              .Setter([](FontDialog *self, double value) {
                  self->_font.size = value;
              })),

      FontWeight(
          Property<sw::FontWeight>::Init(this)
              .Getter([](FontDialog *self) -> sw::FontWeight {
                  return self->_font.weight;
              })
              .Setter([](FontDialog *self, sw::FontWeight value) {
                  self->_font.weight = value;
              })),

      ShowEffects(
          Property<bool>::Init(this)
              .Getter([](FontDialog *self) -> bool {
                  return (self->Flags & FontDialogFlags::Effects) == FontDialogFlags::Effects;
              })
              .Setter([](FontDialog *self, bool value) {
                  if (value) {
                      self->Flags |= FontDialogFlags::Effects;
                  } else {
                      self->Flags &= ~FontDialogFlags::Effects;
                  }
              })),

      SelectedColor(
          Property<Color>::Init(this)
              .Getter([](FontDialog *self) -> Color {
                  return self->_cf.rgbColors;
              })
              .Setter([](FontDialog *self, const Color &value) {
                  self->_cf.rgbColors = value;
              }))
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

    result = ChooseFontW(pCF);
    if (result) _font = logFont;

    pCF->lpLogFont = nullptr;
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
