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
        HBRUSH hBrush = CreateSolidBrush(GetRealBackColor());
        ::SetBkColor(hdc, GetRealBackColor());
        ::SetTextColor(hdc, GetRealTextColor());
        ::SelectObject(hdc, GetFontHandle());

        RECT rtHeaderRow = {
            rect.left,
            rect.top,
            rect.right,
            rtHeader.bottom};

        FillRect(hdc, &rtHeaderRow, hBrush);

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

        FillRect(hdc, &rtHeader, hBrush);
        DrawTextW(hdc, GetText().c_str(), (int)GetText().size(), &rtHeaderText, DT_SINGLELINE);
        DeleteObject(hBrush);
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
    Panel::OnTextChanged();
}

void sw::GroupBox::FontChanged(HFONT hfont)
{
    _UpdateTextSize();
    UpdateBorder();
    Panel::FontChanged(hfont);
}

void sw::GroupBox::SetBackColor(Color color, bool redraw)
{
    UIElement::SetBackColor(color, redraw);
    if (redraw) UpdateBorder();
}

void sw::GroupBox::SetTextColor(Color color, bool redraw)
{
    UIElement::SetTextColor(color, redraw);
    if (redraw) UpdateBorder();
}

void sw::GroupBox::_UpdateTextSize()
{
    HWND hwnd = Handle;
    HDC hdc   = GetDC(hwnd);

    SelectObject(hdc, GetFontHandle());

    RECT rect{};
    std::wstring &text = GetText();
    DrawTextW(hdc, text.c_str(), (int)text.size(), &rect, DT_SINGLELINE | DT_CALCRECT);

    _textSize = {rect.right - rect.left, rect.bottom - rect.top};
    ReleaseDC(hwnd, hdc);
}
