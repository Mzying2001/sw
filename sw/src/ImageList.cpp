#include "ImageList.h"

sw::ImageList::ImageList(HIMAGELIST hImageList, bool isWrap) noexcept
    : _hImageList(hImageList), _isWrap(isWrap)
{
}

sw::ImageList::ImageList(int cx, int cy, UINT flags, int cInitial, int cGrow) noexcept
    : ImageList(ImageList_Create(cx, cy, flags, cInitial, cGrow), false)
{
}

sw::ImageList::ImageList(const ImageList &other) noexcept
{
    if (other._isWrap) {
        _isWrap     = true;
        _hImageList = other._hImageList;
    } else {
        _isWrap     = false;
        _hImageList = ImageList_Duplicate(other._hImageList);
    }
}

sw::ImageList::ImageList(ImageList &&other) noexcept
{
    _isWrap           = other._isWrap;
    _hImageList       = other._hImageList;
    other._hImageList = NULL;
}

sw::ImageList::~ImageList() noexcept
{
    _DestroyIfNotWrap();
}

sw::ImageList &sw::ImageList::operator=(const ImageList &other) noexcept
{
    if (this == &other) {
        return *this;
    }

    _DestroyIfNotWrap();

    if (other._isWrap) {
        _isWrap     = true;
        _hImageList = other._hImageList;
    } else {
        _isWrap     = false;
        _hImageList = ImageList_Duplicate(other._hImageList);
    }

    return *this;
}

sw::ImageList &sw::ImageList::operator=(ImageList &&other) noexcept
{
    if (this == &other) {
        return *this;
    }

    _DestroyIfNotWrap();

    _isWrap           = other._isWrap;
    _hImageList       = other._hImageList;
    other._hImageList = NULL;

    return *this;
}

sw::ImageList sw::ImageList::Create(int cx, int cy, UINT flags, int cInitial, int cGrow) noexcept
{
    return ImageList{cx, cy, flags, cInitial, cGrow};
}

sw::ImageList sw::ImageList::Wrap(HIMAGELIST hImageList) noexcept
{
    return ImageList{hImageList, true};
}

bool sw::ImageList::Copy(const ImageList &dst, int iDst, const ImageList &src, int iSrc, UINT uFlags) noexcept
{
    return ImageList_Copy(dst._hImageList, iDst, src._hImageList, iSrc, uFlags);
}

bool sw::ImageList::DragEnter(HWND hwndLock, int x, int y) noexcept
{
    return ImageList_DragEnter(hwndLock, x, y);
}

bool sw::ImageList::DragLeave(HWND hwndLock) noexcept
{
    return ImageList_DragLeave(hwndLock);
}

bool sw::ImageList::DragMove(int x, int y) noexcept
{
    return ImageList_DragMove(x, y);
}

bool sw::ImageList::DragShowNolock(bool fShow) noexcept
{
    return ImageList_DragShowNolock(fShow);
}

void sw::ImageList::EndDrag() noexcept
{
    ImageList_EndDrag();
}

sw::ImageList sw::ImageList::GetDragImage(POINT *ppt, POINT *pptHotspot) noexcept
{
    return ImageList::Wrap(ImageList_GetDragImage(ppt, pptHotspot));
}

sw::ImageList sw::ImageList::LoadImageA(HINSTANCE hi, LPCSTR lpbmp, int cx, int cGrow, COLORREF crMask, UINT uType, UINT uFlags) noexcept
{
    return ImageList{ImageList_LoadImageA(hi, lpbmp, cx, cGrow, crMask, uType, uFlags), false};
}

sw::ImageList sw::ImageList::LoadImageW(HINSTANCE hi, LPCWSTR lpbmp, int cx, int cGrow, COLORREF crMask, UINT uType, UINT uFlags) noexcept
{
    return ImageList{ImageList_LoadImageW(hi, lpbmp, cx, cGrow, crMask, uType, uFlags), false};
}

sw::ImageList sw::ImageList::Merge(const ImageList &iml1, int i1, const ImageList &iml2, int i2, int dx, int dy) noexcept
{
    return ImageList{ImageList_Merge(iml1._hImageList, i1, iml2._hImageList, i2, dx, dy), false};
}

sw::ImageList sw::ImageList::Read(IStream *pstm) noexcept
{
    return ImageList{ImageList_Read(pstm), false};
}

