#include "IconBox.h"

sw::IconBox::IconBox()
    : IconHandle(
          Property<HICON>::Init(this)
              .Getter([](IconBox *self) -> HICON {
                  return self->_hIcon;
              })),

      StretchIcon(
          Property<bool>::Init(this)
              .Getter([](IconBox *self) -> bool {
                  return !self->GetStyle(SS_CENTERIMAGE);
              })
              .Setter([](IconBox *self, bool value) {
                  self->SetStyle(SS_CENTERIMAGE, !value);
              }))
{
    this->Rect = sw::Rect{0, 0, 50, 50};
    this->SetStyle(SS_ICON, true);
    this->Transparent = true;
}

sw::IconBox::~IconBox()
{
    if (!this->IsDestroyed) {
        DestroyWindow(this->Handle);
    }
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

void sw::IconBox::SizeToIcon()
{
    ICONINFO info;
    BITMAP bm;

    if (!GetIconInfo(this->_hIcon, &info))
        return;

    if (GetObjectW(info.hbmColor, sizeof(bm), &bm)) {
        this->Resize(SIZE{bm.bmWidth, bm.bmHeight});
    }

    DeleteObject(info.hbmColor);
    DeleteObject(info.hbmMask);
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
