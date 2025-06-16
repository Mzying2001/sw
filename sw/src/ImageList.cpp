#include "ImageList.h"

sw::ImageList::ImageList(HIMAGELIST hImageList, bool isWrap)
    : _hImageList(hImageList), _isWrap(isWrap)
{
}

sw::ImageList::ImageList(int cx, int cy, UINT flags, int cInitial, int cGrow)
    : ImageList(ImageList_Create(cx, cy, flags, cInitial, cGrow), false)
{
}

sw::ImageList::ImageList(const ImageList &value)
{
    if (value._isWrap) {
        this->_isWrap     = true;
        this->_hImageList = value._hImageList;
    } else {
        this->_isWrap     = false;
        this->_hImageList = ImageList_Duplicate(value._hImageList);
    }
}

sw::ImageList::ImageList(ImageList &&rvalue)
{
    this->_isWrap      = rvalue._isWrap;
    this->_hImageList  = rvalue._hImageList;
    rvalue._hImageList = NULL;
}

sw::ImageList::~ImageList()
{
    this->_DestroyIfNotWrap();
}

sw::ImageList &sw::ImageList::operator=(const ImageList &value)
{
    if (this == &value) {
        return *this;
    }

    this->_DestroyIfNotWrap();

    if (value._isWrap) {
        this->_isWrap     = true;
        this->_hImageList = value._hImageList;
    } else {
        this->_isWrap     = false;
        this->_hImageList = ImageList_Duplicate(value._hImageList);
    }

    return *this;
}

sw::ImageList &sw::ImageList::operator=(ImageList &&rvalue)
{
    if (this == &rvalue) {
        return *this;
    }

    this->_DestroyIfNotWrap();

    this->_isWrap      = rvalue._isWrap;
    this->_hImageList  = rvalue._hImageList;
    rvalue._hImageList = NULL;

    return *this;
}

sw::ImageList sw::ImageList::Create(int cx, int cy, UINT flags, int cInitial, int cGrow)
{
    return ImageList{cx, cy, flags, cInitial, cGrow};
}

sw::ImageList sw::ImageList::Wrap(HIMAGELIST hImageList)
{
    return ImageList{hImageList, true};
}

bool sw::ImageList::Copy(const ImageList &dst, int iDst, const ImageList &src, int iSrc, UINT uFlags)
{
    return ImageList_Copy(dst._hImageList, iDst, src._hImageList, iSrc, uFlags);
}

bool sw::ImageList::DragEnter(HWND hwndLock, double x, double y)
{
    return ImageList_DragEnter(hwndLock, Dip::DipToPxX(x), Dip::DipToPxY(y));
}

bool sw::ImageList::DragLeave(HWND hwndLock)
{
    return ImageList_DragLeave(hwndLock);
}

bool sw::ImageList::DragMove(double x, double y)
{
    return ImageList_DragMove(Dip::DipToPxX(x), Dip::DipToPxY(y));
}

bool sw::ImageList::DragShowNolock(bool fShow)
{
    return ImageList_DragShowNolock(fShow);
}

void sw::ImageList::EndDrag()
{
    ImageList_EndDrag();
}

sw::ImageList sw::ImageList::GetDragImage(POINT *ppt, POINT *pptHotspot)
{
    return ImageList{ImageList_GetDragImage(ppt, pptHotspot), false};
}

sw::ImageList sw::ImageList::LoadImageA(HINSTANCE hi, LPCSTR lpbmp, int cx, int cGrow, COLORREF crMask, UINT uType, UINT uFlags)
{
    return ImageList{ImageList_LoadImageA(hi, lpbmp, cx, cGrow, crMask, uType, uFlags), false};
}

sw::ImageList sw::ImageList::LoadImageW(HINSTANCE hi, LPCWSTR lpbmp, int cx, int cGrow, COLORREF crMask, UINT uType, UINT uFlags)
{
    return ImageList{ImageList_LoadImageW(hi, lpbmp, cx, cGrow, crMask, uType, uFlags), false};
}

sw::ImageList sw::ImageList::Merge(const ImageList &iml1, int i1, const ImageList &iml2, int i2, int dx, int dy)
{
    return ImageList{ImageList_Merge(iml1._hImageList, i1, iml2._hImageList, i2, dx, dy), false};
}

sw::ImageList sw::ImageList::Read(IStream *pstm)
{
    return ImageList{ImageList_Read(pstm), false};
}

HIMAGELIST sw::ImageList::GetHandle() const
{
    return this->_hImageList;
}

bool sw::ImageList::IsWrap() const
{
    return this->_isWrap;
}