HIMAGELIST sw::ImageList::GetHandle() const noexcept
{
    return _hImageList;
}

bool sw::ImageList::IsWrap() const noexcept
{
    return _isWrap;
}

HIMAGELIST sw::ImageList::ReleaseHandle() noexcept
{
    HIMAGELIST result = _hImageList;
    _hImageList       = NULL;
    return result;
}

int sw::ImageList::Add(HBITMAP hbmImage, HBITMAP hbmMask) noexcept
{
    return ImageList_Add(_hImageList, hbmImage, hbmMask);
}

int sw::ImageList::AddIcon(HICON hIcon) noexcept
{
    return ImageList_AddIcon(_hImageList, hIcon);
}

int sw::ImageList::AddMasked(HBITMAP hbmImage, COLORREF crMask) noexcept
{
    return ImageList_AddMasked(_hImageList, hbmImage, crMask);
}

bool sw::ImageList::BeginDrag(int iTrack, int dxHotspot, int dyHotspot) noexcept
{
    return ImageList_BeginDrag(_hImageList, iTrack, dxHotspot, dyHotspot);
}

bool sw::ImageList::Draw(int i, HDC hdcDst, int x, int y, UINT fStyle) noexcept
{
    return ImageList_Draw(_hImageList, i, hdcDst, x, y, fStyle);
}

bool sw::ImageList::Draw(int i, HDC hdcDst, int x, int y, int dx, int dy, COLORREF rgbBk, COLORREF rgbFg, UINT fStyle) noexcept
{
    return ImageList_DrawEx(_hImageList, i, hdcDst, x, y, dx, dy, rgbBk, rgbFg, fStyle);
}

sw::ImageList sw::ImageList::Duplicate() noexcept
{
    return ImageList{ImageList_Duplicate(_hImageList), false};
}

COLORREF sw::ImageList::GetBkColor() noexcept
{
    return ImageList_GetBkColor(_hImageList);
}

HICON sw::ImageList::GetIcon(int i, UINT flags) noexcept
{
    return ImageList_GetIcon(_hImageList, i, flags);
}

bool sw::ImageList::GetIconSize(int &cx, int &cy) noexcept
{
    return ImageList_GetIconSize(_hImageList, &cx, &cy);
}

int sw::ImageList::GetImageCount() noexcept
{
    return ImageList_GetImageCount(_hImageList);
}

bool sw::ImageList::GetImageInfo(int i, IMAGEINFO *pImageInfo) noexcept
{
    return ImageList_GetImageInfo(_hImageList, i, pImageInfo);
}

bool sw::ImageList::Remove(int i) noexcept
{
    return ImageList_Remove(_hImageList, i);
}

bool sw::ImageList::RemoveAll() noexcept
{
    return Remove(-1);
}

bool sw::ImageList::Replace(int i, HBITMAP hbmImage, HBITMAP hbmMask) noexcept
{
    return ImageList_Replace(_hImageList, i, hbmImage, hbmMask);
}

int sw::ImageList::ReplaceIcon(int i, HICON hicon) noexcept
{
    return ImageList_ReplaceIcon(_hImageList, i, hicon);
}

COLORREF sw::ImageList::SetBkColor(COLORREF clrBk) noexcept
{
    return ImageList_SetBkColor(_hImageList, clrBk);
}

bool sw::ImageList::SetDragCursorImage(int iDrag, int dxHotspot, int dyHotspot) noexcept
{
    return ImageList_SetDragCursorImage(_hImageList, iDrag, dxHotspot, dyHotspot);
}

bool sw::ImageList::SetIconSize(int cx, int cy) noexcept
{
    return ImageList_SetIconSize(_hImageList, cx, cy);
}

bool sw::ImageList::SetImageCount(UINT uNewCount) noexcept
{
    return ImageList_SetImageCount(_hImageList, uNewCount);
}

bool sw::ImageList::SetOverlayImage(int iImage, int iOverlay) noexcept
{
    return ImageList_SetOverlayImage(_hImageList, iImage, iOverlay);
}

bool sw::ImageList::Write(IStream *pstm) noexcept
{
    return ImageList_Write(_hImageList, pstm);
}

void sw::ImageList::_DestroyIfNotWrap() noexcept
{
    if (_hImageList != NULL && !_isWrap) {
        ImageList_Destroy(_hImageList);
    }
}
