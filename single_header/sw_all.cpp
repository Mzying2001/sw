#include "sw_all.h"
#include <cmath>
#include <strsafe.h>
#include <deque>

// App.cpp

/**
 * @brief 储存App::QuitMode的变量，默认为Auto
 */
static sw::AppQuitMode _appQuitMode = sw::AppQuitMode::Auto;

/*================================================================================*/

const sw::ReadOnlyProperty<HINSTANCE> sw::App::Instance(
    []() -> const HINSTANCE & {
        static HINSTANCE hInstance = GetModuleHandleW(NULL);
        return hInstance;
    } //
);

const sw::ReadOnlyProperty<std::wstring> sw::App::ExePath(
    []() -> const std::wstring & {
        static std::wstring exePath = App::_GetExePath();
        return exePath;
    } //
);

const sw::ReadOnlyProperty<std::wstring> sw::App::ExeDirectory(
    []() -> const std::wstring & {
        static std::wstring exeDirectory = Path::GetDirectory(App::ExePath);
        return exeDirectory;
    } //
);

const sw::Property<std::wstring> sw::App::CurrentDirectory(
    // get
    []() -> const std::wstring & {
        static std::wstring result;
        result = App::_GetCurrentDirectory();
        return result;
    },
    // set
    [](const std::wstring &value) {
        SetCurrentDirectoryW(value.c_str());
    } //
);

