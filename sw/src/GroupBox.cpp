#include "GroupBox.h"
#include "Utils.h"

namespace
{
    /**
     * @brief 标题文字与标题背景矩形之间的内边距
     */
    constexpr int _GroupBoxHeaderPadding = 2;

    /**
     * @brief 组合框标题区域水平方向与组合框边缘的距离
     */
    constexpr int _GroupBoxHeaderHorzMargin = 8;
}

sw::GroupBox::GroupBox()
{
    Text        = L"GroupBox";
    BorderStyle = sw::BorderStyle::Etched;
    _UpdateTextSize();
}

sw::LayoutHost *sw::GroupBox::GetDefaultLayout()
{
    return _defaultLayout.get();
}

void sw::GroupBox::OnDrawBorder(HDC hdc, RECT &rect)
{
    auto borderStyle     = BorderStyle.Get();
    int borderThicknessX = borderStyle == sw::BorderStyle::None ? 0 : GetSystemMetrics(SM_CXEDGE);
    int borderThicknessY = borderStyle == sw::BorderStyle::None ? 0 : GetSystemMetrics(SM_CYEDGE);

    int availableWidth = rect.right - rect.left;
    int headerWidth    = Utils::Max(0, Utils::Min<int>(_textSize.cx + _GroupBoxHeaderPadding * 2, availableWidth - _GroupBoxHeaderHorzMargin * 2));
    int headerHeight   = _textSize.cy + _GroupBoxHeaderPadding * 2;

    RECT rtHeader = {
        rect.left + _GroupBoxHeaderHorzMargin,
        rect.top,
        rect.left + _GroupBoxHeaderHorzMargin + headerWidth,
        rect.top + headerHeight};

    if (hdc != NULL) {
        COLORREF backColor    = static_cast<COLORREF>(GetRealBackColor());
        COLORREF textColor    = static_cast<COLORREF>(GetRealTextColor());
        COLORREF oldBackColor = ::SetBkColor(hdc, backColor);
        COLORREF oldTextColor = ::SetTextColor(hdc, textColor);
        HGDIOBJ hOldFont      = ::SelectObject(hdc, GetFontHandle());

        HBRUSH hBrush = CreateSolidBrush(backColor);

        RECT rtHeaderRow = {
            rect.left,
            rect.top,
            rect.right,
            rtHeader.bottom};

        if (hBrush != NULL) {
            FillRect(hdc, &rtHeaderRow, hBrush);
        }

        RECT rtBorder = {
            rect.left,
            rect.top + (headerHeight - borderThicknessY) / 2,
            rect.right,
            rect.bottom};

        rtBorder.right  = Utils::Max(rtBorder.left, rtBorder.right);
        rtBorder.bottom = Utils::Max(rtBorder.top, rtBorder.bottom);

        if (borderStyle != sw::BorderStyle::None) {
            DrawEdge(hdc, &rtBorder, (UINT)borderStyle, BF_RECT);
        }

        RECT rtHeaderText = {
            rtHeader.left + _GroupBoxHeaderPadding,
            rtHeader.top + _GroupBoxHeaderPadding,
            rtHeader.right - _GroupBoxHeaderPadding,
            rtHeader.bottom - _GroupBoxHeaderPadding};

        if (hBrush != NULL) {
            FillRect(hdc, &rtHeader, hBrush);
        }

        std::wstring &text = GetInternalText();
        DrawTextW(hdc, text.c_str(), (int)text.size(), &rtHeaderText, DT_SINGLELINE);

        if (hOldFont != NULL) {
            ::SelectObject(hdc, hOldFont);
        }

        ::SetTextColor(hdc, oldTextColor);
        ::SetBkColor(hdc, oldBackColor);

        if (hBrush != NULL) {
            DeleteObject(hBrush);
        }
    }

    rect.left += borderThicknessX;
    rect.top = rtHeader.bottom;
    rect.right -= borderThicknessX;
    rect.bottom -= borderThicknessY;

    rect.right  = Utils::Max(rect.left, rect.right);
    rect.bottom = Utils::Max(rect.top, rect.bottom);
}

void sw::GroupBox::OnTextChanged()
{
    _UpdateTextSize();
    UpdateBorder();
    TBase::OnTextChanged();
}

void sw::GroupBox::FontChanged(HFONT hfont)
{
    _UpdateTextSize();
    UpdateBorder();
    TBase::FontChanged(hfont);
}

bool sw::GroupBox::OnSize(const Size &newClientSize)
{
    bool result = TBase::OnSize(newClientSize);

    // GroupBox的边框绘制在非客户区，连续调整尺寸时系统可能延后WM_NCPAINT，
    // 导致新尺寸已经生效但边框仍未及时刷新。这里立即重绘frame，并避免擦除背景，
    // 减少调整大小过程中边框短暂消失或闪烁。
    RedrawWindow(Handle, NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_NOERASE | RDW_UPDATENOW);
    return result;
}

void sw::GroupBox::OnSetBackColor(Color color, bool redraw)
{
    TBase::OnSetBackColor(color, redraw);

    if (redraw) {
        RedrawWindow(Handle, NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_NOERASE);
    }
}

void sw::GroupBox::OnSetTextColor(Color color, bool redraw)
{
    TBase::OnSetTextColor(color, redraw);

    if (redraw) {
        RedrawWindow(Handle, NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_NOERASE);
    }
}

void sw::GroupBox::_UpdateTextSize()
{
    HWND hwnd = Handle;
    HDC hdc   = GetDC(hwnd);

    if (hdc == NULL) {
        _textSize = {0, 0};
        return;
    }

    HGDIOBJ hOldFont = SelectObject(hdc, GetFontHandle());

    RECT rect{};
    std::wstring &text = GetInternalText();
    DrawTextW(hdc, text.c_str(), (int)text.size(), &rect, DT_SINGLELINE | DT_CALCRECT);

    _textSize = {rect.right - rect.left, rect.bottom - rect.top};

    if (hOldFont != NULL) {
        SelectObject(hdc, hOldFont);
    }
    ReleaseDC(hwnd, hdc);
}
