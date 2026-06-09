#include "Panel.h"
#include "App.h"
#include "Cursor.h"
#include "Utils.h"
#include "WndMsg.h"

namespace
{
    /**
     * @brief 面板的窗口类名
     */
    constexpr wchar_t _PanelClassName[] = L"sw::Panel";
}

sw::Panel::Panel()
    : BorderStyle(
          Property<sw::BorderStyle>::Init(this)
              .Getter([](Panel *self) -> sw::BorderStyle {
                  return self->_borderStyle;
              })
              .Setter([](Panel *self, sw::BorderStyle value) {
                  if (self->_borderStyle != value) {
                      self->_borderStyle = value;
                      self->RaisePropertyChanged(&Panel::BorderStyle);
                      self->UpdateBorder();
                  }
              })),

      Padding(
          Property<sw::Thickness>::Init(this)
              .Getter([](Panel *self) -> sw::Thickness {
                  return self->_padding;
              })
              .Setter([](Panel *self, const sw::Thickness &value) {
                  if (self->_padding != value) {
                      self->_padding = value;
                      self->RaisePropertyChanged(&Panel::Padding);
                      self->UpdateBorder();
                  }
              }))
{
    static ATOM wndClsAtom = []() -> ATOM {
        WNDCLASSEXW wc{};
        wc.cbSize        = sizeof(wc);
        wc.hInstance     = App::Instance;
        wc.lpfnWndProc   = DefWindowProcW;
        wc.lpszClassName = _PanelClassName;
        wc.hCursor       = CursorHelper::GetCursorHandle(StandardCursor::Arrow);
        return RegisterClassExW(&wc);
    }();

    (void)wndClsAtom; // 消除未使用变量警告

    InitControl(_PanelClassName, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, WS_EX_NOACTIVATE);

    Rect             = sw::Rect{0, 0, 200, 200};
    Transparent      = true;
    InheritTextColor = true;
}

