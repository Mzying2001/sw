#include "IconBox.h"

sw::IconBox::IconBox()
    : IconHandle(
          // get
          [&]() -> const HICON & {
              return this->_hIcon;
          }),

      StretchIcon(
          // get
          [&]() -> const bool & {
              static bool result;
              result = !this->GetStyle(SS_CENTERIMAGE);
              return result;
          },
          // set
          [&](const bool &value) {
              this->SetStyle(SS_CENTERIMAGE, !value);
          })
{
    this->Rect = sw::Rect{0, 0, 50, 50};
    this->SetStyle(SS_ICON, true);
    this->Transparent = true;
}

HICON sw::IconBox::Load(HICON hIcon)
{
    HICON hNewIcon = CopyIcon(hIcon);
    return this->_SetIconIfNotNull(hNewIcon);
}

HICON sw::IconBox::Load(StandardIcon icon)
{
    HICON hNewIcon = IconHelper::GetIconHandle(icon);
    return this->_SetIconIfNotNull(hNewIcon);
}

HICON sw::IconBox::Load(HINSTANCE hInstance, int resourceId)
{
    HICON hNewIcon = IconHelper::GetIconHandle(hInstance, resourceId);
    return this->_SetIconIfNotNull(hNewIcon);
}

HICON sw::IconBox::Load(const std::wstring &fileName)
{
    HICON hNewIcon = IconHelper::GetIconHandle(fileName);
    return this->_SetIconIfNotNull(hNewIcon);
}

void sw::IconBox::Clear()
{
    this->_SetIcon(NULL);
}

bool sw::IconBox::OnDestroy()
{
    if (this->_hIcon != NULL) {
        DestroyIcon(this->_hIcon);
        this->_hIcon = NULL;
    }
    return this->StaticControl::OnDestroy();
}

void sw::IconBox::_SetIcon(HICON hIcon)
{
    HICON hOldIcon = this->_hIcon;

    this->_hIcon = hIcon;
    this->SendMessageW(STM_SETICON, reinterpret_cast<WPARAM>(hIcon), 0);

    if (hOldIcon != NULL) {
        DestroyIcon(hOldIcon);
    }
}

HICON sw::IconBox::_SetIconIfNotNull(HICON hIcon)
{
    if (hIcon != NULL) {
        this->_SetIcon(hIcon);
    }
    return hIcon;
}
