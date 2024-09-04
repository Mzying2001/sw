#include "BmpBox.h"
#include <cmath>

sw::BmpBox::BmpBox()
    : BmpHandle(
          // get
          [this]() -> HBITMAP {
              return this->_hBitmap;
          }),

      SizeMode(
          // get
          [this]() -> BmpBoxSizeMode {
              return this->_sizeMode;
          },
          // set
          [this](const BmpBoxSizeMode &value) {
              if (this->_sizeMode != value) {
                  this->_sizeMode = value;
                  this->Redraw();
                  this->NotifyLayoutUpdated();
              }
          })
{
    this->Rect        = sw::Rect{0, 0, 200, 200};
    this->Transparent = true;
}

HBITMAP sw::BmpBox::Load(HBITMAP hBitmap)
{
    HBITMAP hNewBitmap = (HBITMAP)CopyImage(hBitmap, IMAGE_BITMAP, 0, 0, 0);
    return this->_SetBmpIfNotNull(hNewBitmap);
}

HBITMAP sw::BmpBox::Load(HINSTANCE hInstance, int resourceId)
{
    HBITMAP hNewBitmap = (HBITMAP)LoadImageW(hInstance, MAKEINTRESOURCEW(resourceId), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);
    return this->_SetBmpIfNotNull(hNewBitmap);
}

HBITMAP sw::BmpBox::Load(const std::wstring &fileName)
{
    HBITMAP hNewBitmap = (HBITMAP)LoadImageW(NULL, fileName.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
    return this->_SetBmpIfNotNull(hNewBitmap);
}

void sw::BmpBox::Clear()
{
    this->_SetBmp(NULL);
}

void sw::BmpBox::SizeToImage()
{
    if (this->_hBitmap != NULL) {
        SetWindowPos(this->Handle, NULL, 0, 0, this->_bmpSize.cx, this->_bmpSize.cy, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
    }
}

bool sw::BmpBox::OnDestroy()
{
    if (this->_hBitmap != NULL) {
        DeleteObject(this->_hBitmap);
        this->_hBitmap = NULL;
    }
    return this->StaticControl::OnDestroy();
}

bool sw::BmpBox::OnPaint()
{
    HWND hwnd = this->Handle;

    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);

    RECT clientRect;
    GetClientRect(hwnd, &clientRect);

    HBRUSH hBackColorBrush = CreateSolidBrush(this->GetRealBackColor());
    FillRect(hdc, &clientRect, hBackColorBrush);

    if (this->_hBitmap != NULL &&
        this->_bmpSize.cx > 0 && this->_bmpSize.cy > 0) {
        HDC hdcmem = CreateCompatibleDC(hdc);
        SelectObject(hdcmem, this->_hBitmap);

        switch (this->_sizeMode) {
            case BmpBoxSizeMode::Normal: {
                BitBlt(hdc, 0, 0, this->_bmpSize.cx, this->_bmpSize.cy, hdcmem, 0, 0, SRCCOPY);
                break;
            }

            case BmpBoxSizeMode::StretchImage: {
                StretchBlt(hdc, 0, 0, clientRect.right - clientRect.left, clientRect.bottom - clientRect.top,
                           hdcmem, 0, 0, this->_bmpSize.cx, this->_bmpSize.cy, SRCCOPY);
                break;
            }

            case BmpBoxSizeMode::AutoSize:
            case BmpBoxSizeMode::CenterImage: {
                int x = ((clientRect.right - clientRect.left) - this->_bmpSize.cx) / 2;
                int y = ((clientRect.bottom - clientRect.top) - this->_bmpSize.cy) / 2;
                BitBlt(hdc, x, y, this->_bmpSize.cx, this->_bmpSize.cy, hdcmem, 0, 0, SRCCOPY);
                break;
            }

            case BmpBoxSizeMode::Zoom: {
                int w = clientRect.right - clientRect.left;
                int h = clientRect.bottom - clientRect.top;

                double scale_w = double(w) / this->_bmpSize.cx;
                double scale_h = double(h) / this->_bmpSize.cy;

                if (scale_w < scale_h) {
                    int draw_w = w;
                    int draw_h = std::lround(scale_w * this->_bmpSize.cy);
                    StretchBlt(hdc, 0, (h - draw_h) / 2, draw_w, draw_h,
                               hdcmem, 0, 0, this->_bmpSize.cx, this->_bmpSize.cy, SRCCOPY);
                } else {
                    int draw_w = std::lround(scale_h * this->_bmpSize.cx);
                    int draw_h = h;
                    StretchBlt(hdc, (w - draw_w) / 2, 0, draw_w, draw_h,
                               hdcmem, 0, 0, this->_bmpSize.cx, this->_bmpSize.cy, SRCCOPY);
                }
                break;
            }
        }

        DeleteDC(hdcmem);
    }

    EndPaint(hwnd, &ps);
    DeleteObject(hBackColorBrush);
    return true;
}

bool sw::BmpBox::OnSize(Size newClientSize)
{
    if (this->_sizeMode != BmpBoxSizeMode::Normal) {
        InvalidateRect(this->Handle, NULL, FALSE);
    }
    return this->StaticControl::OnSize(newClientSize);
}

void sw::BmpBox::Measure(const Size &availableSize)
{
    if (this->_sizeMode != BmpBoxSizeMode::AutoSize) {
        this->StaticControl::Measure(availableSize);
        return;
    }

    Thickness margin = this->Margin;

    this->SetDesireSize(sw::Size{
        Dip::PxToDipX(this->_bmpSize.cx) + margin.left + margin.right,
        Dip::PxToDipY(this->_bmpSize.cy) + margin.top + margin.bottom});
}

void sw::BmpBox::_UpdateBmpSize()
{
    BITMAP bm;
    if (GetObjectW(this->_hBitmap, sizeof(bm), &bm)) {
        this->_bmpSize = {bm.bmWidth, bm.bmHeight};
    }
}

void sw::BmpBox::_SetBmp(HBITMAP hBitmap)
{
    HBITMAP hOldBitmap = this->_hBitmap;

    this->_hBitmap = hBitmap;
    this->_UpdateBmpSize();
    this->Redraw();

    if (this->_sizeMode == BmpBoxSizeMode::AutoSize) {
        this->NotifyLayoutUpdated();
    }

    if (hOldBitmap != NULL) {
        DeleteObject(hOldBitmap);
    }
}

HBITMAP sw::BmpBox::_SetBmpIfNotNull(HBITMAP hBitmap)
{
    if (hBitmap != NULL) {
        this->_SetBmp(hBitmap);
    }
    return hBitmap;
}
