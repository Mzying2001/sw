#include "sw_all.h"
#include <cmath>
#include <strsafe.h>
#include <algorithm>

// AbsoluteLayout.cpp

void sw::AbsoluteLayout::MeasureOverride(Size &availableSize)
{
    int count = this->GetChildLayoutCount();
    for (int i = 0; i < count; ++i) {
        ILayout &item = this->GetChildLayoutAt(i);
        item.Measure(Size(INFINITY, INFINITY));
    }
    this->SetDesireSize(Size(NAN, NAN));
    // 注：Layer对AbsoluteLayout有特殊处理，当DesireSize为NAN时将按照普通控件Measure
}

void sw::AbsoluteLayout::ArrangeOverride(Size &finalSize)
{
    int count = this->GetChildLayoutCount();
    for (int i = 0; i < count; ++i) {
        ILayout &item       = this->GetChildLayoutAt(i);
        Size itemDesireSize = item.GetDesireSize();
        item.Arrange(Rect(NAN, NAN, itemDesireSize.width, itemDesireSize.height));
    }
    // 注：UIElement对AbsoluteLayout有特殊处理，当Size的左边或顶边为NAN时，Arrange只调整尺寸不改位置
}

// App.cpp

const sw::ReadOnlyProperty<HINSTANCE> sw::App::Instance(
    []() -> const HINSTANCE & {
        static HINSTANCE _instance;
        _instance = GetModuleHandleW(NULL);
        return _instance;
    } //
);