void sw::Panel::UpdateBorder()
{
    SetWindowPos(Handle, nullptr, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
}

LRESULT sw::Panel::WndProc(ProcMsg &refMsg)
{
    switch (refMsg.uMsg) {
        case WM_NCCALCSIZE: {
            auto result = DefaultWndProc(refMsg);
            RECT *pRect = nullptr;

            if (refMsg.wParam == FALSE) {
                pRect = reinterpret_cast<RECT *>(refMsg.lParam);
            } else {
                pRect = reinterpret_cast<NCCALCSIZE_PARAMS *>(refMsg.lParam)->rgrc;
            }
            OnDrawBorder(NULL, *pRect);
            OnDrawPadding(NULL, *pRect);
            return result;
        }

        case WM_UpdateLayout: {
            if (IsInHierarchy)
                UpdateLayout();
            return 0;
        }

        default: {
            return WndBase::WndProc(refMsg);
        }
    }
}

bool sw::Panel::OnEraseBackground(HDC hdc, LRESULT &result)
{
    result = 1;
    return true;
}

bool sw::Panel::OnPaint()
{
    PAINTSTRUCT ps;
    HWND hwnd = Handle;
    HDC hdc   = BeginPaint(hwnd, &ps);

    HBRUSH hBrush = CreateSolidBrush(static_cast<COLORREF>(GetRealBackColor()));
    FillRect(hdc, &ps.rcPaint, hBrush);

    DeleteObject(hBrush);
    EndPaint(hwnd, &ps);
    return true;
}

bool sw::Panel::OnNcPaint(HRGN hRgn)
{
    HWND hwnd = Handle;

    RECT rtWindow;

    if (!GetWindowRect(hwnd, &rtWindow)) {
        DefaultWndProc(ProcMsg{
            hwnd, WM_NCPAINT, reinterpret_cast<WPARAM>(hRgn), 0}); // scrollbars
        return true;
    }

    // GetWindowDC使用窗口左上角作为原点，这里将屏幕坐标转换为窗口坐标。
    OffsetRect(&rtWindow, -rtWindow.left, -rtWindow.top);

    int width  = rtWindow.right - rtWindow.left;
    int height = rtWindow.bottom - rtWindow.top;

    // 资源不足或区域计算失败时回退到旧的直接绘制方式，保证边框仍能显示。
    auto drawDirect = [this, hwnd, &rtWindow]() {
        HDC hdc = GetWindowDC(hwnd);
        if (hdc != NULL) {
            RECT rect = rtWindow;
            OnDrawBorder(hdc, rect);
            OnDrawPadding(hdc, rect);
            ReleaseDC(hwnd, hdc);
        }
    };

    if (width > 0 && height > 0) {
        // OnDrawBorder/OnDrawPadding在hdc为NULL时只更新rect，用于得到最终客户区。
        RECT rtClient = rtWindow;
        OnDrawBorder(NULL, rtClient);
        OnDrawPadding(NULL, rtClient);

        // 实际需要提交到窗口DC的区域是整个窗口减去最终客户区，也就是非客户区。
        HRGN hRgnWindow = CreateRectRgnIndirect(&rtWindow);
        HRGN hRgnClient = CreateRectRgnIndirect(&rtClient);
        HRGN hRgnNc     = CreateRectRgn(0, 0, 0, 0);

        bool useDirect = hRgnWindow == NULL || hRgnClient == NULL || hRgnNc == NULL;
        int rgnType    = ERROR;

        if (!useDirect) {
            rgnType   = CombineRgn(hRgnNc, hRgnWindow, hRgnClient, RGN_DIFF);
            useDirect = rgnType == ERROR;
        }

        if (useDirect) {
            drawDirect();
        } else if (rgnType != NULLREGION) {
            bool buffered = false;
            HDC hdc       = GetWindowDC(hwnd);

            if (hdc != NULL) {
                HDC hdcMem      = CreateCompatibleDC(hdc);
                HBITMAP hBmpWnd = hdcMem == NULL ? NULL : CreateCompatibleBitmap(hdc, width, height);

                if (hdcMem != NULL && hBmpWnd != NULL) {
                    HBITMAP hBmpOld = (HBITMAP)SelectObject(hdcMem, hBmpWnd);
                    if (hBmpOld != NULL) {
                        HBRUSH hBrush = CreateSolidBrush(static_cast<COLORREF>(GetRealBackColor()));
                        if (hBrush != NULL) {
                            // 先在内存DC中完成整块非客户区绘制，再一次性提交，减少绘制步骤外露。
                            FillRect(hdcMem, &rtWindow, hBrush);
                            DeleteObject(hBrush);

                            RECT rect = rtWindow;
                            OnDrawBorder(hdcMem, rect);
                            OnDrawPadding(hdcMem, rect);

                            // BitBlt会受目标DC裁剪区域限制，因此只会覆盖非客户区，不影响客户区内容。
                            if (SelectClipRgn(hdc, hRgnNc) != ERROR) {
                                buffered = BitBlt(hdc, 0, 0, width, height, hdcMem, 0, 0, SRCCOPY) != FALSE;
                                SelectClipRgn(hdc, NULL);
                            }
                        }
                        SelectObject(hdcMem, hBmpOld);
                    }
                }

                if (hBmpWnd != NULL) {
                    DeleteObject(hBmpWnd);
                }
                if (hdcMem != NULL) {
                    DeleteDC(hdcMem);
                }
                ReleaseDC(hwnd, hdc);
            }

            if (!buffered) {
                drawDirect();
            }
        }

        if (hRgnWindow != NULL) {
            DeleteObject(hRgnWindow);
        }
        if (hRgnClient != NULL) {
            DeleteObject(hRgnClient);
        }
        if (hRgnNc != NULL) {
            DeleteObject(hRgnNc);
        }
    }

    // 自绘只负责边框和Padding，默认非客户区绘制仍用于系统滚动条。
    DefaultWndProc(ProcMsg{
        hwnd, WM_NCPAINT, reinterpret_cast<WPARAM>(hRgn), 0}); // scrollbars
    return true;
}

void sw::Panel::OnDrawBorder(HDC hdc, RECT &rect)
{
    if (_borderStyle == sw::BorderStyle::None) {
        return;
    }

    if (hdc != NULL) {
        DrawEdge(hdc, &rect, (UINT)_borderStyle, BF_RECT);
    }

    int cx = GetSystemMetrics(SM_CXEDGE);
    int cy = GetSystemMetrics(SM_CYEDGE);

    rect.left += cx;
    rect.top += cy;
    rect.right -= cx;
    rect.bottom -= cy;

    rect.right  = Utils::Max(rect.left, rect.right);
    rect.bottom = Utils::Max(rect.top, rect.bottom);
}

void sw::Panel::OnDrawPadding(HDC hdc, RECT &rect)
{
    if (_padding.left == 0 && _padding.top == 0 &&
        _padding.right == 0 && _padding.bottom == 0) {
        return;
    }

    RECT rtPaddingOuter  = rect;
    RECT &rtPaddingInner = rect;

    rtPaddingInner.left += Dip::DipToPxX(_padding.left);
    rtPaddingInner.top += Dip::DipToPxY(_padding.top);
    rtPaddingInner.right -= Dip::DipToPxX(_padding.right);
    rtPaddingInner.bottom -= Dip::DipToPxY(_padding.bottom);

    rtPaddingInner.right  = Utils::Max(rtPaddingInner.left, rtPaddingInner.right);
    rtPaddingInner.bottom = Utils::Max(rtPaddingInner.top, rtPaddingInner.bottom);

    if (hdc != NULL) {
        HRGN hRgnOuter = CreateRectRgnIndirect(&rtPaddingOuter);
        HRGN hRgnInner = CreateRectRgnIndirect(&rtPaddingInner);
        HRGN hRgnDiff  = CreateRectRgn(0, 0, 0, 0);
        CombineRgn(hRgnDiff, hRgnOuter, hRgnInner, RGN_DIFF);

        HBRUSH hBrush = CreateSolidBrush(static_cast<COLORREF>(GetRealBackColor()));
        FillRgn(hdc, hRgnDiff, hBrush);

        DeleteObject(hRgnOuter);
        DeleteObject(hRgnInner);
        DeleteObject(hRgnDiff);
        DeleteObject(hBrush);
    }
}
