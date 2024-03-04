#include "IconBox.h"

sw::IconBox::IconBox()
    : IconHandle(
          // get
          [&]() -> const HICON & {
              return this->_hIcon;
          })
{
    this->SetStyle(SS_ICON, true);
    this->Transparent = true;
}

HICON sw::IconBox::Load(HICON hIcon)
{
    HICON hNewIcon = CopyIcon(hIcon);
    if (hNewIcon == NULL) return NULL;

    this->_SetIcon(hNewIcon);
    return this->_hIcon;
}

HICON sw::IconBox::Load(StandardIcon icon)
{
    HICON hNewIcon = IconHelper::GetIconHandle(icon);
    if (hNewIcon == NULL) return NULL;

    this->_SetIcon(hNewIcon);
    return this->_hIcon;
}

HICON sw::IconBox::Load(HINSTANCE hInstance, int resourceId)
{
    HICON hNewIcon = IconHelper::GetIconHandle(hInstance, resourceId);
    if (hNewIcon == NULL) return NULL;

    this->_SetIcon(hNewIcon);
    return this->_hIcon;
}

HICON sw::IconBox::Load(const std::wstring &fileName)
{
    HICON hNewIcon = IconHelper::GetIconHandle(fileName);
    if (hNewIcon == NULL) return NULL;

    this->_SetIcon(hNewIcon);
    return this->_hIcon;
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