const sw::Property<sw::AppQuitMode> sw::App::QuitMode(
    // get
    []() -> const sw::AppQuitMode & {
        return _appQuitMode;
    },
    // set
    [](const sw::AppQuitMode &value) {
        _appQuitMode = value;
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

void sw::App::QuitMsgLoop(int exitCode)
{
    PostQuitMessage(exitCode);
}

/*================================================================================*/

std::wstring sw::App::_GetExePath()
{
    DWORD bufferSize = MAX_PATH; // 初始缓冲区大小
    PWSTR szExePath  = nullptr;
    DWORD pathLength = 0;

    // 循环直到获取到完整路径或达到一个合理的最大缓冲区大小
    while (true) {

        // 动态分配足够大的缓冲区
        delete[] szExePath;
        szExePath = new WCHAR[bufferSize];

        // 获取当前执行的 EXE 程序的路径
        pathLength = GetModuleFileNameW(nullptr, szExePath, bufferSize);

        // 判断是否缓冲区太小
        if (pathLength == 0 || pathLength >= bufferSize) {
            // 获取失败或缓冲区太小，增加缓冲区大小
            bufferSize *= 2;
        } else {
            // 成功获取完整路径
            break;
        }
    }

    // 转换为 std::wstring
    std::wstring exePath(szExePath);

    // 释放动态分配的内存
    delete[] szExePath;

    return exePath;
}

std::wstring sw::App::_GetCurrentDirectory()
{
    // 先获取路径的长度
    DWORD pathLength = GetCurrentDirectoryW(0, nullptr);
    if (pathLength == 0) {
        // 获取路径失败，返回空字符串
        return L"";
    }

    // 动态分配足够大的缓冲区
    std::unique_ptr<wchar_t[]> szStartUpPath = std::make_unique<wchar_t[]>(pathLength);

    // 获取当前工作目录（即程序启动的路径）
    DWORD actualLength = GetCurrentDirectoryW(pathLength, szStartUpPath.get());
    if (actualLength == 0 || actualLength > pathLength) {
        // 获取路径失败，返回空字符串
        return L"";
    }

    // 转换为 std::wstring
    std::wstring startUpPath(szStartUpPath.get());

    return startUpPath;
}

// Button.cpp

static constexpr DWORD _ButtonStyle_Default = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | BS_NOTIFY | BS_PUSHBUTTON;
static constexpr DWORD _ButtonStyle_Focused = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | BS_NOTIFY | BS_DEFPUSHBUTTON;

sw::Button::Button()
{
    this->InitButtonBase(L"Button", _ButtonStyle_Default, 0);
    this->Rect = sw::Rect(0, 0, 70, 30);
}

void sw::Button::OnDrawFocusRect()
{
    HWND hwnd = this->Handle;
    HDC hdc   = GetDC(hwnd);

    RECT rect;
    GetClientRect(hwnd, &rect);

    rect.left += 3;
    rect.top += 3;
    rect.right -= 3;
    rect.bottom -= 3;

    DrawFocusRect(hdc, &rect);
    ReleaseDC(hwnd, hdc);
}

bool sw::Button::OnSetFocus(HWND hPreFocus)
{
    this->SetStyle(_ButtonStyle_Focused);
    this->Redraw();
    return this->ButtonBase::OnSetFocus(hPreFocus);
}

bool sw::Button::OnKillFocus(HWND hNextFocus)
{
    this->SetStyle(_ButtonStyle_Default);
    this->Redraw();
    return this->ButtonBase::OnKillFocus(hNextFocus);
}

bool sw::Button::OnKeyDown(VirtualKey key, KeyFlags flags)
{
    bool result = this->UIElement::OnKeyDown(key, flags);

    if (key == VirtualKey::Enter) {
        this->OnClicked();
    }

    return result;
}

// ButtonBase.cpp

sw::ButtonBase::ButtonBase()
{
    this->TabStop = true;
}

void sw::ButtonBase::InitButtonBase(LPCWSTR lpWindowName, DWORD dwStyle, DWORD dwExStyle)
{
    this->InitControl(L"BUTTON", lpWindowName, dwStyle, dwExStyle);
    this->Transparent = true;
}

void sw::ButtonBase::OnCommand(int code)
{
    switch (code) {
        case BN_CLICKED:
            this->OnClicked();
            break;

        case BN_DOUBLECLICKED:
            this->OnDoubleClicked();
            break;

        default:
            break;
    }
}

void sw::ButtonBase::OnClicked()
{
    this->RaiseRoutedEvent(ButtonBase_Clicked);
}

void sw::ButtonBase::OnDoubleClicked()
{
    this->RaiseRoutedEvent(ButtonBase_DoubleClicked);
}

// CheckableButton.cpp

sw::CheckableButton::CheckableButton()
    : CheckState(
          // get
          [&]() -> const sw::CheckState & {
              static sw::CheckState result;
              result = (sw::CheckState)this->SendMessageW(BM_GETCHECK, 0, 0);
              return result;
          },
          // set
          [&](const sw::CheckState &value) {
              this->SendMessageW(BM_SETCHECK, (WPARAM)value, 0);
          }),

      IsChecked(
          // get
          [&]() -> const bool & {
              static bool result;
              result = this->CheckState.Get() == sw::CheckState::Checked;
              return result;
          },
          // set
          [&](const bool &value) {
              this->CheckState = value ? sw::CheckState::Checked : sw::CheckState::Unchecked;
          })
{
}

// CheckBox.cpp

static constexpr DWORD _CheckBoxStyle_Normal     = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | BS_NOTIFY | BS_AUTOCHECKBOX;
static constexpr DWORD _CheckBoxStyle_ThreeState = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | BS_NOTIFY | BS_AUTO3STATE;

sw::CheckBox::CheckBox()
    : ThreeState(
          // get
          [&]() -> const bool & {
              static bool result;
              result = this->GetStyle() == _CheckBoxStyle_ThreeState;
              return result;
          },
          // set
          [&](const bool &value) {
              this->SetStyle(value ? _CheckBoxStyle_ThreeState : _CheckBoxStyle_Normal);
          })
{
    this->InitButtonBase(L"CheckBox", _CheckBoxStyle_Normal, 0);
    this->Rect = sw::Rect(0, 0, 100, 20);
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

// ComboBox.cpp

static constexpr DWORD _ComboBoxStyle_Default  = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | CBS_AUTOHSCROLL | CBS_HASSTRINGS | CBS_DROPDOWNLIST;
static constexpr DWORD _ComboBoxStyle_Editable = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | CBS_AUTOHSCROLL | CBS_HASSTRINGS | CBS_DROPDOWN;

sw::ComboBox::ComboBox()
    : IsEditable(
          // get
          [&]() -> const bool & {
              static bool result;
              result = this->GetStyle() == _ComboBoxStyle_Editable;
              return result;
          },
          // set
          [&](const bool &value) {
              if (this->IsEditable != value) {
                  this->SetStyle(value ? _ComboBoxStyle_Editable : _ComboBoxStyle_Default);
                  this->ResetHandle();
                  this->SetText(this->WndBase::GetText()); // 使切换后文本框内容能够保留
              }
          })
{
    this->InitControl(L"COMBOBOX", L"", _ComboBoxStyle_Default, 0);
    this->Rect    = sw::Rect(0, 0, 100, 24);
    this->TabStop = true;
}

int sw::ComboBox::GetItemsCount()
{
    return (int)this->SendMessageW(CB_GETCOUNT, 0, 0);
}

int sw::ComboBox::GetSelectedIndex()
{
    return (int)this->SendMessageW(CB_GETCURSEL, 0, 0);
}

void sw::ComboBox::SetSelectedIndex(int index)
{
    this->SendMessageW(CB_SETCURSEL, index, 0);
    this->OnSelectionChanged();
}

std::wstring sw::ComboBox::GetSelectedItem()
{
    return this->GetItemAt(this->GetSelectedIndex());
}

std::wstring &sw::ComboBox::GetText()
{
    if (this->_isTextChanged) {
        this->UpdateText();
        this->_isTextChanged = false;
    }
    return this->WndBase::GetText();
}

void sw::ComboBox::SetText(const std::wstring &value)
{
    // 当组合框可编辑时，直接调用WndBase::SetText以更新文本框
    // 不可编辑时，直接修改_text字段（WndBase中定义，用于保存窗体文本）
    // 修改IsEditable属性后会重新创建句柄，会直接将_text字段设为新的文本
    // 这里直接修改_text以实现在IsEditable为false时修改的Text能够在IsEditable更改为true时文本框内容能正确显示

    if (this->IsEditable) {
        this->WndBase::SetText(value);
    } else {
        this->WndBase::GetText() = value;
        this->_isTextChanged     = false;
    }
}

void sw::ComboBox::OnCommand(int code)
{
    switch (code) {
        case CBN_EDITCHANGE:
            this->_isTextChanged = true;
            this->OnTextChanged();
            break;

        case CBN_SELCHANGE:
            this->OnSelectionChanged();
            this->OnTextChanged();
            break;

        default:
            break;
    }
}

void sw::ComboBox::OnSelectionChanged()
{
    this->_isTextChanged     = false;
    this->WndBase::GetText() = this->GetSelectedItem();

    this->ItemsControl::OnSelectionChanged();
}

void sw::ComboBox::Clear()
{
    this->SendMessageW(CB_RESETCONTENT, 0, 0);
}

std::wstring sw::ComboBox::GetItemAt(int index)
{
    int len = (int)this->SendMessageW(CB_GETLBTEXTLEN, index, 0);

    if (len <= 0) {
        return L"";
    }

    wchar_t *buf = new wchar_t[len + 1];
    this->SendMessageW(CB_GETLBTEXT, index, reinterpret_cast<LPARAM>(buf));

    std::wstring result = buf;

    delete[] buf;
    return result;
}

bool sw::ComboBox::AddItem(const std::wstring &item)
{
    int count = this->GetItemsCount();
    this->SendMessageW(CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(item.c_str()));
    return this->GetItemsCount() == count + 1;
}

bool sw::ComboBox::InsertItem(int index, const std::wstring &item)
{
    int count = this->GetItemsCount();
    this->SendMessageW(CB_INSERTSTRING, index, reinterpret_cast<LPARAM>(item.c_str()));
    return this->GetItemsCount() == count + 1;
}

bool sw::ComboBox::UpdateItem(int index, const std::wstring &newValue)
{
    bool selected = this->GetSelectedIndex() == index;
    bool updated  = this->RemoveItemAt(index) && this->InsertItem(index, newValue);

    if (updated && selected) {
        this->SetSelectedIndex(index);
    }

    return updated;
}

bool sw::ComboBox::RemoveItemAt(int index)
{
    int count = this->GetItemsCount();
    this->SendMessageW(CB_DELETESTRING, index, 0);
    return this->GetItemsCount() == count - 1;
}

void sw::ComboBox::ShowDropDown()
{
    this->SendMessageW(CB_SHOWDROPDOWN, TRUE, 0);
}

void sw::ComboBox::CloseDropDown()
{
    this->SendMessageW(CB_SHOWDROPDOWN, FALSE, 0);
}

// ContextMenu.cpp

/**
 * @brief 上下文菜单ID的起始位置，用于与普通菜单ID区分
 */
static constexpr int _ContextMenuIDFirst = 50000;

sw::ContextMenu::ContextMenu()
{
    this->InitMenuBase(CreatePopupMenu());
}

sw::ContextMenu::ContextMenu(std::initializer_list<MenuItem> items)
    : ContextMenu()
{
    this->SetItems(items);
}

bool sw::ContextMenu::IsContextMenuID(int id)
{
    return id >= _ContextMenuIDFirst;
}

int sw::ContextMenu::IndexToID(int index)
{
    return index + _ContextMenuIDFirst;
}

int sw::ContextMenu::IDToIndex(int id)
{
    return id - _ContextMenuIDFirst;
}

// Control.cpp

sw::Control::Control()
{
}

void sw::Control::ResetHandle()
{
    HWND &refHwnd = const_cast<HWND &>(this->Handle.Get());

    HWND oldHwnd       = refHwnd;
    HWND parent        = GetParent(oldHwnd);
    DWORD style        = (DWORD)this->GetStyle();
    DWORD exStyle      = (DWORD)this->GetExtendedStyle();
    RECT rect          = this->Rect.Get();
    std::wstring &text = this->GetText();

    wchar_t className[256];
    GetClassNameW(oldHwnd, className, 256);

    refHwnd = CreateWindowExW(
        exStyle,      // Optional window styles
        className,    // Window class
        text.c_str(), // Window text
        style,        // Window style

        // Size and position
        rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,

        parent,        // Parent window
        NULL,          // Menu
        App::Instance, // Instance handle
        this           // Additional application data
    );

    SetWindowLongPtrW(oldHwnd, GWLP_USERDATA, (LONG_PTR)NULL);
    SetWindowLongPtrW(refHwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>((WndBase *)this));

    LONG_PTR wndProc = GetWindowLongPtrW(oldHwnd, GWLP_WNDPROC);
    SetWindowLongPtrW(refHwnd, GWLP_WNDPROC, wndProc);

    this->SendMessageW(WM_SETFONT, (WPARAM)this->GetFontHandle(), TRUE);
    this->HandleChenged();
    this->UpdateSiblingsZOrder();

    DestroyWindow(oldHwnd);
}

void sw::Control::HandleChenged()
{
}

bool sw::Control::OnSetCursor(HWND hwnd, int hitTest, int message, bool &result)
{
    if (this->_useDefaultCursor) {
        return false;
    }
    ::SetCursor(this->_hCursor);
    result = true;
    return true;
}

void sw::Control::SetCursor(HCURSOR hCursor)
{
    this->_hCursor          = hCursor;
    this->_useDefaultCursor = false;
}

void sw::Control::SetCursor(StandardCursor cursor)
{
    this->SetCursor(CursorHelper::GetCursorHandle(cursor));
}

void sw::Control::ResetCursor()
{
    this->_hCursor          = NULL;
    this->_useDefaultCursor = true;
}

// Cursor.cpp

HCURSOR sw::CursorHelper::GetCursorHandle(StandardCursor cursor)
{
    return LoadCursorW(NULL, MAKEINTRESOURCEW(cursor));
}

HCURSOR sw::CursorHelper::GetCursorHandle(HINSTANCE hInstance, int resourceId)
{
    return LoadCursorW(hInstance, MAKEINTRESOURCEW(resourceId));
}

HCURSOR sw::CursorHelper::GetCursorHandle(HINSTANCE hInstance, const std::wstring &cursorName)
{
    return LoadCursorW(hInstance, cursorName.c_str());
}

HCURSOR sw::CursorHelper::GetCursorHandle(const std::wstring &fileName)
{
    return (HCURSOR)LoadImageW(NULL, fileName.c_str(), IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE);
}

// Dip.cpp

#if !defined(USER_DEFAULT_SCREEN_DPI)
#define USER_DEFAULT_SCREEN_DPI 96
#endif

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
    auto tag = item.GetLayoutTag();
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
                desireSize.height = Utils::Max(desireSize.height, itemDesireSize.height);
                break;
            }

            case DockLayoutTag::Top:
            case DockLayoutTag::Bottom: {
                Size itemDesireSize = item.GetDesireSize();
                desireSize.height += itemDesireSize.height;
                desireSize.width = Utils::Max(desireSize.width, itemDesireSize.width);
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
                double arrangeWidth = Utils::Min(itemDesireSize.width, restArea.width);

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
                double arrangeHeight = Utils::Min(itemDesireSize.height, restArea.height);

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
                double arrangeWidth = Utils::Min(itemDesireSize.width, restArea.width);

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
                double arrangeHeight = Utils::Utils::Min(itemDesireSize.height, restArea.height);

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
    this->_dockLayout.Associate(this);
    this->HorizontalAlignment = HorizontalAlignment::Stretch;
    this->VerticalAlignment   = VerticalAlignment::Stretch;
}

sw::DockLayout::DockLayoutTag sw::DockPanel::GetDock(UIElement &element)
{
    return (DockLayout::DockLayoutTag)element.LayoutTag.Get();
}

void sw::DockPanel::SetDock(UIElement &element, DockLayout::DockLayoutTag dock)
{
    element.LayoutTag = dock;
}

sw::LayoutHost *sw::DockPanel::GetDefaultLayout()
{
    return &this->_dockLayout;
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
        desireSize.width    = Utils::Max(desireSize.width, itemDesireSize.width);
        desireSize.height   = Utils::Max(desireSize.height, itemDesireSize.height);
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
    LOGFONTW logFont{};

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

// GroupBox.cpp

sw::GroupBox::GroupBox()
{
    this->InitControl(L"BUTTON", L"GroupBox", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | BS_GROUPBOX, 0);
    this->Rect        = sw::Rect(0, 0, 200, 200);
    this->Transparent = true;
}

// Icon.cpp

HICON sw::IconHelper::GetIconHandle(StandardIcon icon)
{
    return LoadIconW(NULL, MAKEINTRESOURCEW(icon));
}

HICON sw::IconHelper::GetIconHandle(HINSTANCE hInstance, int resourceId)
{
    return LoadIconW(hInstance, MAKEINTRESOURCEW(resourceId));
}

HICON sw::IconHelper::GetIconHandle(HINSTANCE hInstance, const std::wstring &iconName)
{
    return LoadIconW(hInstance, iconName.c_str());
}

HICON sw::IconHelper::GetIconHandle(const std::wstring &fileName)
{
    return (HICON)LoadImageW(NULL, fileName.c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
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
    : HorizontalContentAlignment(
          // get
          [&]() -> const sw::HorizontalAlignment & {
              static sw::HorizontalAlignment result;
              LONG_PTR style = this->GetStyle();
              if (style & SS_CENTER) {
                  result = sw::HorizontalAlignment::Center;
              } else if (style & SS_RIGHT) {
                  result = sw::HorizontalAlignment::Right;
              } else {
                  result = sw::HorizontalAlignment::Left;
              }
              return result;
          },
          // set
          [&](const sw::HorizontalAlignment &value) {
              switch (value) {
                  case sw::HorizontalAlignment::Left: {
                      this->SetStyle(SS_CENTER | SS_RIGHT, false);
                      break;
                  }
                  case sw::HorizontalAlignment::Center: {
                      LONG_PTR style = this->GetStyle();
                      style &= ~(SS_CENTER | SS_RIGHT);
                      style |= SS_CENTER;
                      this->SetStyle(style);
                      break;
                  }
                  case sw::HorizontalAlignment::Right: {
                      LONG_PTR style = this->GetStyle();
                      style &= ~(SS_CENTER | SS_RIGHT);
                      style |= SS_RIGHT;
                      this->SetStyle(style);
                      break;
                  }
                  default: {
                      break;
                  }
              }
              this->Redraw();
          }),

      VerticalContentAlignment(
          // get
          [&]() -> const sw::VerticalAlignment & {
              static sw::VerticalAlignment result;
              result = this->GetStyle(SS_CENTERIMAGE) ? sw::VerticalAlignment::Center : sw::VerticalAlignment::Top;
              return result;
          },
          // set
          [&](const sw::VerticalAlignment &value) {
              this->SetStyle(SS_CENTERIMAGE, value == sw::VerticalAlignment::Center);
          }),

      TextTrimming(
          // get
          [&]() -> const sw::TextTrimming & {
              static sw::TextTrimming result;
              LONG_PTR style = this->GetStyle();
              if ((style & SS_WORDELLIPSIS) == SS_WORDELLIPSIS) {
                  result = sw::TextTrimming::WordEllipsis;
              } else if (style & SS_ENDELLIPSIS) {
                  result = sw::TextTrimming::EndEllipsis;
              } else {
                  result = sw::TextTrimming::None;
              }
              return result;
          },
          // set
          [&](const sw::TextTrimming &value) {
              switch (value) {
                  case sw::TextTrimming::None: {
                      this->SetStyle(SS_WORDELLIPSIS, false);
                      break;
                  }
                  case sw::TextTrimming::WordEllipsis: {
                      this->SetStyle(SS_WORDELLIPSIS, true);
                      break;
                  }
                  case sw::TextTrimming::EndEllipsis: {
                      LONG_PTR style = this->GetStyle();
                      style &= ~SS_WORDELLIPSIS;
                      style |= SS_ENDELLIPSIS;
                      this->SetStyle(style);
                      break;
                  }
              }
              this->Redraw();
          }),

      AutoWrap(
          // get
          [&]() -> const bool & {
              static bool result;
              result = this->GetStyle(SS_EDITCONTROL);
              return result;
          },
          // set
          [&](const bool &value) {
              this->SetStyle(SS_EDITCONTROL, value);
          }),

      AutoSize(
          // get
          [&]() -> const bool & {
              return this->_autoSize;
          },
          // set
          [&](const bool &value) {
              this->_autoSize = value;
              if (value) {
                  this->NotifyLayoutUpdated();
              }
          })
{
    this->SetText(L"Label");
    this->_UpdateTextSize();
    this->_ResizeToTextSize();
    this->Transparent = true;
}

void sw::Label::_UpdateTextSize()
{
    HWND hwnd = this->Handle;
    HDC hdc   = GetDC(hwnd);

    SelectObject(hdc, this->GetFontHandle());

    RECT rect{};
    const std::wstring &text = this->Text;
    DrawTextW(hdc, text.c_str(), (int)text.size(), &rect, DT_CALCRECT);

    sw::Rect textRect = rect;
    this->_textSize   = Size(textRect.width, textRect.height);

    ReleaseDC(hwnd, hdc);
}

void sw::Label::_ResizeToTextSize()
{
    sw::Rect rect = this->Rect;
    rect.width    = this->_textSize.width;
    rect.height   = this->_textSize.height;
    this->Rect    = rect;
}

void sw::Label::OnTextChanged()
{
    this->UIElement::OnTextChanged();
    this->_UpdateTextSize();

    if (this->_autoSize) {
        this->NotifyLayoutUpdated();
    }
}

void sw::Label::FontChanged(HFONT hfont)
{
    this->_UpdateTextSize();
    if (this->_autoSize) {
        this->NotifyLayoutUpdated();
    }
}

void sw::Label::Measure(const Size &availableSize)
{
    if (!this->_autoSize) {
        this->UIElement::Measure(availableSize);
        return;
    }

    Thickness margin = this->Margin;

    Size desireSize(
        this->_textSize.width + margin.left + margin.right,
        this->_textSize.height + margin.top + margin.bottom);

    if (availableSize.width < desireSize.width) {

        if (this->TextTrimming.Get() != sw::TextTrimming::None) {
            desireSize.width = availableSize.width;

        } else if (this->AutoWrap) {
            HWND hwnd = this->Handle;
            HDC hdc   = GetDC(hwnd);

            SelectObject(hdc, this->GetFontHandle());

            double scaleX = Dip::ScaleX;
            double scaleY = Dip::ScaleY;
            RECT rect{0, 0, Utils::Max(0L, std::lround((availableSize.width - margin.left - margin.right) / scaleX)), 0};

            const std::wstring &text = this->Text;
            DrawTextW(hdc, text.c_str(), (int)text.size(), &rect, DT_CALCRECT | DT_WORDBREAK);

            desireSize.width  = availableSize.width;
            desireSize.height = (rect.bottom - rect.top) * scaleY + margin.top + margin.bottom;

            ReleaseDC(hwnd, hdc);
        }
    }

    this->SetDesireSize(desireSize);
}

// Layer.cpp

sw::Layer::Layer()
    : Layout(
          // get
          [&]() -> LayoutHost *const & {
              return this->_customLayout;
          },
          // set
          [&](LayoutHost *const &value) {
              if (value != nullptr)
                  value->Associate(this);
              this->_customLayout = value;
          }),

      HorizontalScrollBar(
          // get
          [&]() -> const bool & {
              static bool result;
              result = this->GetStyle(WS_HSCROLL);
              return result;
          },
          // set
          [&](const bool &value) {
              if (this->HorizontalScrollBar == value) {
                  return;
              }
              if (value) {
                  ShowScrollBar(this->Handle, SB_HORZ, value);
                  this->HorizontalScrollPos = this->HorizontalScrollPos;
              } else {
                  this->HorizontalScrollPos = 0;
                  ShowScrollBar(this->Handle, SB_HORZ, value);
              }
          }),

      VerticalScrollBar(
          // get
          [&]() -> const bool & {
              static bool result;
              result = this->GetStyle(WS_VSCROLL);
              return result;
          },
          // set
          [&](const bool &value) {
              if (this->VerticalScrollBar == value) {
                  return;
              }
              if (value) {
                  ShowScrollBar(this->Handle, SB_VERT, value);
                  this->VerticalScrollPos = this->VerticalScrollPos;
              } else {
                  this->VerticalScrollPos = 0;
                  ShowScrollBar(this->Handle, SB_VERT, value);
              }
          }),

      HorizontalScrollPos(
          // get
          [&]() -> const double & {
              static double result;

              SCROLLINFO info{};
              info.cbSize = sizeof(info);
              info.fMask  = SIF_POS;
              GetScrollInfo(this->Handle, SB_HORZ, &info);

              result = info.nPos * Dip::ScaleX;
              return result;
          },
          // set
          [&](const double &value) {
              SCROLLINFO info{};
              info.cbSize = sizeof(info);
              info.fMask  = SIF_POS;
              info.nPos   = std::lround(value / Dip::ScaleX);
              SetScrollInfo(this->Handle, SB_HORZ, &info, true);

              LayoutHost *layout = this->_GetLayout();

              if (layout != nullptr && !this->_horizontalScrollDisabled && this->HorizontalScrollBar) {
                  this->GetArrangeOffsetX() = -HorizontalScrollPos;
                  layout->Arrange(this->ClientRect);
              }
          }),

      VerticalScrollPos(
          // get
          [&]() -> const double & {
              static double result;

              SCROLLINFO info{};
              info.cbSize = sizeof(info);
              info.fMask  = SIF_POS;
              GetScrollInfo(this->Handle, SB_VERT, &info);

              result = info.nPos * Dip::ScaleY;
              return result;
          },
          // set
          [&](const double &value) {
              SCROLLINFO info{};
              info.cbSize = sizeof(info);
              info.fMask  = SIF_POS;
              info.nPos   = std::lround(value / Dip::ScaleY);
              SetScrollInfo(this->Handle, SB_VERT, &info, true);

              LayoutHost *layout = this->_GetLayout();

              if (layout != nullptr && !this->_verticalScrollDisabled && this->VerticalScrollBar) {
                  this->GetArrangeOffsetY() = -VerticalScrollPos;
                  layout->Arrange(this->ClientRect);
              }
          }),

      HorizontalScrollLimit(
          // get
          [&]() -> const double & {
              static double result;

              if (this->_horizontalScrollDisabled) {
                  result = 0;
                  return result;
              }

              SCROLLINFO info{};
              info.cbSize = sizeof(info);
              info.fMask  = SIF_RANGE | SIF_PAGE;
              GetScrollInfo(this->Handle, SB_HORZ, &info);

              result = (info.nMax - info.nPage + 1) * Dip::ScaleX;
              return result;
          }),

      VerticalScrollLimit(
          // get
          [&]() -> const double & {
              static double result;

              if (this->_verticalScrollDisabled) {
                  result = 0;
                  return result;
              }

              SCROLLINFO info{};
              info.cbSize = sizeof(info);
              info.fMask  = SIF_RANGE | SIF_PAGE;
              GetScrollInfo(this->Handle, SB_VERT, &info);

              result = (info.nMax - info.nPage + 1) * Dip::ScaleY;
              return result;
          })
{
}

sw::LayoutHost *sw::Layer::_GetLayout()
{
    return this->_customLayout != nullptr ? this->_customLayout : this->GetDefaultLayout();
}

void sw::Layer::_MeasureAndArrangeWithoutLayout()
{
    this->GetArrangeOffsetX() = 0;
    this->GetArrangeOffsetY() = 0;

    int childCount = this->GetChildLayoutCount();

    for (int i = 0; i < childCount; ++i) {
        // measure
        UIElement &item = static_cast<UIElement &>(this->GetChildLayoutAt(i));
        item.Measure(Size(INFINITY, INFINITY));
        // arrange
        Size desireSize      = item.GetDesireSize();
        sw::Rect itemRect    = item.Rect;
        Thickness itemMargin = item.Margin;
        item.Arrange(sw::Rect(itemRect.left - itemMargin.left, itemRect.top - itemMargin.top, desireSize.width, desireSize.height));
    }
}

void sw::Layer::UpdateLayout()
{
    if (this->_layoutDisabled) {
        return;
    }

    LayoutHost *layout = this->_GetLayout();

    if (layout == nullptr) {
        this->_MeasureAndArrangeWithoutLayout();
    } else {
        sw::Rect clientRect = this->ClientRect;
        layout->Measure(Size(clientRect.width, clientRect.height));
        layout->Arrange(clientRect);
    }

    this->UpdateScrollRange();
    // this->Redraw();
}

sw::LayoutHost *sw::Layer::GetDefaultLayout()
{
    return nullptr;
}

void sw::Layer::OnScroll(ScrollOrientation scrollbar, ScrollEvent event, double pos)
{
    ScrollingEventArgs args(scrollbar, event, pos);
    this->RaiseRoutedEvent(args);

    if (args.cancel) {
        return;
    }

    if (scrollbar == ScrollOrientation::Horizontal) {
        // 水平滚动条
        switch (event) {
            case ScrollEvent::ThubmTrack: {
                this->HorizontalScrollPos = pos;
                break;
            }
            case ScrollEvent::Left: {
                this->ScrollToLeft();
                break;
            }
            case ScrollEvent::Right: {
                this->ScrollToRight();
                break;
            }
            case ScrollEvent::PageLeft: {
                this->ScrollHorizontal(-this->GetHorizontalScrollPageSize());
                break;
            }
            case ScrollEvent::PageRight: {
                this->ScrollHorizontal(this->GetHorizontalScrollPageSize());
                break;
            }
            case ScrollEvent::LineLeft: {
                this->ScrollHorizontal(-20);
                break;
            }
            case ScrollEvent::LineRight: {
                this->ScrollHorizontal(20);
                break;
            }
            default: {
                break;
            }
        }
    } else {
        // 垂直滚动条
        switch (event) {
            case ScrollEvent::ThubmTrack: {
                this->VerticalScrollPos = pos;
                break;
            }
            case ScrollEvent::Bottom: {
                this->ScrollToBottom();
                break;
            }
            case ScrollEvent::Top: {
                this->ScrollToTop();
                break;
            }
            case ScrollEvent::PageUp: {
                this->ScrollVertical(-this->GetVerticalScrollPageSize());
                break;
            }
            case ScrollEvent::PageDown: {
                this->ScrollVertical(this->GetVerticalScrollPageSize());
                break;
            }
            case ScrollEvent::LineUp: {
                this->ScrollVertical(-20);
                break;
            }
            case ScrollEvent::LineDown: {
                this->ScrollVertical(20);
                break;
            }
            default: {
                break;
            }
        }
    }
}

bool sw::Layer::OnVerticalScroll(int event, int pos)
{
    this->OnScroll(ScrollOrientation::Vertical, (ScrollEvent)event,
                   (event == SB_THUMBTRACK || event == SB_THUMBPOSITION) ? (pos * Dip::ScaleY) : (0.0));
    return true;
}

bool sw::Layer::OnHorizontalScroll(int event, int pos)
{
    this->OnScroll(ScrollOrientation::Horizontal, (ScrollEvent)event,
                   (event == SB_THUMBTRACK || event == SB_THUMBPOSITION) ? (pos * Dip::ScaleX) : (0.0));
    return true;
}

void sw::Layer::Measure(const Size &availableSize)
{
    LayoutHost *layout = this->_GetLayout();

    if (layout == nullptr) {
        this->UIElement::Measure(availableSize);
        return;
    }

    Size measureSize    = availableSize;
    Thickness margin    = this->Margin;
    sw::Rect windowRect = this->Rect;
    sw::Rect clientRect = this->ClientRect;

    // 考虑边框
    measureSize.width -= (windowRect.width - clientRect.width) + margin.left + margin.right;
    measureSize.height -= (windowRect.height - clientRect.height) + margin.top + margin.bottom;

    layout->Measure(measureSize);
    Size desireSize = this->GetDesireSize();

    desireSize.width += (windowRect.width - clientRect.width) + margin.left + margin.right;
    desireSize.height += (windowRect.width - clientRect.width) + margin.top + margin.bottom;
    this->SetDesireSize(desireSize);
}

void sw::Layer::Arrange(const sw::Rect &finalPosition)
{
    this->UIElement::Arrange(finalPosition);

    LayoutHost *layout = this->_GetLayout();
    if (layout == nullptr) {
        this->_MeasureAndArrangeWithoutLayout();
    } else {
        layout->Arrange(this->ClientRect);
    }

    this->UpdateScrollRange();
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

void sw::Layer::GetHorizontalScrollRange(double &refMin, double &refMax)
{
    double scale = Dip::ScaleX;

    INT nMin = 0, nMax = 0;
    GetScrollRange(this->Handle, SB_HORZ, &nMin, &nMax);

    refMin = nMin * scale;
    refMax = nMax * scale;
}

void sw::Layer::GetVerticalScrollRange(double &refMin, double &refMax)
{
    double scale = Dip::ScaleY;

    INT nMin = 0, nMax = 0;
    GetScrollRange(this->Handle, SB_VERT, &nMin, &nMax);

    refMin = nMin * scale;
    refMax = nMax * scale;
}

void sw::Layer::SetHorizontalScrollRange(double min, double max)
{
    double scale = Dip::ScaleX;

    SCROLLINFO info{};
    info.cbSize = sizeof(info);
    info.fMask  = SIF_RANGE | SIF_PAGE;
    info.nMin   = std::lround(min / scale);
    info.nMax   = std::lround(max / scale);
    info.nPage  = std::lround(this->ClientWidth / scale);

    SetScrollInfo(this->Handle, SB_HORZ, &info, true);
}

void sw::Layer::SetVerticalScrollRange(double min, double max)
{
    double scale = Dip::ScaleY;

    SCROLLINFO info{};
    info.cbSize = sizeof(info);
    info.fMask  = SIF_RANGE | SIF_PAGE;
    info.nMin   = std::lround(min / scale);
    info.nMax   = std::lround(max / scale);
    info.nPage  = std::lround(this->ClientHeight / scale);

    SetScrollInfo(this->Handle, SB_VERT, &info, true);
}

double sw::Layer::GetHorizontalScrollPageSize()
{
    SCROLLINFO info{};
    info.cbSize = sizeof(info);
    info.fMask  = SIF_PAGE;
    GetScrollInfo(this->Handle, SB_HORZ, &info);
    return info.nPage * Dip::ScaleX;
}

double sw::Layer::GetVerticalScrollPageSize()
{
    SCROLLINFO info{};
    info.cbSize = sizeof(info);
    info.fMask  = SIF_PAGE;
    GetScrollInfo(this->Handle, SB_VERT, &info);
    return info.nPage * Dip::ScaleY;
}

void sw::Layer::SetHorizontalScrollPageSize(double pageSize)
{
    SCROLLINFO info{};
    info.cbSize = sizeof(info);
    info.fMask  = SIF_PAGE;
    info.nPage  = std::lround(pageSize / Dip::ScaleX);
    SetScrollInfo(this->Handle, SB_HORZ, &info, true);
}

void sw::Layer::SetVerticalScrollPageSize(double pageSize)
{
    SCROLLINFO info{};
    info.cbSize = sizeof(info);
    info.fMask  = SIF_PAGE;
    info.nPage  = std::lround(pageSize / Dip::ScaleY);
    SetScrollInfo(this->Handle, SB_VERT, &info, true);
}

void sw::Layer::UpdateScrollRange()
{
    if (this->_GetLayout() == nullptr) {
        // 当未设置布局方式时滚动条和控件位置需要手动设置
        // 将以下俩字段设为false确保xxxScrollLimit属性在未设置布局方式时仍可用
        this->_horizontalScrollDisabled = false;
        this->_verticalScrollDisabled   = false;
        return;
    }

    if (this->HorizontalScrollBar) {
        double childRightmost = this->GetChildRightmost(true);

        if (int(childRightmost - this->ClientWidth) > 0) {
            this->_horizontalScrollDisabled = false;
            EnableScrollBar(this->Handle, SB_HORZ, ESB_ENABLE_BOTH);
            this->SetHorizontalScrollRange(0, childRightmost);

            // 当尺寸改变时确保子元素位置与滚动条同步
            double pos = this->HorizontalScrollPos;
            if (-this->GetArrangeOffsetX() > pos) {
                this->HorizontalScrollPos = pos;
            }

        } else {
            this->HorizontalScrollPos = 0;
            EnableScrollBar(this->Handle, SB_HORZ, ESB_DISABLE_BOTH);
            this->_horizontalScrollDisabled = true;
        }
    }

    if (this->VerticalScrollBar) {
        double childBottommost = this->GetChildBottommost(true);

        if (int(childBottommost - this->ClientHeight) > 0) {
            this->_verticalScrollDisabled = false;
            EnableScrollBar(this->Handle, SB_VERT, ESB_ENABLE_BOTH);
            this->SetVerticalScrollRange(0, childBottommost);

            // 当尺寸改变时确保子元素位置与滚动条同步
            double pos = this->VerticalScrollPos;
            if (-this->GetArrangeOffsetY() > pos) {
                this->VerticalScrollPos = pos;
            }

        } else {
            this->VerticalScrollPos = 0;
            EnableScrollBar(this->Handle, SB_VERT, ESB_DISABLE_BOTH);
            this->_verticalScrollDisabled = true;
        }
    }
}

void sw::Layer::ScrollToTop()
{
    this->VerticalScrollPos = 0;
}

void sw::Layer::ScrollToBottom()
{
    this->VerticalScrollPos = this->VerticalScrollLimit;
}

void sw::Layer::ScrollToLeft()
{
    this->HorizontalScrollPos = 0;
}

void sw::Layer::ScrollToRight()
{
    this->HorizontalScrollPos = this->HorizontalScrollLimit;
}

void sw::Layer::ScrollHorizontal(double offset)
{
    this->HorizontalScrollPos = this->HorizontalScrollPos + offset;
}

void sw::Layer::ScrollVertical(double offset)
{
    this->VerticalScrollPos = this->VerticalScrollPos + offset;
}

// LayoutHost.cpp

void sw::LayoutHost::Associate(ILayout *obj)
{
    this->_associatedObj = obj;
}

uint64_t sw::LayoutHost::GetLayoutTag()
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

// ListBox.cpp

sw::ListBox::ListBox()
    : TopIndex(
          // get
          [&]() -> const int & {
              static int result;
              result = (int)this->SendMessageW(LB_GETTOPINDEX, 0, 0);
              return result;
          },
          // set
          [&](const int &value) {
              this->SendMessageW(LB_SETTOPINDEX, value, 0);
          }),

      MultiSelect(
          // get
          [&]() -> const bool & {
              static bool result;
              result = this->GetStyle(LBS_MULTIPLESEL);
              return result;
          },
          // set
          [&](const bool &value) {
              if (this->GetStyle(LBS_MULTIPLESEL) != value) {
                  this->SetStyle(LBS_MULTIPLESEL, value);
                  this->ResetHandle();
              }
          }),

      SelectedCount(
          // get
          [&]() -> const int & {
              static int result;
              result = (int)this->SendMessageW(LB_GETSELCOUNT, 0, 0);
              return result;
          })
{
    this->InitControl(L"LISTBOX", L"", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_BORDER | WS_VSCROLL | LBS_NOTIFY, 0);
    this->Rect    = sw::Rect(0, 0, 150, 200);
    this->TabStop = true;
}

int sw::ListBox::GetItemsCount()
{
    return (int)this->SendMessageW(LB_GETCOUNT, 0, 0);
}

int sw::ListBox::GetSelectedIndex()
{
    return (int)this->SendMessageW(LB_GETCURSEL, 0, 0);
}

void sw::ListBox::SetSelectedIndex(int index)
{
    this->SendMessageW(LB_SETCURSEL, index, 0);
    this->OnSelectionChanged();
}

std::wstring sw::ListBox::GetSelectedItem()
{
    return this->GetItemAt(this->GetSelectedIndex());
}

bool sw::ListBox::OnContextMenu(bool isKeyboardMsg, Point mousePosition)
{
    int index = this->GetItemIndexFromPoint(this->PointFromScreen(mousePosition));

    if (index >= 0 && index < this->GetItemsCount()) {
        this->SetSelectedIndex(index);
    }

    return this->UIElement::OnContextMenu(isKeyboardMsg, mousePosition);
}

void sw::ListBox::OnCommand(int code)
{
    if (code == LBN_SELCHANGE) {
        this->OnSelectionChanged();
    }
}

void sw::ListBox::Clear()
{
    this->SendMessageW(LB_RESETCONTENT, 0, 0);
}

std::wstring sw::ListBox::GetItemAt(int index)
{
    int len = (int)this->SendMessageW(LB_GETTEXTLEN, index, 0);

    if (len <= 0) {
        return L"";
    }

    wchar_t *buf = new wchar_t[len + 1];
    this->SendMessageW(LB_GETTEXT, index, reinterpret_cast<LPARAM>(buf));

    std::wstring result = buf;

    delete[] buf;
    return result;
}

bool sw::ListBox::AddItem(const std::wstring &item)
{
    int count = this->GetItemsCount();
    this->SendMessageW(LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(item.c_str()));
    return this->GetItemsCount() == count + 1;
}

bool sw::ListBox::InsertItem(int index, const std::wstring &item)
{
    int count = this->GetItemsCount();
    this->SendMessageW(LB_INSERTSTRING, index, reinterpret_cast<LPARAM>(item.c_str()));
    return this->GetItemsCount() == count + 1;
}

bool sw::ListBox::UpdateItem(int index, const std::wstring &newValue)
{
    bool selected = this->GetSelectedIndex() == index;
    bool updated  = this->RemoveItemAt(index) && this->InsertItem(index, newValue.c_str());

    if (updated && selected) {
        this->SetSelectedIndex(index);
    }

    return updated;
}

bool sw::ListBox::RemoveItemAt(int index)
{
    int count = this->GetItemsCount();
    this->SendMessageW(LB_DELETESTRING, index, 0);
    return this->GetItemsCount() == count - 1;
}

int sw::ListBox::GetItemIndexFromPoint(const Point &point)
{
    POINT p = point;
    return (int)this->SendMessageW(LB_ITEMFROMPOINT, 0, MAKELPARAM(p.x, p.y));
}

sw::List<int> sw::ListBox::GetSelectedIndices()
{
    List<int> result;
    int selectedCount = this->SelectedCount.Get();
    if (selectedCount > 0) {
        int *buf = new int[selectedCount];
        if (this->SendMessageW(LB_GETSELITEMS, selectedCount, reinterpret_cast<LPARAM>(buf)) != LB_ERR) {
            for (int i = 0; i < selectedCount; ++i) result.Append(buf[i]);
        }
        delete[] buf;
    }
    return result;
}

sw::StrList sw::ListBox::GetSelectedItems()
{
    StrList result;
    for (int i : this->GetSelectedIndices()) {
        result.Append(this->GetItemAt(i));
    }
    return result;
}

bool sw::ListBox::GetItemSelectionState(int index)
{
    return this->SendMessageW(LB_GETSEL, index, 0) > 0;
}

void sw::ListBox::SetItemSelectionState(int index, bool value)
{
    this->SendMessageW(LB_SETSEL, value, index);
}

// ListView.cpp

static constexpr int _InitialBufferSize = 256; // 获取文本时缓冲区的初始大小

sw::ListViewColumn::ListViewColumn(const std::wstring &header)
    : ListViewColumn(header, 100)
{
}

sw::ListViewColumn::ListViewColumn(const std::wstring &header, double width)
    : header(header), width(width)
{
}

sw::ListViewColumn::ListViewColumn(const LVCOLUMNW &lvc)
{
    double scaleX = Dip::ScaleX;

    if (lvc.mask & LVCF_TEXT) {
        this->header = lvc.pszText;
    } else {
        this->header = L"";
    }

    if (lvc.mask & LVCF_WIDTH) {
        this->width = lvc.cx * scaleX;
    } else {
        this->width = 0;
    }

    if (lvc.mask & LVCF_FMT) {
        if (lvc.fmt & LVCFMT_RIGHT) {
            this->alignment = ListViewColumnAlignment::Right;
        } else if (lvc.fmt & LVCFMT_CENTER) {
            this->alignment = ListViewColumnAlignment::Center;
        } /*else { this->alignment = ListViewColumnAlignment::Left; }*/
    }
}

sw::ListViewColumn::operator LVCOLUMNW() const
{
    double scaleX = Dip::ScaleX;

    LVCOLUMNW lvc{};
    lvc.mask    = LVCF_TEXT | LVCF_FMT;
    lvc.pszText = const_cast<LPWSTR>(this->header.c_str());
    lvc.fmt     = (int)this->alignment;

    if (this->width >= 0) {
        lvc.mask |= LVCF_WIDTH;
        lvc.cx = std::lround(this->width / scaleX);
    }

    return lvc;
}

sw::ListView::ListView()
    : ColumnsCount(
          // get
          [&]() -> const int & {
              static int result;
              result = this->_GetColCount();
              return result;
          }),

      GridLines(
          // get
          [&]() -> const bool & {
              static bool result;
              result = this->_GetExtendedListViewStyle() & LVS_EX_GRIDLINES;
              return result;
          },
          // set
          [&](const bool &value) {
              DWORD style;
              style = this->_GetExtendedListViewStyle();
              style = value ? (style | LVS_EX_GRIDLINES) : (style & (~LVS_EX_GRIDLINES));
              this->_SetExtendedListViewStyle(style);
          }),

      MultiSelect(
          // get
          [&]() -> const bool & {
              static bool result;
              result = !(this->GetStyle() & LVS_SINGLESEL);
              return result;
          },
          // set
          [&](const bool &value) {
              this->SetStyle(LVS_SINGLESEL, !value);
          }),

      SelectedCount(
          // get
          [&]() -> const int & {
              static int result;
              result = (int)this->SendMessageW(LVM_GETSELECTEDCOUNT, 0, 0);
              return result;
          }),

      CheckBoxes(
          // get
          [&]() -> const bool & {
              static bool result;
              result = this->_GetExtendedListViewStyle() & LVS_EX_CHECKBOXES;
              return result;
          },
          // set
          [&](const bool &value) {
              DWORD style;
              style = this->_GetExtendedListViewStyle();
              style = value ? (style | LVS_EX_CHECKBOXES) : (style & (~LVS_EX_CHECKBOXES));
              this->_SetExtendedListViewStyle(style);
          }),

      TopIndex(
          // get
          [&]() -> const int & {
              static int result;
              result = (int)this->SendMessageW(LVM_GETTOPINDEX, 0, 0);
              return result;
          })
{
    this->InitControl(WC_LISTVIEWW, L"", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_BORDER | LVS_REPORT, 0);
    this->_SetExtendedListViewStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);
    this->Rect    = sw::Rect(0, 0, 200, 200);
    this->TabStop = true;
}

int sw::ListView::GetItemsCount()
{
    return this->_GetRowCount();
}

int sw::ListView::GetSelectedIndex()
{
    return (int)this->SendMessageW(LVM_GETNEXTITEM, -1, LVNI_SELECTED);
}

void sw::ListView::SetSelectedIndex(int index)
{
    LVITEMW lvi;

    lvi.stateMask = LVIS_SELECTED;
    lvi.state     = 0;
    this->SendMessageW(LVM_SETITEMSTATE, -1, reinterpret_cast<LPARAM>(&lvi));

    lvi.stateMask = LVIS_SELECTED | LVIS_FOCUSED;
    lvi.state     = LVIS_SELECTED | LVIS_FOCUSED;
    this->SendMessageW(LVM_SETITEMSTATE, index, reinterpret_cast<LPARAM>(&lvi));
}

sw::StrList sw::ListView::GetSelectedItem()
{
    return this->GetItemAt(this->GetSelectedIndex());
}

void sw::ListView::SetBackColor(Color color, bool redraw)
{
    this->Control::SetBackColor(color, false);
    this->SendMessageW(LVM_SETBKCOLOR, 0, (LPARAM)(COLORREF)color);
    this->SendMessageW(LVM_SETTEXTBKCOLOR, 0, (LPARAM)(COLORREF)color);
}

void sw::ListView::SetTextColor(Color color, bool redraw)
{
    this->Control::SetTextColor(color, false);
    this->SendMessageW(LVM_SETTEXTCOLOR, 0, (LPARAM)(COLORREF)color);
}

bool sw::ListView::OnNotify(NMHDR *pNMHDR)
{
    switch (pNMHDR->code) {
        case HDN_ITEMCLICKW: {
            this->OnHeaderItemClicked(reinterpret_cast<NMHEADERW *>(pNMHDR));
            break;
        }
        case HDN_ITEMDBLCLICKW: {
            this->OnHeaderItemDoubleClicked(reinterpret_cast<NMHEADERW *>(pNMHDR));
            break;
        }
    };
    return false;
}

void sw::ListView::OnNotified(NMHDR *pNMHDR)
{
    switch (pNMHDR->code) {
        case LVN_ITEMCHANGED: {
            this->OnItemChanged(reinterpret_cast<NMLISTVIEW *>(pNMHDR));
            break;
        }
        case NM_CLICK: {
            this->OnItemClicked(reinterpret_cast<NMITEMACTIVATE *>(pNMHDR));
            break;
        }
        case NM_DBLCLK: {
            this->OnItemDoubleClicked(reinterpret_cast<NMITEMACTIVATE *>(pNMHDR));
            break;
        }
    }
}

void sw::ListView::OnItemChanged(NMLISTVIEW *pNMLV)
{
    if (pNMLV->uChanged & LVIF_STATE) {
        auto changedState = pNMLV->uOldState ^ pNMLV->uNewState;

        if (changedState & LVIS_SELECTED) {
            this->OnSelectionChanged();
        }

        if (((changedState & LVIS_STATEIMAGEMASK) >> 12) & ~1) { // checkbox state changed
            this->OnCheckStateChanged(pNMLV->iItem);
        }
    }
}

void sw::ListView::OnCheckStateChanged(int index)
{
    ListViewCheckStateChangedEventArgs args(index);
    this->RaiseRoutedEvent(args);
}

void sw::ListView::OnHeaderItemClicked(NMHEADERW *pNMH)
{
    ListViewHeaderClickedEventArgs args(ListView_HeaderClicked, pNMH->iItem);
    this->RaiseRoutedEvent(args);
}

void sw::ListView::OnHeaderItemDoubleClicked(NMHEADERW *pNMH)
{
    ListViewHeaderClickedEventArgs args(ListView_HeaderDoubleClicked, pNMH->iItem);
    this->RaiseRoutedEvent(args);
}

void sw::ListView::OnItemClicked(NMITEMACTIVATE *pNMIA)
{
    ListViewItemClickedEventArgs args(ListView_ItemClicked, pNMIA->iItem, pNMIA->iSubItem);
    this->RaiseRoutedEvent(args);
}

void sw::ListView::OnItemDoubleClicked(NMITEMACTIVATE *pNMIA)
{
    ListViewItemClickedEventArgs args(ListView_ItemDoubleClicked, pNMIA->iItem, pNMIA->iSubItem);
    this->RaiseRoutedEvent(args);
}

void sw::ListView::Clear()
{
    this->SendMessageW(LVM_DELETEALLITEMS, 0, 0);
}

sw::StrList sw::ListView::GetItemAt(int index)
{
    StrList result;
    if (index < 0) return result;

    int row = this->_GetRowCount();
    if (row <= 0 || index >= row) return result;

    int col = this->_GetColCount();
    if (col <= 0) return result;

    int bufsize = _InitialBufferSize;
    std::unique_ptr<wchar_t[]> buf(new wchar_t[bufsize]);

    LVITEMW lvi;
    lvi.mask       = LVIF_TEXT;
    lvi.iItem      = index;
    lvi.pszText    = buf.get();
    lvi.cchTextMax = bufsize;

    for (int j = 0; j < col; ++j) {
        lvi.iSubItem = j;

        int len = (int)this->SendMessageW(LVM_GETITEMTEXTW, index, reinterpret_cast<LPARAM>(&lvi));

        while (len == bufsize - 1 && bufsize * 2 > bufsize) {
            bufsize *= 2;
            buf.reset(new wchar_t[bufsize]);
            lvi.pszText    = buf.get();
            lvi.cchTextMax = bufsize;
            len            = (int)this->SendMessageW(LVM_GETITEMTEXTW, index, reinterpret_cast<LPARAM>(&lvi));
        }

        result.Append(buf.get());
    }

    return result;
}

bool sw::ListView::AddItem(const StrList &item)
{
    return this->InsertItem(this->_GetRowCount(), item);
}

bool sw::ListView::InsertItem(int index, const StrList &item)
{
    int colCount = item.Count();
    if (colCount == 0) return false;

    LVITEMW lvi;
    lvi.mask     = LVIF_TEXT;
    lvi.iItem    = index;
    lvi.iSubItem = 0;
    lvi.pszText  = const_cast<LPWSTR>(item[0].c_str());

    index = (int)this->SendMessageW(LVM_INSERTITEMW, 0, reinterpret_cast<LPARAM>(&lvi));
    if (index == -1) return false;

    lvi.iItem = index;
    for (int j = 1; j < colCount; ++j) {
        lvi.iSubItem = j;
        lvi.pszText  = const_cast<LPWSTR>(item[j].c_str());
        this->SendMessageW(LVM_SETITEMW, 0, reinterpret_cast<LPARAM>(&lvi));
    }

    return true;
}

bool sw::ListView::UpdateItem(int index, const StrList &newValue)
{
    if (index < 0 || index >= this->_GetRowCount() || newValue.IsEmpty()) {
        return false;
    }

    LVITEMW lvi;
    lvi.mask  = LVIF_TEXT;
    lvi.iItem = index;

    int colCount = newValue.Count();
    for (int j = 0; j < colCount; ++j) {
        lvi.iSubItem = j;
        lvi.pszText  = const_cast<LPWSTR>(newValue[j].c_str());
        this->SendMessageW(LVM_SETITEMW, 0, reinterpret_cast<LPARAM>(&lvi));
    }

    return true;
}

bool sw::ListView::RemoveItemAt(int index)
{
    return this->SendMessageW(LVM_DELETEITEM, index, 0);
}

std::wstring sw::ListView::GetItemAt(int row, int col)
{
    int bufsize = _InitialBufferSize;
    std::unique_ptr<wchar_t[]> buf(new wchar_t[bufsize]);

    LVITEMW lvi;
    lvi.mask       = LVIF_TEXT;
    lvi.iItem      = row;
    lvi.iSubItem   = col;
    lvi.pszText    = buf.get();
    lvi.cchTextMax = bufsize;

    int len = (int)this->SendMessageW(LVM_GETITEMTEXTW, row, reinterpret_cast<LPARAM>(&lvi));
    if (len == 0) return std::wstring();

    while (len == bufsize - 1 && bufsize * 2 > bufsize) {
        bufsize *= 2;
        buf.reset(new wchar_t[bufsize]);
        lvi.pszText    = buf.get();
        lvi.cchTextMax = bufsize;
        len            = (int)this->SendMessageW(LVM_GETITEMTEXTW, row, reinterpret_cast<LPARAM>(&lvi));
    }

    return std::wstring(buf.get());
}

bool sw::ListView::UpdateItem(int row, int col, const std::wstring &newValue)
{
    LVITEMW lvi;
    lvi.mask     = LVIF_TEXT;
    lvi.iItem    = row;
    lvi.iSubItem = col;
    lvi.pszText  = const_cast<LPWSTR>(newValue.c_str());

    return this->SendMessageW(LVM_SETITEMTEXTW, row, reinterpret_cast<LPARAM>(&lvi));
}

bool sw::ListView::AddColumn(const ListViewColumn &column)
{
    return this->InsertColumn(this->_GetColCount(), column);
}

bool sw::ListView::AddColumn(const std::wstring &header)
{
    ListViewColumn column(header);
    return this->InsertColumn(this->_GetColCount(), column);
}

bool sw::ListView::InsertColumn(int index, const ListViewColumn &column)
{
    LVCOLUMNW lvc = column;
    return this->SendMessageW(LVM_INSERTCOLUMNW, index, reinterpret_cast<LPARAM>(&lvc)) != -1;
}

bool sw::ListView::InsertColumn(int index, const std::wstring &header)
{
    ListViewColumn column(header);
    return this->InsertColumn(index, column);
}

bool sw::ListView::SetColumnHeader(int index, const std::wstring &header)
{
    LVCOLUMNW lvc;
    lvc.mask    = LVCF_TEXT;
    lvc.pszText = const_cast<LPWSTR>(header.c_str());
    return this->SendMessageW(LVM_SETCOLUMNW, index, reinterpret_cast<LPARAM>(&lvc));
}

double sw::ListView::GetColumnWidth(int index)
{
    if (index < 0 || index >= this->_GetColCount()) {
        return -1;
    } else {
        return this->SendMessageW(LVM_GETCOLUMNWIDTH, index, 0) * Dip::ScaleX;
    }
}

bool sw::ListView::SetColumnWidth(int index, double width)
{
    return this->SendMessageW(LVM_SETCOLUMNWIDTH, index, std::lround(width / Dip::ScaleX));
}

bool sw::ListView::RemoveColumnAt(int index)
{
    return this->SendMessageW(LVM_DELETECOLUMN, index, 0);
}

sw::List<int> sw::ListView::GetSelectedIndices()
{
    List<int> result;
    for (int i = -1; (i = (int)this->SendMessageW(LVM_GETNEXTITEM, i, LVNI_SELECTED)) != -1;)
        result.Append(i);
    return result;
}

sw::List<int> sw::ListView::GetCheckedIndices()
{
    List<int> result;
    HWND hwnd    = this->Handle;
    int rowCount = this->_GetRowCount();
    for (int i = 0; i < rowCount; ++i) {
        int state = (int)ListView_GetCheckState(hwnd, i);
        if (state != -1 && state) result.Append(i);
    }
    return result;
}

bool sw::ListView::GetItemCheckState(int index)
{
    int result = (int)ListView_GetCheckState(this->Handle, index);
    return result == -1 ? false : result;
}

void sw::ListView::SetItemCheckState(int index, bool value)
{
    ListView_SetCheckState(this->Handle, index, value);
}

int sw::ListView::GetItemIndexFromPoint(const Point &point)
{
    LVHITTESTINFO hitTestInfo{};
    hitTestInfo.pt = point;
    return (int)this->SendMessageW(LVM_HITTEST, 0, reinterpret_cast<LPARAM>(&hitTestInfo));
}

int sw::ListView::_GetRowCount()
{
    return (int)this->SendMessageW(LVM_GETITEMCOUNT, 0, 0);
}

int sw::ListView::_GetColCount()
{
    HWND hHeader = (HWND)this->SendMessageW(LVM_GETHEADER, 0, 0);
    return (int)::SendMessageW(hHeader, HDM_GETITEMCOUNT, 0, 0);
}

DWORD sw::ListView::_GetExtendedListViewStyle()
{
    return (DWORD)this->SendMessageW(LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0);
}

DWORD sw::ListView::_SetExtendedListViewStyle(DWORD style)
{
    return (DWORD)this->SendMessageW(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, (LPARAM)style);
}

// Menu.cpp

sw::Menu::Menu()
{
    this->InitMenuBase(CreateMenu());
}

sw::Menu::Menu(std::initializer_list<MenuItem> items)
    : Menu()
{
    this->SetItems(items);
}

int sw::Menu::IndexToID(int index)
{
    return index;
}

int sw::Menu::IDToIndex(int id)
{
    return id;
}

// MenuBase.cpp

sw::MenuBase::MenuBase()
{
    /*this->_hMenu = CreateMenu();*/
}

sw::MenuBase::MenuBase(const MenuBase &menu)
    : MenuBase()
{
    this->items = menu.items;
    this->Update();
}

sw::MenuBase::~MenuBase()
{
    this->_ClearAddedItems();

    if (this->_hMenu != NULL) {
        DestroyMenu(this->_hMenu);
    }
}

sw::MenuBase &sw::MenuBase::operator=(const MenuBase &menu)
{
    this->items = menu.items;
    this->Update();
    return *this;
}

HMENU sw::MenuBase::GetHandle()
{
    return this->_hMenu;
}

void sw::MenuBase::Update()
{
    this->_ClearAddedItems();

    int i = 0;
    for (std::shared_ptr<MenuItem> pItem : this->items) {
        this->_AppendMenuItem(this->_hMenu, pItem, i++);
    }
}

void sw::MenuBase::SetItems(std::initializer_list<MenuItem> items)
{
    this->_ClearAddedItems();

    for (const MenuItem &item : items) {
        std::shared_ptr<MenuItem> pItem = std::make_shared<MenuItem>(item);
        this->items.push_back(pItem);
    }

    this->Update();
}

bool sw::MenuBase::SetSubItems(MenuItem &item, std::initializer_list<MenuItem> subItems)
{
    auto dependencyInfo = this->_GetMenuItemDependencyInfo(item);

    if (dependencyInfo == nullptr) {
        return false;
    }

    if (dependencyInfo->hSelf == NULL) {
        item = MenuItem(item.text, subItems);
        this->Update();
        return true;
    }

    while (GetMenuItemCount(dependencyInfo->hSelf) > 0) {
        RemoveMenu(dependencyInfo->hSelf, 0, MF_BYPOSITION);
    }

    item.subItems.clear();

    int i = 0;
    for (const MenuItem &subItem : subItems) {
        std::shared_ptr<MenuItem> pSubItem(new MenuItem(subItem));
        item.subItems.push_back(pSubItem);
        this->_AppendMenuItem(dependencyInfo->hSelf, pSubItem, i++);
    }

    return true;
}

void sw::MenuBase::AddItem(const MenuItem &item)
{
    std::shared_ptr<MenuItem> pItem(new MenuItem(item));
    this->_AppendMenuItem(this->_hMenu, pItem, (int)this->items.size());
    this->items.push_back(pItem);
}

bool sw::MenuBase::AddSubItem(MenuItem &item, const MenuItem &subItem)
{
    auto dependencyInfo = this->_GetMenuItemDependencyInfo(item);

    if (dependencyInfo == nullptr) {
        return false;
    }

    if (dependencyInfo->hSelf == NULL) {
        item.subItems.emplace_back(new MenuItem(subItem));
        this->Update();
        return true;
    }

    std::shared_ptr<MenuItem> pSubItem(new MenuItem(subItem));
    this->_AppendMenuItem(dependencyInfo->hSelf, pSubItem, (int)item.subItems.size());
    item.subItems.push_back(pSubItem);
    return true;
}

bool sw::MenuBase::RemoveItem(MenuItem &item)
{
    auto dependencyInfo = this->_GetMenuItemDependencyInfo(item);

    if (dependencyInfo == nullptr) {
        return false;
    }

    if (dependencyInfo->hParent == this->_hMenu) {

        int index = dependencyInfo->index;
        RemoveMenu(dependencyInfo->hParent, index, MF_BYPOSITION);

        this->_dependencyInfoMap.erase(&item);
        this->items.erase(this->items.begin() + index);

        for (int i = index; i < (int)this->items.size(); ++i) {
            this->_dependencyInfoMap[this->items[i].get()].index -= 1;
        }

    } else {

        MenuItem *parent = this->GetParent(item);
        if (parent == nullptr) {
            return false;
        }

        int index = dependencyInfo->index;
        RemoveMenu(dependencyInfo->hParent, index, MF_BYPOSITION);

        this->_dependencyInfoMap.erase(&item);
        parent->subItems.erase(parent->subItems.begin() + index);

        for (int i = index; i < (int)parent->subItems.size(); ++i) {
            this->_dependencyInfoMap[parent->subItems[i].get()].index -= 1;
        }
    }

    return true;
}

sw::MenuItem *sw::MenuBase::GetMenuItem(int id)
{
    int index = this->IDToIndex(id);
    return index >= 0 && index < (int)this->_leaves.size() ? this->_leaves[index].get() : nullptr;
}

sw::MenuItem *sw::MenuBase::GetMenuItem(std::initializer_list<int> path)
{
    MenuItem *result = nullptr;

    std::initializer_list<int>::iterator it  = path.begin();
    std::initializer_list<int>::iterator end = path.end();

    if (it == end) {
        return nullptr;
    }

    int index = *it++;

    if (index < 0 || index >= (int)this->items.size()) {
        return nullptr;
    }

    result = this->items[index].get();

    while (it != end) {
        index = *it++;
        if (index < 0 || index >= (int)result->subItems.size()) {
            return nullptr;
        }
        result = result->subItems[index].get();
    }

    return result;
}

sw::MenuItem *sw::MenuBase::GetMenuItem(std::initializer_list<std::wstring> path)
{
    MenuItem *result = nullptr;

    std::initializer_list<std::wstring>::iterator it  = path.begin();
    std::initializer_list<std::wstring>::iterator end = path.end();

    if (it == end) {
        return nullptr;
    }

    for (std::shared_ptr<MenuItem> pItem : this->items) {
        if (pItem->text == *it) {
            result = pItem.get();
            ++it;
            break;
        }
    }

    if (result == nullptr) {
        return nullptr;
    }

    while (it != end) {
        MenuItem *p = nullptr;

        for (std::shared_ptr<MenuItem> pItem : result->subItems) {
            if (pItem->text == *it) {
                p = pItem.get();
                ++it;
                break;
            }
        }

        if (p == nullptr) {
            return nullptr;
        }

        result = p;
    }

    return result;
}

sw::MenuItem *sw::MenuBase::GetParent(MenuItem &item)
{
    auto dependencyInfo = this->_GetMenuItemDependencyInfo(item);

    if (dependencyInfo == nullptr) {
        return nullptr;
    }

    for (auto &tuple : this->_popupMenus) {
        if (std::get<1>(tuple) == dependencyInfo->hParent) {
            return std::get<0>(tuple).get();
        }
    }

    return nullptr;
}

bool sw::MenuBase::GetEnabled(MenuItem &item, bool &out)
{
    auto dependencyInfo = this->_GetMenuItemDependencyInfo(item);

    if (dependencyInfo == nullptr) {
        return false;
    }

    MENUITEMINFOW info{};
    info.cbSize = sizeof(info);
    info.fMask  = MIIM_STATE;

    if (!GetMenuItemInfoW(dependencyInfo->hParent, dependencyInfo->index, TRUE, &info)) {
        return false;
    }

    out = (info.fState & MFS_DISABLED) != MFS_DISABLED;
    return true;
}

bool sw::MenuBase::SetEnabled(MenuItem &item, bool value)
{
    auto dependencyInfo = this->_GetMenuItemDependencyInfo(item);

    if (dependencyInfo == nullptr) {
        return false;
    }

    MENUITEMINFOW info{};
    info.cbSize = sizeof(info);
    info.fMask  = MIIM_STATE;

    if (!GetMenuItemInfoW(dependencyInfo->hParent, dependencyInfo->index, TRUE, &info)) {
        return false;
    }

    if (value) {
        info.fState &= ~MFS_DISABLED;
    } else {
        info.fState |= MFS_DISABLED;
    }

    return SetMenuItemInfoW(dependencyInfo->hParent, dependencyInfo->index, TRUE, &info);
}

bool sw::MenuBase::GetChecked(MenuItem &item, bool &out)
{
    auto dependencyInfo = this->_GetMenuItemDependencyInfo(item);

    if (dependencyInfo == nullptr) {
        return false;
    }

    MENUITEMINFOW info{};
    info.cbSize = sizeof(info);
    info.fMask  = MIIM_STATE;

    if (!GetMenuItemInfoW(dependencyInfo->hParent, dependencyInfo->index, TRUE, &info)) {
        return false;
    }

    out = (info.fState & MFS_CHECKED) == MFS_CHECKED;
    return true;
}

bool sw::MenuBase::SetChecked(MenuItem &item, bool value)
{
    auto dependencyInfo = this->_GetMenuItemDependencyInfo(item);

    if (dependencyInfo == nullptr) {
        return false;
    }

    MENUITEMINFOW info{};
    info.cbSize = sizeof(info);
    info.fMask  = MIIM_STATE;

    if (!GetMenuItemInfoW(dependencyInfo->hParent, dependencyInfo->index, TRUE, &info)) {
        return false;
    }

    if (value) {
        info.fState |= MFS_CHECKED;
    } else {
        info.fState &= ~MFS_CHECKED;
    }

    return SetMenuItemInfoW(dependencyInfo->hParent, dependencyInfo->index, TRUE, &info);
}

bool sw::MenuBase::SetText(MenuItem &item, const std::wstring &value)
{
    auto dependencyInfo = this->_GetMenuItemDependencyInfo(item);

    if (dependencyInfo == nullptr) {
        return false;
    }

    MENUITEMINFOW info{};
    info.cbSize     = sizeof(info);
    info.fMask      = MIIM_STRING;
    info.dwTypeData = const_cast<LPWSTR>(value.c_str());

    bool success = SetMenuItemInfoW(dependencyInfo->hParent, dependencyInfo->index, TRUE, &info);

    if (success) {
        item.text = value;
    }

    return success;
}

void sw::MenuBase::_ClearAddedItems()
{
    while (GetMenuItemCount(this->_hMenu) > 0) {
        RemoveMenu(this->_hMenu, 0, MF_BYPOSITION);
    }

    for (auto &tuple : this->_popupMenus) {
        DestroyMenu(std::get<1>(tuple));
    }

    this->_dependencyInfoMap.clear();
    this->_popupMenus.clear();
    this->_leaves.clear();
}

void sw::MenuBase::_AppendMenuItem(HMENU hMenu, std::shared_ptr<MenuItem> pItem, int index)
{
    this->_dependencyInfoMap[pItem.get()] = {hMenu, NULL, index};

    if (pItem->IsSeparator()) {
        AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
        return;
    }

    if (pItem->subItems.size() == 0) {
        int id = this->IndexToID(int(this->_leaves.size()));
        AppendMenuW(hMenu, MF_STRING, id, pItem->text.c_str());
        this->_leaves.push_back(pItem);
        return;
    }

    HMENU hSubMenu = CreatePopupMenu();
    this->_popupMenus.push_back({pItem, hSubMenu});
    this->_dependencyInfoMap[pItem.get()].hSelf = hSubMenu;
    AppendMenuW(hMenu, MF_POPUP, reinterpret_cast<UINT_PTR>(hSubMenu), pItem->text.c_str());

    int i = 0;
    for (std::shared_ptr<MenuItem> pSubItem : pItem->subItems) {
        this->_AppendMenuItem(hSubMenu, pSubItem, i++);
    }
}

sw::MenuBase::_MenuItemDependencyInfo *sw::MenuBase::_GetMenuItemDependencyInfo(MenuItem &item)
{
    MenuItem *p = &item;
    return this->_dependencyInfoMap.count(p) ? &this->_dependencyInfoMap[p] : nullptr;
}

void sw::MenuBase::InitMenuBase(HMENU hMenu)
{
    if (this->_hMenu == NULL) {
        this->_hMenu = hMenu;
    }
}

// MenuItem.cpp

sw::MenuItem::MenuItem(const std::wstring &text)
    : text(text)
{
}

sw::MenuItem::MenuItem(const std::wstring &text, std::initializer_list<MenuItem> subItems)
    : MenuItem(text)
{
    for (const MenuItem &subItem : subItems) {
        std::shared_ptr<MenuItem> pSubItem = std::make_shared<MenuItem>(subItem);
        this->subItems.push_back(pSubItem);
    }
}

sw::MenuItem::MenuItem(const std::wstring &text, const decltype(command) &command)
    : MenuItem(text)
{
    this->command = command;
}

bool sw::MenuItem::IsSeparator() const
{
    return this->text == L"-";
}

void sw::MenuItem::CallCommand()
{
    if (this->command)
        this->command(*this);
}

uint64_t sw::MenuItem::GetTag()
{
    return this->tag;
}

void sw::MenuItem::SetTag(uint64_t tag)
{
    this->tag = tag;
}

// MsgBox.cpp

sw::MsgBox::MsgBox(MsgBoxResult result)
    : result(result)
{
}

sw::MsgBox sw::MsgBox::Show(const WndBase *owner, const std::wstring &text, const std::wstring &caption, MsgBoxButton button)
{
    HWND hwnd = owner == nullptr ? reinterpret_cast<HWND>(NULL) : owner->Handle;
    return (MsgBoxResult)MessageBoxW(hwnd, text.c_str(), caption.c_str(), (UINT)button);
}

sw::MsgBox sw::MsgBox::Show(const WndBase &owner, const std::wstring &text, const std::wstring &caption, MsgBoxButton button)
{
    return MsgBox::Show(&owner, text, caption, button);
}

sw::MsgBox sw::MsgBox::ShowInfo(const WndBase *owner, const std::wstring &text, const std::wstring &caption, MsgBoxButton button)
{
    HWND hwnd = owner == nullptr ? reinterpret_cast<HWND>(NULL) : owner->Handle;
    return (MsgBoxResult)MessageBoxW(hwnd, text.c_str(), caption.c_str(), (UINT)button | MB_ICONINFORMATION);
}

sw::MsgBox sw::MsgBox::ShowInfo(const WndBase &owner, const std::wstring &text, const std::wstring &caption, MsgBoxButton button)
{
    return MsgBox::ShowInfo(&owner, text, caption, button);
}

sw::MsgBox sw::MsgBox::ShowError(const WndBase *owner, const std::wstring &text, const std::wstring &caption, MsgBoxButton button)
{
    HWND hwnd = owner == nullptr ? reinterpret_cast<HWND>(NULL) : owner->Handle;
    return (MsgBoxResult)MessageBoxW(hwnd, text.c_str(), caption.c_str(), (UINT)button | MB_ICONERROR);
}

sw::MsgBox sw::MsgBox::ShowError(const WndBase &owner, const std::wstring &text, const std::wstring &caption, MsgBoxButton button)
{
    return MsgBox::ShowError(&owner, text, caption, button);
}

sw::MsgBox sw::MsgBox::ShowWarning(const WndBase *owner, const std::wstring &text, const std::wstring &caption, MsgBoxButton button)
{
    HWND hwnd = owner == nullptr ? reinterpret_cast<HWND>(NULL) : owner->Handle;
    return (MsgBoxResult)MessageBoxW(hwnd, text.c_str(), caption.c_str(), (UINT)button | MB_ICONWARNING);
}

sw::MsgBox sw::MsgBox::ShowWarning(const WndBase &owner, const std::wstring &text, const std::wstring &caption, MsgBoxButton button)
{
    return MsgBox::ShowWarning(&owner, text, caption, button);
}

sw::MsgBox sw::MsgBox::ShowQuestion(const WndBase *owner, const std::wstring &text, const std::wstring &caption, MsgBoxButton button)
{
    HWND hwnd = owner == nullptr ? reinterpret_cast<HWND>(NULL) : owner->Handle;
    return (MsgBoxResult)MessageBoxW(hwnd, text.c_str(), caption.c_str(), (UINT)button | MB_ICONQUESTION);
}

sw::MsgBox sw::MsgBox::ShowQuestion(const WndBase &owner, const std::wstring &text, const std::wstring &caption, MsgBoxButton button)
{
    return MsgBox::ShowQuestion(&owner, text, caption, button);
}

const sw::MsgBox &sw::MsgBox::_On(MsgBoxResult result, const MsgBoxCallback &callback) const
{
    if (this->result == result && callback)
        callback();
    return *this;
}

const sw::MsgBox &sw::MsgBox::OnOk(const MsgBoxCallback &callback) const
{
    return this->_On(MsgBoxResult::Ok, callback);
}

const sw::MsgBox &sw::MsgBox::OnYes(const MsgBoxCallback &callback) const
{
    return this->_On(MsgBoxResult::Yes, callback);
}

const sw::MsgBox &sw::MsgBox::OnNo(const MsgBoxCallback &callback) const
{
    return this->_On(MsgBoxResult::No, callback);
}

const sw::MsgBox &sw::MsgBox::OnCancel(const MsgBoxCallback &callback) const
{
    return this->_On(MsgBoxResult::Cancel, callback);
}

// Panel.cpp

/**
 * @brief 面板的窗口类名
 */
static constexpr wchar_t _PanelClassName[] = L"sw::Panel";

sw::Panel::Panel()
    : BorderStyle(
          // get
          [&]() -> const sw::BorderStyle & {
              return this->_borderStyle;
          },
          // set
          [&](const sw::BorderStyle &value) {
              if (this->_borderStyle != value) {
                  this->_borderStyle = value;
                  this->Redraw();
              }
          })
{
    static WNDCLASSEXW wc = {0};

    if (wc.cbSize == 0) {
        wc.cbSize        = sizeof(wc);
        wc.hInstance     = App::Instance;
        wc.lpfnWndProc   = DefWindowProcW;
        wc.lpszClassName = _PanelClassName;
        RegisterClassExW(&wc);
    }

    this->InitControl(_PanelClassName, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, WS_EX_NOACTIVATE);
    this->Rect        = sw::Rect(0, 0, 200, 200);
    this->Transparent = true;
}

bool sw::Panel::OnPaint()
{
    PAINTSTRUCT ps;
    HWND hwnd = this->Handle;
    HDC hdc   = BeginPaint(hwnd, &ps);

    RECT clientRect;
    GetClientRect(hwnd, &clientRect);

    HBRUSH hBrush = CreateSolidBrush(this->GetRealBackColor());
    FillRect(hdc, &clientRect, hBrush);

    if (this->_borderStyle != sw::BorderStyle::None)
        DrawEdge(hdc, &clientRect, (UINT)this->_borderStyle, BF_RECT);

    DeleteObject(hBrush);
    EndPaint(hwnd, &ps);
    return true;
}

bool sw::Panel::OnSize(Size newClientSize)
{
    InvalidateRect(this->Handle, NULL, TRUE);
    return UIElement::OnSize(newClientSize);
}

bool sw::Panel::OnMouseMove(Point mousePosition, MouseKey keyState)
{
    HWND hwnd = this->Handle;
    this->SendMessageW(WM_SETCURSOR, (WPARAM)hwnd, MAKELONG(HTCLIENT, WM_MOUSEMOVE));
    return UIElement::OnMouseMove(mousePosition, keyState);
}

// PanelBase.cpp

sw::PanelBase::PanelBase()
{
}

bool sw::PanelBase::OnSetCursor(HWND hwnd, int hitTest, int message, bool &result)
{
    return this->Control::OnSetCursor(hwnd, hitTest, message, result);
}

bool sw::PanelBase::OnVerticalScroll(int event, int pos)
{
    return this->Layer::OnVerticalScroll(event, pos);
}

bool sw::PanelBase::OnHorizontalScroll(int event, int pos)
{
    return this->Layer::OnHorizontalScroll(event, pos);
}

void sw::PanelBase::Measure(const Size &availableSize)
{
    this->Layer::Measure(availableSize);
}

void sw::PanelBase::Arrange(const sw::Rect &finalPosition)
{
    this->Layer::Arrange(finalPosition);
}

// PasswordBox.cpp

sw::PasswordBox::PasswordBox()
    : PasswordChar(
          // get
          [&]() -> const wchar_t & {
              static wchar_t result;
              result = (wchar_t)this->SendMessageW(EM_GETPASSWORDCHAR, 0, 0);
              return result;
          },
          // set
          [&](const wchar_t &value) {
              this->SendMessageW(EM_SETPASSWORDCHAR, value, 0);
          })
{
    this->InitTextBoxBase(WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | ES_PASSWORD | ES_LEFT | ES_AUTOHSCROLL, WS_EX_CLIENTEDGE);
    this->Rect = sw::Rect(0, 0, 100, 24);
}

// Path.cpp

std::wstring sw::Path::GetFileName(const std::wstring &path)
{
    // Find the last occurrence of either '/' or '\'
    size_t lastSlashPos = path.find_last_of(L"/\\");

    // If no slash found or the last character is a slash (folder path)
    if (lastSlashPos == std::wstring::npos || lastSlashPos == path.length() - 1) {
        return L"";
    }

    // Extract the file name from the path and return it
    return path.substr(lastSlashPos + 1);
}

std::wstring sw::Path::GetFileNameWithoutExt(const std::wstring &path)
{
    // Find the last occurrence of either '/' or '\'
    size_t lastSlashPos = path.find_last_of(L"/\\");

    // If no slash found or the last character is a slash (folder path)
    if (lastSlashPos == std::wstring::npos || lastSlashPos == path.length() - 1) {
        return L"";
    }

    // Find the last occurrence of the dot (.) after the last slash
    size_t lastDotPos = path.find_last_of(L'.', lastSlashPos);

    // If no dot found or the dot is at the end of the string (file has no extension)
    if (lastDotPos == std::wstring::npos || lastDotPos == path.length() - 1) {
        // Extract the file name from the path and return it
        return path.substr(lastSlashPos + 1);
    }

    // Extract the file name without extension from the path and return it
    return path.substr(lastSlashPos + 1, lastDotPos - lastSlashPos - 1);
}

std::wstring sw::Path::GetExtension(const std::wstring &path)
{
    // Find the last occurrence of either '/' or '\'
    size_t lastSlashPos = path.find_last_of(L"/\\");

    // Find the last occurrence of the dot (.) after the last slash
    size_t lastDotPos = path.find_last_of(L'.');

    // If no dot found or the dot is at the end of the string (file has no extension)
    if (lastDotPos == std::wstring::npos || lastDotPos == path.length() - 1) {
        return L"";
    }

    // If no slash found or the last dot is before the last slash (file name has a dot)
    if (lastSlashPos == std::wstring::npos || lastDotPos < lastSlashPos) {
        return L"";
    }

    // Extract the extension from the path and return it
    return path.substr(lastDotPos + 1);
}

std::wstring sw::Path::GetDirectory(const std::wstring &path)
{
    // Find the last occurrence of either '/' or '\'
    size_t lastSlashPos = path.find_last_of(L"/\\");

    // If no slash found or the last character is a slash (folder path)
    if (lastSlashPos == std::wstring::npos || lastSlashPos == path.length() - 1) {
        return path;
    }

    // Return the directory part of the path along with the last slash
    return path.substr(0, lastSlashPos + 1);
}

std::wstring sw::Path::Combine(std::initializer_list<std::wstring> paths)
{
    std::wstring combinedPath;

    for (const auto &path : paths) {
        if (path.empty()) {
            continue;
        }

        if (!combinedPath.empty() && combinedPath.back() != L'/' && combinedPath.back() != L'\\') {
            combinedPath.append(L"\\");
        }

        if (path.front() == L'/' || path.front() == L'\\') {
            combinedPath.append(path.substr(1)); // Skip the first separator
        } else {
            combinedPath.append(path);
        }
    }

    return combinedPath;
}

std::wstring sw::Path::GetAbsolutePath(const std::wstring &path)
{
    // 获取文件路径的最大长度
    DWORD bufferSize = GetFullPathNameW(path.c_str(), 0, nullptr, nullptr);

    if (bufferSize == 0) {
        // GetFullPathNameW 返回0表示失败
        return L"";
    }

    // 为获取绝对路径分配内存
    std::wstring absolutePath;
    absolutePath.resize(bufferSize);

    // 获取绝对路径
    DWORD result = GetFullPathNameW(path.c_str(), bufferSize, &absolutePath[0], nullptr);
    if (result == 0 || result >= bufferSize) {
        // 获取绝对路径失败
        return L"";
    }

    // 移除不必要的空白字符
    absolutePath.resize(result);

    return absolutePath;
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

// ProcMsg.cpp

sw::ProcMsg::ProcMsg()
    : ProcMsg(NULL, 0, 0, 0)
{
}

sw::ProcMsg::ProcMsg(const HWND &hwnd, const UINT &uMsg, const WPARAM &wParam, const LPARAM &lParam)
    : hwnd(hwnd), uMsg(uMsg), wParam(wParam), lParam(lParam)
{
}

// ProgressBar.cpp

#if !defined(PBM_SETSTATE) // g++
#define PBM_SETSTATE (WM_USER + 16)
#endif

#if !defined(PBM_GETSTATE) // g++
#define PBM_GETSTATE (WM_USER + 17)
#endif

#if !defined(PBS_SMOOTHREVERSE) // g++
#define PBS_SMOOTHREVERSE 0x10
#endif

sw::ProgressBar::ProgressBar()
    : Minimum(
          // get
          [&]() -> const uint16_t & {
              static uint16_t result;
              result = (uint16_t)this->SendMessageW(PBM_GETRANGE, TRUE, 0);
              return result;
          },
          // set
          [&](const uint16_t &value) {
              uint16_t maximum = this->Maximum;
              this->SendMessageW(PBM_SETRANGE, 0, MAKELPARAM(value, maximum));
          }),

      Maximum(
          // get
          [&]() -> const uint16_t & {
              static uint16_t result;
              result = (uint16_t)this->SendMessageW(PBM_GETRANGE, FALSE, 0);
              return result;
          },
          // set
          [&](const uint16_t &value) {
              uint16_t minimum = this->Minimum;
              this->SendMessageW(PBM_SETRANGE, 0, MAKELPARAM(minimum, value));
          }),

      Value(
          // get
          [&]() -> const uint16_t & {
              static uint16_t result;
              result = (uint16_t)this->SendMessageW(PBM_GETPOS, 0, 0);
              return result;
          },
          // set
          [&](const uint16_t &value) {
              this->SendMessageW(PBM_SETPOS, value, 0);
          }),

      State(
          // get
          [&]() -> const ProgressBarState & {
              static ProgressBarState result;
              result = (ProgressBarState)this->SendMessageW(PBM_GETSTATE, 0, 0);
              return result;
          },
          // set
          [&](const ProgressBarState &value) {
              this->SendMessageW(PBM_SETSTATE, (WPARAM)value, 0);
          }),

      Vertical(
          // get
          [&]() -> const bool & {
              static bool result;
              result = this->GetStyle(PBS_VERTICAL);
              return result;
          },
          // set
          [&](const bool &value) {
              auto pos = this->Value.Get();
              this->SetStyle(PBS_VERTICAL, value);
              this->Value = pos;
          })
{
    this->InitControl(PROGRESS_CLASSW, L"", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | PBS_SMOOTH | PBS_SMOOTHREVERSE, 0);
    this->Rect = sw::Rect(0, 0, 150, 20);
}

// RadioButton.cpp

sw::RadioButton::RadioButton()
{
    this->InitButtonBase(L"RadioButton", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | BS_NOTIFY | BS_AUTORADIOBUTTON, 0);
    this->Rect = sw::Rect(0, 0, 100, 20);
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

sw::Point sw::Rect::GetPos() const
{
    return Point(this->left, this->top);
}

sw::Size sw::Rect::GetSize() const
{
    return Size(this->width, this->height);
}

// RoutedEvent.cpp

sw::RoutedEventArgs::RoutedEventArgs(RoutedEventType eventType)
    : eventType(eventType)
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

const sw::ReadOnlyProperty<sw::Point> sw::Screen::CursorPosition(
    []() -> const sw::Point & {
        static sw::Point result;
        POINT p;
        GetCursorPos(&p);
        result = p;
        return result;
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

// Slider.cpp

sw::Slider::Slider()
    : Minimum(
          // get
          [&]() -> const int & {
              static int result;
              result = (int)this->SendMessageW(TBM_GETRANGEMIN, 0, 0);
              return result;
          },
          // set
          [&](const int &value) {
              this->SendMessageW(TBM_SETRANGEMIN, TRUE, value);
          }),

      Maximum(
          // get
          [&]() -> const int & {
              static int result;
              result = (int)this->SendMessageW(TBM_GETRANGEMAX, 0, 0);
              return result;
          },
          // set
          [&](const int &value) {
              this->SendMessageW(TBM_SETRANGEMAX, TRUE, value);
          }),

      Value(
          // get
          [&]() -> const int & {
              static int result;
              result = (int)this->SendMessageW(TBM_GETPOS, 0, 0);
              return result;
          },
          // set
          [&](const int &value) {
              this->SendMessageW(TBM_SETPOS, TRUE, value);
              this->OnValueChanged();
              this->OnEndTrack();
          }),

      Vertical(
          // get
          [&]() -> const bool & {
              static bool result;
              result = this->GetStyle(TBS_VERT);
              return result;
          },
          // set
          [&](const bool &value) {
              this->SetStyle(TBS_VERT, value);
          }),

      ValueTooltips(
          // get
          [&]() -> const bool & {
              static bool result;
              result = this->GetStyle(TBS_TOOLTIPS);
              return result;
          },
          // set
          [&](const bool &value) {
              if (this->ValueTooltips != value) {
                  int maximum  = this->Maximum;
                  int minimum  = this->Minimum;
                  int position = this->Value;
                  this->SetStyle(TBS_TOOLTIPS, value);
                  this->ResetHandle();
                  this->SendMessageW(TBM_SETRANGEMIN, FALSE, minimum);
                  this->SendMessageW(TBM_SETRANGEMAX, FALSE, maximum);
                  this->SendMessageW(TBM_SETPOS, TRUE, position);
              }
          })
{
    this->InitControl(TRACKBAR_CLASSW, L"", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | TBS_NOTIFYBEFOREMOVE | TBS_DOWNISLEFT, 0);
    this->Rect    = sw::Rect(0, 0, 150, 30);
    this->TabStop = true;
}

bool sw::Slider::OnVerticalScroll(int event, int pos)
{
    switch (event) {
        case TB_BOTTOM:
        case TB_LINEDOWN:
        case TB_LINEUP:
        case TB_PAGEDOWN:
        case TB_PAGEUP:
        case TB_THUMBPOSITION:
        case TB_THUMBTRACK:
        case TB_TOP: {
            this->OnValueChanged();
            break;
        }
        case TB_ENDTRACK: {
            this->OnEndTrack();
            break;
        }
    }
    return true;
}

bool sw::Slider::OnHorizontalScroll(int event, int pos)
{
    switch (event) {
        case TB_BOTTOM:
        case TB_LINEDOWN:
        case TB_LINEUP:
        case TB_PAGEDOWN:
        case TB_PAGEUP:
        case TB_THUMBTRACK:
        case TB_TOP: {
            this->OnValueChanged();
            break;
        }
        case TB_ENDTRACK: {
            this->OnEndTrack();
            break;
        }
    }
    return true;
}

void sw::Slider::OnValueChanged()
{
    this->RaiseRoutedEvent(Slider_ValueChanged);
}

void sw::Slider::OnEndTrack()
{
    this->RaiseRoutedEvent(Slider_EndTrack);
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
        ILayout &item = this->GetChildLayoutAt(i);
        item.Measure(Size(INFINITY, std::isinf(availableSize.height) ? INFINITY : availableSize.height));

        Size itemDesireSize = item.GetDesireSize();
        desireSize.width += itemDesireSize.width;
        desireSize.height = Utils::Max(desireSize.height, itemDesireSize.height);
    }

    this->SetDesireSize(desireSize);
}

void sw::StackLayoutH::ArrangeOverride(Size &finalSize)
{
    double width   = 0;
    int childCount = this->GetChildLayoutCount();

    for (int i = 0; i < childCount; ++i) {
        ILayout &item = this->GetChildLayoutAt(i);

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
        ILayout &item = this->GetChildLayoutAt(i);
        item.Measure(Size(std::isinf(availableSize.width) ? INFINITY : availableSize.width, INFINITY));

        Size itemDesireSize = item.GetDesireSize();
        desireSize.height += itemDesireSize.height;
        desireSize.width = Utils::Max(desireSize.width, itemDesireSize.width);
    }

    this->SetDesireSize(desireSize);
}

void sw::StackLayoutV::ArrangeOverride(Size &finalSize)
{
    double top     = 0;
    int childCount = this->GetChildLayoutCount();

    for (int i = 0; i < childCount; ++i) {
        ILayout &item = this->GetChildLayoutAt(i);

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
    this->_stackLayout.Associate(this);
    this->HorizontalAlignment = HorizontalAlignment::Stretch;
    this->VerticalAlignment   = VerticalAlignment::Stretch;
}

sw::LayoutHost *sw::StackPanel::GetDefaultLayout()
{
    return &this->_stackLayout;
}

// StaticControl.cpp

sw::StaticControl::StaticControl()
{
    this->InitControl(L"STATIC", L"", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, 0);
}

// TabControl.cpp

sw::TabControl::TabControl()
    : ContentRect(
          // get
          [&]() -> const sw::Rect & {
              static sw::Rect result;

              RECT rect;
              GetClientRect(this->Handle, &rect);
              this->SendMessageW(TCM_ADJUSTRECT, FALSE, reinterpret_cast<LPARAM>(&rect));

              result = rect;
              return result;
          }),

      SelectedIndex(
          // get
          [&]() -> const int & {
              static int result;
              result = (int)this->SendMessageW(TCM_GETCURSEL, 0, 0);
              return result;
          },
          // set
          [&](const int &value) {
              this->SendMessageW(TCM_SETCURSEL, (WPARAM)value, 0);
              this->_UpdateChildVisible();
          }),

      Alignment(
          // get
          [&]() -> const TabAlignment & {
              static TabAlignment result;
              auto style = this->GetStyle();

              if (style & TCS_VERTICAL) {
                  result = (style & TCS_RIGHT) ? TabAlignment::Right : TabAlignment::Left;
                  return result;
              } else {
                  result = (style & TCS_BOTTOM) ? TabAlignment::Bottom : TabAlignment::Top;
                  return result;
              }
          },
          // set
          [&](const TabAlignment &value) {
              auto oldStyle = this->GetStyle();
              auto style    = oldStyle;

              switch (value) {
                  case TabAlignment::Top: {
                      style &= ~(TCS_VERTICAL | TCS_BOTTOM);
                      break;
                  }
                  case TabAlignment::Bottom: {
                      style &= ~TCS_VERTICAL;
                      style |= TCS_BOTTOM;
                      break;
                  }
                  case TabAlignment::Left: {
                      style |= (TCS_VERTICAL | TCS_MULTILINE);
                      style &= ~TCS_RIGHT;
                      break;
                  }
                  case TabAlignment::Right: {
                      style |= (TCS_VERTICAL | TCS_MULTILINE | TCS_RIGHT);
                      break;
                  }
              }

              if (style == oldStyle) {
                  return;
              } else {
                  this->SetStyle(style);
              }

              // 特定情况下需要重新创建控件
              if ((style & TCS_VERTICAL) ||                               // TCS_VERTICAL位为1
                  ((style & TCS_VERTICAL) ^ (oldStyle & TCS_VERTICAL))) { // TCS_VERTICAL位改变

                  int selectedIndex = this->SelectedIndex;
                  int childCount    = this->ChildCount;

                  std::vector<UIElement *> children;
                  children.reserve(childCount);
                  for (int i = childCount - 1; i >= 0; --i) {
                      children.push_back(&(*this)[i]);
                      this->RemoveChildAt(i);
                  }

                  this->ResetHandle();
                  for (int i = childCount - 1; i >= 0; --i) {
                      this->AddChild(children[i]);
                  }

                  this->SelectedIndex = selectedIndex;

              } else {
                  this->NotifyLayoutUpdated();
              }
          }),

      MultiLine(
          // get
          [&]() -> const bool & {
              static bool result;
              result = this->GetStyle(TCS_MULTILINE);
              return result;
          },
          // set
          [&](const bool &value) {
              this->SetStyle(TCS_MULTILINE, value);
              this->NotifyLayoutUpdated();
          })
{
    this->InitControl(WC_TABCONTROLW, L"", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | TCS_TABS, 0);
    this->Rect = sw::Rect(0, 0, 200, 200);
}

int sw::TabControl::GetTabCount()
{
    return (int)this->SendMessageW(TCM_GETITEMCOUNT, 0, 0);
}

void sw::TabControl::UpdateTab()
{
    TCITEMW item{};
    item.mask    = TCIF_TEXT;
    item.pszText = (LPWSTR)L"";

    int childCount = this->ChildCount;
    int tabCount   = this->GetTabCount();

    while (tabCount < childCount) {
        this->_InsertItem(tabCount, item);
        tabCount = this->GetTabCount();
    }

    while (tabCount > childCount) {
        this->_DeleteItem(tabCount - 1);
        tabCount = this->GetTabCount();
    }

    for (int i = 0; i < childCount; ++i) {
        item.pszText = (LPWSTR)(*this)[i].Text->c_str();
        this->_SetItem(i, item);
    }
}

void sw::TabControl::UpdateTabText(int index)
{
    if (index < 0) {
        return;
    }

    int childCount = this->ChildCount;
    int tabCount   = this->GetTabCount();

    if (index < childCount && index < tabCount) {
        TCITEMW item{};
        item.mask    = TCIF_TEXT;
        item.pszText = (LPWSTR)(*this)[index].Text->c_str();
        this->_SetItem(index, item);
    }
}

void sw::TabControl::Arrange(const sw::Rect &finalPosition)
{
    this->UIElement::Arrange(finalPosition);

    int selectedIndex = this->SelectedIndex;
    if (selectedIndex < 0 || selectedIndex >= this->ChildCount) return;

    UIElement &selectedItem = this->operator[](selectedIndex);
    sw::Rect contentRect    = this->ContentRect;

    selectedItem.Measure({contentRect.width, contentRect.height});
    selectedItem.Arrange(contentRect);
}

void sw::TabControl::OnAddedChild(UIElement &element)
{
    TCITEMW item{};
    item.mask    = TCIF_TEXT;
    item.pszText = (LPWSTR)element.Text->c_str();

    int index = this->IndexOf(element);

    this->_InsertItem(index, item);
    // element.Visible = index == this->SelectedIndex;
    ShowWindow(element.Handle, index == this->SelectedIndex ? SW_SHOW : SW_HIDE);
}

void sw::TabControl::OnRemovedChild(UIElement &element)
{
    this->UpdateTab();
    this->_UpdateChildVisible();
}

void sw::TabControl::OnNotified(NMHDR *pNMHDR)
{
    if (pNMHDR->code == TCN_SELCHANGE) {
        this->OnSelectedIndexChanged();
    }
}

void sw::TabControl::OnSelectedIndexChanged()
{
    this->_UpdateChildVisible();
    this->RaiseRoutedEvent(TabControl_SelectedIndexChanged);
}

void sw::TabControl::_UpdateChildVisible()
{
    int selectedIndex = this->SelectedIndex;
    int childCount    = this->ChildCount;

    for (int i = 0; i < childCount; ++i) {
        UIElement &item = (*this)[i];
        // item.Visible    = i == selectedIndex;
        ShowWindow(item.Handle, i == selectedIndex ? SW_SHOW : SW_HIDE);
        ShowWindow(item.Handle, i == selectedIndex ? SW_SHOW : SW_HIDE); // 不加这个在点击按钮后立刻换页按钮会莫名其妙固定在界面上

        sw::Rect contentRect = this->ContentRect;
        item.Measure({contentRect.width, contentRect.height});
        item.Arrange(contentRect);
    }
}

int sw::TabControl::_InsertItem(int index, TCITEMW &item)
{
    return (int)this->SendMessageW(TCM_INSERTITEMW, (WPARAM)index, reinterpret_cast<LPARAM>(&item));
}

bool sw::TabControl::_SetItem(int index, TCITEMW &item)
{
    return this->SendMessageW(TCM_SETITEMW, (WPARAM)index, reinterpret_cast<LPARAM>(&item));
}

bool sw::TabControl::_DeleteItem(int index)
{
    return this->SendMessageW(TCM_DELETEITEM, (WPARAM)index, 0);
}

bool sw::TabControl::_DeleteAllItems()
{
    return this->SendMessageW(TCM_DELETEALLITEMS, 0, 0);
}

// TextBox.cpp

sw::TextBox::TextBox()
    : AutoWrap(
          // get
          [&]() -> const bool & {
              return this->_autoWrap;
          },
          // set
          [&](const bool &value) {
              if (this->_autoWrap == value) {
                  return;
              }
              this->_autoWrap = value;
              if (this->MultiLine && this->GetStyle(ES_AUTOHSCROLL) == value) {
                  this->SetStyle(ES_AUTOHSCROLL, !value);
                  this->ResetHandle();
              }
          }),

      MultiLine(
          // get
          [&]() -> const bool & {
              static bool result;
              result = this->GetStyle(ES_MULTILINE);
              return result;
          },
          // set
          [&](const bool &value) {
              if (this->MultiLine != value) {
                  this->SetStyle(ES_MULTILINE, value);
                  this->SetStyle(ES_AUTOHSCROLL, !(value && this->_autoWrap));
                  this->ResetHandle();
              }
          }),

      HorizontalScrollBar(
          // get
          [&]() -> const bool & {
              static bool result;
              result = this->GetStyle(WS_HSCROLL);
              return result;
          },
          // set
          [&](const bool &value) {
              if (this->HorizontalScrollBar != value) {
                  this->SetStyle(WS_HSCROLL, value);
                  this->ResetHandle();
              }
          }),

      VerticalScrollBar(
          // get
          [&]() -> const bool & {
              static bool result;
              result = this->GetStyle(WS_VSCROLL);
              return result;
          },
          // set
          [&](const bool &value) {
              if (this->VerticalScrollBar != value) {
                  this->SetStyle(WS_VSCROLL, value);
                  this->ResetHandle();
              }
          })
{
    this->InitTextBoxBase(WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | ES_LEFT | ES_AUTOHSCROLL | ES_AUTOVSCROLL, WS_EX_CLIENTEDGE);
    this->Rect = sw::Rect(0, 0, 100, 24);
}

// TextBoxBase.cpp

sw::TextBoxBase::TextBoxBase()
    : ReadOnly(
          // get
          [&]() -> const bool & {
              static bool result;
              result = this->GetStyle(ES_READONLY);
              return result;
          },
          // set
          [&](const bool &value) {
              this->SendMessageW(EM_SETREADONLY, value, 0);
          }),

      HorizontalContentAlignment(
          // get
          [&]() -> const sw::HorizontalAlignment & {
              static sw::HorizontalAlignment result;
              LONG_PTR style = this->GetStyle();
              if (style & ES_CENTER) {
                  result = sw::HorizontalAlignment::Center;
              } else if (style & ES_RIGHT) {
                  result = sw::HorizontalAlignment::Right;
              } else {
                  result = sw::HorizontalAlignment::Left;
              }
              return result;
          },
          // set
          [&](const sw::HorizontalAlignment &value) {
              switch (value) {
                  case sw::HorizontalAlignment::Left: {
                      this->SetStyle(ES_CENTER | ES_RIGHT, false);
                      break;
                  }
                  case sw::HorizontalAlignment::Center: {
                      LONG_PTR style = this->GetStyle();
                      style &= ~(ES_CENTER | ES_RIGHT);
                      style |= ES_CENTER;
                      this->SetStyle(style);
                      break;
                  }
                  case sw::HorizontalAlignment::Right: {
                      LONG_PTR style = this->GetStyle();
                      style &= ~(ES_CENTER | ES_RIGHT);
                      style |= ES_RIGHT;
                      this->SetStyle(style);
                      break;
                  }
                  default: {
                      break;
                  }
              }
              this->Redraw();
          }),

      CanUndo(
          // get
          [&]() -> const bool & {
              static bool result;
              result = this->SendMessageW(EM_CANUNDO, 0, 0);
              return result;
          }),

      AcceptTab(
          // get
          [&]() -> const bool & {
              return this->_acceptTab;
          },
          // set
          [&](const bool &value) {
              this->_acceptTab = value;
          })
{
    this->TabStop = true;
}

void sw::TextBoxBase::InitTextBoxBase(DWORD dwStyle, DWORD dwExStyle)
{
    this->InitControl(L"EDIT", L"", dwStyle, dwExStyle);
}

std::wstring &sw::TextBoxBase::GetText()
{
    if (this->_isTextChanged) {
        this->UpdateText();
        this->_isTextChanged = false;
    }
    return this->WndBase::GetText();
}

void sw::TextBoxBase::OnCommand(int code)
{
    switch (code) {
        case EN_CHANGE: {
            this->_isTextChanged = true;
            this->OnTextChanged();
            break;
        }

        default:
            break;
    }
}

bool sw::TextBoxBase::OnChar(wchar_t ch, KeyFlags flags)
{
    GotCharEventArgs e(ch, flags);
    this->RaiseRoutedEvent(e);

    if (!e.handledMsg && ch == L'\t') {
        if (this->_acceptTab && !this->ReadOnly)
            this->SendMessageW(EM_REPLACESEL, TRUE, reinterpret_cast<LPARAM>(L"\t"));
        e.handledMsg = true; // 取消duang~~
    }

    return e.handledMsg;
}

bool sw::TextBoxBase::OnKeyDown(VirtualKey key, KeyFlags flags)
{
    KeyDownEventArgs e(key, flags);
    this->RaiseRoutedEvent(e);

    if (!e.handledMsg && key == VirtualKey::Tab && (!this->_acceptTab || this->ReadOnly)) {
        this->SetNextTabStopFocus();
    }

    return e.handledMsg;
}

void sw::TextBoxBase::OnDrawFocusRect()
{
    // 不绘制虚线框
}

void sw::TextBoxBase::Select(int start, int length)
{
    this->SendMessageW(EM_SETSEL, start, start + length);
}

void sw::TextBoxBase::SelectAll()
{
    this->SendMessageW(EM_SETSEL, 0, -1);
}

void sw::TextBoxBase::ScrollToCaret()
{
    this->SendMessageW(EM_SCROLLCARET, 0, 0);
}

bool sw::TextBoxBase::Undo()
{
    return this->SendMessageW(EM_UNDO, 0, 0);
}

void sw::TextBoxBase::Clear()
{
    this->Text = L"";
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

sw::Thickness::Thickness(double horizontal, double vertical)
    : Thickness(horizontal, vertical, horizontal, vertical)
{
}

sw::Thickness::Thickness(double left, double top, double right, double bottom)
    : left(left), top(top), right(right), bottom(bottom)
{
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

      Tag(
          // get
          [&]() -> const uint64_t & {
              return this->_tag;
          },
          // set
          [&](const uint64_t &value) {
              this->_tag = value;
          }),

      LayoutTag(
          // get
          [&]() -> const uint64_t & {
              return this->_layoutTag;
          },
          // set
          [&](const uint64_t &value) {
              this->_layoutTag = value;
              this->NotifyLayoutUpdated();
          }),

      ContextMenu(
          // get
          [&]() -> sw::ContextMenu *const & {
              return this->_contextMenu;
          },
          // set
          [&](sw::ContextMenu *const &value) {
              this->_contextMenu = value;
          }),

      Float(
          // get
          [&]() -> const bool & {
              return this->_float;
          },
          // set
          [&](const bool &value) {
              this->_float = value;
              this->UpdateSiblingsZOrder();
              this->NotifyLayoutUpdated();
          }),

      TabStop(
          // get
          [&]() -> const bool & {
              return this->_tabStop;
          },
          // set
          [&](const bool &value) {
              this->_tabStop = value;
          }),

      Transparent(
          // get
          [&]() -> const bool & {
              return this->_transparent;
          },
          // set
          [&](const bool &value) {
              this->_transparent = value;
              this->Redraw();
          })
{
}

sw::UIElement::~UIElement()
{
    // 将自己从父窗口的children中移除
    this->SetParent(nullptr);
}

void sw::UIElement::RegisterRoutedEvent(RoutedEventType eventType, const RoutedEvent &handler)
{
    if (handler) {
        this->_eventMap[eventType] = handler;
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

    if (element->_parent != nullptr && !element->_parent->RemoveChild(element)) {
        return false;
    }

    if (!element->WndBase::SetParent(this)) {
        return false;
    }

    this->_children.push_back(element);
    this->OnAddedChild(*element);

    // 处理z轴顺序，确保悬浮的元素在最前
    if (!element->_float) {
        for (UIElement *child : this->_children) {
            if (child->_float)
                SetWindowPos(child->Handle, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        }
    }

    this->NotifyLayoutUpdated();
    return true;
}

bool sw::UIElement::AddChild(UIElement &element)
{
    return this->AddChild(&element);
}

bool sw::UIElement::AddChild(UIElement *element, uint64_t layoutTag)
{
    if (element == nullptr) {
        return false;
    }
    element->_layoutTag = layoutTag;
    return this->AddChild(element);
}

bool sw::UIElement::AddChild(UIElement &element, uint64_t layoutTag)
{
    element._layoutTag = layoutTag;
    return this->AddChild(&element);
}

bool sw::UIElement::RemoveChildAt(int index)
{
    if (index < 0 || index >= (int)this->_children.size()) {
        return false;
    }

    std::vector<UIElement *>::iterator it =
        this->_children.begin() + index;

    if (!(*it)->WndBase::SetParent(nullptr)) {
        return false;
    }

    UIElement *element = *it;
    this->_children.erase(it);

    this->OnRemovedChild(*element);
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

    this->OnRemovedChild(*element);
    this->NotifyLayoutUpdated();
    return true;
}

bool sw::UIElement::RemoveChild(UIElement &element)
{
    return this->RemoveChild(&element);
}

void sw::UIElement::ClearChildren()
{
    while (!this->_children.empty()) {
        UIElement *item = this->_children.back();
        item->WndBase::SetParent(nullptr);
        this->_children.pop_back();
        this->OnRemovedChild(*item);
    }
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

void sw::UIElement::ShowContextMenu(const Point &point)
{
    if (this->_contextMenu != nullptr) {
        POINT p = point;
        TrackPopupMenu(this->_contextMenu->GetHandle(), TPM_LEFTALIGN | TPM_TOPALIGN, p.x, p.y, 0, this->Handle, nullptr);
    }
}

void sw::UIElement::MoveToTop()
{
    UIElement *parent = this->_parent;
    if (parent == nullptr) return;

    int index = parent->IndexOf(this);
    if (index == -1 || index == (int)parent->_children.size() - 1) return;

    parent->_children.erase(parent->_children.begin() + index);
    parent->_children.push_back(this);
    SetWindowPos(this->Handle, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    if (!this->_float) {
        for (UIElement *item : parent->_children) {
            if (item->_float)
                SetWindowPos(item->Handle, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        }
    }
}

void sw::UIElement::MoveToBottom()
{
    UIElement *parent = this->_parent;
    if (parent == nullptr) return;

    int index = parent->IndexOf(this);
    if (index == -1 || index == 0) return;

    for (int i = index; i; --i)
        parent->_children[i] = parent->_children[i - 1];
    parent->_children[0] = this;

    if (this->_float) {
        for (UIElement *item : parent->_children) {
            if (item->_float)
                SetWindowPos(item->Handle, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        }
    } else {
        SetWindowPos(this->Handle, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }
}

bool sw::UIElement::IsRootElement()
{
    return this->_parent == nullptr;
}

sw::UIElement *sw::UIElement::GetRootElement()
{
    UIElement *root;
    UIElement *element = this;
    do {
        root    = element;
        element = element->_parent;
    } while (element != nullptr);
    return root;
}

sw::UIElement *sw::UIElement::GetNextElement()
{
    return _GetNextElement(this);
}

sw::UIElement *sw::UIElement::GetNextTabStopElement()
{
    UIElement *element = this;
    do {
        element = element->GetNextElement();
    } while (element != nullptr && element != this && (!element->_tabStop || !element->Visible));
    return element;
}

sw::Color sw::UIElement::GetRealBackColor()
{
    if (!this->_transparent) {
        return this->BackColor.Get();
    }

    UIElement *p = this;
    while (p->_transparent && p->_parent != nullptr)
        p = p->_parent;

    return p->BackColor.Get();
}

uint64_t sw::UIElement::GetTag()
{
    return this->_tag;
}

void sw::UIElement::SetTag(uint64_t tag)
{
    this->_tag = tag;
}

uint64_t sw::UIElement::GetLayoutTag()
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

    sw::Rect rect;
    rect.width  = desireSize.width - margin.left - margin.right;
    rect.height = desireSize.height - margin.top - margin.bottom;

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

    if (this->_parent && !this->_float) { // 考虑偏移量
        rect.left += this->_parent->_arrangeOffsetX;
        rect.top += this->_parent->_arrangeOffsetY;
    }

    rect.width       = Utils::Max(0.0, rect.width);
    rect.height      = Utils::Max(0.0, rect.height);
    this->Rect       = rect;
    this->_arranging = false;
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

void sw::UIElement::NotifyLayoutUpdated()
{
    if (!this->_arranging) {
        UIElement *root = this->GetRootElement();
        if (root) root->SendMessageW(WM_UpdateLayout, 0, 0);
    }
}

double &sw::UIElement::GetArrangeOffsetX()
{
    return this->_arrangeOffsetX;
}

double &sw::UIElement::GetArrangeOffsetY()
{
    return this->_arrangeOffsetY;
}

double sw::UIElement::GetChildRightmost(bool update)
{
    if (update) {
        this->_childRightmost = 0;
        for (UIElement *item : this->_childrenNotCollapsed) {
            if (item->_float) continue;
            this->_childRightmost = Utils::Max(this->_childRightmost, item->Left + item->Width + item->_margin.right - this->_arrangeOffsetX);
        }
    }
    return this->_childRightmost;
}

double sw::UIElement::GetChildBottommost(bool update)
{
    if (update) {
        this->_childBottommost = 0;
        for (UIElement *item : this->_childrenNotCollapsed) {
            if (item->_float) continue;
            this->_childBottommost = Utils::Max(this->_childBottommost, item->Top + item->Height + item->_margin.bottom - this->_arrangeOffsetY);
        }
    }
    return this->_childBottommost;
}

void sw::UIElement::UpdateChildrenZOrder()
{
    int childCount = (int)this->_children.size();
    if (childCount < 2) return;

    std::deque<HWND> floatingElements;

    for (UIElement *child : this->_children) {
        HWND hwnd = child->Handle;
        if (child->_float) {
            floatingElements.push_back(hwnd);
        } else {
            SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        }
    }

    while (!floatingElements.empty()) {
        SetWindowPos(floatingElements.front(), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        floatingElements.pop_front();
    }
}

void sw::UIElement::UpdateSiblingsZOrder()
{
    if (this->_parent != nullptr) {
        this->_parent->UpdateChildrenZOrder();
    }
}

void sw::UIElement::SetNextTabStopFocus()
{
    UIElement *next = this->GetNextTabStopElement();
    if (next && next != this) next->OnTabStop();
}

void sw::UIElement::OnAddedChild(UIElement &element)
{
}

void sw::UIElement::OnRemovedChild(UIElement &element)
{
}

void sw::UIElement::OnTabStop()
{
    this->_drawFocusRect = true;
    this->Focused        = true;
}

void sw::UIElement::OnDrawFocusRect()
{
    HWND hwnd = this->Handle;
    RECT rect = this->ClientRect.Get();
    HDC hdc   = GetDC(hwnd);

    DrawFocusRect(hdc, &rect);
    ReleaseDC(hwnd, hdc);
}

bool sw::UIElement::SetParent(WndBase *parent)
{
    UIElement *oldParentElement = this->_parent;
    UIElement *newParentElement = dynamic_cast<UIElement *>(parent);

    if (newParentElement == nullptr) {
        /*
         * 要设置的父元素为nullptr
         */
        if (oldParentElement == nullptr) {
            // 在析构函数中会调用SetParent函数以保证自己从父元素的Children中移除
            // 当当前元素为顶级窗口或者不在界面中的控件（已移除/未加入）时会出现该情况
            this->_parent = nullptr;
            return true;
        } else {
            // UIElement虚构函数时调用SerParent函数保证从父元素的Children中移除
            // 当当前元素已经添加到界面中但中途对象被销毁时会出现该情况，调用父元素的RemoveChild函数移除引用
            bool temp = oldParentElement->RemoveChild(this);
            // 当程序将要退出时::SetParent函数可能会失败，此时RemoveChild函数返回false
            // 此时直接将其从父元素的Children中移除
            if (!temp) {
                auto it = std::find(oldParentElement->_children.begin(), oldParentElement->_children.end(), this);
                if (it != oldParentElement->_children.end()) oldParentElement->_children.erase(it);
            }
            this->_parent = nullptr;
            return true;
        }
    } else {
        /*
         * 添加为某个元素的子元素
         */
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

void sw::UIElement::OnEndPaint()
{
    if (this->_drawFocusRect)
        this->OnDrawFocusRect();
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
    return args.handledMsg;
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
    return args.handledMsg;
}

void sw::UIElement::OnTextChanged()
{
    this->RaiseRoutedEvent(UIElement_TextChanged);
}

void sw::UIElement::VisibleChanged(bool newVisible)
{
    if (newVisible || this->_collapseWhenHide) {
        this->NotifyLayoutUpdated();
    }
}

bool sw::UIElement::OnSetFocus(HWND hPrevFocus)
{
    RoutedEventArgsOfType<UIElement_GotFocus> args;
    this->RaiseRoutedEvent(args);
    return args.handledMsg;
}

bool sw::UIElement::OnKillFocus(HWND hNextFocus)
{
    this->_drawFocusRect = false;

    RoutedEventArgsOfType<UIElement_LostFocus> args;
    this->RaiseRoutedEvent(args);
    return args.handledMsg;
}

bool sw::UIElement::OnChar(wchar_t ch, KeyFlags flags)
{
    GotCharEventArgs args(ch, flags);
    this->RaiseRoutedEvent(args);
    return args.handledMsg;
}

bool sw::UIElement::OnKeyDown(VirtualKey key, KeyFlags flags)
{
    KeyDownEventArgs args(key, flags);
    this->RaiseRoutedEvent(args);

    // 实现按下Tab键转移焦点
    if (!args.handledMsg && key == VirtualKey::Tab) {
        this->SetNextTabStopFocus();
    }

    return args.handledMsg;
}

bool sw::UIElement::OnKeyUp(VirtualKey key, KeyFlags flags)
{
    KeyUpEventArgs args(key, flags);
    this->RaiseRoutedEvent(args);
    return args.handledMsg;
}

bool sw::UIElement::OnMouseMove(Point mousePosition, MouseKey keyState)
{
    MouseMoveEventArgs args(mousePosition, keyState);
    this->RaiseRoutedEvent(args);
    return args.handledMsg;
}

bool sw::UIElement::OnMouseLeave()
{
    RoutedEventArgsOfType<UIElement_MouseLeave> args;
    this->RaiseRoutedEvent(args);
    return args.handledMsg;
}

bool sw::UIElement::OnMouseWheel(int wheelDelta, Point mousePosition, MouseKey keyState)
{
    MouseWheelEventArgs args(wheelDelta, mousePosition, keyState);
    this->RaiseRoutedEvent(args);
    return args.handledMsg;
}

bool sw::UIElement::OnMouseLeftButtonDown(Point mousePosition, MouseKey keyState)
{
    MouseButtonDownEventArgs args(MouseKey::MouseLeft, mousePosition, keyState);
    this->RaiseRoutedEvent(args);
    return args.handledMsg;
}

bool sw::UIElement::OnMouseLeftButtonUp(Point mousePosition, MouseKey keyState)
{
    MouseButtonUpEventArgs args(MouseKey::MouseLeft, mousePosition, keyState);
    this->RaiseRoutedEvent(args);
    return args.handledMsg;
}

bool sw::UIElement::OnMouseRightButtonDown(Point mousePosition, MouseKey keyState)
{
    MouseButtonDownEventArgs args(MouseKey::MouseRight, mousePosition, keyState);
    this->RaiseRoutedEvent(args);
    return args.handledMsg;
}

bool sw::UIElement::OnMouseRightButtonUp(Point mousePosition, MouseKey keyState)
{
    MouseButtonUpEventArgs args(MouseKey::MouseRight, mousePosition, keyState);
    this->RaiseRoutedEvent(args);
    return args.handledMsg;
}

bool sw::UIElement::OnMouseMiddleButtonDown(Point mousePosition, MouseKey keyState)
{
    MouseButtonDownEventArgs args(MouseKey::MouseMiddle, mousePosition, keyState);
    this->RaiseRoutedEvent(args);
    return args.handledMsg;
}

bool sw::UIElement::OnMouseMiddleButtonUp(Point mousePosition, MouseKey keyState)
{
    MouseButtonUpEventArgs args(MouseKey::MouseMiddle, mousePosition, keyState);
    this->RaiseRoutedEvent(args);
    return args.handledMsg;
}

bool sw::UIElement::OnContextMenu(bool isKeyboardMsg, Point mousePosition)
{
    if (this->_contextMenu == nullptr) {
        return false;
    }

    ShowContextMenuEventArgs args(isKeyboardMsg, mousePosition);
    this->RaiseRoutedEvent(args);

    if (!args.cancel) {
        this->ShowContextMenu(isKeyboardMsg ? this->PointToScreen({0, 0}) : mousePosition);
    }

    return true;
}

void sw::UIElement::OnMenuCommand(int id)
{
    if (this->_contextMenu) {
        MenuItem *item = this->_contextMenu->GetMenuItem(id);
        if (item) item->CallCommand();
    }
}

bool sw::UIElement::OnCtlColor(HDC hdc, HWND hControl, HBRUSH &hRetBrush)
{
    WndBase *childWnd = WndBase::GetWndBase(hControl);
    if (childWnd == nullptr) return false;

    UIElement *child = dynamic_cast<UIElement *>(childWnd);
    if (child == nullptr) return this->WndBase::OnCtlColor(hdc, hControl, hRetBrush);

    static HBRUSH hBrush = NULL;
    COLORREF textColor   = child->TextColor.Get();
    COLORREF backColor   = child->GetRealBackColor();

    ::SetTextColor(hdc, textColor);
    ::SetBkColor(hdc, backColor);

    if (hBrush != NULL) {
        DeleteObject(hBrush);
    }

    hBrush    = CreateSolidBrush(backColor);
    hRetBrush = hBrush;
    return true;
}

sw::UIElement *sw::UIElement::_GetNextElement(UIElement *element, bool searchChildren)
{
    if (searchChildren && !element->_children.empty()) {
        return element->_children.front();
    }

    UIElement *parent = element->_parent;
    if (parent == nullptr) {
        return element; // 回到根节点
    }

    int index = parent->IndexOf(element);
    if (index == (int)parent->_children.size() - 1) {
        return _GetNextElement(parent, false);
    }

    return parent->_children[index + 1];
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

std::wstring sw::Utils::Trim(const std::wstring &str)
{
    size_t firstNonSpace = str.find_first_not_of(L" \t\n\r\f\v");
    if (firstNonSpace == std::wstring::npos) {
        return L"";
    }
    size_t lastNonSpace = str.find_last_not_of(L" \t\n\r\f\v");
    return str.substr(firstNonSpace, lastNonSpace - firstNonSpace + 1);
}

std::wstring sw::Utils::TrimStart(const std::wstring &str)
{
    size_t firstNonSpace = str.find_first_not_of(L" \t\n\r\f\v");
    if (firstNonSpace == std::wstring::npos) {
        return L"";
    }
    return str.substr(firstNonSpace);
}

std::wstring sw::Utils::TrimEnd(const std::wstring &str)
{
    size_t lastNonSpace = str.find_last_not_of(L" \t\n\r\f\v");
    if (lastNonSpace == std::wstring::npos) {
        return L"";
    }
    return str.substr(0, lastNonSpace + 1);
}

std::vector<std::wstring> sw::Utils::Split(const std::wstring &str, const std::wstring &delimiter)
{
    std::vector<std::wstring> result;

    if (delimiter.empty()) {
        result.push_back(str);
        return result;
    }

    size_t start = 0;
    size_t end   = str.find(delimiter);

    const size_t delimiterLength = delimiter.length();

    while (end != std::wstring::npos) {
        result.emplace_back(str, start, end - start);
        start = end + delimiterLength;
        end   = str.find(delimiter, start);
    }

    result.emplace_back(str, start);

    return result;
}

// Window.cpp

#if !defined(WM_DPICHANGED)
#define WM_DPICHANGED 0x02E0
#endif

/**
 * @brief 记录当前创建的窗口数
 */
static int _windowCount = 0;

/**
 * @brief DPI更新时调用该函数递归地更新所有子项的字体
 */
static void _UpdateFontForAllChild(sw::UIElement &element);

/**
 * @brief  获取窗口默认图标（即当前exe图标）
 * @return 图标句柄
 */
static HICON _GetWindowDefaultIcon();

/**
 * @brief 程序的当前活动窗体
 */
const sw::ReadOnlyProperty<sw::Window *> sw::Window::ActiveWindow(
    []() -> sw::Window *const & {
        static sw::Window *pWindow;
        HWND hwnd = GetActiveWindow();
        pWindow   = dynamic_cast<sw::Window *>(sw::WndBase::GetWndBase(hwnd));
        return pWindow;
    } //
);

/**
 * @brief 当前已创建的窗口数
 */
const sw::ReadOnlyProperty<int> sw::Window::WindowCount(
    []() -> const int & {
        return _windowCount;
    } //
);

sw::Window::Window()
    : StartupLocation(
          // get
          [&]() -> const WindowStartupLocation & {
              return this->_startupLocation;
          },
          // set
          [&](const WindowStartupLocation &value) {
              this->_startupLocation = value;
          }),

      State(
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
          }),

      Menu(
          // get
          [&]() -> sw::Menu *const & {
              return this->_menu;
          },
          // set
          [&](sw::Menu *const &value) {
              this->_menu = value;
              SetMenu(this->Handle, value != nullptr ? value->GetHandle() : NULL);
          })
{
    this->InitWindow(L"Window", WS_OVERLAPPEDWINDOW, 0);
    this->SetCursor(StandardCursor::Arrow);
    this->SetIcon(_GetWindowDefaultIcon());
}

LRESULT sw::Window::WndProc(const ProcMsg &refMsg)
{
    switch (refMsg.uMsg) {
        case WM_CREATE: {
            ++_windowCount;
            return this->WndBase::WndProc(refMsg);
        }

        case WM_DESTROY: {
            // 若当前窗口为模态窗口则在窗口关闭时退出消息循环
            if (this->IsModal()) {
                App::QuitMsgLoop();
            }
            // 所有窗口都关闭时若App::QuitMode为Auto则退出主消息循环
            if (--_windowCount <= 0 && App::QuitMode.Get() == AppQuitMode::Auto) {
                App::QuitMsgLoop();
            }
            return this->WndBase::WndProc(refMsg);
        }

        case WM_SHOWWINDOW: {
            if (this->_isFirstShow) {
                this->_isFirstShow = false;
                this->OnFirstShow();
            }
            return this->WndBase::WndProc(refMsg);
        }

        case WM_GETMINMAXINFO: {
            double scaleX     = Dip::ScaleX;
            double scaleY     = Dip::ScaleY;
            PMINMAXINFO pInfo = reinterpret_cast<PMINMAXINFO>(refMsg.lParam);
            // 按照设置限制窗口大小
            if (this->_maxWidth > 0) {
                LONG maxWidth           = std::lround(this->_maxWidth / scaleX);
                pInfo->ptMaxTrackSize.x = Utils::Min(pInfo->ptMaxTrackSize.x, maxWidth);
            }
            if (this->_maxHeight > 0) {
                LONG maxHeight          = std::lround(this->_maxHeight / scaleY);
                pInfo->ptMaxTrackSize.y = Utils::Min(pInfo->ptMaxTrackSize.y, maxHeight);
            }
            if (this->_minWidth > 0) {
                LONG minWidth           = std::lround(this->_minWidth / scaleX);
                pInfo->ptMinTrackSize.x = Utils::Max(pInfo->ptMinTrackSize.x, minWidth);
            }
            if (this->_minHeight > 0) {
                LONG minHeight          = std::lround(this->_minHeight / scaleY);
                pInfo->ptMinTrackSize.y = Utils::Max(pInfo->ptMinTrackSize.y, minHeight);
            }
            return 0;
        }

        case WM_DPICHANGED: {
            Dip::Update(LOWORD(refMsg.wParam), HIWORD(refMsg.wParam));
            this->UpdateLayout();
            _UpdateFontForAllChild(*this);
            return 0;
        }

        case WM_ERASEBKGND: {
            return 1; // 阻止擦除背景
        }

        case WM_ACTIVATE: {
            if (refMsg.wParam == WA_INACTIVE)
                this->OnInactived();
            else
                this->OnActived();
            return 0;
        }

        case WM_UpdateLayout: {
            this->UpdateLayout();
            return 0;
        }

        default: {
            return this->WndBase::WndProc(refMsg);
        }
    }
}

sw::LayoutHost *sw::Window::GetDefaultLayout()
{
    return this->_layout.get();
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

bool sw::Window::OnDestroy()
{
    RaiseRoutedEvent(Window_Closed);
    return true;
}

bool sw::Window::OnPaint()
{
    PAINTSTRUCT ps;
    HWND hwnd = this->Handle;
    HDC hdc   = BeginPaint(hwnd, &ps);

    RECT rtClient;
    GetClientRect(hwnd, &rtClient);

    // 创建内存 DC 和位图
    HDC hdcMem         = CreateCompatibleDC(hdc);
    HBITMAP hBitmap    = CreateCompatibleBitmap(hdc, rtClient.right - rtClient.left, rtClient.bottom - rtClient.top);
    HBITMAP hBitmapOld = (HBITMAP)SelectObject(hdcMem, hBitmap);

    // 在内存 DC 上进行绘制
    HBRUSH hBrush = CreateSolidBrush(this->GetRealBackColor());
    FillRect(hdcMem, &rtClient, hBrush);

    // 将内存 DC 的内容绘制到窗口客户区
    BitBlt(hdc, 0, 0, rtClient.right - rtClient.left, rtClient.bottom - rtClient.top, hdcMem, 0, 0, SRCCOPY);

    // 清理资源
    SelectObject(hdcMem, hBitmapOld);
    DeleteObject(hBitmap);
    DeleteObject(hBrush);
    DeleteDC(hdcMem);

    EndPaint(hwnd, &ps);
    return true;
}

bool sw::Window::OnMouseMove(Point mousePosition, MouseKey keyState)
{
    ::SetCursor(this->_hCursor);
    return this->UIElement::OnMouseMove(mousePosition, keyState);
}

void sw::Window::OnMenuCommand(int id)
{
    if (ContextMenu::IsContextMenuID(id)) {
        this->UIElement::OnMenuCommand(id);
        return;
    }
    if (this->_menu) {
        MenuItem *item = this->_menu->GetMenuItem(id);
        if (item) item->CallCommand();
    }
}

void sw::Window::OnFirstShow()
{
    // 若未设置焦点元素则默认第一个元素为焦点元素
    if (this->ChildCount && GetAncestor(GetFocus(), GA_ROOT) != this->Handle) {
        this->operator[](0).Focused = true;
    }

    // 按照StartupLocation修改位置
    if (this->_startupLocation == WindowStartupLocation::CenterScreen) {
        sw::Rect rect = this->Rect;
        rect.left     = (Screen::Width - rect.width) / 2;
        rect.top      = (Screen::Height - rect.height) / 2;
        this->Rect    = rect;
    } else if (this->_startupLocation == WindowStartupLocation::CenterOwner) {
        if (this->IsModal()) {
            sw::Rect windowRect = this->Rect;
            sw::Rect ownerRect  = this->_modalOwner->Rect;
            windowRect.left     = ownerRect.left + (ownerRect.width - windowRect.width) / 2;
            windowRect.top      = ownerRect.top + (ownerRect.height - windowRect.height) / 2;
            this->Rect          = windowRect;
        }
    }
}

void sw::Window::OnActived()
{
    SetFocus(this->_hPrevFocused);
    this->RaiseRoutedEvent(Window_Actived);
}

void sw::Window::OnInactived()
{
    this->RaiseRoutedEvent(Window_Inactived);
    this->_hPrevFocused = GetFocus();
}

void sw::Window::Show()
{
    this->WndBase::Show(SW_SHOW);
}

void sw::Window::ShowDialog(Window &owner)
{
    if (this->IsModal() || this == &owner || this->IsDestroyed) {
        return;
    }

    this->_modalOwner = &owner;
    SetWindowLongPtrW(this->Handle, GWLP_HWNDPARENT, reinterpret_cast<LONG_PTR>(owner.Handle.Get()));

    bool oldIsEnabled = owner.Enabled;
    owner.Enabled     = false;

    this->Show();
    App::MsgLoop();
    SetForegroundWindow(owner.Handle);

    if (oldIsEnabled) {
        owner.Enabled = true;
    }
}

void sw::Window::SetCursor(HCURSOR hCursor)
{
    this->_hCursor = hCursor;
}

void sw::Window::SetCursor(StandardCursor cursor)
{
    this->SetCursor(CursorHelper::GetCursorHandle(cursor));
}

void sw::Window::SetIcon(HICON hIcon)
{
    this->SendMessageW(WM_SETICON, ICON_BIG, (LPARAM)hIcon);
    this->SendMessageW(WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
}

void sw::Window::DrawMenuBar()
{
    ::DrawMenuBar(this->Handle);
}

bool sw::Window::IsModal()
{
    return this->_modalOwner != nullptr;
}

void _UpdateFontForAllChild(sw::UIElement &element)
{
    element.UpdateFont();

    int count = element.ChildCount;
    for (int i = 0; i < count; ++i) {
        _UpdateFontForAllChild(element[i]);
    }
}

HICON _GetWindowDefaultIcon()
{
    static HICON hIcon = ExtractIconW(sw::App::Instance, sw::App::ExePath->c_str(), 0);
    return hIcon;
}

// WndBase.cpp

/**
 * @brief 窗口类名
 */
static constexpr wchar_t _WindowClassName[] = L"sw::Window";

/**
 * @brief 控件初始化时所在的窗口
 */
static struct : sw::WndBase{} *_controlInitContainer = nullptr;

/**
 */

LRESULT sw::WndBase::_WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    WndBase *pWnd = WndBase::GetWndBase(hwnd);

    if (pWnd == NULL && (uMsg == WM_NCCREATE || uMsg == WM_CREATE)) {
        LPCREATESTRUCTW pCreate;
        pCreate = reinterpret_cast<LPCREATESTRUCTW>(lParam);
        pWnd    = reinterpret_cast<WndBase *>(pCreate->lpCreateParams);
    }

    if (pWnd != NULL) {
        ProcMsg msg(hwnd, uMsg, wParam, lParam);
        return pWnd->WndProc(msg);
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
              if (this->_font.name != value) {
                  this->_font.name = value;
                  this->UpdateFont();
              }
          }),

      FontSize(
          // get
          [&]() -> const double & {
              return this->_font.size;
          },
          // set
          [&](const double &value) {
              if (this->_font.size != value) {
                  this->_font.size = value;
                  this->UpdateFont();
              }
          }),

      FontWeight(
          // get
          [&]() -> const sw::FontWeight & {
              return this->_font.weight;
          },
          // set
          [&](const sw::FontWeight &value) {
              if (this->_font.weight != value) {
                  this->_font.weight = value;
                  this->UpdateFont();
              }
          }),

      Rect(
          // get
          [&]() -> const sw::Rect & {
              return this->_rect;
          },
          // set
          [&](const sw::Rect &value) {
              if (this->_rect != value) {
                  double scaleX = Dip::ScaleX;
                  double scaleY = Dip::ScaleY;
                  int left      = std::lround(value.left / scaleX);
                  int top       = std::lround(value.top / scaleY);
                  int width     = std::lround(value.width / scaleX);
                  int height    = std::lround(value.height / scaleY);
                  SetWindowPos(this->_hwnd, NULL, left, top, width, height, SWP_NOACTIVATE | SWP_NOZORDER);
              }
          }),

      Left(
          // get
          [&]() -> const double & {
              return this->_rect.left;
          },
          // set
          [&](const double &value) {
              if (this->_rect.left != value) {
                  int x = std::lround(value / Dip::ScaleX);
                  int y = std::lround(this->_rect.top / Dip::ScaleY);
                  SetWindowPos(this->_hwnd, NULL, x, y, 0, 0, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
              }
          }),

      Top(
          // get
          [&]() -> const double & {
              return this->_rect.top;
          },
          // set
          [&](const double &value) {
              if (this->_rect.top != value) {
                  int x = std::lround(this->_rect.left / Dip::ScaleX);
                  int y = std::lround(value / Dip::ScaleY);
                  SetWindowPos(this->_hwnd, NULL, x, y, 0, 0, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
              }
          }),

      Width(
          // get
          [&]() -> const double & {
              return this->_rect.width;
          },
          // set
          [&](const double &value) {
              if (this->_rect.width != value) {
                  int cx = std::lround(value / Dip::ScaleX);
                  int cy = std::lround(this->_rect.height / Dip::ScaleY);
                  SetWindowPos(this->_hwnd, NULL, 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
              }
          }),

      Height(
          // get
          [&]() -> const double & {
              return this->_rect.height;
          },
          // set
          [&](const double &value) {
              if (this->_rect.height != value) {
                  int cx = std::lround(this->_rect.width / Dip::ScaleX);
                  int cy = std::lround(value / Dip::ScaleY);
                  SetWindowPos(this->_hwnd, NULL, 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
              }
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
              ShowWindow(this->_hwnd, value ? SW_SHOW : SW_HIDE);
              this->VisibleChanged(value);
          }),

      Text(
          // get
          [&]() -> const std::wstring & {
              return this->GetText();
          },
          // set
          [&](const std::wstring &value) {
              this->SetText(value);
          }),

      BackColor(
          // get
          [&]() -> const Color & {
              return this->_backColor;
          },
          // set
          [&](const Color &value) {
              this->SetBackColor(value, true);
          }),

      TextColor(
          // get
          [&]() -> const Color & {
              return this->_textColor;
          },
          // set
          [&](const Color &value) {
              this->SetTextColor(value, true);
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
        wc.hInstance     = App::Instance;
        wc.lpfnWndProc   = WndBase::_WndProc;
        wc.lpszClassName = _WindowClassName;
        RegisterClassExW(&wc);
    }

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

void sw::WndBase::InitWindow(LPCWSTR lpWindowName, DWORD dwStyle, DWORD dwExStyle)
{
    if (this->_hwnd == NULL) {

        this->_text = lpWindowName ? lpWindowName : L"";

        this->_hwnd = CreateWindowExW(
            dwExStyle,        // Optional window styles
            _WindowClassName, // Window class
            lpWindowName,     // Window text
            dwStyle,          // Window style

            // Size and position
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

            NULL,          // Parent window
            NULL,          // Menu
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

void sw::WndBase::InitControl(LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, DWORD dwExStyle)
{
    if (_controlInitContainer == nullptr) {
        _controlInitContainer = new std::remove_reference_t<decltype(*_controlInitContainer)>;
        _controlInitContainer->InitWindow(L"", WS_POPUP, 0);
    }

    if (this->_hwnd == NULL) {

        this->_text = lpWindowName ? lpWindowName : L"";

        this->_hwnd = CreateWindowExW(
            dwExStyle,    // Optional window styles
            lpClassName,  // Window class
            lpWindowName, // Window text
            dwStyle,      // Window style

            // Size and position
            0, 0, 0, 0,

            _controlInitContainer->_hwnd, // Parent window
            NULL,                         // Menu
            App::Instance,                // Instance handle
            this                          // Additional application data
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

HFONT sw::WndBase::GetFontHandle()
{
    return this->_hfont;
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
            LRESULT result = this->OnPaint() ? 0 : this->DefaultWndProc(refMsg);
            this->OnEndPaint();
            return result;
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
                this->OnTextChanged();
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
            if (refMsg.lParam != (LPARAM)NULL) {
                // 接收到控件消息，获取控件对象
                WndBase *pControl = WndBase::GetWndBase(reinterpret_cast<HWND>(refMsg.lParam));
                // Windows一些控件的内部句柄也是使用WM_COMMAND实现一些功能，如可编辑状态的组合框
                // 此处尝试获取控件WndBase对象，若为nullptr说明为内部句柄，此时调用原来的WndProc
                if (pControl == nullptr) {
                    this->DefaultWndProc(refMsg);
                } else {
                    this->OnControlCommand(pControl, HIWORD(refMsg.wParam), LOWORD(refMsg.wParam));
                }
            } else {
                // 接收到菜单或快捷键消息
                int id = LOWORD(refMsg.wParam);
                HIWORD(refMsg.wParam) ? this->OnAcceleratorCommand(id) : this->OnMenuCommand(id);
            }
            return 0;
        }

        case WM_NOTIFY: {
            NMHDR *pNMHDR = reinterpret_cast<NMHDR *>(refMsg.lParam);
            bool handled  = this->OnNotify(pNMHDR);

            WndBase *pWnd = WndBase::GetWndBase(pNMHDR->hwndFrom);
            if (pWnd) pWnd->OnNotified(pNMHDR);

            return handled ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_CTLCOLORMSGBOX:
        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORLISTBOX:
        case WM_CTLCOLORBTN:
        case WM_CTLCOLORDLG:
        case WM_CTLCOLORSCROLLBAR:
        case WM_CTLCOLORSTATIC: {
            HDC hdc       = (HDC)refMsg.wParam;
            HWND hControl = (HWND)refMsg.lParam;
            HBRUSH hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
            return this->OnCtlColor(hdc, hControl, hBrush) ? (LRESULT)hBrush : this->DefaultWndProc(refMsg);
        }

        case WM_SETCURSOR: {
            HWND hwnd   = (HWND)refMsg.wParam;
            int hitTest = LOWORD(refMsg.lParam);
            int message = HIWORD(refMsg.lParam);
            bool result = false;
            return this->OnSetCursor(hwnd, hitTest, message, result) ? result : this->DefaultWndProc(refMsg);
        }

        case WM_CONTEXTMENU: {
            WndBase *pWnd = WndBase::GetWndBase((HWND)refMsg.wParam);
            if (pWnd == nullptr) {
                return this->DefaultWndProc(refMsg);
            } else {
                int xPos = GET_X_LPARAM(refMsg.lParam);
                int yPos = GET_Y_LPARAM(refMsg.lParam);
                bool res = this->OnContextMenu(xPos == -1 && yPos == -1, POINT{xPos, yPos});
                return res ? 0 : this->DefaultWndProc(refMsg);
            }
        }

        case WM_VSCROLL: {
            if (!refMsg.lParam /*refMsg.lParam == NULL*/) {
                return this->OnVerticalScroll(LOWORD(refMsg.wParam), (int16_t)HIWORD(refMsg.wParam)) ? 0 : this->DefaultWndProc(refMsg);
            }
            WndBase *pWnd = WndBase::GetWndBase(reinterpret_cast<HWND>(refMsg.lParam));
            if (pWnd) {
                return pWnd->OnVerticalScroll(LOWORD(refMsg.wParam), (int16_t)HIWORD(refMsg.wParam)) ? 0 : this->DefaultWndProc(refMsg);
            } else {
                return this->DefaultWndProc(refMsg);
            }
        }

        case WM_HSCROLL: {
            if (!refMsg.lParam /*refMsg.lParam == NULL*/) {
                return this->OnHorizontalScroll(LOWORD(refMsg.wParam), (int16_t)HIWORD(refMsg.wParam)) ? 0 : this->DefaultWndProc(refMsg);
            }
            WndBase *pWnd = WndBase::GetWndBase(reinterpret_cast<HWND>(refMsg.lParam));
            if (pWnd) {
                return pWnd->OnHorizontalScroll(LOWORD(refMsg.wParam), (int16_t)HIWORD(refMsg.wParam)) ? 0 : this->DefaultWndProc(refMsg);
            } else {
                return this->DefaultWndProc(refMsg);
            }
        }

        case WM_ENABLE: {
            return this->OnEnabledChanged(refMsg.wParam) ? 0 : this->DefaultWndProc(refMsg);
        }

        default: {
            return this->DefaultWndProc(refMsg);
        }
    }
}

void sw::WndBase::UpdateText()
{
    int len = GetWindowTextLengthW(this->_hwnd);

    if (len <= 0) {
        this->_text = L"";
        return;
    }

    wchar_t *buf = new wchar_t[len + 1];
    GetWindowTextW(this->_hwnd, buf, len + 1);

    this->_text = buf;
    delete[] buf;
}

std::wstring &sw::WndBase::GetText()
{
    return this->_text;
}

void sw::WndBase::SetText(const std::wstring &value)
{
    SetWindowTextW(this->_hwnd, value.c_str());
}

void sw::WndBase::SetBackColor(Color color, bool redraw)
{
    this->_backColor = color;
    if (redraw) this->Redraw();
}

void sw::WndBase::SetTextColor(Color color, bool redraw)
{
    this->_textColor = color;
    if (redraw) this->Redraw();
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

void sw::WndBase::OnEndPaint()
{
}

bool sw::WndBase::OnMove(Point newClientPosition)
{
    return false;
}

bool sw::WndBase::OnSize(Size newClientSize)
{
    return false;
}

void sw::WndBase::OnTextChanged()
{
}

bool sw::WndBase::OnSetFocus(HWND hPrevFocus)
{
    return false;
}

bool sw::WndBase::OnKillFocus(HWND hNextFocus)
{
    return false;
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
        hParent = this->IsControl() ? _controlInitContainer->_hwnd : NULL;
    } else {
        hParent = parent->_hwnd;
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

void sw::WndBase::OnCommand(int code)
{
}

void sw::WndBase::OnControlCommand(WndBase *pControl, int code, int id)
{
    pControl->OnCommand(code);
}

void sw::WndBase::OnMenuCommand(int id)
{
}

void sw::WndBase::OnAcceleratorCommand(int id)
{
}

void sw::WndBase::HandleInitialized(HWND hwnd)
{
}

void sw::WndBase::FontChanged(HFONT hfont)
{
}

bool sw::WndBase::OnSetCursor(HWND hwnd, int hitTest, int message, bool &result)
{
    return false;
}

bool sw::WndBase::OnContextMenu(bool isKeyboardMsg, Point mousePosition)
{
    return false;
}

bool sw::WndBase::OnNotify(NMHDR *pNMHDR)
{
    return false;
}

void sw::WndBase::OnNotified(NMHDR *pNMHDR)
{
}

bool sw::WndBase::OnVerticalScroll(int event, int pos)
{
    return false;
}

bool sw::WndBase::OnHorizontalScroll(int event, int pos)
{
    return false;
}

bool sw::WndBase::OnEnabledChanged(bool newValue)
{
    return false;
}

bool sw::WndBase::OnCtlColor(HDC hdc, HWND hControl, HBRUSH &hRetBrush)
{
    static HBRUSH hBrush = NULL;

    WndBase *control = WndBase::GetWndBase(hControl);

    if (control) {

        if (hBrush != NULL) {
            DeleteObject(hBrush);
        }

        hBrush = CreateSolidBrush(control->_backColor);

        ::SetTextColor(hdc, control->_textColor);
        ::SetBkColor(hdc, control->_backColor);

        hRetBrush = hBrush;
        return true;
    }

    return false;
}

void sw::WndBase::Show(int nCmdShow)
{
    ShowWindow(this->_hwnd, nCmdShow);
}

void sw::WndBase::Close()
{
    this->SendMessageW(WM_CLOSE, 0, 0);
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
    this->SendMessageW(WM_SETFONT, (WPARAM)this->_hfont, TRUE);
    this->FontChanged(this->_hfont);
}

void sw::WndBase::Redraw(bool erase)
{
    InvalidateRect(this->_hwnd, NULL, erase);
    UpdateWindow(this->_hwnd);
}

bool sw::WndBase::IsControl()
{
    return this->_controlOldWndProc != nullptr;
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

LRESULT sw::WndBase::SendMessageW(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return ::SendMessageW(this->_hwnd, uMsg, wParam, lParam);
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
            size.height = Utils::Max(size.height, itemDesireSize.height);
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
                rowHeight = Utils::Max(rowHeight, itemDesireSize.height);
            } else {
                top += rowHeight;
                rowWidth  = itemDesireSize.width;
                rowHeight = itemDesireSize.height;
            }
            size.width = Utils::Max(size.width, rowWidth);
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
            rowHeight = Utils::Max(rowHeight, itemDesireSize.height);
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
            size.width = Utils::Max(size.width, itemDesireSize.width);
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
                colWidth = Utils::Max(colWidth, itemDesireSize.width);
            } else {
                left += colWidth;
                colHeight = itemDesireSize.height;
                colWidth  = itemDesireSize.width;
            }
            size.height = Utils::Max(size.height, colHeight);
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
            colWidth = Utils::Max(colWidth, itemDesireSize.width);
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
    this->_wrapLayout.Associate(this);
    this->HorizontalAlignment = HorizontalAlignment::Stretch;
    this->VerticalAlignment   = VerticalAlignment::Stretch;
}

sw::LayoutHost *sw::WrapPanel::GetDefaultLayout()
{
    return &this->_wrapLayout;
}