int sw::App::MsgLoop()
{
    MSG msg{};
    while (GetMessageW(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return (int)msg.wParam;
}

void sw::App::Quit(int exitCode)
{
    PostQuitMessage(exitCode);
}

// Button.cpp

#define BUTTONSTYLE_DEFAULT (WS_CHILD | WS_VISIBLE | BS_NOTIFY | BS_PUSHBUTTON)
#define BUTTONSTYLE_FOCUSED (WS_CHILD | WS_VISIBLE | BS_NOTIFY | BS_DEFPUSHBUTTON)

sw::Button::Button()
{
    this->InitButtonBase(L"Button", BUTTONSTYLE_DEFAULT);
    this->Rect = sw::Rect(0, 0, 70, 30);
}

bool sw::Button::OnSetFocus(HWND hPreFocus)
{
    this->SetStyle(BUTTONSTYLE_FOCUSED);
    this->Redraw();
    return this->ButtonBase::OnSetFocus(hPreFocus);
}

bool sw::Button::OnKillFocus(HWND hNextFocus)
{
    this->SetStyle(BUTTONSTYLE_DEFAULT);
    this->Redraw();
    return this->ButtonBase::OnKillFocus(hNextFocus);
}

// ButtonBase.cpp

sw::ButtonBase::ButtonBase()
{
}

void sw::ButtonBase::InitButtonBase(LPCWSTR lpWindowName, DWORD dwStyle)
{
    this->InitControl(L"BUTTON", lpWindowName, dwStyle);
}

void sw::ButtonBase::ParentReceivedCommand(int code)
{
    switch (code) {
        case BN_CLICKED:
            this->RaiseRoutedEvent(ButtonBase_Clicked);
            break;

        case BN_DOUBLECLICKED:
            this->RaiseRoutedEvent(ButtonBase_DoubleClicked);
            break;

        default:
            break;
    }
}

// Color.cpp

sw::Color::Color()
    : Color(0, 0, 0)
{
}

sw::Color::Color(uint8_t r, uint8_t g, uint8_t b)
    : r(r), g(g), b(b)
{
}

sw::Color::Color(COLORREF color)
{
    this->r = (color >> 0) & 0xFF;
    this->g = (color >> 8) & 0xFF;
    this->b = (color >> 16) & 0xFF;
}

sw::Color::operator COLORREF() const
{
    return RGB(this->r, this->g, this->b);
}

std::wostream &sw::operator<<(std::wostream &wos, const Color &color)
{
    return wos << L"Color{r=" << (int)color.r << L", g=" << (int)color.g << L", b=" << (int)color.b << L"}";
}

// Control.cpp

sw::Control::Control()
    : BackColor(
          // get
          [&]() -> const Color & {
              return this->_backColor;
          },
          // set
          [&](const Color &value) {
              this->_backColor = value;
              this->Redraw();
          }),

      TextColor(
          // get
          [&]() -> const Color & {
              return this->_textColor;
          },
          // set
          [&](const Color &value) {
              this->_textColor = value;
              this->Redraw();
          })
{
}

/*void sw::Control::HandleInitialized(HWND hwnd)
{
    HDC hdc          = GetDC(hwnd);
    this->_backColor = GetBkColor(hdc);
    this->_textColor = GetTextColor(hdc);
    ReleaseDC(hwnd, hdc);
}*/

LRESULT sw::Control::CtlColor(HDC hdc, HWND hwnd)
{
    static HBRUSH hBrush = NULL;

    if (hBrush != NULL) {
        DeleteObject(hBrush);
    }

    hBrush = CreateSolidBrush(this->_backColor);

    SetTextColor(hdc, this->_textColor);
    SetBkColor(hdc, this->_backColor);
    return (LRESULT)hBrush;
}

// Dip.cpp

static sw::Dip::DipScaleInfo _GetScaleInfo();
static sw::Dip::DipScaleInfo _dipScaleInfo = _GetScaleInfo();

void sw::Dip::Update(int dpiX, int dpiY)
{
    _dipScaleInfo.scaleX = (double)USER_DEFAULT_SCREEN_DPI / dpiX;
    _dipScaleInfo.scaleY = (double)USER_DEFAULT_SCREEN_DPI / dpiY;
}

const sw::ReadOnlyProperty<double> sw::Dip::ScaleX(
    []() -> const double & {
        return _dipScaleInfo.scaleX;
    } //
);

const sw::ReadOnlyProperty<double> sw::Dip::ScaleY(
    []() -> const double & {
        return _dipScaleInfo.scaleY;
    } //
);

static sw::Dip::DipScaleInfo _GetScaleInfo()
{
    sw::Dip::DipScaleInfo info{};
    HDC hdc     = GetDC(NULL);
    info.scaleX = (double)USER_DEFAULT_SCREEN_DPI / GetDeviceCaps(hdc, LOGPIXELSX);
    info.scaleY = (double)USER_DEFAULT_SCREEN_DPI / GetDeviceCaps(hdc, LOGPIXELSY);
    ReleaseDC(NULL, hdc);
    return info;
}

// DockLayout.cpp

static sw::DockLayout::DockLayoutTag _GetDockLayoutTag(sw::ILayout &item)
{
    uint32_t tag = item.GetLayoutTag();
    return tag > sw::DockLayout::Bottom ? sw::DockLayout::Left : (sw::DockLayout::DockLayoutTag)tag;
}

void sw::DockLayout::MeasureOverride(Size &availableSize)
{
    Size restSize = availableSize;
    Size desireSize;

    int count = this->GetChildLayoutCount();

    for (int i = 0; i < count; ++i) {
        ILayout &item = this->GetChildLayoutAt(i);

        switch (_GetDockLayoutTag(item)) {
            case DockLayoutTag::Left:
            case DockLayoutTag::Right: {
                item.Measure(Size(restSize.width, restSize.height));
                restSize.width -= item.GetDesireSize().width;
                break;
            }

            case DockLayoutTag::Top:
            case DockLayoutTag::Bottom: {
                item.Measure(Size(restSize.width, restSize.height));
                restSize.height -= item.GetDesireSize().height;
                break;
            }
        }

        if (restSize.width < 0) restSize.width = 0;
        if (restSize.height < 0) restSize.height = 0;
    }

    for (int i = count - 1; i >= 0; --i) {
        ILayout &item = this->GetChildLayoutAt(i);

        switch (_GetDockLayoutTag(item)) {
            case DockLayoutTag::Left:
            case DockLayoutTag::Right: {
                Size itemDesireSize = item.GetDesireSize();
                desireSize.width += itemDesireSize.width;
                desireSize.height = max(desireSize.height, itemDesireSize.height);
                break;
            }

            case DockLayoutTag::Top:
            case DockLayoutTag::Bottom: {
                Size itemDesireSize = item.GetDesireSize();
                desireSize.height += itemDesireSize.height;
                desireSize.width = max(desireSize.width, itemDesireSize.width);
                break;
            }
        }
    }

    this->SetDesireSize(desireSize);
}

void sw::DockLayout::ArrangeOverride(Size &finalSize)
{
    Rect restArea(0, 0, finalSize.width, finalSize.height);

    int count = this->GetChildLayoutCount();
    for (int i = 0; i < count; ++i) {
        ILayout &item = this->GetChildLayoutAt(i);

        if (this->lastChildFill && i == count - 1) {
            item.Arrange(restArea);
            break;
        }

        switch (_GetDockLayoutTag(item)) {
            case DockLayoutTag::Left: {
                Size itemDesireSize = item.GetDesireSize();
                double arrangeWidth = min(itemDesireSize.width, restArea.width);

                item.Arrange(Rect(
                    restArea.left,
                    restArea.top,
                    arrangeWidth,
                    restArea.height));

                restArea.left += arrangeWidth;
                restArea.width -= arrangeWidth;
                break;
            }

            case DockLayoutTag::Top: {
                Size itemDesireSize  = item.GetDesireSize();
                double arrangeHeight = min(itemDesireSize.height, restArea.height);

                item.Arrange(Rect(
                    restArea.left,
                    restArea.top,
                    restArea.width,
                    arrangeHeight));

                restArea.top += arrangeHeight;
                restArea.height -= arrangeHeight;
                break;
            }

            case DockLayoutTag::Right: {
                Size itemDesireSize = item.GetDesireSize();
                double arrangeWidth = min(itemDesireSize.width, restArea.width);

                item.Arrange(Rect(
                    restArea.left + restArea.width - arrangeWidth,
                    restArea.top,
                    arrangeWidth,
                    restArea.height));

                restArea.width -= arrangeWidth;
                break;
            }

            case DockLayoutTag::Bottom: {
                Size itemDesireSize  = item.GetDesireSize();
                double arrangeHeight = min(itemDesireSize.height, restArea.height);

                item.Arrange(Rect(
                    restArea.left,
                    restArea.top + restArea.height - arrangeHeight,
                    restArea.width,
                    arrangeHeight));

                restArea.height -= arrangeHeight;
                break;
            }
        }
    }
}

// DockPanel.cpp

sw::DockPanel::DockPanel()
    : LastChildFill(
          // get
          [&]() -> const bool & {
              return this->_dockLayout.lastChildFill;
          },
          // set
          [&](const bool &value) {
              this->_dockLayout.lastChildFill = value;
              this->NotifyLayoutUpdated();
          })
{
    this->Layout = &this->_dockLayout;
}

// FillLayout.cpp

void sw::FillLayout::MeasureOverride(Size &availableSize)
{
    Size desireSize;
    int count = this->GetChildLayoutCount();
    for (int i = 0; i < count; ++i) {
        ILayout &item = this->GetChildLayoutAt(i);
        item.Measure(availableSize);
        Size itemDesireSize = item.GetDesireSize();
        desireSize.width    = max(desireSize.width, itemDesireSize.width);
        desireSize.height   = max(desireSize.height, itemDesireSize.height);
    }
    this->SetDesireSize(desireSize);
}

void sw::FillLayout::ArrangeOverride(Size &finalSize)
{
    int count = this->GetChildLayoutCount();
    for (int i = 0; i < count; ++i) {
        ILayout &item = this->GetChildLayoutAt(i);
        item.Arrange(Rect(0, 0, finalSize.width, finalSize.height));
    }
}

// Font.cpp

sw::Font::Font()
{
}

sw::Font::Font(const std::wstring &name, FontCharSet charSet)
    : name(name), charSet(charSet)
{
}

sw::Font::Font(const std::wstring &name, double size, FontWeight weight)
    : name(name), size(size), weight(weight)
{
}

sw::Font::Font(const LOGFONTW &logFont)
{
    this->name          = logFont.lfFaceName;
    this->size          = std::abs(logFont.lfHeight) * Dip::ScaleY;
    this->escapement    = logFont.lfEscapement;
    this->orientation   = logFont.lfOrientation;
    this->weight        = (FontWeight)logFont.lfWeight;
    this->italic        = logFont.lfItalic;
    this->underline     = logFont.lfUnderline;
    this->strikeOut     = logFont.lfStrikeOut;
    this->charSet       = (FontCharSet)logFont.lfCharSet;
    this->outPrecision  = (FontOutPrecision)logFont.lfOutPrecision;
    this->clipPrecision = (FontClipPrecision)logFont.lfClipPrecision;
    this->quality       = (FontQuality)logFont.lfQuality;
    this->pitch         = (FontPitch)(logFont.lfPitchAndFamily & 0x07);
    this->family        = (FontFamily)(logFont.lfPitchAndFamily & 0xF8);
}

sw::Font::operator LOGFONTW() const
{
    LOGFONT logFont{};

    if (this->name.size() < 32) {
        StringCchCopyW(logFont.lfFaceName, 32, this->name.c_str());
    } else {
        logFont.lfFaceName[0] = L'\0';
    }

    logFont.lfHeight         = -std::lround(std::abs(this->size) / Dip::ScaleY);
    logFont.lfWidth          = 0;
    logFont.lfEscapement     = this->escapement;
    logFont.lfOrientation    = this->orientation;
    logFont.lfWeight         = (LONG)this->weight;
    logFont.lfItalic         = this->italic;
    logFont.lfUnderline      = this->underline;
    logFont.lfStrikeOut      = this->strikeOut;
    logFont.lfCharSet        = (BYTE)this->charSet;
    logFont.lfOutPrecision   = (BYTE)this->outPrecision;
    logFont.lfClipPrecision  = (BYTE)this->clipPrecision;
    logFont.lfQuality        = (BYTE)this->quality;
    logFont.lfPitchAndFamily = (BYTE)this->pitch | (BYTE)this->family;

    return logFont;
}

HFONT sw::Font::CreateHandle()
{
    LOGFONTW logFont = *this;
    return CreateFontIndirectW(&logFont);
}

sw::Font sw::Font::GetFont(HFONT hFont)
{
    LOGFONTW logFont{};
    GetObjectW(hFont, sizeof(logFont), &logFont);
    return logFont;
}

sw::Font &sw::Font::GetDefaultFont(bool update)
{
    static Font *pFont = nullptr;

    if (pFont == nullptr) {
        update = true;
    }

    if (update) {
        NONCLIENTMETRICSW ncm{};
        ncm.cbSize = sizeof(ncm);

        if (pFont != nullptr) {
            delete pFont;
        }

        if (SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0)) {
            pFont = new Font(ncm.lfMessageFont);
        } else {
            pFont = new Font();
        }
    }

    return *pFont;
}

// Keys.cpp

sw::KeyFlags::KeyFlags(LPARAM lParam)
{
    this->repeatCount      = (lParam >> 0) & 0xFFFF;
    this->scanCode         = (lParam >> 16) & 0xFF;
    this->isExtendedKey    = (lParam >> 24) & 0x01;
    this->contextCode      = (lParam >> 29) & 0x01;
    this->previousKeyState = (lParam >> 30) & 0x01;
    this->transitionState  = (lParam >> 31) & 0x01;
}

// Label.cpp

sw::Label::Label()
{
    this->InitControl(L"STATIC", L"Label", WS_CHILD | WS_VISIBLE);
    this->Rect = sw::Rect(0, 0, 40, 20);
}

// Layer.cpp

sw::Layer::Layer()
    : Layout(
          // get
          [&]() -> LayoutHost *const & {
              return this->_layout;
          },
          // set
          [&](LayoutHost *const &value) {
              if (value != nullptr)
                  value->Associate(this);
              this->_layout = value;
          })
{
    this->_defaultLayout.Associate(this);
}

sw::LayoutHost &sw::Layer::GetLayoutHost()
{
    return this->_layout == nullptr ? this->_defaultLayout : *this->_layout;
}

void sw::Layer::UpdateLayout()
{
    if (!this->_layoutDisabled) {
        sw::Rect clientRect = this->ClientRect;
        this->GetLayoutHost().Measure(Size(clientRect.width, clientRect.height));
        this->GetLayoutHost().Arrange(clientRect);
    }
}

void sw::Layer::Measure(const Size &availableSize)
{
    Size measureSize    = availableSize;
    Thickness margin    = this->Margin;
    sw::Rect windowRect = this->Rect;
    sw::Rect clientRect = this->ClientRect;

    // 考虑边框
    measureSize.width -= (windowRect.width - clientRect.width) + margin.left + margin.right;
    measureSize.height -= (windowRect.height - clientRect.height) + margin.top + margin.bottom;

    this->GetLayoutHost().Measure(measureSize);

    Size desireSize = this->GetDesireSize();

    if (std::isnan(desireSize.width) || std::isnan(desireSize.height)) {
        // AbsoluteLayout特殊处理：用nan表示按照普通控件处理
        this->UIElement::Measure(availableSize);
    } else {
        // 考虑边框
        desireSize.width += (windowRect.width - clientRect.width) + margin.left + margin.right;
        desireSize.height += (windowRect.width - clientRect.width) + margin.top + margin.bottom;
        this->SetDesireSize(desireSize);
    }
}

void sw::Layer::Arrange(const sw::Rect &finalPosition)
{
    this->UIElement::Arrange(finalPosition);
    this->GetLayoutHost().Arrange(this->ClientRect);
}

void sw::Layer::DisableLayout()
{
    this->_layoutDisabled = true;
}

void sw::Layer::EnableLayout()
{
    this->_layoutDisabled = false;
    this->UpdateLayout();
}

// LayoutHost.cpp

void sw::LayoutHost::Associate(ILayout *obj)
{
    this->_associatedObj = obj;
}

uint32_t sw::LayoutHost::GetLayoutTag()
{
    return this->_associatedObj->GetLayoutTag();
}

int sw::LayoutHost::GetChildLayoutCount()
{
    return this->_associatedObj->GetChildLayoutCount();
}

sw::ILayout &sw::LayoutHost::GetChildLayoutAt(int index)
{
    return this->_associatedObj->GetChildLayoutAt(index);
}

sw::Size sw::LayoutHost::GetDesireSize()
{
    return this->_associatedObj->GetDesireSize();
}

void sw::LayoutHost::SetDesireSize(const Size &size)
{
    this->_associatedObj->SetDesireSize(size);
}

void sw::LayoutHost::Measure(const Size &availableSize)
{
    Size size = availableSize;
    this->MeasureOverride(size);
}

void sw::LayoutHost::Arrange(const Rect &finalPosition)
{
    Size size(finalPosition.width, finalPosition.height);
    this->ArrangeOverride(size);
}

// MsgBox.cpp

sw::MsgBox::MsgBox(MsgBoxResult result)
    : result(result)
{
}

sw::MsgBox sw::MsgBox::Show(const WndBase *owner, const std::wstring &text, const std::wstring &caption, MsgBoxButton button)
{
    HWND hwnd = owner == NULL ? NULL : owner->Handle;
    return (MsgBoxResult)MessageBoxW(hwnd, text.c_str(), caption.c_str(), (UINT)button);
}

sw::MsgBox sw::MsgBox::ShowInfo(const WndBase *owner, const std::wstring &text, const std::wstring &caption, MsgBoxButton button)
{
    HWND hwnd = owner == NULL ? NULL : owner->Handle;
    return (MsgBoxResult)MessageBoxW(hwnd, text.c_str(), caption.c_str(), (UINT)button | MB_ICONINFORMATION);
}

sw::MsgBox sw::MsgBox::ShowError(const WndBase *owner, const std::wstring &text, const std::wstring &caption, MsgBoxButton button)
{
    HWND hwnd = owner == NULL ? NULL : owner->Handle;
    return (MsgBoxResult)MessageBoxW(hwnd, text.c_str(), caption.c_str(), (UINT)button | MB_ICONERROR);
}

sw::MsgBox sw::MsgBox::ShowWarning(const WndBase *owner, const std::wstring &text, const std::wstring &caption, MsgBoxButton button)
{
    HWND hwnd = owner == NULL ? NULL : owner->Handle;
    return (MsgBoxResult)MessageBoxW(hwnd, text.c_str(), caption.c_str(), (UINT)button | MB_ICONWARNING);
}

sw::MsgBox sw::MsgBox::ShowQuestion(const WndBase *owner, const std::wstring &text, const std::wstring &caption, MsgBoxButton button)
{
    HWND hwnd = owner == NULL ? NULL : owner->Handle;
    return (MsgBoxResult)MessageBoxW(hwnd, text.c_str(), caption.c_str(), (UINT)button | MB_ICONQUESTION);
}

const sw::MsgBox &sw::MsgBox::On(MsgBoxResult result, const MsgBoxCallback &callback) const
{
    if (this->result == result && callback)
        callback();
    return *this;
}

const sw::MsgBox &sw::MsgBox::OnOk(const MsgBoxCallback &callback) const
{
    return this->On(MsgBoxResult::Ok, callback);
}

const sw::MsgBox &sw::MsgBox::OnYes(const MsgBoxCallback &callback) const
{
    return this->On(MsgBoxResult::Yes, callback);
}

const sw::MsgBox &sw::MsgBox::OnNo(const MsgBoxCallback &callback) const
{
    return this->On(MsgBoxResult::No, callback);
}

const sw::MsgBox &sw::MsgBox::OnCancel(const MsgBoxCallback &callback) const
{
    return this->On(MsgBoxResult::Cancel, callback);
}

// Panel.cpp

sw::Panel::Panel()
{
    this->InitControl(L"STATIC", NULL, WS_CHILD | WS_VISIBLE);
    this->HorizontalAlignment = HorizontalAlignment::Stretch;
    this->VerticalAlignment   = VerticalAlignment::Stretch;
}

void sw::Panel::HandleInitialized(HWND hwnd)
{
    this->Control::HandleInitialized(hwnd);
}

void sw::Panel::Measure(const Size &availableSize)
{
    this->Layer::Measure(availableSize);
}

void sw::Panel::Arrange(const sw::Rect &finalPosition)
{
    this->Layer::Arrange(finalPosition);
}

// Point.cpp

sw::Point::Point()
    : Point(0, 0)
{
}

sw::Point::Point(double x, double y)
    : x(x), y(y)
{
}

sw::Point::Point(const POINT &point)
{
    this->x = Dip::ScaleX * point.x;
    this->y = Dip::ScaleY * point.y;
}

sw::Point::operator POINT() const
{
    POINT point{};
    point.x = std::lround(this->x / Dip::ScaleX);
    point.y = std::lround(this->y / Dip::ScaleY);
    return point;
}

std::wostream &sw::operator<<(std::wostream &wos, const Point &point)
{
    return wos << L"(" << point.x << L", " << point.y << L")";
}

// ProcMsg.cpp

sw::ProcMsg::ProcMsg()
    : ProcMsg(NULL, 0, 0, 0)
{
}

sw::ProcMsg::ProcMsg(const HWND &hwnd, const UINT &uMsg, const WPARAM &wParam, const LPARAM &lParam)
    : hwnd(hwnd), uMsg(uMsg), wParam(wParam), lParam(lParam)
{
}

// Rect.cpp

sw::Rect::Rect()
    : Rect(0, 0, 0, 0)
{
}

sw::Rect::Rect(double left, double top, double width, double height)
    : left(left), top(top), width(width), height(height)
{
}

sw::Rect::Rect(const RECT &rect)
{
    double scaleX = Dip::ScaleX;
    double scaleY = Dip::ScaleY;
    this->left    = scaleX * rect.left;
    this->top     = scaleY * rect.top;
    this->width   = scaleX * (rect.right - rect.left);
    this->height  = scaleY * (rect.bottom - rect.top);
}

sw::Rect::operator RECT() const
{
    RECT rect{};
    double scaleX;
    double scaleY;
    scaleX      = Dip::ScaleX;
    scaleY      = Dip::ScaleY;
    rect.left   = std::lround(this->left / scaleX);
    rect.top    = std::lround(this->top / scaleY);
    rect.right  = std::lround((this->left + this->width) / scaleX);
    rect.bottom = std::lround((this->top + this->height) / scaleY);
    return rect;
}

std::wostream &sw::operator<<(std::wostream &wos, const Rect &rect)
{
    return wos << L"Rect{left=" << rect.left << L", top=" << rect.top << L", width=" << rect.width << L", height=" << rect.height << L"}";
}

// RoutedEvent.cpp

sw::RoutedEventArgs::RoutedEventArgs(RoutedEventType eventType)
    : eventType(eventType)
{
}

// RoutedEventArgs.cpp

sw::SizeChangedEventArgs::SizeChangedEventArgs(Size newClientSize)
    : newClientSize(newClientSize)
{
}

sw::PositionChangedEventArgs::PositionChangedEventArgs(Point newClientPosition)
    : newClientPosition(newClientPosition)
{
}

sw::TextChangedEventArgs::TextChangedEventArgs(const wchar_t *newText)
    : newText(newText)
{
}

sw::GotCharEventArgs::GotCharEventArgs(wchar_t ch, KeyFlags flags)
    : ch(ch), flags(flags)
{
}

sw::KeyDownEventArgs::KeyDownEventArgs(VirtualKey key, KeyFlags falgs)
    : key(key), flags(flags)
{
}

sw::KeyUpEventArgs::KeyUpEventArgs(VirtualKey key, KeyFlags falgs)
    : key(key), flags(flags)
{
}

sw::MouseMoveEventArgs::MouseMoveEventArgs(Point mousePosition, MouseKey keyState)
    : mousePosition(mousePosition), keyState(keyState)
{
}

sw::MouseWheelEventArgs::MouseWheelEventArgs(int wheelDelta, Point mousePosition, MouseKey keyState)
    : wheelDelta(wheelDelta), mousePosition(mousePosition), keyState(keyState)
{
}

sw::MouseButtonDownEventArgs::MouseButtonDownEventArgs(MouseKey key, Point mousePosition, MouseKey keyState)
    : key(key), mousePosition(mousePosition), keyState(keyState)
{
}

sw::MouseButtonUpEventArgs::MouseButtonUpEventArgs(MouseKey key, Point mousePosition, MouseKey keyState)
    : key(key), mousePosition(mousePosition), keyState(keyState)
{
}

// Screen.cpp

const sw::ReadOnlyProperty<double> sw::Screen::Width(
    []() -> const double & {
        static double width;
        width = GetSystemMetrics(SM_CXSCREEN) * Dip::ScaleX;
        return width;
    } //
);

const sw::ReadOnlyProperty<double> sw::Screen::Height(
    []() -> const double & {
        static double height;
        height = GetSystemMetrics(SM_CYSCREEN) * Dip::ScaleY;
        return height;
    } //
);

// Size.cpp

sw::Size::Size()
    : Size(0, 0)
{
}

sw::Size::Size(double width, double height)
    : width(width), height(height)
{
}

sw::Size::Size(const SIZE &size)
{
    this->width  = size.cx * Dip::ScaleX;
    this->height = size.cy * Dip::ScaleY;
}

sw::Size::operator SIZE() const
{
    SIZE size{};
    size.cx = std::lround(this->width / Dip::ScaleX);
    size.cy = std::lround(this->height / Dip::ScaleY);
    return size;
}

std::wostream &sw::operator<<(std::wostream &wos, const Size &size)
{
    return wos << L"Size{width=" << size.width << L", height=" << size.height << L"}";
}

// StackLayout.cpp

void sw::StackLayout::MeasureOverride(Size &availableSize)
{
    this->orientation == Orientation::Horizontal
        ? this->StackLayoutH::MeasureOverride(availableSize)
        : this->StackLayoutV::MeasureOverride(availableSize);
}

void sw::StackLayout::ArrangeOverride(Size &finalSize)
{
    this->orientation == Orientation::Horizontal
        ? this->StackLayoutH::ArrangeOverride(finalSize)
        : this->StackLayoutV::ArrangeOverride(finalSize);
}

// StackLayoutH.cpp

void sw::StackLayoutH::MeasureOverride(Size &availableSize)
{
    Size desireSize;
    int childCount = this->GetChildLayoutCount();

    for (int i = 0; i < childCount; ++i) {
        ILayout &item = GetChildLayoutAt(i);
        item.Measure(Size(INFINITY, std::isinf(availableSize.height) ? INFINITY : availableSize.height));

        Size itemDesireSize = item.GetDesireSize();
        desireSize.width += itemDesireSize.width;
        desireSize.height = max(desireSize.height, itemDesireSize.height);
    }

    this->SetDesireSize(desireSize);
}

void sw::StackLayoutH::ArrangeOverride(Size &finalSize)
{
    double width   = 0;
    int childCount = this->GetChildLayoutCount();

    for (int i = 0; i < childCount; ++i) {
        ILayout &item       = GetChildLayoutAt(i);
        Size itemDesireSize = item.GetDesireSize();
        item.Arrange(Rect(width, 0, itemDesireSize.width, finalSize.height));
        width += itemDesireSize.width;
    }
}

// StackLayoutV.cpp

void sw::StackLayoutV::MeasureOverride(Size &availableSize)
{
    Size desireSize;
    int childCount = this->GetChildLayoutCount();

    for (int i = 0; i < childCount; ++i) {
        ILayout &item = GetChildLayoutAt(i);
        item.Measure(Size(std::isinf(availableSize.width) ? INFINITY : availableSize.width, INFINITY));

        Size itemDesireSize = item.GetDesireSize();
        desireSize.height += itemDesireSize.height;
        desireSize.width = max(desireSize.width, itemDesireSize.width);
    }

    this->SetDesireSize(desireSize);
}

void sw::StackLayoutV::ArrangeOverride(Size &finalSize)
{
    double top     = 0;
    int childCount = this->GetChildLayoutCount();

    for (int i = 0; i < childCount; ++i) {
        ILayout &item       = GetChildLayoutAt(i);
        Size itemDesireSize = item.GetDesireSize();
        item.Arrange(Rect(0, top, finalSize.width, itemDesireSize.height));
        top += itemDesireSize.height;
    }
}

// StackPanel.cpp

sw::StackPanel::StackPanel()
    : Orientation(
          // get
          [&]() -> const sw::Orientation & {
              return this->_stackLayout.orientation;
          },
          // set
          [&](const sw::Orientation &value) {
              this->_stackLayout.orientation = value;
              this->NotifyLayoutUpdated();
          })
{
    this->Layout = &this->_stackLayout;
}

// Thickness.cpp

sw::Thickness::Thickness()
    : Thickness(0, 0, 0, 0)
{
}

sw::Thickness::Thickness(double thickness)
    : Thickness(thickness, thickness, thickness, thickness)
{
}

sw::Thickness::Thickness(double left, double top, double right, double bottom)
    : left(left), top(top), right(right), bottom(bottom)
{
}

std::wostream &sw::operator<<(std::wostream &wos, const Thickness &thickness)
{
    return wos << L"Thickness{left=" << thickness.left << L", top=" << thickness.top << L", right=" << thickness.right << L", bottom=" << thickness.bottom << L"}";
}

// UIElement.cpp

sw::UIElement::UIElement()
    : Margin(
          // get
          [&]() -> const Thickness & {
              return this->_margin;
          },
          // set
          [&](const Thickness &value) {
              this->_margin = value;
              this->NotifyLayoutUpdated();
          }),

      HorizontalAlignment(
          [&]() -> const sw::HorizontalAlignment & {
              return this->_horizontalAlignment;
          },
          [&](const sw::HorizontalAlignment &value) {
              if (value == this->_horizontalAlignment) {
                  return;
              }
              if (value == sw::HorizontalAlignment::Stretch) {
                  this->_horizontalAlignment = value;
                  this->_origionalSize.width = this->Width;
              } else {
                  this->_horizontalAlignment = value;
                  this->Width                = this->_origionalSize.width;
              }
              this->NotifyLayoutUpdated();
          }),

      VerticalAlignment(
          [&]() -> const sw::VerticalAlignment & {
              return this->_verticalAlignment;
          },
          [&](const sw::VerticalAlignment &value) {
              if (value == this->_verticalAlignment) {
                  return;
              }
              if (value == sw::VerticalAlignment::Stretch) {
                  this->_verticalAlignment    = value;
                  this->_origionalSize.height = this->Height;
              } else {
                  this->_verticalAlignment = value;
                  this->Height             = this->_origionalSize.height;
              }
              this->NotifyLayoutUpdated();
          }),

      ChildCount(
          // get
          [&]() -> const int & {
              static int count;
              count = (int)this->_children.size();
              return count;
          }),

      CollapseWhenHide(
          // get
          [&]() -> const bool & {
              return this->_collapseWhenHide;
          },
          // set
          [&](const bool &value) {
              if (this->_collapseWhenHide != value) {
                  this->_collapseWhenHide = value;
                  if (!this->Visible)
                      this->NotifyLayoutUpdated();
              }
          }),

      Parent(
          // get
          [&]() -> UIElement *const & {
              return this->_parent;
          }),

      LayoutTag(
          // get
          [&]() -> const uint32_t & {
              return this->_layoutTag;
          },
          // set
          [&](const uint32_t &value) {
              this->_layoutTag = value;
              this->NotifyLayoutUpdated();
          })
{
}

sw::UIElement::~UIElement()
{
    // 将自己从父窗口的children中移除
    this->SetParent(nullptr);

    // 取消子窗口对自己的引用
    for (UIElement *item : this->_children)
        item->_parent = nullptr;
    this->_children.clear();
}

void sw::UIElement::RegisterRoutedEvent(RoutedEventType eventType, const RoutedEvent &event)
{
    if (event) {
        this->_eventMap[eventType] = event;
    } else {
        this->UnregisterRoutedEvent(eventType);
    }
}

void sw::UIElement::UnregisterRoutedEvent(RoutedEventType eventType)
{
    if (this->IsRoutedEventRegistered(eventType))
        this->_eventMap.erase(eventType);
}

bool sw::UIElement::IsRoutedEventRegistered(RoutedEventType eventType)
{
    return this->_eventMap.count(eventType);
}

bool sw::UIElement::AddChild(UIElement *element)
{
    if (element == nullptr) {
        return false;
    }

    if (std::find(this->_children.begin(), this->_children.end(), element) != this->_children.end()) {
        return false;
    }

    if (!element->WndBase::SetParent(this)) {
        return false;
    }

    element->SetStyle(WS_CHILD, true);
    this->_children.push_back(element);
    this->NotifyLayoutUpdated();
    return true;
}

bool sw::UIElement::AddChild(UIElement &element)
{
    return this->AddChild(&element);
}

bool sw::UIElement::AddChild(UIElement *element, uint32_t layoutTag)
{
    if (element == nullptr) {
        return false;
    }
    element->_layoutTag = layoutTag;
    return this->AddChild(element);
}

bool sw::UIElement::AddChild(UIElement &element, uint32_t layoutTag)
{
    element._layoutTag = layoutTag;
    return this->AddChild(&element);
}

bool sw::UIElement::RemoveChildAt(int index)
{
    if (index < 0 || index >= this->_children.size()) {
        return false;
    }

    std::vector<UIElement *>::iterator it =
        this->_children.begin() + index;

    if (!(*it)->WndBase::SetParent(nullptr)) {
        return false;
    }

    this->_children.erase(it);
    this->NotifyLayoutUpdated();
    return true;
}

bool sw::UIElement::RemoveChild(UIElement *element)
{
    if (element == nullptr) {
        return false;
    }

    std::vector<UIElement *>::iterator it =
        std::find(this->_children.begin(), this->_children.end(), element);

    if (it == this->_children.end()) {
        return false;
    }

    if (!element->WndBase::SetParent(nullptr)) {
        return false;
    }

    this->_children.erase(it);
    this->NotifyLayoutUpdated();
    return true;
}

bool sw::UIElement::RemoveChild(UIElement &element)
{
    return this->RemoveChild(&element);
}

void sw::UIElement::Clear()
{
    for (UIElement *item : this->_children) {
        item->WndBase::SetParent(nullptr);
    }
    this->_children.clear();
}

int sw::UIElement::IndexOf(UIElement *element)
{
    std::vector<UIElement *>::iterator beg = this->_children.begin();
    std::vector<UIElement *>::iterator end = this->_children.end();
    std::vector<UIElement *>::iterator it  = std::find(beg, end, element);
    return it == end ? -1 : int(it - beg);
}

int sw::UIElement::IndexOf(UIElement &element)
{
    return this->IndexOf(&element);
}

sw::UIElement &sw::UIElement::operator[](int index) const
{
    return *this->_children[index];
}

uint32_t sw::UIElement::GetLayoutTag()
{
    return this->_layoutTag;
}

int sw::UIElement::GetChildLayoutCount()
{
    this->_childrenNotCollapsed.clear();
    for (UIElement *item : this->_children) {
        if (item->Visible || !item->_collapseWhenHide)
            this->_childrenNotCollapsed.push_back(item);
    }
    return (int)this->_childrenNotCollapsed.size();
}

sw::ILayout &sw::UIElement::GetChildLayoutAt(int index)
{
    return *this->_childrenNotCollapsed[index];
}

sw::Size sw::UIElement::GetDesireSize()
{
    return this->_desireSize;
}

void sw::UIElement::SetDesireSize(const Size &size)
{
    this->_desireSize = size;
}

void sw::UIElement::Measure(const Size &availableSize)
{
    sw::Rect rect     = this->Rect;
    Thickness &margin = this->_margin;

    if (this->_horizontalAlignment == HorizontalAlignment::Stretch) {
        rect.width = this->_origionalSize.width;
    }
    if (this->_verticalAlignment == VerticalAlignment::Stretch) {
        rect.height = this->_origionalSize.height;
    }

    this->SetDesireSize(Size(
        rect.width + margin.left + margin.right,
        rect.height + margin.top + margin.bottom));
}

void sw::UIElement::Arrange(const sw::Rect &finalPosition)
{
    this->_arranging  = true;
    Size &desireSize  = this->_desireSize;
    Thickness &margin = this->_margin;

    sw::Rect rect = this->Rect;
    rect.width    = desireSize.width - margin.left - margin.right;
    rect.height   = desireSize.height - margin.top - margin.bottom;

    switch (this->_horizontalAlignment) {
        case HorizontalAlignment::Center: {
            rect.left = finalPosition.left + (finalPosition.width - rect.width - margin.left - margin.right) / 2 + margin.left;
            break;
        }
        case HorizontalAlignment::Stretch: {
            rect.left  = finalPosition.left + margin.left;
            rect.width = finalPosition.width - margin.left - margin.right;
            break;
        }
        case HorizontalAlignment::Left: {
            rect.left = finalPosition.left + margin.left;
            break;
        }
        case HorizontalAlignment::Right: {
            rect.left = finalPosition.left + finalPosition.width - rect.width - margin.right;
            break;
        }
    }

    switch (this->_verticalAlignment) {
        case VerticalAlignment::Center: {
            rect.top = finalPosition.top + (finalPosition.height - rect.height - margin.top - margin.bottom) / 2 + margin.top;
            break;
        }
        case VerticalAlignment::Stretch: {
            rect.top    = finalPosition.top + margin.top;
            rect.height = finalPosition.height - margin.top - margin.bottom;
            break;
        }
        case VerticalAlignment::Top: {
            rect.top = finalPosition.top + margin.top;
            break;
        }
        case VerticalAlignment::Bottom: {
            rect.top = finalPosition.top + finalPosition.height - rect.height - margin.bottom;
            break;
        }
    }

    // AbsoluteLayout特殊处理：用nan表示不需要调整位置
    if (std::isnan(finalPosition.left) || std::isnan(finalPosition.top)) {
        rect.left = this->Left;
        rect.top  = this->Top;
    }

    rect.width       = max(rect.width, 0);
    rect.height      = max(rect.height, 0);
    this->Rect       = rect;
    this->_arranging = false;

    this->Redraw();
}

void sw::UIElement::RaiseRoutedEvent(RoutedEventType eventType)
{
    RoutedEventArgs eventArgs(eventType);
    this->RaiseRoutedEvent(eventArgs);
}

void sw::UIElement::RaiseRoutedEvent(RoutedEventArgs &eventArgs)
{
    UIElement *element = this;
    do {
        if (element->IsRoutedEventRegistered(eventArgs.eventType)) {
            element->_eventMap[eventArgs.eventType](*this, eventArgs);
        }
        if (eventArgs.handled) {
            break;
        } else {
            element = element->_parent;
        }
    } while (element != nullptr);
}

sw::UIElement &sw::UIElement::GetRootElement()
{
    UIElement *root;
    UIElement *element = this;
    do {
        root    = element;
        element = element->_parent;
    } while (element != nullptr);
    return *root;
}

bool sw::UIElement::IsRootElement()
{
    return this->_parent == nullptr;
}

void sw::UIElement::NotifyLayoutUpdated()
{
    if (!this->_arranging) {
        UIElement &root = this->GetRootElement();
        SendMessageW(root.Handle, WM_UpdateLayout, NULL, NULL);
    }
}

bool sw::UIElement::SetParent(WndBase *parent)
{
    UIElement *oldParentElement = this->_parent;
    UIElement *newParentElement = dynamic_cast<UIElement *>(parent);

    if (newParentElement == nullptr) {
        if (oldParentElement == nullptr) {
            return this->WndBase::SetParent(parent);
        } else {
            return oldParentElement->RemoveChild(this) &&
                   this->WndBase::SetParent(parent);
        }
    } else {
        if (oldParentElement == nullptr) {
            return newParentElement->AddChild(this);
        } else {
            return oldParentElement->RemoveChild(this) &&
                   newParentElement->AddChild(this);
        }
    }
}

void sw::UIElement::ParentChanged(WndBase *newParent)
{
    this->_parent = dynamic_cast<UIElement *>(newParent);
}

bool sw::UIElement::OnClose()
{
    this->SetParent(nullptr);
    return this->WndBase::OnClose();
}

bool sw::UIElement::OnMove(Point newClientPosition)
{
    PositionChangedEventArgs args(newClientPosition);
    this->RaiseRoutedEvent(args);
    return true;
}

bool sw::UIElement::OnSize(Size newClientSize)
{
    if (this->_horizontalAlignment != sw::HorizontalAlignment::Stretch) {
        this->_origionalSize.width = this->Width;
    }
    if (this->_verticalAlignment != sw::VerticalAlignment::Stretch) {
        this->_origionalSize.height = this->Height;
    }

    SizeChangedEventArgs args(newClientSize);
    this->RaiseRoutedEvent(args);

    this->NotifyLayoutUpdated();
    return true;
}

void sw::UIElement::OnTextChanged(const std::wstring &newText)
{
    TextChangedEventArgs args(newText.c_str());
    this->RaiseRoutedEvent(args);
}

void sw::UIElement::VisibleChanged(bool newVisible)
{
    if (newVisible || this->_collapseWhenHide) {
        this->NotifyLayoutUpdated();
    }
}

bool sw::UIElement::OnSetFocus(HWND hPrevFocus)
{
    this->RaiseRoutedEvent(UIElement_GotFocus);
    return true;
}

bool sw::UIElement::OnKillFocus(HWND hNextFocus)
{
    this->RaiseRoutedEvent(UIElement_LostFocus);
    return true;
}

bool sw::UIElement::OnChar(wchar_t ch, KeyFlags flags)
{
    GotCharEventArgs args(ch, flags);
    this->RaiseRoutedEvent(args);
    return false;
}

bool sw::UIElement::OnKeyDown(VirtualKey key, KeyFlags flags)
{
    KeyDownEventArgs args(key, flags);
    this->RaiseRoutedEvent(args);
    return false;
}

bool sw::UIElement::OnKeyUp(VirtualKey key, KeyFlags flags)
{
    KeyUpEventArgs args(key, flags);
    this->RaiseRoutedEvent(args);
    return false;
}

bool sw::UIElement::OnMouseMove(Point mousePosition, MouseKey keyState)
{
    MouseMoveEventArgs args(mousePosition, keyState);
    this->RaiseRoutedEvent(args);
    return false;
}

bool sw::UIElement::OnMouseLeave()
{
    this->RaiseRoutedEvent(UIElement_MouseLeave);
    return false;
}

bool sw::UIElement::OnMouseWheel(int wheelDelta, Point mousePosition, MouseKey keyState)
{
    MouseWheelEventArgs args(wheelDelta, mousePosition, keyState);
    this->RaiseRoutedEvent(args);
    return false;
}

bool sw::UIElement::OnMouseLeftButtonDown(Point mousePosition, MouseKey keyState)
{
    MouseButtonDownEventArgs args(MouseKey::MouseLeft, mousePosition, keyState);
    this->RaiseRoutedEvent(args);
    return false;
}

bool sw::UIElement::OnMouseLeftButtonUp(Point mousePosition, MouseKey keyState)
{
    MouseButtonUpEventArgs args(MouseKey::MouseLeft, mousePosition, keyState);
    this->RaiseRoutedEvent(args);
    return false;
}

bool sw::UIElement::OnMouseRightButtonDown(Point mousePosition, MouseKey keyState)
{
    MouseButtonDownEventArgs args(MouseKey::MouseRight, mousePosition, keyState);
    this->RaiseRoutedEvent(args);
    return false;
}

bool sw::UIElement::OnMouseRightButtonUp(Point mousePosition, MouseKey keyState)
{
    MouseButtonUpEventArgs args(MouseKey::MouseRight, mousePosition, keyState);
    this->RaiseRoutedEvent(args);
    return false;
}

bool sw::UIElement::OnMouseMiddleButtonDown(Point mousePosition, MouseKey keyState)
{
    MouseButtonDownEventArgs args(MouseKey::MouseMiddle, mousePosition, keyState);
    this->RaiseRoutedEvent(args);
    return false;
}

bool sw::UIElement::OnMouseMiddleButtonUp(Point mousePosition, MouseKey keyState)
{
    MouseButtonUpEventArgs args(MouseKey::MouseMiddle, mousePosition, keyState);
    this->RaiseRoutedEvent(args);
    return false;
}

// Utils.cpp

std::wstring sw::Utils::ToWideStr(const std::string &str, bool utf8)
{
    int code = utf8 ? CP_UTF8 : CP_ACP;
    int size = MultiByteToWideChar(code, 0, str.c_str(), -1, nullptr, 0);
    std::wstring wstr(size - 1, L'\0');
    MultiByteToWideChar(code, 0, str.c_str(), -1, &wstr[0], size);
    return wstr;
}

std::string sw::Utils::ToMultiByteStr(const std::wstring &wstr, bool utf8)
{
    int code = utf8 ? CP_UTF8 : CP_ACP;
    int size = WideCharToMultiByte(code, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string str(size - 1, '\0');
    WideCharToMultiByte(code, 0, wstr.c_str(), -1, &str[0], size, nullptr, nullptr);
    return str;
}

std::wostream &sw::operator<<(std::wostream &wos, const std::string &str)
{
    return wos << Utils::ToWideStr(str);
}

std::wostream &sw::operator<<(std::wostream &wos, const char *str)
{
    return wos << Utils::ToWideStr(str);
}

// Window.cpp

/**
 * @brief 记录当前创建的窗口数
 */
static unsigned int _windowCount = 0;

/**
 * @brief DPI更新时调用该函数递归地更新所有子项的字体
 */
static void _UpdateFontForAllChild(sw::UIElement &element);

/**
 * @brief 是否在关闭所有窗口后退出程序
 */
bool sw::Window::PostQuitWhenAllClosed = true;

/**
 * @brief 程序的当前活动窗体
 */
const sw::ReadOnlyProperty<sw::Window *> sw::Window::ActiveWindow(
    []() -> sw::Window *const & {
        static sw::Window *pWindow;
        HWND hwnd = GetForegroundWindow();
        pWindow   = dynamic_cast<sw::Window *>(sw::WndBase::GetWndBase(hwnd));
        return pWindow;
    } //
);

sw::Window::Window()
    : State(
          // get
          [&]() -> const WindowState & {
              static WindowState state;
              HWND hwnd = this->Handle;
              if (IsIconic(hwnd)) {
                  state = WindowState::Minimized;
              } else if (IsZoomed(hwnd)) {
                  state = WindowState::Maximized;
              } else {
                  state = WindowState::Normal;
              }
              return state;
          },
          // set
          [&](const WindowState &value) {
              HWND hwnd = this->Handle;
              switch (value) {
                  case WindowState::Normal:
                      ShowWindow(hwnd, SW_RESTORE);
                      break;
                  case WindowState::Minimized:
                      ShowWindow(hwnd, SW_MINIMIZE);
                      break;
                  case WindowState::Maximized:
                      ShowWindow(hwnd, SW_MAXIMIZE);
                      break;
              }
          }),

      SizeBox(
          // get
          [&]() -> const bool & {
              static bool result;
              result = this->GetStyle(WS_SIZEBOX);
              return result;
          },
          // set
          [&](const bool &value) {
              this->SetStyle(WS_SIZEBOX, value);
          }),

      MaximizeBox(
          // get
          [&]() -> const bool & {
              static bool result;
              result = this->GetStyle(WS_MAXIMIZEBOX);
              return result;
          },
          // set
          [&](const bool &value) {
              this->SetStyle(WS_MAXIMIZEBOX, value);
          }),

      MinimizeBox(
          // get
          [&]() -> const bool & {
              static bool result;
              result = this->GetStyle(WS_MINIMIZEBOX);
              return result;
          },
          // set
          [&](const bool &value) {
              this->SetStyle(WS_MINIMIZEBOX, value);
          }),

      Topmost(
          // get
          [&]() -> const bool & {
              static bool result;
              result = this->GetExtendedStyle(WS_EX_TOPMOST);
              return result;
          },
          // set
          [&](const bool &value) {
              /*this->SetExtendedStyle(WS_EX_TOPMOST, value);*/
              HWND hWndInsertAfter = value ? HWND_TOPMOST : HWND_NOTOPMOST;
              SetWindowPos(this->Handle, hWndInsertAfter, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
          }),

      ToolWindow(
          // get
          [&]() -> const bool & {
              static bool result;
              result = this->GetExtendedStyle(WS_EX_TOOLWINDOW);
              return result;
          },
          // set
          [&](const bool &value) {
              this->SetExtendedStyle(WS_EX_TOOLWINDOW, value);
          }),

      BackColor(
          // get
          [&]() -> const Color & {
              return this->_backColor;
          },
          // set
          [&](const Color &value) {
              this->_backColor = value;
              this->Redraw();
          }),

      MaxWidth(
          // get
          [&]() -> const double & {
              return this->_maxWidth;
          },
          // set
          [&](const double &value) {
              this->_maxWidth = value;
              this->Width     = this->Width;
          }),

      MaxHeight(
          // get
          [&]() -> const double & {
              return this->_maxHeight;
          },
          // set
          [&](const double &value) {
              this->_maxHeight = value;
              this->Height     = this->Height;
          }),

      MinWidth(
          // get
          [&]() -> const double & {
              return this->_minWidth;
          },
          // set
          [&](const double &value) {
              this->_minWidth = value;
              this->Width     = this->Width;
          }),

      MinHeight(
          // get
          [&]() -> const double & {
              return this->_minHeight;
          },
          // set
          [&](const double &value) {
              this->_minHeight = value;
              this->Height     = this->Height;
          })
{
    this->InitWindow(
        L"Window",           // Window text
        WS_OVERLAPPEDWINDOW, // Window style
        NULL,                // Parent window
        NULL);               // Menu
}

LRESULT sw::Window::WndProc(const ProcMsg &refMsg)
{
    switch (refMsg.uMsg) {
        case WM_SHOWWINDOW: {
            // 窗口首次启动时按照StartupLocation修改位置
            if (this->_isFirstShow) {
                this->_isFirstShow = false;
                if (this->StartupLocation == WindowStartupLocation::CenterScreen) {
                    sw::Rect rect = this->Rect;
                    rect.left     = (Screen::Width - rect.width) / 2;
                    rect.top      = (Screen::Height - rect.height) / 2;
                    this->Rect    = rect;
                }
            }
            return this->UIElement::WndProc(refMsg);
        }

        case WM_GETMINMAXINFO: {
            double scaleX     = Dip::ScaleX;
            double scaleY     = Dip::ScaleY;
            PMINMAXINFO pInfo = reinterpret_cast<PMINMAXINFO>(refMsg.lParam);
            // 按照设置限制窗口大小
            if (this->_maxWidth > 0) {
                LONG maxWidth           = std::lround(this->_maxWidth / scaleX);
                pInfo->ptMaxTrackSize.x = min(pInfo->ptMaxTrackSize.x, maxWidth);
            }
            if (this->_maxHeight > 0) {
                LONG maxHeight          = std::lround(this->_maxHeight / scaleY);
                pInfo->ptMaxTrackSize.y = min(pInfo->ptMaxTrackSize.y, maxHeight);
            }
            if (this->_minWidth > 0) {
                LONG minWidth           = std::lround(this->_minWidth / scaleX);
                pInfo->ptMinTrackSize.x = max(pInfo->ptMinTrackSize.x, minWidth);
            }
            if (this->_minHeight > 0) {
                LONG minHeight          = std::lround(this->_minHeight / scaleY);
                pInfo->ptMinTrackSize.y = max(pInfo->ptMinTrackSize.y, minHeight);
            }
            return 0;
        }

        case WM_DPICHANGED: {
            Dip::Update(LOWORD(refMsg.wParam), HIWORD(refMsg.wParam));
            this->UpdateLayout();
            _UpdateFontForAllChild(*this);
            return 0;
        }

        case WM_UpdateLayout: {
            this->UpdateLayout();
            return 0;
        }

        default: {
            return this->UIElement::WndProc(refMsg);
        }
    }
}

bool sw::Window::OnClose()
{
    WindowClosingEventArgs args;
    RaiseRoutedEvent(args);

    if (!args.cancel) {
        this->UIElement::OnClose();
    }

    return true;
}

bool sw::Window::OnCreate()
{
    ++_windowCount;
    return true;
}

bool sw::Window::OnDestroy()
{
    // 触发路由事件
    RaiseRoutedEvent(RoutedEventType::Window_Closed);
    // 所有窗口都关闭时若PostQuitWhenAllClosed为true则退出程序
    if (!--_windowCount && PostQuitWhenAllClosed) {
        App::Quit();
    }
    return true;
}

bool sw::Window::OnPaint()
{
    PAINTSTRUCT ps;
    HWND hwnd     = this->Handle;
    HDC hdc       = BeginPaint(hwnd, &ps);
    HBRUSH hBrush = CreateSolidBrush(this->_backColor);
    FillRect(hdc, &ps.rcPaint, hBrush);
    DeleteObject(hBrush);
    EndPaint(hwnd, &ps);
    return true;
}

void sw::Window::Show()
{
    this->WndBase::Show(SW_SHOW);
}

void sw::Window::Measure(const Size &availableSize)
{
    this->Layer::Measure(availableSize);
}

void sw::Window::Arrange(const sw::Rect &finalPosition)
{
    this->Layer::Arrange(finalPosition);
}

void _UpdateFontForAllChild(sw::UIElement &element)
{
    element.UpdateFont();

    int count = element.ChildCount;
    for (int i = 0; i < count; ++i) {
        _UpdateFontForAllChild(element[i]);
    }
}

// WndBase.cpp

/**
 * @brief 窗口类名
 */
static PCWSTR WINDOW_CLASS_NAME = L"sw::WndBase";

/**
 * @brief 控件初始化时所在的窗口
 */
static HWND _controlInitContainer = NULL;

LRESULT sw::WndBase::_WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    WndBase *pWnd = WndBase::GetWndBase(hwnd);

    if (pWnd == NULL && (uMsg == WM_NCCREATE || uMsg == WM_CREATE)) {
        LPCREATESTRUCTW pCreate;
        pCreate = reinterpret_cast<LPCREATESTRUCTW>(lParam);
        pWnd    = reinterpret_cast<WndBase *>(pCreate->lpCreateParams);
    }

    if (pWnd != NULL) {
        return pWnd->WndProc(ProcMsg(hwnd, uMsg, wParam, lParam));
    } else {
        return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }
}

sw::WndBase::WndBase()
    : Handle(
          // get
          [&]() -> const HWND & {
              return this->_hwnd;
          }),

      Font(
          // get
          [&]() -> const sw::Font & {
              return this->_font;
          },
          // set
          [&](const sw::Font &value) {
              this->_font = value;
              this->UpdateFont();
          }),

      FontName(
          // get
          [&]() -> const std::wstring & {
              return this->_font.name;
          },
          // set
          [&](const std::wstring &value) {
              this->_font.name = value;
              this->UpdateFont();
          }),

      FontSize(
          // get
          [&]() -> const double & {
              return this->_font.size;
          },
          // set
          [&](const double &value) {
              this->_font.size = value;
              this->UpdateFont();
          }),

      FontWeight(
          // get
          [&]() -> const sw::FontWeight & {
              return this->_font.weight;
          },
          // set
          [&](const sw::FontWeight &value) {
              this->_font.weight = value;
              this->UpdateFont();
          }),

      Rect(
          // get
          [&]() -> const sw::Rect & {
              return this->_rect;
          },
          // set
          [&](const sw::Rect &value) {
              /*RECT rect = value.GetRECT();
              MoveWindow(this->_hwnd, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, TRUE);*/
              double scaleX = Dip::ScaleX;
              double scaleY = Dip::ScaleY;
              int left      = std::lround(value.left / scaleX);
              int top       = std::lround(value.top / scaleY);
              int width     = std::lround(value.width / scaleX);
              int height    = std::lround(value.height / scaleY);
              SetWindowPos(this->_hwnd, NULL, left, top, width, height, SWP_NOACTIVATE | SWP_NOZORDER);
          }),

      Left(
          // get
          [&]() -> const double & {
              return this->_rect.left;
          },
          // set
          [&](const double &value) {
              int x = std::lround(value / Dip::ScaleX);
              int y = std::lround(this->_rect.top / Dip::ScaleY);
              SetWindowPos(this->_hwnd, NULL, x, y, 0, 0, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
          }),

      Top(
          // get
          [&]() -> const double & {
              return this->_rect.top;
          },
          // set
          [&](const double &value) {
              int x = std::lround(this->_rect.left / Dip::ScaleX);
              int y = std::lround(value / Dip::ScaleY);
              SetWindowPos(this->_hwnd, NULL, x, y, 0, 0, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
          }),

      Width(
          // get
          [&]() -> const double & {
              return this->_rect.width;
          },
          // set
          [&](const double &value) {
              int cx = std::lround(value / Dip::ScaleX);
              int cy = std::lround(this->_rect.height / Dip::ScaleY);
              SetWindowPos(this->_hwnd, NULL, 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
          }),

      Height(
          // get
          [&]() -> const double & {
              return this->_rect.height;
          },
          // set
          [&](const double &value) {
              int cx = std::lround(this->_rect.width / Dip::ScaleX);
              int cy = std::lround(value / Dip::ScaleY);
              SetWindowPos(this->_hwnd, NULL, 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
          }),

      ClientRect(
          // get
          [&]() -> const sw::Rect & {
              static sw::Rect swRect;
              RECT rect;
              GetClientRect(this->_hwnd, &rect);
              swRect = rect;
              return swRect;
          }),

      ClientWidth(
          // get
          [&]() -> const double & {
              return this->ClientRect->width;
          }),

      ClientHeight(
          // get
          [&]() -> const double & {
              return this->ClientRect->height;
          }),

      Enabled(
          // get
          [&]() -> const bool & {
              static bool enabled;
              enabled = IsWindowEnabled(this->_hwnd);
              return enabled;
          },
          // set
          [&](const bool &value) {
              EnableWindow(this->_hwnd, value);
          }),

      Visible(
          // get
          [&]() -> const bool & {
              static bool visible;
              visible = IsWindowVisible(this->_hwnd);
              return visible;
          },
          // set
          [&](const bool &value) {
              if (value != this->Visible) {
                  ShowWindow(this->_hwnd, value ? SW_SHOW : SW_HIDE);
                  this->VisibleChanged(value);
              }
          }),

      Text(
          // get
          [&]() -> const std::wstring & {
              return this->_text;
          },
          // set
          [&](const std::wstring &value) {
              std::wstring newText = value;
              if (this->OnSetText(newText))
                  SetWindowTextW(this->_hwnd, newText.c_str());
          }),

      Focused(
          // get
          [&]() -> const bool & {
              return this->_focused;
          },
          // set
          [&](const bool &value) {
              SetFocus(value ? this->_hwnd : NULL);
          }),

      Parent(
          // get
          [&]() -> WndBase *const & {
              static WndBase *pWndBase;
              HWND hwnd = GetParent(this->_hwnd);
              pWndBase  = WndBase::GetWndBase(hwnd);
              return pWndBase;
          }),

      IsDestroyed(
          // get
          [&]() -> const bool & {
              return this->_isDestroyed;
          })
{
    static WNDCLASSEXW wc = {0};

    if (wc.cbSize == 0) {
        wc.cbSize        = sizeof(wc);
        wc.hCursor       = LoadCursorW(NULL, IDC_ARROW);
        wc.hInstance     = App::Instance;
        wc.lpfnWndProc   = WndBase::_WndProc;
        wc.lpszClassName = WINDOW_CLASS_NAME;
    }

    RegisterClassExW(&wc);
    this->_font = sw::Font::GetDefaultFont();
}

sw::WndBase::~WndBase()
{
    if (this->_hwnd != NULL) {
        DestroyWindow(this->_hwnd);
    }
    if (this->_hfont != NULL) {
        DeleteObject(this->_hfont);
    }
}

void sw::WndBase::InitWindow(LPCWSTR lpWindowName, DWORD dwStyle, HWND hWndParent, HMENU hMenu)
{
    if (this->_hwnd == NULL) {

        this->_text = lpWindowName ? lpWindowName : L"";

        this->_hwnd = CreateWindowExW(
            0,                 // Optional window styles
            WINDOW_CLASS_NAME, // Window class
            lpWindowName,      // Window text
            dwStyle,           // Window style

            // Size and position
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

            hWndParent,    // Parent window
            hMenu,         // Menu
            App::Instance, // Instance handle
            this           // Additional application data
        );

        this->HandleInitialized(this->_hwnd);

        RECT rect;
        GetWindowRect(this->_hwnd, &rect);
        this->_rect = rect;

        SetWindowLongPtrW(this->_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
        this->UpdateFont();
    }
}

void sw::WndBase::InitControl(LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle)
{
    if (_controlInitContainer == NULL) {
        _controlInitContainer = CreateWindowExW(0, WINDOW_CLASS_NAME, L"", WS_POPUP, 0, 0, 0, 0, NULL, NULL, NULL, NULL);
    }

    if (this->_hwnd == NULL) {

        this->_text = lpWindowName ? lpWindowName : L"";

        this->_hwnd = CreateWindowExW(
            0,            // Optional window styles
            lpClassName,  // Window class
            lpWindowName, // Window text
            dwStyle,      // Window style

            // Size and position
            0, 0, 0, 0,

            _controlInitContainer, // Parent window
            NULL,                  // Menu
            App::Instance,         // Instance handle
            this                   // Additional application data
        );

        this->HandleInitialized(this->_hwnd);

        this->_controlOldWndProc =
            reinterpret_cast<WNDPROC>(SetWindowLongPtrW(this->_hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndBase::_WndProc)));

        RECT rect;
        GetWindowRect(this->_hwnd, &rect);
        this->_rect = rect;

        SetWindowLongPtrW(this->_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
        this->UpdateFont();
    }
}

LONG_PTR sw::WndBase::GetStyle()
{
    return GetWindowLongPtrW(this->_hwnd, GWL_STYLE);
}

void sw::WndBase::SetStyle(LONG_PTR style)
{
    SetWindowLongPtrW(this->_hwnd, GWL_STYLE, style);
}

bool sw::WndBase::GetStyle(LONG_PTR style)
{
    return GetWindowLongPtrW(this->_hwnd, GWL_STYLE) & style;
}

void sw::WndBase::SetStyle(LONG_PTR style, bool value)
{
    if (value) {
        style = GetWindowLongPtrW(this->_hwnd, GWL_STYLE) | style;
    } else {
        style = GetWindowLongPtrW(this->_hwnd, GWL_STYLE) & ~style;
    }
    SetWindowLongPtrW(this->_hwnd, GWL_STYLE, style);
}

LONG_PTR sw::WndBase::GetExtendedStyle()
{
    return GetWindowLongPtrW(this->_hwnd, GWL_EXSTYLE);
}

void sw::WndBase::SetExtendedStyle(LONG_PTR style)
{
    SetWindowLongPtrW(this->_hwnd, GWL_EXSTYLE, style);
}

bool sw::WndBase::GetExtendedStyle(LONG_PTR style)
{
    return GetWindowLongPtrW(this->_hwnd, GWL_EXSTYLE) & style;
}

void sw::WndBase::SetExtendedStyle(LONG_PTR style, bool value)
{
    if (value) {
        style = GetWindowLongPtrW(this->_hwnd, GWL_EXSTYLE) | style;
    } else {
        style = GetWindowLongPtrW(this->_hwnd, GWL_EXSTYLE) & ~style;
    }
    SetWindowLongPtrW(this->_hwnd, GWL_EXSTYLE, style);
}

LRESULT sw::WndBase::DefaultWndProc(const ProcMsg &refMsg)
{
    WNDPROC wndproc = this->IsControl() ? this->_controlOldWndProc : DefWindowProcW;
    return wndproc(refMsg.hwnd, refMsg.uMsg, refMsg.wParam, refMsg.lParam);
}

LRESULT sw::WndBase::WndProc(const ProcMsg &refMsg)
{
    switch (refMsg.uMsg) {
        case WM_CREATE: {
            return this->OnCreate() ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_CLOSE: {
            return this->OnClose() ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_DESTROY: {
            this->_isDestroyed = true;
            return this->OnDestroy() ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_PAINT: {
            return this->OnPaint() ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_WINDOWPOSCHANGED: {
            PWINDOWPOS pWndPos = reinterpret_cast<PWINDOWPOS>(refMsg.lParam);
            double scaleX      = Dip::ScaleX;
            double scaleY      = Dip::ScaleY;
            this->_rect.left   = scaleX * pWndPos->x;
            this->_rect.top    = scaleY * pWndPos->y;
            this->_rect.width  = scaleX * pWndPos->cx;
            this->_rect.height = scaleY * pWndPos->cy;
            return this->DefaultWndProc(refMsg);
        }

        case WM_MOVE: {
            int xPos = (int)(short)LOWORD(refMsg.lParam); // horizontal position
            int yPos = (int)(short)HIWORD(refMsg.lParam); // vertical position
            return this->OnMove(Point(xPos, yPos)) ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_SIZE: {
            int width  = LOWORD(refMsg.lParam); // the new width of the client area
            int height = HIWORD(refMsg.lParam); // the new height of the client area
            return this->OnSize(Size(width, height)) ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_SETTEXT: {
            LRESULT result = this->DefaultWndProc(refMsg);
            if (result == TRUE) {
                this->_text = reinterpret_cast<PCWSTR>(refMsg.lParam);
                this->OnTextChanged(this->_text);
            }
            return result;
        }

        case WM_SETFOCUS: {
            this->_focused = true;
            return this->OnSetFocus((HWND)refMsg.wParam) ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_KILLFOCUS: {
            this->_focused = false;
            return this->OnKillFocus((HWND)refMsg.wParam) ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_MOUSEMOVE: {
            POINT mousePosition{GET_X_LPARAM(refMsg.lParam), GET_Y_LPARAM(refMsg.lParam)};
            return this->OnMouseMove(Point(mousePosition), (MouseKey)refMsg.wParam) ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_MOUSELEAVE: {
            return this->OnMouseLeave() ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_MOUSEWHEEL: {
            int fwKeys = GET_KEYSTATE_WPARAM(refMsg.wParam);
            int zDelta = GET_WHEEL_DELTA_WPARAM(refMsg.wParam);
            POINT mousePosition{GET_X_LPARAM(refMsg.lParam), GET_Y_LPARAM(refMsg.lParam)};
            return this->OnMouseWheel(zDelta, Point(mousePosition), (MouseKey)fwKeys) ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_LBUTTONDOWN: {
            POINT mousePosition{GET_X_LPARAM(refMsg.lParam), GET_Y_LPARAM(refMsg.lParam)};
            return this->OnMouseLeftButtonDown(Point(mousePosition), (MouseKey)refMsg.wParam) ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_LBUTTONUP: {
            POINT mousePosition{GET_X_LPARAM(refMsg.lParam), GET_Y_LPARAM(refMsg.lParam)};
            return this->OnMouseLeftButtonUp(Point(mousePosition), (MouseKey)refMsg.wParam) ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_LBUTTONDBLCLK: {
            POINT mousePosition{GET_X_LPARAM(refMsg.lParam), GET_Y_LPARAM(refMsg.lParam)};
            return this->OnMouseLeftButtonDoubleClick(Point(mousePosition), (MouseKey)refMsg.wParam) ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_RBUTTONDOWN: {
            POINT mousePosition{GET_X_LPARAM(refMsg.lParam), GET_Y_LPARAM(refMsg.lParam)};
            return this->OnMouseRightButtonDown(Point(mousePosition), (MouseKey)refMsg.wParam) ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_RBUTTONUP: {
            POINT mousePosition{GET_X_LPARAM(refMsg.lParam), GET_Y_LPARAM(refMsg.lParam)};
            return this->OnMouseRightButtonUp(Point(mousePosition), (MouseKey)refMsg.wParam) ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_RBUTTONDBLCLK: {
            POINT mousePosition{GET_X_LPARAM(refMsg.lParam), GET_Y_LPARAM(refMsg.lParam)};
            return this->OnMouseRightButtonDoubleClick(Point(mousePosition), (MouseKey)refMsg.wParam) ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_MBUTTONDOWN: {
            POINT mousePosition{GET_X_LPARAM(refMsg.lParam), GET_Y_LPARAM(refMsg.lParam)};
            return this->OnMouseMiddleButtonDown(Point(mousePosition), (MouseKey)refMsg.wParam) ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_MBUTTONUP: {
            POINT mousePosition{GET_X_LPARAM(refMsg.lParam), GET_Y_LPARAM(refMsg.lParam)};
            return this->OnMouseMiddleButtonUp(Point(mousePosition), (MouseKey)refMsg.wParam) ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_MBUTTONDBLCLK: {
            POINT mousePosition{GET_X_LPARAM(refMsg.lParam), GET_Y_LPARAM(refMsg.lParam)};
            return this->OnMouseMiddleButtonDoubleClick(Point(mousePosition), (MouseKey)refMsg.wParam) ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_CHAR: {
            return this->OnChar((wchar_t)refMsg.wParam, refMsg.lParam) ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_DEADCHAR: {
            return this->OnDeadChar((wchar_t)refMsg.wParam, refMsg.lParam) ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_KEYDOWN: {
            return this->OnKeyDown((VirtualKey)refMsg.wParam, refMsg.lParam) ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_KEYUP: {
            return this->OnKeyUp((VirtualKey)refMsg.wParam, refMsg.lParam) ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_SYSCHAR: {
            return this->OnSysChar((wchar_t)refMsg.wParam, refMsg.lParam) ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_SYSDEADCHAR: {
            return this->OnSysDeadChar((wchar_t)refMsg.wParam, refMsg.lParam) ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_SYSKEYDOWN: {
            return this->OnSysKeyDown((VirtualKey)refMsg.wParam, refMsg.lParam) ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_SYSKEYUP: {
            return this->OnSysKeyUp((VirtualKey)refMsg.wParam, refMsg.lParam) ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_COMMAND: {
            this->OnCommand(refMsg.wParam, refMsg.lParam);
            return 0;
        }

        case WM_ParentReceivedCommand: {
            this->ParentReceivedCommand(HIWORD(refMsg.wParam));
            return 0;
        }

        case WM_CTLCOLORBTN:
        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORDLG:
        case WM_CTLCOLORLISTBOX:
        case WM_CTLCOLORSCROLLBAR:
        case WM_CTLCOLORSTATIC: {
            HDC hdc              = (HDC)refMsg.wParam;
            HWND hwnd            = (HWND)refMsg.lParam;
            WndBase *pWnd        = WndBase::GetWndBase(hwnd);
            ICtlColor *pCtlColor = dynamic_cast<ICtlColor *>(pWnd);

            return pCtlColor == nullptr
                       ? this->DefaultWndProc(refMsg)
                       : pCtlColor->CtlColor(hdc, hwnd);
        }

        default: {
            return this->DefaultWndProc(refMsg);
        }
    }
}

bool sw::WndBase::OnCreate()
{
    return true;
}

bool sw::WndBase::OnClose()
{
    DestroyWindow(this->_hwnd);
    return true;
}

bool sw::WndBase::OnDestroy()
{
    return true;
}

bool sw::WndBase::OnPaint()
{
    return false;
}

bool sw::WndBase::OnMove(Point newClientPosition)
{
    return true;
}

bool sw::WndBase::OnSize(Size newClientSize)
{
    return true;
}

bool sw::WndBase::OnSetText(std::wstring &newText)
{
    return true;
}

void sw::WndBase::OnTextChanged(const std::wstring &newText)
{
}

bool sw::WndBase::OnSetFocus(HWND hPrevFocus)
{
    return true;
}

bool sw::WndBase::OnKillFocus(HWND hNextFocus)
{
    return true;
}

bool sw::WndBase::OnMouseMove(Point mousePos, MouseKey keyState)
{
    return false;
}

bool sw::WndBase::OnMouseLeave()
{
    return false;
}

bool sw::WndBase::OnMouseWheel(int wheelDelta, Point mousePosition, MouseKey keyState)
{
    return false;
}

bool sw::WndBase::OnMouseLeftButtonDown(Point mousePos, MouseKey keyState)
{
    return false;
}

bool sw::WndBase::OnMouseLeftButtonUp(Point mousePos, MouseKey keyState)
{
    return false;
}

bool sw::WndBase::OnMouseLeftButtonDoubleClick(Point mousePos, MouseKey keyState)
{
    return false;
}

bool sw::WndBase::OnMouseRightButtonDown(Point mousePos, MouseKey keyState)
{
    return false;
}

bool sw::WndBase::OnMouseRightButtonUp(Point mousePos, MouseKey keyState)
{
    return false;
}

bool sw::WndBase::OnMouseRightButtonDoubleClick(Point mousePos, MouseKey keyState)
{
    return false;
}

bool sw::WndBase::OnMouseMiddleButtonDown(Point mousePos, MouseKey keyState)
{
    return false;
}

bool sw::WndBase::OnMouseMiddleButtonUp(Point mousePos, MouseKey keyState)
{
    return false;
}

bool sw::WndBase::OnMouseMiddleButtonDoubleClick(Point mousePos, MouseKey keyState)
{
    return false;
}

bool sw::WndBase::OnChar(wchar_t ch, KeyFlags flags)
{
    return false;
}

bool sw::WndBase::OnDeadChar(wchar_t ch, KeyFlags flags)
{
    return false;
}

bool sw::WndBase::OnKeyDown(VirtualKey key, KeyFlags flags)
{
    return false;
}

bool sw::WndBase::OnKeyUp(VirtualKey key, KeyFlags flags)
{
    return false;
}

bool sw::WndBase::OnSysChar(wchar_t ch, KeyFlags flags)
{
    return false;
}

bool sw::WndBase::OnSysDeadChar(wchar_t ch, KeyFlags flags)
{
    return false;
}

bool sw::WndBase::OnSysKeyDown(VirtualKey key, KeyFlags flags)
{
    return false;
}

bool sw::WndBase::OnSysKeyUp(VirtualKey key, KeyFlags flags)
{
    return false;
}

void sw::WndBase::VisibleChanged(bool newVisible)
{
}

bool sw::WndBase::SetParent(WndBase *parent)
{
    bool success;
    HWND hParent;

    if (parent == nullptr) {
        hParent = this->IsControl() ? _controlInitContainer : NULL;
    } else {
        hParent = parent->Handle;
    }

    success = ::SetParent(this->_hwnd, hParent) != NULL;

    if (success) {
        this->ParentChanged(parent);
    }

    return success;
}

void sw::WndBase::ParentChanged(WndBase *newParent)
{
}

void sw::WndBase::OnCommand(WPARAM wParam, LPARAM lParam)
{
    if (lParam != NULL) {
        // 接收到控件消息
        SendMessageW((HWND)lParam, WM_ParentReceivedCommand, wParam, lParam);
    } else {
        // Menu / Accelerator
        // ...
    }
}

void sw::WndBase::ParentReceivedCommand(int code)
{
}

void sw::WndBase::HandleInitialized(HWND hwnd)
{
}

void sw::WndBase::Show(int nCmdShow)
{
    ShowWindow(this->_hwnd, nCmdShow);
}

void sw::WndBase::Close()
{
    SendMessageW(this->_hwnd, WM_CLOSE, NULL, NULL);
}

void sw::WndBase::Update()
{
    UpdateWindow(this->_hwnd);
}

void sw::WndBase::UpdateFont()
{
    if (this->_hfont != NULL) {
        DeleteObject(this->_hfont);
    }
    this->_hfont = this->_font.CreateHandle();
    SendMessageW(this->_hwnd, WM_SETFONT, (WPARAM)this->_hfont, TRUE);
}

void sw::WndBase::Redraw()
{
    InvalidateRect(this->_hwnd, NULL, TRUE);
}

bool sw::WndBase::IsControl()
{
    return this->_controlOldWndProc != nullptr;
}

void sw::WndBase::MoveToTop()
{
    SetWindowPos(this->_hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
}

void sw::WndBase::MoveToBottom()
{
    SetWindowPos(this->_hwnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
}

sw::Point sw::WndBase::PointToScreen(const Point &point)
{
    POINT p = point;
    ClientToScreen(this->_hwnd, &p);
    return p;
}

sw::Point sw::WndBase::PointFromScreen(const Point &screenPoint)
{
    POINT p = screenPoint;
    ScreenToClient(this->_hwnd, &p);
    return p;
}

sw::WndBase *sw::WndBase::GetWndBase(HWND hwnd)
{
    return reinterpret_cast<WndBase *>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
}

// WrapLayout.cpp

void sw::WrapLayout::MeasureOverride(Size &availableSize)
{
    this->orientation == Orientation::Horizontal
        ? this->WrapLayoutH::MeasureOverride(availableSize)
        : this->WrapLayoutV::MeasureOverride(availableSize);
}

void sw::WrapLayout::ArrangeOverride(Size &finalSize)
{
    this->orientation == Orientation::Horizontal
        ? this->WrapLayoutH::ArrangeOverride(finalSize)
        : this->WrapLayoutV::ArrangeOverride(finalSize);
}

// WrapLayoutH.cpp

void sw::WrapLayoutH::MeasureOverride(Size &availableSize)
{
    Size size;
    int count = this->GetChildLayoutCount();

    if (std::isinf(availableSize.width)) {
        for (int i = 0; i < count; ++i) {
            ILayout &item = this->GetChildLayoutAt(i);
            item.Measure(Size(INFINITY, INFINITY));

            Size itemDesireSize = item.GetDesireSize();
            size.width += itemDesireSize.width;
            size.height = max(size.height, itemDesireSize.height);
        }
    } else {
        double top       = 0;
        double rowWidth  = 0;
        double rowHeight = 0;

        for (int i = 0; i < count; ++i) {
            ILayout &item = this->GetChildLayoutAt(i);
            item.Measure(Size(availableSize.width - rowWidth, INFINITY));

            Size itemDesireSize = item.GetDesireSize();
            if (rowWidth + itemDesireSize.width <= availableSize.width) {
                rowWidth += itemDesireSize.width;
                rowHeight = max(rowHeight, itemDesireSize.height);
            } else {
                top += rowHeight;
                rowWidth  = itemDesireSize.width;
                rowHeight = itemDesireSize.height;
            }
            size.width = max(size.width, rowWidth);
        }
        size.height = top + rowHeight;
    }

    this->SetDesireSize(size);
}

void sw::WrapLayoutH::ArrangeOverride(Size &finalSize)
{
    double top       = 0;
    double rowWidth  = 0;
    double rowHeight = 0;

    int count = this->GetChildLayoutCount();
    for (int i = 0; i < count; ++i) {
        ILayout &item = this->GetChildLayoutAt(i);

        Size itemDesireSize = item.GetDesireSize();
        if (rowWidth + itemDesireSize.width <= finalSize.width) {
            item.Arrange(Rect(rowWidth, top, itemDesireSize.width, itemDesireSize.height));
            rowWidth += itemDesireSize.width;
            rowHeight = max(rowHeight, itemDesireSize.height);
        } else {
            top += rowHeight;
            item.Arrange(Rect(0, top, itemDesireSize.width, itemDesireSize.height));
            rowWidth  = itemDesireSize.width;
            rowHeight = itemDesireSize.height;
        }
    }
}

// WrapLayoutV.cpp

void sw::WrapLayoutV::MeasureOverride(Size &availableSize)
{
    Size size;
    int count = this->GetChildLayoutCount();

    if (std::isinf(availableSize.height)) {
        for (int i = 0; i < count; ++i) {
            ILayout &item = this->GetChildLayoutAt(i);
            item.Measure(Size(INFINITY, INFINITY));

            Size itemDesireSize = item.GetDesireSize();
            size.height += itemDesireSize.height;
            size.width = max(size.width, itemDesireSize.width);
        }
    } else {
        double left      = 0;
        double colHeight = 0;
        double colWidth  = 0;

        for (int i = 0; i < count; ++i) {
            ILayout &item = this->GetChildLayoutAt(i);
            item.Measure(Size(INFINITY, availableSize.height - colHeight));

            Size itemDesireSize = item.GetDesireSize();
            if (colHeight + itemDesireSize.height <= availableSize.height) {
                colHeight += itemDesireSize.height;
                colWidth = max(colWidth, itemDesireSize.width);
            } else {
                left += colWidth;
                colHeight = itemDesireSize.height;
                colWidth  = itemDesireSize.width;
            }
            size.height = max(size.height, colHeight);
        }
        size.width = left + colWidth;
    }

    this->SetDesireSize(size);
}

void sw::WrapLayoutV::ArrangeOverride(Size &finalSize)
{
    double left      = 0;
    double colHeight = 0;
    double colWidth  = 0;

    int count = this->GetChildLayoutCount();
    for (int i = 0; i < count; ++i) {
        ILayout &item = this->GetChildLayoutAt(i);

        Size itemDesireSize = item.GetDesireSize();
        if (colHeight + itemDesireSize.height <= finalSize.height) {
            item.Arrange(Rect(left, colHeight, itemDesireSize.width, itemDesireSize.height));
            colHeight += itemDesireSize.height;
            colWidth = max(colWidth, itemDesireSize.width);
        } else {
            left += colWidth;
            item.Arrange(Rect(left, 0, itemDesireSize.width, itemDesireSize.height));
            colHeight = itemDesireSize.height;
            colWidth  = itemDesireSize.width;
        }
    }
}

// WrapPanel.cpp

sw::WrapPanel::WrapPanel()
    : Orientation(
          // get
          [&]() -> const sw::Orientation & {
              return this->_wrapLayout.orientation;
          },
          // set
          [&](const sw::Orientation &value) {
              this->_wrapLayout.orientation = value;
              this->NotifyLayoutUpdated();
          })
{
    this->Layout = &this->_wrapLayout;
}