HIMAGELIST sw::ImageList::ReleaseHandle()
{
    HIMAGELIST result = this->_hImageList;
    this->_hImageList = NULL;
    return result;
}

int sw::ImageList::Add(HBITMAP hbmImage, HBITMAP hbmMask)
{
    return ImageList_Add(this->_hImageList, hbmImage, hbmMask);
}

int sw::ImageList::AddIcon(HICON hIcon)
{
    return ImageList_AddIcon(this->_hImageList, hIcon);
}

int sw::ImageList::AddMasked(HBITMAP hbmImage, COLORREF crMask)
{
    return ImageList_AddMasked(this->_hImageList, hbmImage, crMask);
}

bool sw::ImageList::BeginDrag(int iTrack, int dxHotspot, int dyHotspot)
{
    return ImageList_BeginDrag(this->_hImageList, iTrack, dxHotspot, dyHotspot);
}

bool sw::ImageList::Draw(int i, HDC hdcDst, double x, double y, UINT fStyle)
{
    return this->DrawPx(i, hdcDst, Dip::DipToPxX(x), Dip::DipToPxY(y), fStyle);
}

bool sw::ImageList::Draw(int i, HDC hdcDst, double x, double y, double dx, double dy, COLORREF rgbBk, COLORREF rgbFg, UINT fStyle)
{
    return this->DrawPx(i, hdcDst, Dip::DipToPxX(x), Dip::DipToPxY(y), Dip::DipToPxX(dx), Dip::DipToPxY(dy), rgbBk, rgbFg, fStyle);
}

bool sw::ImageList::DrawPx(int i, HDC hdcDst, int x, int y, UINT fStyle)
{
    return ImageList_Draw(this->_hImageList, i, hdcDst, x, y, fStyle);
}

bool sw::ImageList::DrawPx(int i, HDC hdcDst, int x, int y, int dx, int dy, COLORREF rgbBk, COLORREF rgbFg, UINT fStyle)
{
    return ImageList_DrawEx(this->_hImageList, i, hdcDst, x, y, dx, dy, rgbBk, rgbFg, fStyle);
}

sw::ImageList sw::ImageList::Duplicate()
{
    return ImageList{ImageList_Duplicate(this->_hImageList), false};
}

COLORREF sw::ImageList::GetBkColor()
{
    return ImageList_GetBkColor(this->_hImageList);
}

HICON sw::ImageList::GetIcon(int i, UINT flags)
{
    return ImageList_GetIcon(this->_hImageList, i, flags);
}

bool sw::ImageList::GetIconSize(int &cx, int &cy)
{
    return ImageList_GetIconSize(this->_hImageList, &cx, &cy);
}

int sw::ImageList::GetImageCount()
{
    return ImageList_GetImageCount(this->_hImageList);
}

bool sw::ImageList::GetImageInfo(int i, IMAGEINFO *pImageInfo)
{
    return ImageList_GetImageInfo(this->_hImageList, i, pImageInfo);
}

bool sw::ImageList::Remove(int i)
{
    return ImageList_Remove(this->_hImageList, i);
}

bool sw::ImageList::RemoveAll()
{
    return this->Remove(-1);
}

bool sw::ImageList::Replace(int i, HBITMAP hbmImage, HBITMAP hbmMask)
{
    return ImageList_Replace(this->_hImageList, i, hbmImage, hbmMask);
}

int sw::ImageList::ReplaceIcon(int i, HICON hicon)
{
    return ImageList_ReplaceIcon(this->_hImageList, i, hicon);
}

COLORREF sw::ImageList::SetBkColor(COLORREF clrBk)
{
    return ImageList_SetBkColor(this->_hImageList, clrBk);
}

bool sw::ImageList::SetDragCursorImage(int iDrag, int dxHotspot, int dyHotspot)
{
    return ImageList_SetDragCursorImage(this->_hImageList, iDrag, dxHotspot, dyHotspot);
}

bool sw::ImageList::SetIconSize(int cx, int cy)
{
    return ImageList_SetIconSize(this->_hImageList, cx, cy);
}

bool sw::ImageList::SetImageCount(UINT uNewCount)
{
    return ImageList_SetImageCount(this->_hImageList, uNewCount);
}

bool sw::ImageList::SetOverlayImage(int iImage, int iOverlay)
{
    return ImageList_SetOverlayImage(this->_hImageList, iImage, iOverlay);
}

bool sw::ImageList::Write(IStream *pstm)
{
    return ImageList_Write(this->_hImageList, pstm);
}

void sw::ImageList::_DestroyIfNotWrap()
{
    if (this->_hImageList != NULL && !this->_isWrap) {
        ImageList_Destroy(this->_hImageList);
    }
}
