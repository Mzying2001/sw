#include "sw_all.h"
#include <cmath>
#include <limits>
#include <strsafe.h>
#include <climits>
#include <deque>
#include <cstdarg>
#include <atomic>

// Animation.cpp

sw::Animation::Animation()
    : Center(
          // get
          [this]() -> bool {
              return this->GetStyle(ACS_CENTER);
          },
          // set
          [this](const bool &value) {
              this->SetStyle(ACS_CENTER, value);
          }),

      AutoPlay(
          // get
          [this]() -> bool {
              return this->GetStyle(ACS_AUTOPLAY);
          },
          // set
          [this](const bool &value) {
              this->SetStyle(ACS_AUTOPLAY, value);
          }),

      IsPlaying(
          // get
          [this]() -> bool {
              return this->SendMessageW(ACM_ISPLAYING, 0, 0);
          })
{
    this->InitControl(ANIMATE_CLASSW, L"", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, 0);
    this->Rect = {0, 0, 200, 200};
}

bool sw::Animation::Open(HINSTANCE hInstance, int resourceId)
{
    return this->SendMessageW(ACM_OPENW, reinterpret_cast<WPARAM>(hInstance), reinterpret_cast<LPARAM>(MAKEINTRESOURCEW(resourceId)));
}

bool sw::Animation::Open(const std::wstring &fileName)
{
    return this->Open(fileName.c_str());
}

bool sw::Animation::Open(const wchar_t *fileName)
{
    return this->SendMessageW(ACM_OPENW, 0, reinterpret_cast<LPARAM>(fileName));
}

bool sw::Animation::Play(int times, int beginFrame, int endFrame)
{
    return this->SendMessageW(ACM_PLAY, (WPARAM)times, MAKELPARAM(beginFrame, endFrame));
}

bool sw::Animation::Play(int times)
{
    return this->Play(times, 0, -1);
}

bool sw::Animation::Stop()
{
    return this->SendMessageW(ACM_STOP, 0, 0);
}

// App.cpp

namespace
{
    /**
     * @brief 程序退出消息循环的方式，默认为Auto
     */
    thread_local sw::AppQuitMode _appQuitMode = sw::AppQuitMode::Auto;

    /**
     * @brief  获取当前exe文件路径
     */
    std::wstring _GetExePath()
    {
        HMODULE hModule = GetModuleHandleW(NULL);

        std::wstring exePath;
        exePath.resize(MAX_PATH);

        while (true) {
            DWORD len =
                GetModuleFileNameW(hModule, &exePath[0], (DWORD)exePath.size());
            if (len == 0) {
                exePath.clear();
                break;
            } else if (len < exePath.size()) {
                exePath.resize(len);
                break;
            } else {
                exePath.resize(exePath.size() * 2);
            }
        }
        return exePath;
    }

    /**
     * @brief 获取当前工作路径
     */
    std::wstring _GetCurrentDirectory()
    {
        std::wstring curdir;
        curdir.resize(GetCurrentDirectoryW(0, NULL));
        if (curdir.size())
            curdir.resize(GetCurrentDirectoryW((DWORD)curdir.size(), &curdir[0]));
        return curdir;
    }
}

/**
 * @brief 当前线程消息循环中处理空句柄消息的回调函数
 */
thread_local sw::Action<MSG &> sw::App::NullHwndMsgHandler;

/**
 */

const sw::ReadOnlyProperty<HINSTANCE> sw::App::Instance(
    []() -> HINSTANCE {
        static HINSTANCE hInstance = GetModuleHandleW(NULL);
        return hInstance;
    } //
);

const sw::ReadOnlyProperty<std::wstring> sw::App::ExePath(
    []() -> std::wstring {
        static std::wstring exePath = _GetExePath();
        return exePath;
    } //
);

const sw::ReadOnlyProperty<std::wstring> sw::App::ExeDirectory(
    []() -> std::wstring {
        static std::wstring exeDirectory = Path::GetDirectory(App::ExePath);
        return exeDirectory;
    } //
);

const sw::Property<std::wstring> sw::App::CurrentDirectory(
    // get
    []() -> std::wstring {
        return _GetCurrentDirectory();
    },
    // set
    [](const std::wstring &value) {
        SetCurrentDirectoryW(value.c_str());
    } //
);

const sw::Property<sw::AppQuitMode> sw::App::QuitMode(
    // get
    []() -> sw::AppQuitMode {
        return _appQuitMode;
    },
    // set
    [](const sw::AppQuitMode &value) {
        _appQuitMode = value;
    } //
);

int sw::App::MsgLoop()
{
    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0) > 0) {
        if (msg.hwnd == NULL) {
            if (NullHwndMsgHandler)
                NullHwndMsgHandler(msg);
        } else {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }
    return (int)msg.wParam;
}

void sw::App::QuitMsgLoop(int exitCode)
{
    PostQuitMessage(exitCode);
}

// BmpBox.cpp

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
                  this->InvalidateMeasure();
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

sw::Size sw::BmpBox::MeasureOverride(const Size &availableSize)
{
    return this->_sizeMode == BmpBoxSizeMode::AutoSize
               ? static_cast<Size>(this->_bmpSize)
               : this->StaticControl::MeasureOverride(availableSize);
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
        this->InvalidateMeasure();
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

// Button.cpp

sw::Button::Button()
{
    this->InitButtonBase(L"Button", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | BS_NOTIFY | BS_PUSHBUTTON, 0);
    this->Rect = sw::Rect(0, 0, 70, 30);
}

void sw::Button::UpdateButtonStyle(bool focused)
{
    this->SetStyle(BS_DEFPUSHBUTTON, focused); // BS_PUSHBUTTON == 0
}

void sw::Button::OnDrawFocusRect(HDC hdc)
{
    HWND hwnd = this->Handle;

    RECT rect;
    GetClientRect(hwnd, &rect);

    rect.left += 3;
    rect.top += 3;
    rect.right -= 3;
    rect.bottom -= 3;

    DrawFocusRect(hdc, &rect);
}

bool sw::Button::OnSetFocus(HWND hPreFocus)
{
    this->UpdateButtonStyle(true);
    this->Redraw();
    return this->ButtonBase::OnSetFocus(hPreFocus);
}

bool sw::Button::OnKillFocus(HWND hNextFocus)
{
    this->UpdateButtonStyle(false);
    this->Redraw();
    return this->ButtonBase::OnKillFocus(hNextFocus);
}

bool sw::Button::OnKeyDown(VirtualKey key, KeyFlags flags)
{
    bool result = this->UIElement::OnKeyDown(key, flags);

    if (!result && key == VirtualKey::Enter) {
        this->OnClicked();
    }
    return result;
}

// ButtonBase.cpp

sw::ButtonBase::ButtonBase()
    : AutoSize(
          // get
          [this]() -> bool {
              return _autoSize;
          },
          // set
          [this](const bool &value) {
              if (_autoSize != value) {
                  _autoSize = value;
                  _UpdateLayoutFlags();
                  InvalidateMeasure();
              }
          }),

      MultiLine(
          // get
          [this]() -> bool {
              return GetStyle(BS_MULTILINE);
          },
          // set
          [this](const bool &value) {
              SetStyle(BS_MULTILINE, value);
              if (_autoSize) InvalidateMeasure();
          }),

      TextMargin(
          // get
          [this]() -> Thickness {
              RECT rect{};
              _GetTextMargin(rect);
              return rect;
          },
          // set
          [this](const Thickness &value) {
              RECT rect = value;
              _SetTextMargin(rect);
              if (_autoSize) InvalidateMeasure();
          })
{
    TabStop          = true;
    Transparent      = true;
    InheritTextColor = true;
}

sw::ButtonBase::~ButtonBase()
{
}

void sw::ButtonBase::InitButtonBase(LPCWSTR lpWindowName, DWORD dwStyle, DWORD dwExStyle)
{
    InitControl(L"BUTTON", lpWindowName, dwStyle, dwExStyle);
}

void sw::ButtonBase::OnClicked()
{
    RaiseRoutedEvent(ButtonBase_Clicked);
}

void sw::ButtonBase::OnDoubleClicked()
{
    RaiseRoutedEvent(ButtonBase_DoubleClicked);
}

void sw::ButtonBase::OnCommand(int code)
{
    switch (code) {
        case BN_CLICKED:
            OnClicked();
            break;

        case BN_DOUBLECLICKED:
            OnDoubleClicked();
            break;

        default:
            break;
    }
}

sw::Size sw::ButtonBase::MeasureOverride(const Size &availableSize)
{
    if (!_autoSize) {
        return UIElement::MeasureOverride(availableSize);
    }

    SIZE szIdeal = {0, 0};

    if (!_GetIdealSize(szIdeal)) {
        return UIElement::MeasureOverride(availableSize);
    }

    Size desireSize = szIdeal;

    if (szIdeal.cx == 0 && std::isinf(availableSize.width)) {
        desireSize = UIElement::MeasureOverride(availableSize);
        szIdeal.cx = Dip::DipToPxX(desireSize.width);
        szIdeal.cy = 0;
        _GetIdealSize(szIdeal);
        desireSize.height = Dip::PxToDipY(szIdeal.cy);
    } else if (availableSize.width < desireSize.width ||
               (szIdeal.cx == 0 && !std::isinf(availableSize.width))) {
        szIdeal.cx = Dip::DipToPxX(availableSize.width);
        szIdeal.cy = 0;
        _GetIdealSize(szIdeal);
        desireSize.width  = availableSize.width;
        desireSize.height = Dip::PxToDipY(szIdeal.cy);
    }
    return desireSize;
}

void sw::ButtonBase::_UpdateLayoutFlags()
{
    constexpr auto flags =
        sw::LayoutUpdateCondition::TextChanged |
        sw::LayoutUpdateCondition::FontChanged;

    if (_autoSize) {
        LayoutUpdateCondition |= flags;
    } else {
        LayoutUpdateCondition &= ~flags;
    }
}

bool sw::ButtonBase::_GetIdealSize(SIZE &size)
{
    return SendMessageW(BCM_GETIDEALSIZE, 0, reinterpret_cast<LPARAM>(&size)) == TRUE;
}

bool sw::ButtonBase::_GetTextMargin(RECT &rect)
{
    return SendMessageW(BCM_GETTEXTMARGIN, 0, reinterpret_cast<LPARAM>(&rect)) == TRUE;
}

bool sw::ButtonBase::_SetTextMargin(RECT &rect)
{
    return SendMessageW(BCM_SETTEXTMARGIN, 0, reinterpret_cast<LPARAM>(&rect)) == TRUE;
}

// Canvas.cpp

sw::Canvas::Canvas()
{
    this->_canvasLayout.Associate(this);
    this->SetAlignment(HorizontalAlignment::Stretch, VerticalAlignment::Stretch);
}

sw::CanvasLayoutTag sw::Canvas::GetCanvasLayoutTag(UIElement &element)
{
    return element.LayoutTag.Get();
}

void sw::Canvas::SetCanvasLayoutTag(UIElement &element, const CanvasLayoutTag &tag)
{
    element.LayoutTag.Set(tag);
}

sw::LayoutHost *sw::Canvas::GetDefaultLayout()
{
    return &this->_canvasLayout;
}

// CanvasLayout.cpp

sw::CanvasLayoutTag::CanvasLayoutTag()
    : left(0), top(0)
{
}

sw::CanvasLayoutTag::CanvasLayoutTag(float left, float top)
    : left(left), top(top)
{
}

sw::CanvasLayoutTag::CanvasLayoutTag(uint64_t layoutTag)
    : left(reinterpret_cast<float *>(&layoutTag)[0]),
      top(reinterpret_cast<float *>(&layoutTag)[1])
{
}

sw::CanvasLayoutTag::operator uint64_t() const
{
    uint64_t result;
    reinterpret_cast<float *>(&result)[0] = left;
    reinterpret_cast<float *>(&result)[1] = top;
    return result;
}

sw::Size sw::CanvasLayout::MeasureOverride(const Size &availableSize)
{
    Size desireSize{};
    Size measureSize{INFINITY, INFINITY};

    int childCount = this->GetChildLayoutCount();
    for (int i = 0; i < childCount; ++i) {
        ILayout &item = this->GetChildLayoutAt(i);

        item.Measure(measureSize);
        Size childDesireSize = item.GetDesireSize();
        CanvasLayoutTag tag  = item.GetLayoutTag();
        desireSize.width     = Utils::Max(tag.left + childDesireSize.width, desireSize.width);
        desireSize.height    = Utils::Max(tag.top + childDesireSize.height, desireSize.height);
    }

    return desireSize;
}

void sw::CanvasLayout::ArrangeOverride(const Size &finalSize)
{
    int childCount = this->GetChildLayoutCount();
    for (int i = 0; i < childCount; ++i) {
        ILayout &item        = this->GetChildLayoutAt(i);
        Size childDesireSize = item.GetDesireSize();
        CanvasLayoutTag tag  = item.GetLayoutTag();
        item.Arrange(Rect{tag.left, tag.top, childDesireSize.width, childDesireSize.height});
    }
}

// CheckBox.cpp

sw::CheckBox::CheckBox()
    : ThreeState(
          // get
          [this]() -> bool {
              return (GetStyle() & BS_AUTO3STATE) == BS_AUTO3STATE;
          },
          // set
          [this](const bool &value) {
              auto style = GetStyle() & ~(BS_AUTOCHECKBOX | BS_AUTO3STATE);
              SetStyle(value ? (style | BS_AUTO3STATE) : (style | BS_AUTOCHECKBOX));
          })
{
    InitButtonBase(L"CheckBox", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | BS_NOTIFY | BS_AUTOCHECKBOX, 0);
    Rect     = sw::Rect{0, 0, 100, 20};
    AutoSize = true;
}

// CheckableButton.cpp

sw::CheckableButton::CheckableButton()
    : CheckState(
          // get
          [this]() -> sw::CheckState {
              return (sw::CheckState)this->SendMessageW(BM_GETCHECK, 0, 0);
          },
          // set
          [this](const sw::CheckState &value) {
              this->SendMessageW(BM_SETCHECK, (WPARAM)value, 0);
          }),

      IsChecked(
          // get
          [this]() -> bool {
              return this->CheckState.Get() == sw::CheckState::Checked;
          },
          // set
          [this](const bool &value) {
              this->CheckState = value ? sw::CheckState::Checked : sw::CheckState::Unchecked;
          })
{
}

sw::CheckableButton::~CheckableButton()
{
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

sw::Color::Color(KnownColor knownColor)
    : Color(COLORREF(knownColor))
{
}

sw::Color::Color(COLORREF color)
    : r((color >> 0) & 0xFF), g((color >> 8) & 0xFF), b((color >> 16) & 0xFF)
{
}

sw::Color::operator COLORREF() const
{
    return RGB(this->r, this->g, this->b);
}

bool sw::Color::operator==(const Color &other) const
{
    return (this->r == other.r) && (this->g == other.g) && (this->b == other.b);
}

bool sw::Color::operator!=(const Color &other) const
{
    return (this->r != other.r) || (this->g != other.g) || (this->b != other.b);
}

std::wstring sw::Color::ToString() const
{
    return Utils::FormatStr(L"Color{r=%u, g=%u, b=%u}", this->r, this->g, this->b);
}

// ColorDialog.cpp

namespace
{
    /**
     * @brief 默认自定义颜色数组
     */
    COLORREF _defaultCustomColors[16] = {
        RGB(255, 255, 255),
        RGB(255, 255, 255),
        RGB(255, 255, 255),
        RGB(255, 255, 255),
        RGB(255, 255, 255),
        RGB(255, 255, 255),
        RGB(255, 255, 255),
        RGB(255, 255, 255),
        RGB(255, 255, 255),
        RGB(255, 255, 255),
        RGB(255, 255, 255),
        RGB(255, 255, 255),
        RGB(255, 255, 255),
        RGB(255, 255, 255),
        RGB(255, 255, 255),
        RGB(255, 255, 255),
    };
}

sw::ColorDialog::ColorDialog()
    : Flags(
          // get
          [this]() -> ColorDialogFlags {
              return static_cast<ColorDialogFlags>(_cc.Flags);
          },
          // set
          [this](const ColorDialogFlags &value) {
              _cc.Flags = static_cast<DWORD>(value);
          }),

      SelectedColor(
          // get
          [this]() -> Color {
              return _cc.rgbResult;
          },
          // set
          [this](const Color &value) {
              _cc.rgbResult = value;
          }),

      FullOpen(
          // get
          [this]() -> bool {
              return (Flags & ColorDialogFlags::FullOpen) == ColorDialogFlags::FullOpen;
          },
          // set
          [this](const bool &value) {
              if (value) {
                  Flags |= ColorDialogFlags::FullOpen;
              } else {
                  Flags &= ~ColorDialogFlags::FullOpen;
              }
          }),

      CustomColors(
          // get
          [this]() -> COLORREF * {
              return _cc.lpCustColors;
          },
          // set
          [this](COLORREF *value) {
              _cc.lpCustColors = value ? value : _defaultCustomColors;
          })
{
    _cc.lStructSize  = sizeof(CHOOSECOLORW);
    _cc.Flags        = DWORD(ColorDialogFlags::RgbInit);
    _cc.rgbResult    = Color(KnownColor::Black);
    _cc.lpCustColors = _defaultCustomColors;
}

void sw::ColorDialog::Close()
{
}

void sw::ColorDialog::Show()
{
}

int sw::ColorDialog::ShowDialog(Window *owner)
{
    if (owner == nullptr) {
        owner = Window::ActiveWindow;
    }

    HWND hOwner    = owner ? owner->Handle.Get() : NULL;
    auto pCC       = GetChooseColorStruct();
    pCC->hwndOwner = hOwner;

    return ChooseColorW(pCC);
}

int sw::ColorDialog::ShowDialog(Window &owner)
{
    return ShowDialog(&owner);
}

CHOOSECOLORW *sw::ColorDialog::GetChooseColorStruct()
{
    return &_cc;
}

// ComboBox.cpp

namespace
{
    constexpr DWORD _ComboBoxStyle_Default  = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | CBS_AUTOHSCROLL | CBS_HASSTRINGS | CBS_DROPDOWNLIST;
    constexpr DWORD _ComboBoxStyle_Editable = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | CBS_AUTOHSCROLL | CBS_HASSTRINGS | CBS_DROPDOWN;
}

sw::ComboBox::ComboBox()
    : IsEditable(
          // get
          [this]() -> bool {
              return this->GetStyle() == _ComboBoxStyle_Editable;
          },
          // set
          [this](const bool &value) {
              if (this->IsEditable != value) {
                  this->SetStyle(value ? _ComboBoxStyle_Editable : _ComboBoxStyle_Default);
                  this->ResetHandle();
                  this->SetInternalText(this->WndBase::GetInternalText()); // 使切换后文本框内容能够保留
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

std::wstring &sw::ComboBox::GetInternalText()
{
    if (this->_isTextChanged) {
        this->UpdateInternalText();
        this->_isTextChanged = false;
    }
    return this->WndBase::GetInternalText();
}

void sw::ComboBox::SetInternalText(const std::wstring &value)
{
    // 当组合框可编辑时，直接调用WndBase::SetInternalText以更新文本框
    // 不可编辑时，直接修改_text字段（WndBase中定义，用于保存窗体文本）
    // 修改IsEditable属性后会重新创建句柄，会直接将_text字段设为新的文本
    // 这里直接修改_text以实现在IsEditable为false时修改的Text能够在IsEditable更改为true时文本框内容能正确显示

    if (this->IsEditable) {
        this->WndBase::SetInternalText(value);
    } else {
        this->WndBase::GetInternalText() = value;
        this->_isTextChanged             = false;
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
    this->_isTextChanged             = false;
    this->WndBase::GetInternalText() = this->GetSelectedItem();

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
        return std::wstring{};
    }

    std::wstring result;
    result.resize(len + 1);
    this->SendMessageW(CB_GETLBTEXT, index, reinterpret_cast<LPARAM>(&result[0]));
    result.resize(len);
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

// CommandLink.cpp

sw::CommandLink::CommandLink()
    : NoteText(
          // get
          [this]() -> std::wstring {
              int len = (int)SendMessageW(BCM_GETNOTELENGTH, 0, 0);
              if (len <= 0)
                  return std::wstring{};
              else {
                  std::wstring result;
                  DWORD buflen = len + 1;
                  result.resize(buflen);
                  SendMessageW(BCM_GETNOTE, reinterpret_cast<WPARAM>(&buflen), reinterpret_cast<LPARAM>(&result[0]));
                  result.resize(len);
                  return result;
              }
          },
          // set
          [this](const std::wstring &value) {
              SendMessageW(BCM_SETNOTE, 0, reinterpret_cast<LPARAM>(value.c_str()));
              if (AutoSize) InvalidateMeasure();
          })
{
    InitButtonBase(L"CommandLink", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | BS_NOTIFY | BS_COMMANDLINK, 0);
    Rect     = sw::Rect{0, 0, 180, 60};
    NoteText = L"Description";
}

void sw::CommandLink::UpdateButtonStyle(bool focused)
{
    auto style = GetStyle() & ~(BS_COMMANDLINK | BS_DEFCOMMANDLINK);
    SetStyle(focused ? (style | BS_DEFCOMMANDLINK) : (style | BS_COMMANDLINK));
}

bool sw::CommandLink::OnSetFocus(HWND hPreFocus)
{
    UpdateButtonStyle(true);
    Redraw();
    return ButtonBase::OnSetFocus(hPreFocus);
}

bool sw::CommandLink::OnKillFocus(HWND hNextFocus)
{
    UpdateButtonStyle(false);
    Redraw();
    return ButtonBase::OnKillFocus(hNextFocus);
}

bool sw::CommandLink::OnKeyDown(VirtualKey key, KeyFlags flags)
{
    bool result = UIElement::OnKeyDown(key, flags);

    if (!result && key == VirtualKey::Enter) {
        OnClicked();
    }
    return result;
}

// ContextMenu.cpp

namespace
{
    /**
     * @brief 上下文菜单ID的起始位置，用于与普通菜单ID区分
     */
    constexpr int _ContextMenuIDFirst = (std::numeric_limits<uint16_t>::max)() / 2;
}

sw::ContextMenu::ContextMenu()
    : MenuBase(CreatePopupMenu())
{
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
    : ControlId(
          // get
          [this]() -> int {
              return GetDlgCtrlID(_hwnd);
          }),

      IsInHierarchy(
          // get
          [this]() -> bool {
              if (_hwnd == NULL || _isDestroyed) {
                  return false;
              }
              auto container = WndBase::_GetControlInitContainer();
              return container == nullptr || GetParent(_hwnd) != container->_hwnd;
          })
{
}

sw::Control::~Control()
{
}

sw::Control *sw::Control::ToControl()
{
    return this;
}

bool sw::Control::ResetHandle(LPVOID lpParam)
{
    DWORD style   = GetStyle();
    DWORD exStyle = GetExtendedStyle();
    return ResetHandle(style, exStyle, lpParam);
}

bool sw::Control::ResetHandle(DWORD style, DWORD exStyle, LPVOID lpParam)
{
    if (!CheckAccess()) {
        return false;
    }

    RECT rect = Rect;
    auto text = GetInternalText().c_str();

    HWND oldHwnd = _hwnd;
    HWND hParent = GetParent(oldHwnd);

    wchar_t className[256];
    GetClassNameW(oldHwnd, className, 256);

    HMENU id = reinterpret_cast<HMENU>(
        static_cast<uintptr_t>(GetDlgCtrlID(oldHwnd)));

    HWND newHwnd = CreateWindowExW(
        exStyle,   // Optional window styles
        className, // Window class
        text,      // Window text
        style,     // Window style

        // Size and position
        rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,

        hParent,       // Parent window
        id,            // Control id
        App::Instance, // Instance handle
        lpParam        // Additional application data
    );

    LONG_PTR wndproc =
        SetWindowLongPtrW(oldHwnd, GWLP_WNDPROC, GetWindowLongPtrW(newHwnd, GWLP_WNDPROC));

    WndBase::_SetWndBase(newHwnd, *this);
    SetWindowLongPtrW(newHwnd, GWLP_WNDPROC, wndproc);

    _hwnd = newHwnd;
    DestroyWindow(oldHwnd);

    SendMessageW(WM_SETFONT, (WPARAM)GetFontHandle(), TRUE);
    UpdateSiblingsZOrder();
    OnHandleChenged(_hwnd);
    return true;
}

bool sw::Control::OnNotified(NMHDR *pNMHDR, LRESULT &result)
{
    switch (pNMHDR->code) {
        case NM_CUSTOMDRAW: {
            return OnCustomDraw(reinterpret_cast<NMCUSTOMDRAW *>(pNMHDR), result);
        }
        default: {
            return UIElement::OnNotified(pNMHDR, result);
        }
    }
}

bool sw::Control::OnKillFocus(HWND hNextFocus)
{
    _drawFocusRect = false;
    return UIElement::OnKillFocus(hNextFocus);
}

void sw::Control::OnTabStop()
{
    _drawFocusRect = true;
    UIElement::OnTabStop();
}

void sw::Control::OnEndPaint()
{
    if (!_hasCustomDraw && _drawFocusRect) {
        HDC hdc = GetDC(_hwnd);
        OnDrawFocusRect(hdc);
        ReleaseDC(_hwnd, hdc);
    }
}

bool sw::Control::OnCustomDraw(NMCUSTOMDRAW *pNMCD, LRESULT &result)
{
    _hasCustomDraw = true;

    switch (pNMCD->dwDrawStage) {
        case CDDS_PREERASE: {
            return OnPreErase(pNMCD->hdc, result);
        }
        case CDDS_POSTERASE: {
            return OnPostErase(pNMCD->hdc, result);
        }
        case CDDS_PREPAINT: {
            return OnPrePaint(pNMCD->hdc, result);
        }
        case CDDS_POSTPAINT: {
            return OnPostPaint(pNMCD->hdc, result);
        }
        default: {
            return false;
        }
    }
}

bool sw::Control::OnPreErase(HDC hdc, LRESULT &result)
{
    result = CDRF_NOTIFYPOSTERASE;
    return true;
}

bool sw::Control::OnPostErase(HDC hdc, LRESULT &result)
{
    return false;
}

bool sw::Control::OnPrePaint(HDC hdc, LRESULT &result)
{
    result = CDRF_NOTIFYPOSTPAINT;
    return true;
}

bool sw::Control::OnPostPaint(HDC hdc, LRESULT &result)
{
    if (_drawFocusRect) {
        OnDrawFocusRect(hdc);
    }
    return false;
}

void sw::Control::OnDrawFocusRect(HDC hdc)
{
    // RECT rect = ClientRect;
    RECT rect;
    GetClientRect(_hwnd, &rect);
    DrawFocusRect(hdc, &rect);
}

void sw::Control::OnHandleChenged(HWND hwnd)
{
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

HCURSOR sw::CursorHelper::GetCursorHandle(const std::wstring &fileName)
{
    return (HCURSOR)LoadImageW(NULL, fileName.c_str(), IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE);
}

// DateTimePicker.cpp

sw::DateTimePicker::DateTimePicker()
    : ShowUpDownButton(
          // get
          [this]() -> bool {
              return this->GetStyle(DTS_UPDOWN);
          },
          // set
          [this](const bool &value) {
              if (this->ShowUpDownButton != value) {
                  this->_UpdateStyle(
                      value ? (this->GetStyle() | DTS_UPDOWN)
                            : (this->GetStyle() & ~DTS_UPDOWN));
              }
          }),

      Format(
          // get
          [this]() -> DateTimePickerFormat {
              return this->_format;
          },
          // set
          [this](const DateTimePickerFormat &value) {
              if (this->_format == value) {
                  return;
              }
              DWORD style = this->GetStyle();
              style &= ~(DTS_SHORTDATEFORMAT | DTS_LONGDATEFORMAT);
              switch (value) {
                  case DateTimePickerFormat::Short:
                  case DateTimePickerFormat::Custom:
                      style |= DTS_SHORTDATEFORMAT;
                      break;
                  case DateTimePickerFormat::Long:
                      style |= DTS_LONGDATEFORMAT;
                      break;
              }
              this->_format = value;
              this->_UpdateStyle(style);
          }),

      CustomFormat(
          // get
          [this]() -> std::wstring {
              return this->_customFormat;
          },
          // set
          [this](const std::wstring &value) {
              this->_format       = DateTimePickerFormat::Custom;
              this->_customFormat = value;
              this->_SetFormat(this->_customFormat);
          })
{
    this->InitControl(DATETIMEPICK_CLASSW, L"", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | DTS_SHORTDATEFORMAT, 0);
    this->Rect    = sw::Rect{0, 0, 100, 24};
    this->TabStop = true;
}

bool sw::DateTimePicker::GetTime(SYSTEMTIME &out)
{
    return this->SendMessageW(DTM_GETSYSTEMTIME, 0, reinterpret_cast<LPARAM>(&out)) == GDT_VALID;
}

bool sw::DateTimePicker::SetTime(const SYSTEMTIME &time)
{
    bool result = this->SendMessageW(DTM_SETSYSTEMTIME, GDT_VALID, reinterpret_cast<LPARAM>(&time));
    if (result) {
        SYSTEMTIME time{};
        this->GetTime(time);
        DateTimePickerTimeChangedEventArgs arg{time};
        this->RaiseRoutedEvent(arg);
    }
    return result;
}

bool sw::DateTimePicker::SetRange(const SYSTEMTIME &minTime, const SYSTEMTIME &maxTime)
{
    SYSTEMTIME range[2] = {minTime, maxTime};
    return this->SendMessageW(DTM_SETRANGE, GDTR_MIN | GDTR_MAX, reinterpret_cast<LPARAM>(range));
}

bool sw::DateTimePicker::OnNotified(NMHDR *pNMHDR, LRESULT &result)
{
    if (pNMHDR->code == DTN_DATETIMECHANGE) {
        this->OnTimeChanged(reinterpret_cast<NMDATETIMECHANGE *>(pNMHDR));
    }
    return this->Control::OnNotified(pNMHDR, result);
}

void sw::DateTimePicker::OnTimeChanged(NMDATETIMECHANGE *pInfo)
{
    DateTimePickerTimeChangedEventArgs arg{pInfo->st};
    this->RaiseRoutedEvent(arg);
}

void sw::DateTimePicker::_SetFormat(const std::wstring &value)
{
    LPCWSTR fmt = value.empty() ? nullptr : value.c_str();
    this->SendMessageW(DTM_SETFORMATW, 0, reinterpret_cast<LPARAM>(fmt));
}

void sw::DateTimePicker::_UpdateStyle(DWORD style)
{
    SYSTEMTIME time;
    this->GetTime(time);

    SYSTEMTIME range[2];
    DWORD flag = (DWORD)this->SendMessageW(DTM_GETRANGE, 0, reinterpret_cast<LPARAM>(range));

    this->ResetHandle(style, this->GetExtendedStyle());

    this->SendMessageW(DTM_SETSYSTEMTIME, GDT_VALID, reinterpret_cast<LPARAM>(&time));
    this->SendMessageW(DTM_SETRANGE, flag, reinterpret_cast<LPARAM>(range));

    if (this->_format == DateTimePickerFormat::Custom) {
        this->_SetFormat(this->_customFormat);
    }
}

// Dip.cpp

#if !defined(USER_DEFAULT_SCREEN_DPI)
#define USER_DEFAULT_SCREEN_DPI 96
#endif

namespace
{
    /**
     * @brief 内部类，储存缩放信息
     */
    struct _ScaleInfo {
        /**
         * @brief 横向缩放比例
         */
        double scaleX;

        /**
         * @brief 纵向缩放比例
         */
        double scaleY;

        /**
         * @brief 构造函数，根据系统DPI计算缩放比例
         */
        _ScaleInfo()
        {
            HDC hdc = GetDC(NULL);
            if (hdc == NULL) {
                this->scaleX = 1;
                this->scaleY = 1;
            } else {
                this->scaleX = static_cast<double>(USER_DEFAULT_SCREEN_DPI) / GetDeviceCaps(hdc, LOGPIXELSX);
                this->scaleY = static_cast<double>(USER_DEFAULT_SCREEN_DPI) / GetDeviceCaps(hdc, LOGPIXELSY);
                ReleaseDC(NULL, hdc);
            }
        }
    };

    /**
     * @brief 储存缩放信息
     */
    _ScaleInfo _scaleInfo;
}

/*================================================================================*/

const sw::ReadOnlyProperty<double> sw::Dip::ScaleX(
    []() -> double {
        return _scaleInfo.scaleX;
    } //
);

const sw::ReadOnlyProperty<double> sw::Dip::ScaleY(
    []() -> double {
        return _scaleInfo.scaleY;
    } //
);

void sw::Dip::Update(int dpiX, int dpiY)
{
    _scaleInfo.scaleX = static_cast<double>(USER_DEFAULT_SCREEN_DPI) / dpiX;
    _scaleInfo.scaleY = static_cast<double>(USER_DEFAULT_SCREEN_DPI) / dpiY;
}

double sw::Dip::PxToDipX(int px)
{
    return px * _scaleInfo.scaleX;
}

double sw::Dip::PxToDipY(int px)
{
    return px * _scaleInfo.scaleY;
}

int sw::Dip::DipToPxX(double dip)
{
    return (int)std::lround(dip / _scaleInfo.scaleX);
}

int sw::Dip::DipToPxY(double dip)
{
    return (int)std::lround(dip / _scaleInfo.scaleY);
}

// DockLayout.cpp

static int _GetDockLayoutTag(sw::ILayout &item)
{
    auto tag = item.GetLayoutTag();
    return (tag > sw::DockLayoutTag::Bottom) ? sw::DockLayoutTag::Left : int(tag);
}

sw::DockLayoutTag::DockLayoutTag(uint64_t value)
    : _value(value)
{
}

sw::DockLayoutTag::operator uint64_t() const
{
    return this->_value;
}

bool sw::DockLayoutTag::operator==(const DockLayoutTag &other) const
{
    return this->_value == other._value;
}

bool sw::DockLayoutTag::operator!=(const DockLayoutTag &other) const
{
    return this->_value != other._value;
}

bool sw::DockLayoutTag::operator==(uint64_t value) const
{
    return this->_value == value;
}

bool sw::DockLayoutTag::operator!=(uint64_t value) const
{
    return this->_value != value;
}

sw::Size sw::DockLayout::MeasureOverride(const Size &availableSize)
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

    return desireSize;
}

void sw::DockLayout::ArrangeOverride(const Size &finalSize)
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
                double arrangeHeight = Utils::Min(itemDesireSize.height, restArea.height);

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
          [this]() -> bool {
              return this->_dockLayout.lastChildFill;
          },
          // set
          [this](const bool &value) {
              this->_dockLayout.lastChildFill = value;
              this->InvalidateMeasure();
          })
{
    this->_dockLayout.Associate(this);
    this->HorizontalAlignment = HorizontalAlignment::Stretch;
    this->VerticalAlignment   = VerticalAlignment::Stretch;
}

sw::DockLayoutTag sw::DockPanel::GetDock(UIElement &element)
{
    return DockLayoutTag{element.LayoutTag.Get()};
}

void sw::DockPanel::SetDock(UIElement &element, DockLayoutTag dock)
{
    element.LayoutTag = dock;
}

sw::LayoutHost *sw::DockPanel::GetDefaultLayout()
{
    return &this->_dockLayout;
}

// FileDialog.cpp

namespace
{
    /**
     * @brief FileDialog缓冲区默认大小
     */
    constexpr int _FileDialogInitialBufferSize = 1024;
}

sw::FileFilter::FileFilter(std::initializer_list<FileFilterItem> filters)
{
    SetFilter(filters);
}

bool sw::FileFilter::AddFilter(const std::wstring &name, const std::wstring &filter, const std::wstring &defaultExt)
{
    if (name.empty() || filter.empty()) {
        return false;
    }

    if (!_buffer.empty()) {
        _buffer.pop_back();
    }

    for (auto c : name) {
        _buffer.push_back(c);
    }
    _buffer.push_back(0);

    for (auto c : filter) {
        _buffer.push_back(c);
    }
    _buffer.push_back(0);
    _buffer.push_back(0);

    _defaultExts.emplace_back(defaultExt);
    return true;
}

int sw::FileFilter::SetFilter(std::initializer_list<FileFilterItem> filters)
{
    int result = 0;
    Clear();
    for (auto &item : filters) {
        result += AddFilter(item.name, item.filter, item.defaultExt);
    }
    return result;
}

void sw::FileFilter::Clear()
{
    _buffer.clear();
    _defaultExts.clear();
}

wchar_t *sw::FileFilter::GetFilterStr()
{
    return _buffer.empty() ? nullptr : _buffer.data();
}

const wchar_t *sw::FileFilter::GetDefaultExt(int index)
{
    return (index >= 0 && index < (int)_defaultExts.size()) ? _defaultExts[index].c_str() : L"";
}

sw::FileDialog::FileDialog()
    : BufferSize(
          // get
          [this]() -> int {
              return (int)_buffer.size();
          },
          // set
          [this](const int &value) {
              int size = Utils::Max(MAX_PATH, value);
              _buffer.resize(size);
              _ofn.lpstrFile = _buffer.data();
              _ofn.nMaxFile  = (DWORD)_buffer.size();
              ClearBuffer(); // 清空缓冲区，防止BufferSize比原来小时获取FileName访问到缓冲区外的内存
          }),

      Flags(
          // get
          [this]() -> FileDialogFlags {
              return static_cast<FileDialogFlags>(_ofn.Flags);
          },
          // set
          [this](const FileDialogFlags &value) {
              _ofn.Flags = static_cast<DWORD>(value);
          }),

      Title(
          // get
          [this]() -> std::wstring {
              return _title;
          },
          // set
          [this](const std::wstring &value) {
              _title = value;
              if (_title.empty()) {
                  _ofn.lpstrTitle = nullptr;
              } else {
                  _ofn.lpstrTitle = _title.c_str();
              }
          }),

      InitialDir(
          // get
          [this]() -> std::wstring {
              return _initialDir;
          },
          // set
          [this](const std::wstring &value) {
              _initialDir = value;
              if (_initialDir.empty()) {
                  _ofn.lpstrInitialDir = nullptr;
              } else {
                  _ofn.lpstrInitialDir = _initialDir.c_str();
              }
          }),

      Filter(
          // get
          [this]() -> FileFilter * {
              return &_filter;
          }),

      FilterIndex(
          // get
          [this]() -> int {
              return (int)_ofn.nFilterIndex - 1;
          },
          // set
          [this](const int &value) {
              _ofn.nFilterIndex = Utils::Max(0, value) + 1;
          }),

      FileName(
          // get
          [this]() -> std::wstring {
              if (!MultiSelect) {
                  std::wstring result(GetBuffer());
                  ProcessFileName(result);
                  return result;
              } else {
                  std::wstring path(GetBuffer());
                  wchar_t *pFile = GetBuffer() + path.size() + 1;
                  std::wstring result(*pFile ? Path::Combine({path, pFile}) : path);
                  ProcessFileName(result);
                  return result;
              }
          }),

      MultiSelect(
          // get
          [this]() -> bool {
              return (Flags & FileDialogFlags::AllowMultiSelect) == FileDialogFlags::AllowMultiSelect;
          },
          // set
          [this](const bool &value) {
              if (value) {
                  Flags |= FileDialogFlags::AllowMultiSelect;
              } else {
                  Flags &= ~FileDialogFlags::AllowMultiSelect;
              }
          }),

      FileNames(
          // get
          [this]() -> List<std::wstring> {
              List<std::wstring> result;

              if (!MultiSelect) {
                  auto fileName = FileName.Get();
                  if (!fileName.empty())
                      result.Append(fileName);
                  return result;
              }

              std::wstring path(GetBuffer());
              wchar_t *pFile = GetBuffer() + path.size() + 1;

              if (*pFile == 0) { // 多选状态下只选中一项时，buffer中存放的就是选择的文件路径
                  if (!path.empty()) {
                      result.Append(path);
                      ProcessFileName(result[result.Count() - 1]);
                  }
                  return result;
              }

              while (*pFile) {
                  std::wstring file = pFile;
                  result.Append(Path::Combine({path, file}));
                  ProcessFileName(result[result.Count() - 1]);
                  pFile += file.size() + 1;
              }
              return result;
          })
{
    _ofn.lStructSize       = sizeof(_ofn);
    _ofn.lpstrFileTitle    = nullptr;
    _ofn.nMaxFileTitle     = 0;
    _ofn.lpstrCustomFilter = nullptr; // 直接设置Filter即可，不提供CustomFilter支持
    _ofn.nMaxCustFilter    = 0;

    BufferSize  = _FileDialogInitialBufferSize;
    Flags       = FileDialogFlags::Explorer;
    Title       = L"";
    InitialDir  = L"";
    FilterIndex = 0;
}

void sw::FileDialog::SetFilter(const FileFilter &filter)
{
    _filter = filter;
}

void sw::FileDialog::Close()
{
}

void sw::FileDialog::Show()
{
}

OPENFILENAMEW *sw::FileDialog::GetOFN()
{
    _ofn.lpstrFilter = _filter.GetFilterStr();
    return &_ofn;
}

wchar_t *sw::FileDialog::GetBuffer()
{
    return _buffer.data();
}

void sw::FileDialog::ClearBuffer()
{
    _buffer.at(0) = 0;
    _buffer.at(1) = 0; // 两个'\0'表示结束，防止多选时FileName的意外拼接
}

void sw::FileDialog::ProcessFileName(std::wstring &fileName)
{
}

sw::OpenFileDialog::OpenFileDialog()
{
    Flags |= FileDialogFlags::PathMustExist |
             FileDialogFlags::FileMustExist;
}

int sw::OpenFileDialog::ShowDialog(Window *owner)
{
    if (owner == nullptr) {
        owner = Window::ActiveWindow;
    }

    HWND hOwner     = owner ? owner->Handle.Get() : NULL;
    auto pOFN       = GetOFN();
    pOFN->hwndOwner = hOwner;

    bool result   = false;
    DWORD errcode = 0;
    do {
        if (errcode == FNERR_BUFFERTOOSMALL) {
            BufferSize = *reinterpret_cast<uint16_t *>(GetBuffer());
        }
        ClearBuffer();
        result = GetOpenFileNameW(pOFN);
    } while (!result && ((errcode = CommDlgExtendedError()) == FNERR_BUFFERTOOSMALL));

    return result;
}

int sw::OpenFileDialog::ShowDialog(Window &owner)
{
    return ShowDialog(&owner);
}

sw::SaveFileDialog::SaveFileDialog()
    : InitialFileName(
          // get
          [this]() -> std::wstring {
              return _initialFileName;
          },
          // set
          [this](const std::wstring &value) {
              _initialFileName = value;
          })
{
    Flags |= FileDialogFlags::PathMustExist |
             FileDialogFlags::FileMustExist |
             FileDialogFlags::OverwritePrompt;
}

int sw::SaveFileDialog::ShowDialog(Window *owner)
{
    if (owner == nullptr) {
        owner = Window::ActiveWindow;
    }

    HWND hOwner     = owner ? owner->Handle.Get() : NULL;
    auto pOFN       = GetOFN();
    pOFN->hwndOwner = hOwner;

    bool result   = false;
    DWORD errcode = 0;
    do {
        if (errcode == FNERR_BUFFERTOOSMALL) {
            BufferSize = *reinterpret_cast<uint16_t *>(GetBuffer());
        }
        if (_initialFileName.empty()) {
            ClearBuffer();
        } else {
            _SetInitialFileName();
        }
        result = GetSaveFileNameW(pOFN);
    } while (!result && ((errcode = CommDlgExtendedError()) == FNERR_BUFFERTOOSMALL));

    if (!result) {
        ClearBuffer();
    }
    return result;
}

int sw::SaveFileDialog::ShowDialog(Window &owner)
{
    return ShowDialog(&owner);
}

void sw::SaveFileDialog::ProcessFileName(std::wstring &fileName)
{
    const wchar_t *ext = Filter->GetDefaultExt(FilterIndex);
    if (ext == nullptr || ext[0] == L'\0') return;

    size_t indexSlash = fileName.find_last_of(L"\\/");
    size_t indexDot   = fileName.find_last_of(L'.');

    static const auto npos = std::wstring::npos;
    if (indexDot == npos || (indexSlash != npos && indexSlash > indexDot)) {
        fileName += ext;
    }
}

void sw::SaveFileDialog::_SetInitialFileName()
{
    auto &str = _initialFileName;
    int size  = (int)str.size();

    if (BufferSize < size + 2) {
        BufferSize = size + 2;
    }

    wchar_t *buffer = GetBuffer();
    memcpy(buffer, &str[0], sizeof(wchar_t) * size);
    buffer[size] = buffer[size + 1] = 0;
}

// FillLayout.cpp

sw::Size sw::FillLayout::MeasureOverride(const Size &availableSize)
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
    return desireSize;
}

void sw::FillLayout::ArrangeOverride(const Size &finalSize)
{
    int count = this->GetChildLayoutCount();
    for (int i = 0; i < count; ++i) {
        ILayout &item = this->GetChildLayoutAt(i);
        item.Arrange(Rect{0, 0, finalSize.width, finalSize.height});
    }
}

// FolderDialog.cpp

namespace
{
    /**
     * @brief FolderBrowserDialog缓冲区默认大小
     */
    constexpr int _FolderBrowserDialogInitialBufferSize = MAX_PATH;
}

sw::FolderBrowserDialog::FolderBrowserDialog()
    : BufferSize(
          // get
          [this]() -> int {
              return (int)_buffer.size();
          },
          // set
          [this](const int &value) {
              _buffer.resize(Utils::Max(MAX_PATH, value));
              ClearBuffer();
          }),

      Flags(
          // get
          [this]() -> FolderDialogFlags {
              return static_cast<FolderDialogFlags>(_bi.ulFlags);
          },
          // set
          [this](const FolderDialogFlags &value) {
              _bi.ulFlags = static_cast<UINT>(value);
          }),

      Description(
          // get
          [this]() -> std::wstring {
              return _description;
          },
          // set
          [this](const std::wstring &value) {
              _description = value;
              if (_description.empty()) {
                  _bi.lpszTitle = nullptr;
              } else {
                  _bi.lpszTitle = _description.c_str();
              }
          }),

      SelectedPath(
          // get
          [this]() -> std::wstring {
              return GetBuffer();
          }),

      NewFolderButton(
          // get
          [this]() -> bool {
              return !((Flags & FolderDialogFlags::NoNewFolderButton) == FolderDialogFlags::NoNewFolderButton);
          },
          // set
          [this](const bool &value) {
              if (value) {
                  Flags &= ~FolderDialogFlags::NoNewFolderButton;
              } else {
                  Flags |= FolderDialogFlags::NoNewFolderButton;
              }
          })
{
    BufferSize  = _FolderBrowserDialogInitialBufferSize;
    Flags       = FolderDialogFlags::NewDialogStyle | FolderDialogFlags::ReturnOnlyFileSystemDirs;
    Description = L"";
}

void sw::FolderBrowserDialog::Close()
{
}

void sw::FolderBrowserDialog::Show()
{
}

int sw::FolderBrowserDialog::ShowDialog(Window *owner)
{
    if (owner == nullptr) {
        owner = Window::ActiveWindow;
    }

    HWND hOwner    = owner ? owner->Handle.Get() : NULL;
    auto pBI       = GetBI();
    pBI->hwndOwner = hOwner;

    auto pidl = SHBrowseForFolderW(pBI);
    if (pidl == nullptr) return false;

    bool result = SHGetPathFromIDListW(pidl, GetBuffer());
    CoTaskMemFree(pidl);
    return result;
}

int sw::FolderBrowserDialog::ShowDialog(Window &owner)
{
    return ShowDialog(&owner);
}

BROWSEINFOW *sw::FolderBrowserDialog::GetBI()
{
    return &_bi;
}

wchar_t *sw::FolderBrowserDialog::GetBuffer()
{
    return _buffer.data();
}

void sw::FolderBrowserDialog::ClearBuffer()
{
    _buffer.at(0) = 0;
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

HFONT sw::Font::CreateHandle() const
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
    static auto getter = []() -> Font {
        NONCLIENTMETRICSW ncm{};
        ncm.cbSize   = sizeof(ncm);
        bool success = SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0);
        return success ? static_cast<Font>(ncm.lfMessageFont) : Font{};
    };

    static thread_local Font font = getter();
    if (update) font = getter();

    return font;
}

// FontDialog.cpp

sw::FontDialog::FontDialog()
    : Flags(
          // get
          [this]() -> FontDialogFlags {
              return static_cast<FontDialogFlags>(_cf.Flags);
          },
          // set
          [this](const FontDialogFlags &value) {
              _cf.Flags = static_cast<DWORD>(value);
          }),

      Font(
          // get
          [this]() -> sw::Font {
              return _font;
          },
          // set
          [this](const sw::Font &value) {
              _font = value;
          }),

      FontName(
          // get
          [this]() -> std::wstring {
              return _font.name;
          },
          // set
          [this](const std::wstring &value) {
              _font.name = value;
          }),

      FontSize(
          // get
          [this]() -> double {
              return _font.size;
          },
          // set
          [this](const double &value) {
              _font.size = value;
          }),

      FontWeight(
          // get
          [this]() -> sw::FontWeight {
              return _font.weight;
          },
          // set
          [this](const sw::FontWeight &value) {
              _font.weight = value;
          }),

      ShowEffects(
          // get
          [this]() -> bool {
              return (Flags & FontDialogFlags::Effects) == FontDialogFlags::Effects;
          },
          // set
          [this](const bool &value) {
              if (value) {
                  Flags |= FontDialogFlags::Effects;
              } else {
                  Flags &= ~FontDialogFlags::Effects;
              }
          }),

      SelectedColor(
          // get
          [this]() -> Color {
              return _cf.rgbColors;
          },
          // set
          [this](const Color &value) {
              _cf.rgbColors = value;
          })
{
    _font           = Font::GetDefaultFont();
    _cf.lStructSize = sizeof(CHOOSEFONTW);
    _cf.Flags       = DWORD(FontDialogFlags::InitToLogFontStruct | FontDialogFlags::Effects);
}

void sw::FontDialog::Close()
{
}

void sw::FontDialog::Show()
{
}

int sw::FontDialog::ShowDialog(Window *owner)
{
    int result = 0;

    if (owner == nullptr) {
        owner = Window::ActiveWindow;
    }

    HWND hOwner    = owner ? owner->Handle.Get() : NULL;
    auto pCF       = GetChooseFontStruct();
    pCF->hwndOwner = hOwner;

    LOGFONTW logFont = _font;
    pCF->lpLogFont   = &logFont;

    result = ChooseFontW(pCF);
    if (result) _font = logFont;

    pCF->lpLogFont = nullptr;
    return result;
}

int sw::FontDialog::ShowDialog(Window &owner)
{
    return ShowDialog(&owner);
}

CHOOSEFONTW *sw::FontDialog::GetChooseFontStruct()
{
    return &_cf;
}

// Grid.cpp

sw::Grid::Grid()
{
    this->_gridLayout.Associate(this);
    this->HorizontalAlignment = HorizontalAlignment::Stretch;
    this->VerticalAlignment   = VerticalAlignment::Stretch;
}

void sw::Grid::AddRow(const GridRow &row)
{
    this->_gridLayout.rows.Append(row);
    this->InvalidateMeasure();
}

void sw::Grid::SetRows(std::initializer_list<GridRow> rows)
{
    List<GridRow> rowsList = rows;
    this->_gridLayout.rows = rowsList;
    this->InvalidateMeasure();
}

void sw::Grid::AddColumn(const GridColumn &col)
{
    this->_gridLayout.columns.Append(col);
    this->InvalidateMeasure();
}

void sw::Grid::SetColumns(std::initializer_list<GridColumn> cols)
{
    List<GridColumn> colsList = cols;
    this->_gridLayout.columns = colsList;
    this->InvalidateMeasure();
}

void sw::Grid::ClearRows()
{
    this->_gridLayout.rows.Clear();
    this->InvalidateMeasure();
}

void sw::Grid::ClearColumns()
{
    this->_gridLayout.columns.Clear();
    this->InvalidateMeasure();
}

sw::GridLayoutTag sw::Grid::GetGridLayoutTag(UIElement &element)
{
    return element.LayoutTag.Get();
}

void sw::Grid::SetGridLayoutTag(UIElement &element, const GridLayoutTag &tag)
{
    element.LayoutTag.Set(tag);
}

sw::LayoutHost *sw::Grid::GetDefaultLayout()
{
    return &this->_gridLayout;
}

// GridLayout.cpp

sw::GridLayoutTag::GridLayoutTag()
    : row(0), column(0), rowSpan(1), columnSpan(1)
{
}

sw::GridLayoutTag::GridLayoutTag(uint16_t row, uint16_t column, uint16_t rowSpan, uint16_t columnSpan)
    : row(row), column(column), rowSpan(rowSpan), columnSpan(columnSpan)
{
}

sw::GridLayoutTag::GridLayoutTag(uint16_t row, uint16_t column)
    : row(row), column(column), rowSpan(1), columnSpan(1)
{
}

sw::GridLayoutTag::GridLayoutTag(uint64_t layoutTag)
    : row((layoutTag >> 0) & 0xffff), column((layoutTag >> 16) & 0xffff),
      rowSpan((layoutTag >> 32) & 0xffff), columnSpan((layoutTag >> 48) & 0xffff)
{
}

sw::GridLayoutTag::operator uint64_t() const
{
    return ((uint64_t)(this->row) << 0) |
           ((uint64_t)(this->column) << 16) |
           ((uint64_t)(this->rowSpan) << 32) |
           ((uint64_t)(this->columnSpan) << 48);
}

sw::GridRow::GridRow()
    : type(GridRCType::FillRemain), height(1)
{
}

sw::GridRow::GridRow(GridRCType type, double height)
    : type(type), height(height)
{
}

sw::GridRow::GridRow(double height)
    : type(GridRCType::FixSize), height(height)
{
}

sw::FixSizeGridRow::FixSizeGridRow(double height)
    : GridRow(GridRCType::FixSize, height)
{
}

sw::AutoSizeGridRow::AutoSizeGridRow()
    : GridRow(GridRCType::AutoSize, 0)
{
}

sw::FillRemainGridRow::FillRemainGridRow(double proportion)
    : GridRow(GridRCType::FillRemain, proportion)
{
}

sw::GridColumn::GridColumn()
    : type(GridRCType::FillRemain), width(1)
{
}

sw::GridColumn::GridColumn(GridRCType type, double width)
    : type(type), width(width)
{
}

sw::GridColumn::GridColumn(double width)
    : type(GridRCType::FixSize), width(width)
{
}

sw::FixSizeGridColumn::FixSizeGridColumn(double width)
    : GridColumn(GridRCType::FixSize, width)
{
}

sw::AutoSizeGridColumn::AutoSizeGridColumn()
    : GridColumn(GridRCType::AutoSize, 0)
{
}

sw::FillRemainGridColumn::FillRemainGridColumn(double proportion)
    : GridColumn(GridRCType::FillRemain, proportion)
{
}

sw::Size sw::GridLayout::MeasureOverride(const Size &availableSize)
{
    Size desireSize{};
    this->_UpdateInternalData();

    int rowCount   = (int)this->_internalData.rowsInfo.size();
    int colCount   = (int)this->_internalData.colsInfo.size();
    int childCount = (int)this->_internalData.childrenInfo.size();

    bool widthSizeToContent  = std::isinf(availableSize.width);
    bool heightSizeToContent = std::isinf(availableSize.height);

    // 子元素为空，可以直接计算DesireSize
    // 若无类型为FillRemain的行/列，对固定大小的行/列求和即可
    // 若有类型为FillRemain的行/列，且大小不是由内容决定时，铺满availableSize
    if (childCount == 0) {
        bool hasFillRemainCols = false;
        bool hasFillRemainRows = false;
        for (_ColInfo &colInfo : this->_internalData.colsInfo) {
            if (colInfo.col.type == GridRCType::FixSize)
                desireSize.width += colInfo.size;
            else if (colInfo.col.type == GridRCType::FillRemain && colInfo.proportion != 0)
                hasFillRemainCols = true;
        }
        for (_RowInfo &rowInfo : this->_internalData.rowsInfo) {
            if (rowInfo.row.type == GridRCType::FixSize)
                desireSize.height += rowInfo.size;
            else if (rowInfo.row.type == GridRCType::FillRemain && rowInfo.proportion != 0)
                hasFillRemainRows = true;
        }
        if (!widthSizeToContent && hasFillRemainCols)
            desireSize.width = Utils::Max(availableSize.width, desireSize.width);
        if (!heightSizeToContent && hasFillRemainRows)
            desireSize.height = Utils::Max(availableSize.height, desireSize.height);
        return desireSize;
    }

    // Measure列

    // 对childrenInfo进行排序
    // 优先级 FixSize > AutoSize > FillRemain
    // 对于优先级相同的则按照columnSpan从小到大排序
    std::sort(
        this->_internalData.childrenInfo.begin(),
        this->_internalData.childrenInfo.end(),
        [](const _ChildInfo &a, const _ChildInfo &b) -> bool {
            if (a.colMeasureType != b.colMeasureType) {
                // 先FixSize，后AutoSize，最后FillRemain
                return a.colMeasureType < b.colMeasureType;
            } else {
                // 若类型相同，则按照跨列数从小到大measure
                return a.layoutTag.columnSpan < b.layoutTag.columnSpan;
            }
        });

    for (_ChildInfo &childInfo : this->_internalData.childrenInfo) {
        bool breakFlag = false; // 标记是否退出循环

        switch (childInfo.colMeasureType) {
            case GridRCType::FixSize: {
                // 第一次measure目的是确定列宽度，因此FixSize列的元素没必要在此时measure
                break;
            }

            case GridRCType::AutoSize: {
                childInfo.instance->Measure(Size(INFINITY, INFINITY));
                Size childDesireSize = childInfo.instance->GetDesireSize();

                if (childInfo.layoutTag.columnSpan <= 1) {
                    // 若ColumnSpan为1，直接更新对应列的尺寸
                    this->_internalData.colsInfo[childInfo.layoutTag.column].size =
                        Utils::Max(this->_internalData.colsInfo[childInfo.layoutTag.column].size, childDesireSize.width);
                } else {
                    // 若ColumnSpan不为1，则对所跨类型为AutoSize的列均匀贡献
                    int count  = 0;
                    double sum = 0;

                    for (int i = 0; i < childInfo.layoutTag.columnSpan; ++i) {
                        _ColInfo &colInfo =
                            this->_internalData.colsInfo[childInfo.layoutTag.column + i];
                        count += colInfo.col.type == GridRCType::AutoSize;
                        sum += colInfo.size;
                    }

                    if (sum < childDesireSize.width) {
                        double tmp =
                            (childDesireSize.width - sum) / count;
                        for (int i = 0; i < childInfo.layoutTag.columnSpan; ++i) {
                            _ColInfo &colInfo =
                                this->_internalData.colsInfo[childInfo.layoutTag.column + i];
                            if (colInfo.col.type == GridRCType::AutoSize) {
                                colInfo.size += tmp;
                            }
                        }
                    }
                }
                break;
            }

            case GridRCType::FillRemain: {
                if (!widthSizeToContent) {
                    // 若宽度不是由内容确定（availableSize.width为inf），则类型为FillRemain的列宽与内容无关
                    // 此时可以直接根据剩余的大小计算，直接退出循环
                    breakFlag = true;
                    break;
                }

                // 宽度由内容决定，对元素进行measure，以占空间最大的元素为基准分配大小
                childInfo.instance->Measure(Size(INFINITY, INFINITY));
                Size childDesireSize = childInfo.instance->GetDesireSize();

                if (childInfo.layoutTag.columnSpan <= 1) {
                    // 若ColumnSpan为1，直接更新对应列的尺寸
                    this->_internalData.colsInfo[childInfo.layoutTag.column].size =
                        Utils::Max(this->_internalData.colsInfo[childInfo.layoutTag.column].size, childDesireSize.width);
                } else {
                    // 若ColumnSpan不为1，则对所跨类型为FillRemain的列均匀贡献
                    int count  = 0;
                    double sum = 0;

                    for (int i = 0; i < childInfo.layoutTag.columnSpan; ++i) {
                        _ColInfo &colInfo =
                            this->_internalData.colsInfo[childInfo.layoutTag.column + i];
                        count += colInfo.col.type == GridRCType::FillRemain;
                        sum += colInfo.size;
                    }

                    if (sum < childDesireSize.width) {
                        double tmp =
                            (childDesireSize.width - sum) / count;
                        for (int i = 0; i < childInfo.layoutTag.columnSpan; ++i) {
                            _ColInfo &colInfo =
                                this->_internalData.colsInfo[childInfo.layoutTag.column + i];
                            if (colInfo.col.type == GridRCType::FillRemain) {
                                colInfo.size += tmp;
                            }
                        }
                    }
                }
                break;
            }
        }

        if (breakFlag) {
            break;
        }
    }

    if (!widthSizeToContent) {
        // 宽度不是由内容决定时计算类型为FillRemain的列的宽度
        double remainWidth = availableSize.width;
        for (_ColInfo &colInfo : this->_internalData.colsInfo) {
            if (colInfo.col.type != GridRCType::FillRemain) {
                remainWidth -= colInfo.size;
            }
        }
        if (remainWidth < 0) {
            remainWidth = 0;
        }
        for (_ColInfo &colInfo : this->_internalData.colsInfo) {
            if (colInfo.col.type == GridRCType::FillRemain) {
                colInfo.size = remainWidth * colInfo.proportion;
            }
        }
    } else {
        // 宽度由内容决定，依据所占宽度最大元素为基准计算列宽度
        _ColInfo *maxWidthCol = nullptr;
        for (_ColInfo &colInfo : this->_internalData.colsInfo) {
            if (colInfo.col.type == GridRCType::FillRemain &&
                (maxWidthCol == nullptr || colInfo.size > maxWidthCol->size)) {
                maxWidthCol = &colInfo;
            }
        }
        if (maxWidthCol != nullptr) {
            double tmp =
                maxWidthCol->size / maxWidthCol->proportion;
            for (_ColInfo &colInfo : this->_internalData.colsInfo) {
                if (colInfo.col.type == GridRCType::FillRemain) {
                    colInfo.size = tmp * colInfo.proportion;
                }
            }
        }
    }

    // Measure行

    // 对childrenInfo进行排序
    // 优先级 FixSize > AutoSize > FillRemain
    // 对于优先级相同的则按照rowSpan从小到大排序
    std::sort(
        this->_internalData.childrenInfo.begin(),
        this->_internalData.childrenInfo.end(),
        [](const _ChildInfo &a, const _ChildInfo &b) -> bool {
            if (a.rowMeasureType != b.rowMeasureType) {
                // 先FixSize，后AutoSize，最后FillRemain
                return a.rowMeasureType < b.rowMeasureType;
            } else {
                // 若类型相同，则按照跨列数从小到大measure
                return a.layoutTag.rowSpan < b.layoutTag.rowSpan;
            }
        });

    int measureIndex = 0;

    // Measure行类型为FixSize的元素
    while (measureIndex < childCount &&
           this->_internalData.childrenInfo[measureIndex].rowMeasureType == GridRCType::FixSize) {
        _ChildInfo &childInfo = this->_internalData.childrenInfo[measureIndex++];

        Size measureSize{};
        for (int i = 0; i < childInfo.layoutTag.columnSpan; ++i)
            measureSize.width += this->_internalData.colsInfo[childInfo.layoutTag.column + i].size;
        for (int i = 0; i < childInfo.layoutTag.rowSpan; ++i)
            measureSize.height += this->_internalData.rowsInfo[childInfo.layoutTag.row + i].size;
        childInfo.instance->Measure(measureSize);
    }

    // Measure行类型为AutoSize的元素
    while (measureIndex < childCount &&
           this->_internalData.childrenInfo[measureIndex].rowMeasureType == GridRCType::AutoSize) {
        _ChildInfo &childInfo = this->_internalData.childrenInfo[measureIndex++];

        Size measureSize{0, INFINITY};
        for (int i = 0; i < childInfo.layoutTag.columnSpan; ++i) {
            measureSize.width += this->_internalData.colsInfo[childInfo.layoutTag.column + i].size;
        }

        childInfo.instance->Measure(measureSize);
        Size childDesireSize = childInfo.instance->GetDesireSize();

        if (childInfo.layoutTag.rowSpan <= 1) {
            // 若RowSpan为1，直接更新对应行的尺寸
            this->_internalData.rowsInfo[childInfo.layoutTag.row].size =
                Utils::Max(this->_internalData.rowsInfo[childInfo.layoutTag.row].size, childDesireSize.height);
        } else {
            // 若RowSpan不为1，则对所跨类型为AutoSize的行均匀贡献
            int count  = 0;
            double sum = 0;

            for (int i = 0; i < childInfo.layoutTag.rowSpan; ++i) {
                _RowInfo &rowInfo =
                    this->_internalData.rowsInfo[childInfo.layoutTag.row + i];
                count += rowInfo.row.type == GridRCType::AutoSize;
                sum += rowInfo.size;
            }

            if (sum < childDesireSize.height) {
                double tmp =
                    (childDesireSize.height - sum) / count;
                for (int i = 0; i < childInfo.layoutTag.rowSpan; ++i) {
                    _RowInfo &rowInfo =
                        this->_internalData.rowsInfo[childInfo.layoutTag.row + i];
                    if (rowInfo.row.type == GridRCType::AutoSize) {
                        rowInfo.size += tmp;
                    }
                }
            }
        }
    }

    // Measure行类型为FillRemain的元素
    if (heightSizeToContent) {
        // 高度由内容决定，依据所占宽度最大元素为基准计算列宽度
        while (measureIndex < childCount) {
            _ChildInfo &childInfo = this->_internalData.childrenInfo[measureIndex++];

            Size measureSize{0, INFINITY};
            for (int i = 0; i < childInfo.layoutTag.columnSpan; ++i)
                measureSize.width += this->_internalData.colsInfo[childInfo.layoutTag.column + i].size;

            childInfo.instance->Measure(measureSize);
            Size childDesireSize = childInfo.instance->GetDesireSize();

            if (childInfo.layoutTag.rowSpan <= 1) {
                // 若RowSpan为1，直接更新对应列的尺寸
                this->_internalData.rowsInfo[childInfo.layoutTag.row].size =
                    Utils::Max(this->_internalData.rowsInfo[childInfo.layoutTag.row].size, childDesireSize.height);
            } else {
                // 若RowSpan不为1，则对所跨类型为FillRemain的列均匀贡献
                int count  = 0;
                double sum = 0;

                for (int i = 0; i < childInfo.layoutTag.rowSpan; ++i) {
                    _RowInfo &rowInfo =
                        this->_internalData.rowsInfo[childInfo.layoutTag.row + i];
                    count += rowInfo.row.type == GridRCType::FillRemain;
                    sum += rowInfo.size;
                }

                if (sum < childDesireSize.height) {
                    double tmp =
                        (childDesireSize.height - sum) / count;
                    for (int i = 0; i < childInfo.layoutTag.rowSpan; ++i) {
                        _RowInfo &rowInfo =
                            this->_internalData.rowsInfo[childInfo.layoutTag.row + i];
                        if (rowInfo.row.type == GridRCType::FillRemain) {
                            rowInfo.size += tmp;
                        }
                    }
                }
            }
        }

        // 高度由内容决定，依据所占高度最大元素为基准计算行高度
        _RowInfo *maxHeightRow = nullptr;
        for (_RowInfo &rowInfo : this->_internalData.rowsInfo) {
            if (rowInfo.row.type == GridRCType::FillRemain &&
                (maxHeightRow == nullptr || rowInfo.size > maxHeightRow->size)) {
                maxHeightRow = &rowInfo;
            }
        }
        if (maxHeightRow != nullptr) {
            double tmp =
                maxHeightRow->size / maxHeightRow->proportion;
            for (_RowInfo &rowInfo : this->_internalData.rowsInfo) {
                if (rowInfo.row.type == GridRCType::FillRemain) {
                    rowInfo.size = tmp * rowInfo.proportion;
                }
            }
        }

    } else {
        // 高度不是由内容决定时计算类型为FillRemain的列的宽度
        double remainHeight = availableSize.height;
        for (_RowInfo &rowInfo : this->_internalData.rowsInfo) {
            if (rowInfo.row.type != GridRCType::FillRemain) {
                remainHeight -= rowInfo.size;
            }
        }
        if (remainHeight < 0) {
            remainHeight = 0;
        }
        for (_RowInfo &rowInfo : this->_internalData.rowsInfo) {
            if (rowInfo.row.type == GridRCType::FillRemain) {
                rowInfo.size = remainHeight * rowInfo.proportion;
            }
        }
        // Measure
        while (measureIndex < childCount) {
            _ChildInfo &childInfo = this->_internalData.childrenInfo[measureIndex++];

            Size measureSize{};
            for (int i = 0; i < childInfo.layoutTag.columnSpan; ++i)
                measureSize.width += this->_internalData.colsInfo[childInfo.layoutTag.column + i].size;
            for (int i = 0; i < childInfo.layoutTag.rowSpan; ++i)
                measureSize.height += this->_internalData.rowsInfo[childInfo.layoutTag.row + i].size;
            childInfo.instance->Measure(measureSize);
        }
    }

    // 更新网格信息
    {
        double left = 0;
        double top  = 0;

        for (int i = 0; i < rowCount; ++i) {
            _RowInfo &rowInfo = this->_internalData.rowsInfo[i];
            for (int j = 0; j < colCount; ++j) {
                _ColInfo &colInfo = this->_internalData.colsInfo[j];
                Rect &cell        = this->_GetCell(i, j);
                cell.left         = left;
                cell.top          = top;
                cell.width        = colInfo.size;
                cell.height       = rowInfo.size;
                left += colInfo.size;
            }
            left = 0;
            top += rowInfo.size;
        }
    }

    // 设置DesireSize
    for (_ColInfo &colInfo : this->_internalData.colsInfo)
        desireSize.width += colInfo.size;
    for (_RowInfo &rowInfo : this->_internalData.rowsInfo)
        desireSize.height += rowInfo.size;
    return desireSize;
}

void sw::GridLayout::ArrangeOverride(const Size &finalSize)
{
    int childCount = (int)this->_internalData.childrenInfo.size();
    if (childCount == 0) return;

    for (_ChildInfo &childInfo : this->_internalData.childrenInfo) {
        int row = childInfo.layoutTag.row;
        int col = childInfo.layoutTag.column;

        Rect arrangeRect = this->_GetCell(row, col);
        for (int i = 1; i < childInfo.layoutTag.columnSpan; ++i)
            arrangeRect.width += this->_GetCell(row, col + i).width;
        for (int i = 1; i < childInfo.layoutTag.rowSpan; ++i)
            arrangeRect.height += this->_GetCell(row + i, col).height;

        childInfo.instance->Arrange(arrangeRect);
    }
}

void sw::GridLayout::_UpdateInternalData()
{
    this->_internalData.rowsInfo.clear();
    this->_internalData.colsInfo.clear();
    this->_internalData.childrenInfo.clear();

    // rowsInfo
    {
        _RowInfo info;

        if (this->rows.IsEmpty()) {
            info.proportion = 1;
            this->_internalData.rowsInfo.emplace_back(info);
        } else {
            int count  = 0; // 类型为FillRemain的行数
            double sum = 0; // FillRemain行的高度求和

            for (GridRow &row : this->rows) {
                info.row = row;
                // 防止值小于0
                if (info.row.height < 0) {
                    info.row.height = 0;
                }
                if (info.row.type == GridRCType::FixSize) {
                    // 若类型为FixSize，此时即可确定行高
                    info.size = info.row.height;
                } else {
                    // 其他类型此时无法确定行高
                    info.size = 0;
                }
                // 记录FillRemain类型行的信息，用于计算proportion
                if (info.row.type == GridRCType::FillRemain) {
                    ++count;
                    sum += info.row.height;
                }
                this->_internalData.rowsInfo.emplace_back(info);
            }

            // 设置proportion字段
            if (count && sum > 0) {
                for (_RowInfo &rowInfo : this->_internalData.rowsInfo) {
                    if (rowInfo.row.type == GridRCType::FillRemain) {
                        rowInfo.proportion = rowInfo.row.height / sum;
                    }
                }
            }
        }
    }

    // colsInfo
    {
        _ColInfo info;

        if (this->columns.IsEmpty()) {
            info.proportion = 1;
            this->_internalData.colsInfo.emplace_back(info);
        } else {
            int count  = 0; // 类型为FillRemain的列数
            double sum = 0; // FillRemain列的高度求和

            for (GridColumn &col : this->columns) {
                info.col = col;
                // 防止值小于0
                if (info.col.width < 0) {
                    info.col.width = 0;
                }
                if (info.col.type == GridRCType::FixSize) {
                    // 若类型为FixSize，此时即可确定列宽
                    info.size = info.col.width;
                } else {
                    // 其他类型此时无法确定列宽
                    info.size = 0;
                }
                // 记录FillRemain类型列的信息，用于计算proportion
                if (info.col.type == GridRCType::FillRemain) {
                    ++count;
                    sum += info.col.width;
                }
                this->_internalData.colsInfo.emplace_back(info);
            }

            // 设置proportion字段
            if (count && sum > 0) {
                for (_ColInfo &colInfo : this->_internalData.colsInfo) {
                    if (colInfo.col.type == GridRCType::FillRemain) {
                        colInfo.proportion = colInfo.col.width / sum;
                    }
                }
            }
        }
    }

    // childrenInfo
    {
        int rowCount   = (int)this->_internalData.rowsInfo.size();
        int colCount   = (int)this->_internalData.colsInfo.size();
        int childCount = this->GetChildLayoutCount();

        _ChildInfo info;

        for (int i = 0; i < childCount; ++i) {
            ILayout &item     = this->GetChildLayoutAt(i);
            GridLayoutTag tag = item.GetLayoutTag();

            // 确保row和column的值不会超过网格的大小
            // 由于类型是uint16_t，不存在值小于0的情况
            tag.row    = Utils::Min<uint16_t>(rowCount - 1, tag.row);
            tag.column = Utils::Min<uint16_t>(colCount - 1, tag.column);

            // 确保rowSpan的值合理
            if (tag.rowSpan == 0) {
                tag.rowSpan = 1;
            } else if (tag.row + tag.rowSpan > rowCount) {
                tag.rowSpan = rowCount - tag.row;
            }
            // 确保colSpan的值合理
            if (tag.columnSpan == 0) {
                tag.columnSpan = 1;
            } else if (tag.column + tag.columnSpan > colCount) {
                tag.columnSpan = colCount - tag.column;
            }

            // 子元素在measure时的行列类型，这个值受其所在的位置影响
            // 若span为1，则该值与其所在的行/列的类型相同，否则由所跨所有的行/列共同决定：
            // - 若所有行/列均为FixSize，则为FixSize
            // - 若行/列中存在AutoSize，且没有FillRemain，则为AutoSize
            // - 若行/列中存在FillRemain，则为FillRemain
            GridRCType rowMeasureType = this->_internalData.rowsInfo[tag.row].row.type;
            GridRCType colMeasureType = this->_internalData.colsInfo[tag.column].col.type;

            // 处理跨多行情况下的rowMeasureType
            for (int j = 1; j < tag.rowSpan && rowMeasureType != GridRCType::FillRemain; ++j) {
                GridRCType type =
                    this->_internalData.rowsInfo[tag.row + j].row.type;
                switch (type) {
                    case GridRCType::FixSize: {
                        break; // nothing
                    }
                    case GridRCType::AutoSize:
                    case GridRCType::FillRemain: {
                        rowMeasureType = type;
                    }
                }
            }
            // 处理跨多列情况下的colMeasureType
            for (int k = 1; k < tag.columnSpan && colMeasureType != GridRCType::FillRemain; ++k) {
                GridRCType type =
                    this->_internalData.colsInfo[tag.column + k].col.type;
                switch (type) {
                    case GridRCType::FixSize: {
                        break; // nothing
                    }
                    case GridRCType::AutoSize:
                    case GridRCType::FillRemain: {
                        colMeasureType = type;
                    }
                }
            }

            info.instance       = &item;
            info.layoutTag      = tag;
            info.rowMeasureType = rowMeasureType;
            info.colMeasureType = colMeasureType;

            this->_internalData.childrenInfo.emplace_back(info);
        }
    }

    // cells
    {
        this->_internalData.cells.resize(this->_internalData.rowsInfo.size() * this->_internalData.colsInfo.size());
    }
}

sw::Rect &sw::GridLayout::_GetCell(int row, int col)
{
    return this->_internalData.cells[this->_internalData.colsInfo.size() * row + col];
}

// GroupBox.cpp

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

        std::wstring &text = GetInternalText();
        DrawTextW(hdc, text.c_str(), (int)text.size(), &rtHeaderText, DT_SINGLELINE);

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
    std::wstring &text = GetInternalText();
    DrawTextW(hdc, text.c_str(), (int)text.size(), &rect, DT_SINGLELINE | DT_CALCRECT);

    _textSize = {rect.right - rect.left, rect.bottom - rect.top};
    ReleaseDC(hwnd, hdc);
}

// HotKeyControl.cpp

sw::HotKeyControl::HotKeyControl()
    : Value(
          // get
          [this]() -> HotKey {
              return this->_value;
          },
          // set
          [this](const HotKey &value) {
              if (value.key != this->_value.key && value.modifier != this->_value.modifier) {
                  WORD val = MAKEWORD(value.key, value.modifier);
                  this->SendMessageW(HKM_SETHOTKEY, val, 0);
                  this->_UpdateValue();
                  this->OnValueChanged(this->_value);
              }
          })
{
    this->InitControl(HOTKEY_CLASSW, L"", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, 0);
    this->_UpdateValue();
    this->Rect = {0, 0, 100, 24};
}

void sw::HotKeyControl::SetRules(HotKeyCombination invalidComb, HotKeyModifier defaultModifier)
{
    this->SendMessageW(HKM_SETRULES, static_cast<WPARAM>(invalidComb), static_cast<LPARAM>(defaultModifier));
}

void sw::HotKeyControl::OnCommand(int code)
{
    if (code == EN_CHANGE) {
        this->_UpdateValue();
        this->OnValueChanged(this->_value);
    }
}

void sw::HotKeyControl::OnValueChanged(HotKey value)
{
    HotKeyValueChangedEventArgs arg{value.key, value.modifier};
    this->RaiseRoutedEvent(arg);
}

void sw::HotKeyControl::_UpdateValue()
{
    WORD val     = LOWORD(this->SendMessageW(HKM_GETHOTKEY, 0, 0));
    this->_value = {/*key*/ VirtualKey(LOBYTE(val)), /*modifier*/ HotKeyModifier(HIBYTE(val))};
}

// HwndHost.cpp

sw::HwndHost::HwndHost()
    : FillContent(
          // get
          [this]() -> bool {
              return this->_fillContent;
          },
          // set
          [this](const bool &value) {
              this->_fillContent = value;
          })
{
    this->Rect = sw::Rect{0, 0, 100, 100};
}

void sw::HwndHost::InitHwndHost()
{
    if (this->_hWindowCore == NULL && !this->IsDestroyed)
        this->_hWindowCore = this->BuildWindowCore(this->Handle);
}

bool sw::HwndHost::OnSize(Size newClientSize)
{
    if (this->_hWindowCore != NULL && this->_fillContent) {
        SetWindowPos(this->_hWindowCore, NULL, 0, 0,
                     Dip::DipToPxX(newClientSize.width),
                     Dip::DipToPxY(newClientSize.height),
                     SWP_NOACTIVATE | SWP_NOZORDER);
    }
    return this->StaticControl::OnSize(newClientSize);
}

bool sw::HwndHost::OnDestroy()
{
    this->DestroyWindowCore(this->_hWindowCore);
    this->_hWindowCore = NULL;
    return this->StaticControl::OnDestroy();
}

// HwndWrapper.cpp

void sw::HwndWrapper::InitHwndWrapper()
{
    if (this->_hwnd != NULL) {
        return;
    }

    bool isControl = false;

    this->_hwnd = this->BuildWindowCore(
        isControl, WndBase::_NextControlId());

    this->_isControl = isControl;
    WndBase::_SetWndBase(this->_hwnd, *this);

    this->_originalWndProc = reinterpret_cast<WNDPROC>(
        SetWindowLongPtrW(this->_hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndBase::_WndProc)));

    if (this->_originalWndProc == WndBase::_WndProc) {
        this->_originalWndProc = nullptr; // 防止无限递归
    }

    RECT rect;
    GetWindowRect(this->_hwnd, &rect);
    this->_rect = rect;

    this->UpdateInternalText();
    this->HandleInitialized(this->_hwnd);
    this->UpdateFont();

    if (this->_isControl) {
        WndBase::_InitControlContainer();
        this->WndBase::SetParent(nullptr);
    }
}

// IPAddressControl.cpp

sw::IPAddressControl::IPAddressControl()
    : IPAddressControl(sw::Size{150, 24})
{
}

sw::IPAddressControl::IPAddressControl(sw::Size size)
    : IsBlank(
          // get
          [this]() -> bool {
              return ::SendMessageW(this->_hIPAddrCtrl, IPM_ISBLANK, 0, 0);
          }),

      Address(
          // get
          [this]() -> uint32_t {
              uint32_t result;
              ::SendMessageW(this->_hIPAddrCtrl, IPM_GETADDRESS, 0, reinterpret_cast<LPARAM>(&result));
              return result;
          },
          // set
          [this](const uint32_t &value) {
              ::SendMessageW(this->_hIPAddrCtrl, IPM_SETADDRESS, 0, (LPARAM)value);
              this->OnAddressChanged();
          })
{
    this->Rect    = sw::Rect{0, 0, size.width, size.height};
    this->TabStop = true;

    this->InitHwndHost();
    ::SendMessageW(this->_hIPAddrCtrl, WM_SETFONT, reinterpret_cast<WPARAM>(this->GetFontHandle()), FALSE);
}

void sw::IPAddressControl::Clear()
{
    ::SendMessageW(this->_hIPAddrCtrl, IPM_CLEARADDRESS, 0, 0);
}

bool sw::IPAddressControl::SetRange(int field, uint8_t min, uint8_t max)
{
    return ::SendMessageW(this->_hIPAddrCtrl, IPM_SETRANGE, field, MAKEIPRANGE(min, max));
}

HWND sw::IPAddressControl::BuildWindowCore(HWND hParent)
{
    RECT rect;
    GetClientRect(hParent, &rect);

    this->_hIPAddrCtrl = CreateWindowExW(0, WC_IPADDRESSW, L"", WS_CHILD | WS_VISIBLE,
                                         0, 0, rect.right - rect.left, rect.bottom - rect.top,
                                         hParent, NULL, App::Instance, NULL);

    return this->_hIPAddrCtrl;
}

void sw::IPAddressControl::DestroyWindowCore(HWND hwnd)
{
    DestroyWindow(this->_hIPAddrCtrl);
    this->_hIPAddrCtrl = NULL;
}

void sw::IPAddressControl::FontChanged(HFONT hfont)
{
    if (this->_hIPAddrCtrl != NULL) {
        ::SendMessageW(this->_hIPAddrCtrl, WM_SETFONT, reinterpret_cast<WPARAM>(hfont), TRUE);
    }
    this->HwndHost::FontChanged(hfont);
}

bool sw::IPAddressControl::OnSetFocus(HWND hPrevFocus)
{
    // SetFocus(this->_hIPAddrCtrl);
    ::SendMessageW(this->_hIPAddrCtrl, IPM_SETFOCUS, -1, 0);
    return this->HwndHost::OnSetFocus(hPrevFocus);
}

bool sw::IPAddressControl::OnNotify(NMHDR *pNMHDR, LRESULT &result)
{
    if (pNMHDR->code == IPN_FIELDCHANGED) {
        this->OnAddressChanged();
    }
    return this->HwndHost::OnNotify(pNMHDR, result);
}

void sw::IPAddressControl::OnAddressChanged()
{
    this->RaiseRoutedEvent(IPAddressControl_AddressChanged);
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

HICON sw::IconHelper::GetIconHandle(const std::wstring &fileName)
{
    return (HICON)LoadImageW(NULL, fileName.c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
}

// IconBox.cpp

sw::IconBox::IconBox()
    : IconHandle(
          // get
          [this]() -> HICON {
              return this->_hIcon;
          }),

      StretchIcon(
          // get
          [this]() -> bool {
              return !this->GetStyle(SS_CENTERIMAGE);
          },
          // set
          [this](const bool &value) {
              this->SetStyle(SS_CENTERIMAGE, !value);
          })
{
    this->Rect = sw::Rect{0, 0, 50, 50};
    this->SetStyle(SS_ICON, true);
    this->Transparent = true;
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
        SetWindowPos(this->Handle, NULL, 0, 0, bm.bmWidth, bm.bmHeight, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
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

// ImageList.cpp

sw::ImageList::ImageList(HIMAGELIST hImageList, bool isWrap)
    : _hImageList(hImageList), _isWrap(isWrap)
{
}

sw::ImageList::ImageList(int cx, int cy, UINT flags, int cInitial, int cGrow)
    : ImageList(ImageList_Create(cx, cy, flags, cInitial, cGrow), false)
{
}

sw::ImageList::ImageList(const ImageList &other)
{
    if (other._isWrap) {
        this->_isWrap     = true;
        this->_hImageList = other._hImageList;
    } else {
        this->_isWrap     = false;
        this->_hImageList = ImageList_Duplicate(other._hImageList);
    }
}

sw::ImageList::ImageList(ImageList &&other)
{
    this->_isWrap     = other._isWrap;
    this->_hImageList = other._hImageList;
    other._hImageList = NULL;
}

sw::ImageList::~ImageList()
{
    this->_DestroyIfNotWrap();
}

sw::ImageList &sw::ImageList::operator=(const ImageList &other)
{
    if (this == &other) {
        return *this;
    }

    this->_DestroyIfNotWrap();

    if (other._isWrap) {
        this->_isWrap     = true;
        this->_hImageList = other._hImageList;
    } else {
        this->_isWrap     = false;
        this->_hImageList = ImageList_Duplicate(other._hImageList);
    }

    return *this;
}

sw::ImageList &sw::ImageList::operator=(ImageList &&other)
{
    if (this == &other) {
        return *this;
    }

    this->_DestroyIfNotWrap();

    this->_isWrap     = other._isWrap;
    this->_hImageList = other._hImageList;
    other._hImageList = NULL;

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

bool sw::ImageList::DragEnterPx(HWND hwndLock, int x, int y)
{
    return ImageList_DragEnter(hwndLock, x, y);
}

bool sw::ImageList::DragLeave(HWND hwndLock)
{
    return ImageList_DragLeave(hwndLock);
}

bool sw::ImageList::DragMove(double x, double y)
{
    return ImageList_DragMove(Dip::DipToPxX(x), Dip::DipToPxY(y));
}

bool sw::ImageList::DragMovePx(int x, int y)
{
    return ImageList_DragMove(x, y);
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
          [this]() -> sw::HorizontalAlignment {
              DWORD style = this->GetStyle();
              if (style & SS_CENTER) {
                  return sw::HorizontalAlignment::Center;
              } else if (style & SS_RIGHT) {
                  return sw::HorizontalAlignment::Right;
              } else {
                  return sw::HorizontalAlignment::Left;
              }
          },
          // set
          [this](const sw::HorizontalAlignment &value) {
              switch (value) {
                  case sw::HorizontalAlignment::Left: {
                      this->SetStyle(SS_CENTER | SS_RIGHT, false);
                      break;
                  }
                  case sw::HorizontalAlignment::Center: {
                      DWORD style = this->GetStyle();
                      style &= ~(SS_CENTER | SS_RIGHT);
                      style |= SS_CENTER;
                      this->SetStyle(style);
                      break;
                  }
                  case sw::HorizontalAlignment::Right: {
                      DWORD style = this->GetStyle();
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
          [this]() -> sw::VerticalAlignment {
              return this->GetStyle(SS_CENTERIMAGE) ? sw::VerticalAlignment::Center : sw::VerticalAlignment::Top;
          },
          // set
          [this](const sw::VerticalAlignment &value) {
              this->SetStyle(SS_CENTERIMAGE, value == sw::VerticalAlignment::Center);
          }),

      TextTrimming(
          // get
          [this]() -> sw::TextTrimming {
              DWORD style = this->GetStyle();
              if ((style & SS_WORDELLIPSIS) == SS_WORDELLIPSIS) {
                  return sw::TextTrimming::WordEllipsis;
              } else if (style & SS_ENDELLIPSIS) {
                  return sw::TextTrimming::EndEllipsis;
              } else {
                  return sw::TextTrimming::None;
              }
          },
          // set
          [this](const sw::TextTrimming &value) {
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
                      DWORD style = this->GetStyle();
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
          [this]() -> bool {
              return this->GetStyle(SS_EDITCONTROL);
          },
          // set
          [this](const bool &value) {
              this->SetStyle(SS_EDITCONTROL, value);
          }),

      AutoSize(
          // get
          [this]() -> bool {
              return this->_autoSize;
          },
          // set
          [this](const bool &value) {
              if (this->_autoSize != value) {
                  this->_autoSize = value;
                  this->_UpdateLayoutFlags();
                  this->InvalidateMeasure();
              }
          })
{
    this->SetInternalText(L"Label");
    this->_UpdateTextSize();
    this->_ResizeToTextSize();
    this->_UpdateLayoutFlags();
    this->Transparent      = true;
    this->InheritTextColor = true;
}

bool sw::Label::OnSize(Size newClientSize)
{
    this->Redraw();
    return StaticControl::OnSize(newClientSize);
}

void sw::Label::OnTextChanged()
{
    this->_UpdateTextSize();
    this->Control::OnTextChanged();
}

void sw::Label::FontChanged(HFONT hfont)
{
    this->_UpdateTextSize();
    this->Control::FontChanged(hfont);
}

sw::Size sw::Label::MeasureOverride(const Size &availableSize)
{
    if (!this->_autoSize) {
        return this->UIElement::MeasureOverride(availableSize);
    }

    Size desireSize = this->_textSize;

    if (availableSize.width < desireSize.width) {
        if (this->TextTrimming != sw::TextTrimming::None) {
            desireSize.width = availableSize.width;
        } else if (this->AutoWrap) {
            HWND hwnd = this->Handle;
            HDC hdc   = GetDC(hwnd);
            SelectObject(hdc, this->GetFontHandle());

            std::wstring &text = this->GetInternalText();
            RECT rect{0, 0, Utils::Max(0, Dip::DipToPxX(availableSize.width)), 0};
            DrawTextW(hdc, text.c_str(), (int)text.size(), &rect, DT_CALCRECT | DT_WORDBREAK);

            desireSize.width  = availableSize.width;
            desireSize.height = Dip::PxToDipY(rect.bottom - rect.top);
            ReleaseDC(hwnd, hdc);
        }
    }
    return desireSize;
}

void sw::Label::_UpdateTextSize()
{
    HWND hwnd = this->Handle;
    HDC hdc   = GetDC(hwnd);
    SelectObject(hdc, this->GetFontHandle());

    RECT rect{};
    std::wstring &text = this->GetInternalText();
    DrawTextW(hdc, text.c_str(), (int)text.size(), &rect, DT_CALCRECT);

    this->_textSize = sw::Rect(rect).GetSize();
    ReleaseDC(hwnd, hdc);
}

void sw::Label::_ResizeToTextSize()
{
    this->Resize(this->_textSize);
}

void sw::Label::_UpdateLayoutFlags()
{
    constexpr auto flags =
        sw::LayoutUpdateCondition::TextChanged |
        sw::LayoutUpdateCondition::FontChanged;

    if (this->_autoSize) {
        this->LayoutUpdateCondition |= flags;
    } else {
        this->LayoutUpdateCondition &= ~flags;
    }
}

// Layer.cpp

namespace
{
    /**
     * @brief 滚动条滚动一行的距离
     */
    constexpr int _LayerScrollBarLineInterval = 20;
}

sw::Layer::Layer()
    : Layout(
          // get
          [this]() -> LayoutHost * {
              return this->_customLayout;
          },
          // set
          [this](LayoutHost *value) {
              if (value != nullptr)
                  value->Associate(this);
              this->_customLayout = value;
          }),

      AutoSize(
          // get
          [this]() -> bool {
              return this->_autoSize;
          },
          // set
          [this](const bool &value) {
              if (this->_autoSize != value) {
                  this->_autoSize = value;
                  this->InvalidateMeasure();
              }
          }),

      HorizontalScrollBar(
          // get
          [this]() -> bool {
              return this->GetStyle(WS_HSCROLL);
          },
          // set
          [this](const bool &value) {
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
          [this]() -> bool {
              return this->GetStyle(WS_VSCROLL);
          },
          // set
          [this](const bool &value) {
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
          [this]() -> double {
              SCROLLINFO info{};
              info.cbSize = sizeof(info);
              info.fMask  = SIF_POS;
              GetScrollInfo(this->Handle, SB_HORZ, &info);
              return Dip::PxToDipX(info.nPos);
          },
          // set
          [this](const double &value) {
              SCROLLINFO info{};
              info.cbSize = sizeof(info);
              info.fMask  = SIF_POS;
              info.nPos   = Dip::DipToPxX(value);
              SetScrollInfo(this->Handle, SB_HORZ, &info, true);

              LayoutHost *layout = this->_GetLayout();

              if (layout != nullptr && !this->_horizontalScrollDisabled && this->HorizontalScrollBar) {
                  this->GetInternalArrangeOffsetX() = -HorizontalScrollPos;
                  this->_MeasureAndArrangeWithoutResize();
              }
          }),

      VerticalScrollPos(
          // get
          [this]() -> double {
              SCROLLINFO info{};
              info.cbSize = sizeof(info);
              info.fMask  = SIF_POS;
              GetScrollInfo(this->Handle, SB_VERT, &info);
              return Dip::PxToDipY(info.nPos);
          },
          // set
          [this](const double &value) {
              SCROLLINFO info{};
              info.cbSize = sizeof(info);
              info.fMask  = SIF_POS;
              info.nPos   = Dip::DipToPxY(value);
              SetScrollInfo(this->Handle, SB_VERT, &info, true);

              LayoutHost *layout = this->_GetLayout();

              if (layout != nullptr && !this->_verticalScrollDisabled && this->VerticalScrollBar) {
                  this->GetInternalArrangeOffsetY() = -VerticalScrollPos;
                  this->_MeasureAndArrangeWithoutResize();
              }
          }),

      HorizontalScrollLimit(
          // get
          [this]() -> double {
              if (this->_horizontalScrollDisabled) {
                  return 0;
              }
              SCROLLINFO info{};
              info.cbSize = sizeof(info);
              info.fMask  = SIF_RANGE | SIF_PAGE;
              GetScrollInfo(this->Handle, SB_HORZ, &info);
              return Dip::PxToDipX(info.nMax - info.nPage + 1);
          }),

      VerticalScrollLimit(
          // get
          [this]() -> double {
              if (this->_verticalScrollDisabled) {
                  return 0;
              }
              SCROLLINFO info{};
              info.cbSize = sizeof(info);
              info.fMask  = SIF_RANGE | SIF_PAGE;
              GetScrollInfo(this->Handle, SB_VERT, &info);
              return Dip::PxToDipY(info.nMax - info.nPage + 1);
          }),

      MouseWheelScrollEnabled(
          // get
          [this]() -> bool {
              return this->_mouseWheelScrollEnabled;
          },
          // set
          [this](const bool &value) {
              this->_mouseWheelScrollEnabled = value;
          })
{
}

sw::Layer::~Layer()
{
}

sw::LayoutHost *sw::Layer::_GetLayout()
{
    auto layout = (this->_customLayout != nullptr) ? this->_customLayout : this->GetDefaultLayout();
    return (layout != nullptr && layout->IsAssociated(this)) ? layout : nullptr;
}

void sw::Layer::_MeasureAndArrangeWithoutLayout()
{
    this->GetInternalArrangeOffsetX() = 0;
    this->GetInternalArrangeOffsetY() = 0;

    int childCount = this->GetChildLayoutCount();

    for (int i = 0; i < childCount; ++i) {
        // measure
        UIElement &item = static_cast<UIElement &>(this->GetChildLayoutAt(i));
        item.Measure(Size(INFINITY, INFINITY));
        // arrange
        Size desireSize      = item.GetDesireSize();
        sw::Rect itemRect    = item.Rect;
        Thickness itemMargin = item.Margin;
        item.Arrange(sw::Rect{itemRect.left - itemMargin.left, itemRect.top - itemMargin.top, desireSize.width, desireSize.height});
    }
}

void sw::Layer::_MeasureAndArrangeWithoutResize()
{
    LayoutHost *layout  = this->_GetLayout();
    sw::Size clientSize = this->ClientRect->GetSize();
    layout->MeasureOverride(clientSize);
    layout->ArrangeOverride(clientSize);
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
        this->_MeasureAndArrangeWithoutResize();
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
                this->ScrollHorizontal(-_LayerScrollBarLineInterval);
                break;
            }
            case ScrollEvent::LineRight: {
                this->ScrollHorizontal(_LayerScrollBarLineInterval);
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
                this->ScrollVertical(-_LayerScrollBarLineInterval);
                break;
            }
            case ScrollEvent::LineDown: {
                this->ScrollVertical(_LayerScrollBarLineInterval);
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
                   (event == SB_THUMBTRACK || event == SB_THUMBPOSITION) ? Dip::PxToDipY(pos) : (0.0));
    return true;
}

bool sw::Layer::OnHorizontalScroll(int event, int pos)
{
    this->OnScroll(ScrollOrientation::Horizontal, (ScrollEvent)event,
                   (event == SB_THUMBTRACK || event == SB_THUMBPOSITION) ? Dip::PxToDipX(pos) : (0.0));
    return true;
}

sw::Size sw::Layer::MeasureOverride(const Size &availableSize)
{
    LayoutHost *layout = this->_GetLayout();

    // 未设置布局时无法使用自动尺寸
    // 若未使用自动尺寸，则按照普通元素measure
    if (layout == nullptr || !this->_autoSize) {
        return this->UIElement::MeasureOverride(availableSize);
    }

    return layout->MeasureOverride(availableSize);
}

void sw::Layer::ArrangeOverride(const Size &finalSize)
{
    LayoutHost *layout = this->_GetLayout();

    if (layout == nullptr) {
        // 未设置布局方式，此时需要对子元素进行Measure和Arrange
        this->_MeasureAndArrangeWithoutLayout();
    } else if (!this->_autoSize) {
        // 已设置布局方式，但是AutoSize被取消，此时子元素也未Measure
        this->_MeasureAndArrangeWithoutResize();
    } else {
        // 已设置布局方式且AutoSize为true，此时子元素已Measure，调用Arrange即可
        layout->ArrangeOverride(finalSize);
    }

    this->UpdateScrollRange();
}

bool sw::Layer::RequestBringIntoView(const sw::Rect &screenRect)
{
    bool handled = false;

    sw::Point pos = this->PointFromScreen(screenRect.GetPos());
    sw::Rect rect = {pos.x, pos.y, screenRect.width, screenRect.height};

    sw::Rect clientRect = this->ClientRect;

    if (this->VerticalScrollBar) {
        double curPos = this->VerticalScrollPos;
        if (rect.top < curPos) {
            this->VerticalScrollPos = rect.top;
        } else if (rect.top + rect.height > curPos + clientRect.height) {
            if (rect.height >= clientRect.height) {
                this->VerticalScrollPos = rect.top;
            } else {
                this->VerticalScrollPos = rect.top + rect.height - clientRect.height;
            }
        }
        handled = true;
    }

    if (this->HorizontalScrollBar) {
        double curPos = this->HorizontalScrollPos;
        if (rect.left < curPos) {
            this->HorizontalScrollPos = rect.left;
        } else if (rect.left + rect.width > curPos + clientRect.width) {
            if (rect.width >= clientRect.width) {
                this->HorizontalScrollPos = rect.left;
            } else {
                this->HorizontalScrollPos = rect.left + rect.width - clientRect.width;
            }
        }
        handled = true;
    }

    return handled;
}

bool sw::Layer::OnRoutedEvent(RoutedEventArgs &eventArgs, const RoutedEventHandler &handler)
{
    if (handler) {
        handler(*this, eventArgs);
    }
    if (eventArgs.handled) {
        return true;
    }

    if (eventArgs.eventType == UIElement_MouseWheel && this->_mouseWheelScrollEnabled) {
        auto &wheelArgs = static_cast<MouseWheelEventArgs &>(eventArgs);
        bool shiftDown  = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
        double offset   = -std::copysign(_LayerScrollBarLineInterval, wheelArgs.wheelDelta);
        if (shiftDown) {
            if (this->HorizontalScrollBar) {
                this->ScrollHorizontal(offset);
                eventArgs.handled = true;
            }
        } else {
            if (this->VerticalScrollBar) {
                this->ScrollVertical(offset);
                eventArgs.handled = true;
            }
        }
    }
    return true;
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

bool sw::Layer::IsLayoutDisabled()
{
    return this->_layoutDisabled;
}

void sw::Layer::GetHorizontalScrollRange(double &refMin, double &refMax)
{
    INT nMin = 0, nMax = 0;
    GetScrollRange(this->Handle, SB_HORZ, &nMin, &nMax);

    refMin = Dip::PxToDipX(nMin);
    refMax = Dip::PxToDipX(nMax);
}

void sw::Layer::GetVerticalScrollRange(double &refMin, double &refMax)
{
    INT nMin = 0, nMax = 0;
    GetScrollRange(this->Handle, SB_VERT, &nMin, &nMax);

    refMin = Dip::PxToDipY(nMin);
    refMax = Dip::PxToDipY(nMax);
}

void sw::Layer::SetHorizontalScrollRange(double min, double max)
{
    SCROLLINFO info{};
    info.cbSize = sizeof(info);
    info.fMask  = SIF_RANGE | SIF_PAGE;
    info.nMin   = Dip::DipToPxX(min);
    info.nMax   = Dip::DipToPxX(max);
    info.nPage  = Dip::DipToPxX(this->ClientWidth);

    SetScrollInfo(this->Handle, SB_HORZ, &info, true);
}

void sw::Layer::SetVerticalScrollRange(double min, double max)
{
    SCROLLINFO info{};
    info.cbSize = sizeof(info);
    info.fMask  = SIF_RANGE | SIF_PAGE;
    info.nMin   = Dip::DipToPxY(min);
    info.nMax   = Dip::DipToPxY(max);
    info.nPage  = Dip::DipToPxY(this->ClientHeight);

    SetScrollInfo(this->Handle, SB_VERT, &info, true);
}

double sw::Layer::GetHorizontalScrollPageSize()
{
    SCROLLINFO info{};
    info.cbSize = sizeof(info);
    info.fMask  = SIF_PAGE;
    GetScrollInfo(this->Handle, SB_HORZ, &info);
    return Dip::PxToDipX(info.nPage);
}

double sw::Layer::GetVerticalScrollPageSize()
{
    SCROLLINFO info{};
    info.cbSize = sizeof(info);
    info.fMask  = SIF_PAGE;
    GetScrollInfo(this->Handle, SB_VERT, &info);
    return Dip::PxToDipY(info.nPage);
}

void sw::Layer::SetHorizontalScrollPageSize(double pageSize)
{
    SCROLLINFO info{};
    info.cbSize = sizeof(info);
    info.fMask  = SIF_PAGE;
    info.nPage  = Dip::DipToPxX(pageSize);
    SetScrollInfo(this->Handle, SB_HORZ, &info, true);
}

void sw::Layer::SetVerticalScrollPageSize(double pageSize)
{
    SCROLLINFO info{};
    info.cbSize = sizeof(info);
    info.fMask  = SIF_PAGE;
    info.nPage  = Dip::DipToPxY(pageSize);
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
            if (-this->GetInternalArrangeOffsetX() > pos) {
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
            if (-this->GetInternalArrangeOffsetY() > pos) {
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
    this->HorizontalScrollPos += offset;
}

void sw::Layer::ScrollVertical(double offset)
{
    this->VerticalScrollPos += offset;
}

// LayoutHost.cpp

void sw::LayoutHost::Associate(ILayout *obj)
{
    this->_associatedObj = obj;
}

bool sw::LayoutHost::IsAssociated(ILayout *obj)
{
    return obj == nullptr
               ? (this->_associatedObj != nullptr)
               : (this->_associatedObj == obj);
}

int sw::LayoutHost::GetChildLayoutCount()
{
    return this->_associatedObj->GetChildLayoutCount();
}

sw::ILayout &sw::LayoutHost::GetChildLayoutAt(int index)
{
    return this->_associatedObj->GetChildLayoutAt(index);
}

// ListBox.cpp

sw::ListBox::ListBox()
    : TopIndex(
          // get
          [this]() -> int {
              return (int)this->SendMessageW(LB_GETTOPINDEX, 0, 0);
          },
          // set
          [this](const int &value) {
              this->SendMessageW(LB_SETTOPINDEX, value, 0);
          }),

      MultiSelect(
          // get
          [this]() -> bool {
              return this->GetStyle(LBS_MULTIPLESEL);
          },
          // set
          [this](const bool &value) {
              if (this->GetStyle(LBS_MULTIPLESEL) != value) {
                  this->SetStyle(LBS_MULTIPLESEL, value);
                  this->ResetHandle();
              }
          }),

      SelectedCount(
          // get
          [this]() -> int {
              return (int)this->SendMessageW(LB_GETSELCOUNT, 0, 0);
          })
{
    this->InitControl(L"LISTBOX", L"", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_BORDER | WS_VSCROLL | LBS_NOINTEGRALHEIGHT | LBS_NOTIFY, 0);
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

void sw::ListBox::OnDrawFocusRect(HDC hdc)
{
    // 不绘制虚线框
}

void sw::ListBox::Clear()
{
    this->SendMessageW(LB_RESETCONTENT, 0, 0);
}

std::wstring sw::ListBox::GetItemAt(int index)
{
    int len = (int)this->SendMessageW(LB_GETTEXTLEN, index, 0);

    if (len <= 0) {
        return std::wstring{};
    }

    // wchar_t *buf = new wchar_t[len + 1];
    // this->SendMessageW(LB_GETTEXT, index, reinterpret_cast<LPARAM>(buf));
    // std::wstring result = buf;
    // delete[] buf;
    // return result;

    std::wstring result;
    result.resize(len + 1);
    this->SendMessageW(LB_GETTEXT, index, reinterpret_cast<LPARAM>(&result[0]));
    result.resize(len);
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

namespace
{
    /**
     * @brief 获取文本时缓冲区的初始大小
     */
    constexpr int _ListViewTextInitialBufferSize = 256;
}

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
          [this]() -> int {
              return this->_GetColCount();
          }),

      GridLines(
          // get
          [this]() -> bool {
              return this->_GetExtendedListViewStyle() & LVS_EX_GRIDLINES;
          },
          // set
          [this](const bool &value) {
              DWORD style;
              style = this->_GetExtendedListViewStyle();
              style = value ? (style | LVS_EX_GRIDLINES) : (style & (~LVS_EX_GRIDLINES));
              this->_SetExtendedListViewStyle(style);
          }),

      MultiSelect(
          // get
          [this]() -> bool {
              return !(this->GetStyle() & LVS_SINGLESEL);
          },
          // set
          [this](const bool &value) {
              this->SetStyle(LVS_SINGLESEL, !value);
          }),

      SelectedCount(
          // get
          [this]() -> int {
              return (int)this->SendMessageW(LVM_GETSELECTEDCOUNT, 0, 0);
          }),

      CheckBoxes(
          // get
          [this]() -> bool {
              return this->_GetExtendedListViewStyle() & LVS_EX_CHECKBOXES;
          },
          // set
          [this](const bool &value) {
              DWORD style;
              style = this->_GetExtendedListViewStyle();
              style = value ? (style | LVS_EX_CHECKBOXES) : (style & (~LVS_EX_CHECKBOXES));
              this->_SetExtendedListViewStyle(style);
          }),

      TopIndex(
          // get
          [this]() -> int {
              return (int)this->SendMessageW(LVM_GETTOPINDEX, 0, 0);
          }),

      ShareImageLists(
          // get
          [this]() -> bool {
              return this->GetStyle(LVS_SHAREIMAGELISTS);
          },
          // set
          [this](const bool &value) {
              this->SetStyle(LVS_SHAREIMAGELISTS, value);
          }),

      Editable(
          // get
          [this]() -> bool {
              return this->GetStyle(LVS_EDITLABELS);
          },
          // set
          [this](const bool &value) {
              this->SetStyle(LVS_EDITLABELS, value);
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

bool sw::ListView::OnNotify(NMHDR *pNMHDR, LRESULT &result)
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

bool sw::ListView::OnNotified(NMHDR *pNMHDR, LRESULT &result)
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
        case LVN_GETDISPINFOW: {
            this->OnGetDispInfo(reinterpret_cast<NMLVDISPINFOW *>(pNMHDR));
            return true;
        }
        case LVN_ENDLABELEDITW: {
            result = (LRESULT)this->OnEndEdit(reinterpret_cast<NMLVDISPINFOW *>(pNMHDR));
            return true;
        }
    }
    return this->Control::OnNotified(pNMHDR, result);
}

void sw::ListView::OnDrawFocusRect(HDC hdc)
{
    // 不绘制虚线框
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

void sw::ListView::OnGetDispInfo(NMLVDISPINFOW *pNMInfo)
{
}

bool sw::ListView::OnEndEdit(NMLVDISPINFOW *pNMInfo)
{
    if (pNMInfo->item.pszText == nullptr) {
        return false;
    }
    ListViewEndEditEventArgs args(pNMInfo->item.iItem, pNMInfo->item.pszText);
    this->RaiseRoutedEvent(args);
    pNMInfo->item.pszText = args.newText;
    return !args.cancel;
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

    int bufsize = _ListViewTextInitialBufferSize;
    std::unique_ptr<wchar_t[]> buf(new wchar_t[bufsize]);

    LVITEMW lvi;
    lvi.mask       = LVIF_TEXT;
    lvi.iItem      = index;
    lvi.pszText    = buf.get();
    lvi.cchTextMax = bufsize;

    for (int j = 0; j < col; ++j) {
        lvi.iSubItem = j;

        int len = (int)this->SendMessageW(LVM_GETITEMTEXTW, index, reinterpret_cast<LPARAM>(&lvi));
        while (len == bufsize - 1 && bufsize < INT_MAX / 2) {
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
    std::wstring result;

    int bufsize = _ListViewTextInitialBufferSize;
    result.resize(bufsize);

    LVITEMW lvi;
    lvi.mask       = LVIF_TEXT;
    lvi.iItem      = row;
    lvi.iSubItem   = col;
    lvi.pszText    = &result[0];
    lvi.cchTextMax = bufsize;

    int len = (int)this->SendMessageW(LVM_GETITEMTEXTW, row, reinterpret_cast<LPARAM>(&lvi));
    while (len == bufsize - 1 && bufsize < INT_MAX / 2) {
        bufsize *= 2;
        result.resize(bufsize);
        lvi.pszText    = &result[0];
        lvi.cchTextMax = bufsize;
        len            = (int)this->SendMessageW(LVM_GETITEMTEXTW, row, reinterpret_cast<LPARAM>(&lvi));
    }

    result.resize(len);
    return result;
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

sw::ImageList sw::ListView::GetImageList(ListViewImageList imageList)
{
    return ImageList::Wrap((HIMAGELIST)this->SendMessageW(LVM_GETIMAGELIST, (WPARAM)imageList, 0));
}

HIMAGELIST sw::ListView::SetImageList(ListViewImageList imageList, HIMAGELIST value)
{
    return (HIMAGELIST)this->SendMessageW(LVM_SETIMAGELIST, (WPARAM)imageList, (LPARAM)value);
}

bool sw::ListView::SetItemImage(int index, int imgIndex)
{
    LVITEMW lvi;
    lvi.mask     = LVIF_IMAGE;
    lvi.iItem    = index;
    lvi.iSubItem = 0;
    lvi.iImage   = imgIndex;

    return this->SendMessageW(LVM_SETITEMW, 0, reinterpret_cast<LPARAM>(&lvi));
}

bool sw::ListView::EditItem(int index)
{
    return this->SendMessageW(LVM_EDITLABELW, index, 0) != 0;
}

void sw::ListView::CancelEdit()
{
    this->SendMessageW(LVM_CANCELEDITLABEL, 0, 0);
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
    : MenuBase(CreateMenu())
{
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

sw::MenuBase::MenuBase(HMENU hMenu)
    : _hMenu(hMenu)
{
}

sw::MenuBase::~MenuBase()
{
    this->_ClearAddedItems();

    if (this->_hMenu != NULL) {
        DestroyMenu(this->_hMenu);
    }
}

HMENU sw::MenuBase::GetHandle()
{
    return this->_hMenu;
}

void sw::MenuBase::Update()
{
    this->_ClearAddedItems();

    int i = 0;
    for (std::shared_ptr<MenuItem> pItem : this->_items) {
        this->_AppendMenuItem(this->_hMenu, pItem, i++);
    }
}

void sw::MenuBase::SetItems(std::initializer_list<MenuItem> items)
{
    this->_ClearAddedItems();

    for (const MenuItem &item : items) {
        std::shared_ptr<MenuItem> pItem = std::make_shared<MenuItem>(item);
        this->_items.push_back(pItem);
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
    this->_AppendMenuItem(this->_hMenu, pItem, (int)this->_items.size());
    this->_items.push_back(pItem);
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
        this->_items.erase(this->_items.begin() + index);

        for (int i = index; i < (int)this->_items.size(); ++i) {
            this->_dependencyInfoMap[this->_items[i].get()].index -= 1;
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
    return index >= 0 && index < (int)this->_ids.size() ? this->_ids[index].get() : nullptr;
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

    if (index < 0 || index >= (int)this->_items.size()) {
        return nullptr;
    }

    result = this->_items[index].get();

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

    for (std::shared_ptr<MenuItem> pItem : this->_items) {
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

sw::MenuItem *sw::MenuBase::GetMenuItemByTag(uint64_t tag)
{
    return this->_GetMenuItemByTag(this->_items, tag);
}

sw::MenuItem *sw::MenuBase::GetParent(MenuItem &item)
{
    auto dependencyInfo = this->_GetMenuItemDependencyInfo(item);

    if (dependencyInfo == nullptr) {
        return nullptr;
    }

    for (auto &info : this->_popupMenus) {
        if (info.hSelf == dependencyInfo->hParent) {
            return info.pItem.get();
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

bool sw::MenuBase::SetBitmap(MenuItem &item, HBITMAP hBitmap)
{
    auto dependencyInfo = this->_GetMenuItemDependencyInfo(item);

    if (dependencyInfo == nullptr) {
        return false;
    }

    MENUITEMINFOW info{};
    info.cbSize   = sizeof(info);
    info.fMask    = MIIM_BITMAP;
    info.hbmpItem = hBitmap;

    return SetMenuItemInfoW(dependencyInfo->hParent, dependencyInfo->index, TRUE, &info);
}

bool sw::MenuBase::SetCheckBitmap(MenuItem &item, HBITMAP hBmpUnchecked, HBITMAP hBmpChecked)
{
    auto dependencyInfo = this->_GetMenuItemDependencyInfo(item);

    if (dependencyInfo == nullptr) {
        return false;
    }

    return SetMenuItemBitmaps(dependencyInfo->hParent, dependencyInfo->index, MF_BYPOSITION, hBmpUnchecked, hBmpChecked);
}

void sw::MenuBase::_ClearAddedItems()
{
    while (GetMenuItemCount(this->_hMenu) > 0) {
        RemoveMenu(this->_hMenu, 0, MF_BYPOSITION);
    }

    for (auto &info : this->_popupMenus) {
        DestroyMenu(info.hSelf);
    }

    this->_dependencyInfoMap.clear();
    this->_popupMenus.clear();
    this->_ids.clear();
}

void sw::MenuBase::_AppendMenuItem(HMENU hMenu, std::shared_ptr<MenuItem> pItem, int index)
{
    this->_dependencyInfoMap[pItem.get()] =
        {/*hParent*/ hMenu, /*hSelf*/ NULL, /*index*/ index};

    // 分隔条
    if (pItem->IsSeparator()) {
        AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
        return;
    }

    // 菜单项
    if (pItem->subItems.empty()) {
        // 无子项，该菜单项没有句柄
        int id = this->IndexToID((int)this->_ids.size());
        AppendMenuW(hMenu, MF_STRING, id, pItem->text.c_str());
        this->_ids.push_back(pItem);
    } else {
        // 有子项，需创建菜单句柄
        HMENU hSelf = CreatePopupMenu();
        this->_popupMenus.push_back({/*pItem*/ pItem, /*hSelf*/ hSelf});
        this->_dependencyInfoMap[pItem.get()].hSelf = hSelf;
        AppendMenuW(hMenu, MF_POPUP, reinterpret_cast<UINT_PTR>(hSelf), pItem->text.c_str());
        // 递归添加子项
        int i = 0;
        for (std::shared_ptr<MenuItem> pSubItem : pItem->subItems) {
            this->_AppendMenuItem(hSelf, pSubItem, i++);
        }
    }
}

sw::MenuBase::_MenuItemDependencyInfo *sw::MenuBase::_GetMenuItemDependencyInfo(MenuItem &item)
{
    MenuItem *p = &item;
    return this->_dependencyInfoMap.count(p) ? &this->_dependencyInfoMap[p] : nullptr;
}

sw::MenuItem *sw::MenuBase::_GetMenuItemByTag(std::vector<std::shared_ptr<MenuItem>> &items, uint64_t tag)
{
    MenuItem *result = nullptr;

    for (std::shared_ptr<MenuItem> pItem : items) {
        if (pItem->tag == tag) {
            result = pItem.get();
            break;
        }
        if (!pItem->subItems.empty()) {
            result = this->_GetMenuItemByTag(pItem->subItems, tag);
            if (result) break;
        }
    }

    return result;
}

// MenuItem.cpp

sw::MenuItem::MenuItem(const std::wstring &text)
    : tag(0), text(text), command(nullptr)
{
}

sw::MenuItem::MenuItem(const std::wstring &text, const MenuItemCommand &command)
    : tag(0), text(text), command(command)
{
}

sw::MenuItem::MenuItem(const std::wstring &text, std::initializer_list<MenuItem> subItems)
    : tag(0), text(text), command(nullptr)
{
    for (const MenuItem &subItem : subItems) {
        std::shared_ptr<MenuItem> pSubItem = std::make_shared<MenuItem>(subItem);
        this->subItems.push_back(pSubItem);
    }
}

sw::MenuItem::MenuItem(uint64_t tag, const std::wstring &text)
    : tag(tag), text(text), command(nullptr)
{
}

sw::MenuItem::MenuItem(uint64_t tag, const std::wstring &text, const MenuItemCommand &command)
    : tag(tag), text(text), command(command)
{
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

// MonthCalendar.cpp

sw::MonthCalendar::MonthCalendar()
    : ShowToday(
          // get
          [this]() -> bool {
              return !this->GetStyle(MCS_NOTODAY);
          },
          // set
          [this](const bool &value) {
              this->SetStyle(MCS_NOTODAY, !value);
          })
{
    this->InitControl(MONTHCAL_CLASSW, L"", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, 0);
    this->Rect    = {0, 0, 250, 200};
    this->TabStop = true;
}

bool sw::MonthCalendar::GetToday(SYSTEMTIME &out)
{
    return this->SendMessageW(MCM_GETTODAY, 0, reinterpret_cast<LPARAM>(&out));
}

bool sw::MonthCalendar::SetToday(const SYSTEMTIME &today)
{
    return this->SendMessageW(MCM_SETTODAY, 0, reinterpret_cast<LPARAM>(&today));
}

bool sw::MonthCalendar::GetTime(SYSTEMTIME &out)
{
    return this->SendMessageW(MCM_GETCURSEL, 0, reinterpret_cast<LPARAM>(&out));
}

bool sw::MonthCalendar::SetTime(const SYSTEMTIME &time)
{
    bool result = this->SendMessageW(MCM_SETCURSEL, 0, reinterpret_cast<LPARAM>(&time));
    if (result) {
        SYSTEMTIME time{};
        this->GetTime(time);
        MonthCalendarTimeChangedEventArgs arg{time};
        this->RaiseRoutedEvent(arg);
    }
    return result;
}

bool sw::MonthCalendar::SetRange(const SYSTEMTIME &minTime, const SYSTEMTIME &maxTime)
{
    SYSTEMTIME range[2] = {minTime, maxTime};
    return this->SendMessageW(MCM_SETRANGE, GDTR_MIN | GDTR_MAX, reinterpret_cast<LPARAM>(range));
}

void sw::MonthCalendar::OnDrawFocusRect(HDC hdc)
{
    // 不绘制虚线框
}

void sw::MonthCalendar::SetBackColor(Color color, bool redraw)
{
    this->Control::SetBackColor(color, false);
    this->SendMessageW(MCM_SETCOLOR, MCSC_BACKGROUND, (COLORREF)color);
}

void sw::MonthCalendar::SetTextColor(Color color, bool redraw)
{
    this->Control::SetTextColor(color, false);
    this->SendMessageW(MCM_SETCOLOR, MCSC_TEXT, (COLORREF)color);
}

bool sw::MonthCalendar::OnNotified(NMHDR *pNMHDR, LRESULT &result)
{
    if (pNMHDR->code == MCN_SELCHANGE) {
        this->OnTimeChanged(reinterpret_cast<NMSELCHANGE *>(pNMHDR));
    }
    return this->Control::OnNotified(pNMHDR, result);
}

void sw::MonthCalendar::OnTimeChanged(NMSELCHANGE *pInfo)
{
    MonthCalendarTimeChangedEventArgs arg{pInfo->stSelStart};
    this->RaiseRoutedEvent(arg);
}

// MsgBox.cpp

sw::MsgBoxResultHelper::MsgBoxResultHelper(MsgBoxResult result)
    : result(result)
{
}

sw::MsgBoxResultHelper::operator sw::MsgBoxResult() const
{
    return this->result;
}

sw::MsgBoxResultHelper &sw::MsgBoxResultHelper::OnOk(const MsgBoxCallback &callback)
{
    return this->On<MsgBoxResult::Ok>(callback);
}

sw::MsgBoxResultHelper &sw::MsgBoxResultHelper::OnYes(const MsgBoxCallback &callback)
{
    return this->On<MsgBoxResult::Yes>(callback);
}

sw::MsgBoxResultHelper &sw::MsgBoxResultHelper::OnNo(const MsgBoxCallback &callback)
{
    return this->On<MsgBoxResult::No>(callback);
}

sw::MsgBoxResultHelper &sw::MsgBoxResultHelper::OnCancel(const MsgBoxCallback &callback)
{
    return this->On<MsgBoxResult::Cancel>(callback);
}

sw::MsgBoxResultHelper &sw::MsgBoxResultHelper::OnAbort(const MsgBoxCallback &callback)
{
    return this->On<MsgBoxResult::Abort>(callback);
}

sw::MsgBoxResultHelper &sw::MsgBoxResultHelper::OnContinue(const MsgBoxCallback &callback)
{
    return this->On<MsgBoxResult::Continue>(callback);
}

sw::MsgBoxResultHelper &sw::MsgBoxResultHelper::OnIgnore(const MsgBoxCallback &callback)
{
    return this->On<MsgBoxResult::Ignore>(callback);
}

sw::MsgBoxResultHelper &sw::MsgBoxResultHelper::OnRetry(const MsgBoxCallback &callback)
{
    return this->On<MsgBoxResult::Retry>(callback);
}

sw::MsgBoxResultHelper &sw::MsgBoxResultHelper::OnTryAgain(const MsgBoxCallback &callback)
{
    return this->On<MsgBoxResult::TryAgain>(callback);
}

sw::MsgBoxResultHelper sw::MsgBox::Show(const WndBase *owner, const std::wstring &text, const std::wstring &caption, MsgBoxButton button, MsgBoxIcon icon)
{
    HWND hwnd = (owner == nullptr) ? reinterpret_cast<HWND>(NULL) : owner->Handle;
    return (MsgBoxResult)MessageBoxW(hwnd, text.c_str(), caption.c_str(), (UINT)button | (UINT)icon);
}

sw::MsgBoxResultHelper sw::MsgBox::Show(const WndBase &owner, const std::wstring &text, const std::wstring &caption, MsgBoxButton button, MsgBoxIcon icon)
{
    return MsgBox::Show(&owner, text, caption, button, icon);
}

sw::MsgBoxResultHelper sw::MsgBox::Show(const std::wstring &text, const std::wstring &caption, MsgBoxButton button, MsgBoxIcon icon)
{
    return MsgBox::Show(Window::ActiveWindow, text, caption, button, icon);
}

sw::MsgBoxResultHelper sw::MsgBox::ShowInfo(const WndBase *owner, const std::wstring &text, const std::wstring &caption, MsgBoxButton button)
{
    HWND hwnd = (owner == nullptr) ? reinterpret_cast<HWND>(NULL) : owner->Handle;
    return (MsgBoxResult)MessageBoxW(hwnd, text.c_str(), caption.c_str(), (UINT)button | MB_ICONINFORMATION);
}

sw::MsgBoxResultHelper sw::MsgBox::ShowInfo(const WndBase &owner, const std::wstring &text, const std::wstring &caption, MsgBoxButton button)
{
    return MsgBox::ShowInfo(&owner, text, caption, button);
}

sw::MsgBoxResultHelper sw::MsgBox::ShowInfo(const std::wstring &text, const std::wstring &caption, MsgBoxButton button)
{
    return MsgBox::ShowInfo(Window::ActiveWindow, text, caption, button);
}

sw::MsgBoxResultHelper sw::MsgBox::ShowError(const WndBase *owner, const std::wstring &text, const std::wstring &caption, MsgBoxButton button)
{
    HWND hwnd = (owner == nullptr) ? reinterpret_cast<HWND>(NULL) : owner->Handle;
    return (MsgBoxResult)MessageBoxW(hwnd, text.c_str(), caption.c_str(), (UINT)button | MB_ICONERROR);
}

sw::MsgBoxResultHelper sw::MsgBox::ShowError(const WndBase &owner, const std::wstring &text, const std::wstring &caption, MsgBoxButton button)
{
    return MsgBox::ShowError(&owner, text, caption, button);
}

sw::MsgBoxResultHelper sw::MsgBox::ShowError(const std::wstring &text, const std::wstring &caption, MsgBoxButton button)
{
    return MsgBox::ShowError(Window::ActiveWindow, text, caption, button);
}

sw::MsgBoxResultHelper sw::MsgBox::ShowWarning(const WndBase *owner, const std::wstring &text, const std::wstring &caption, MsgBoxButton button)
{
    HWND hwnd = (owner == nullptr) ? reinterpret_cast<HWND>(NULL) : owner->Handle;
    return (MsgBoxResult)MessageBoxW(hwnd, text.c_str(), caption.c_str(), (UINT)button | MB_ICONWARNING);
}

sw::MsgBoxResultHelper sw::MsgBox::ShowWarning(const WndBase &owner, const std::wstring &text, const std::wstring &caption, MsgBoxButton button)
{
    return MsgBox::ShowWarning(&owner, text, caption, button);
}

sw::MsgBoxResultHelper sw::MsgBox::ShowWarning(const std::wstring &text, const std::wstring &caption, MsgBoxButton button)
{
    return MsgBox::ShowWarning(Window::ActiveWindow, text, caption, button);
}

sw::MsgBoxResultHelper sw::MsgBox::ShowQuestion(const WndBase *owner, const std::wstring &text, const std::wstring &caption, MsgBoxButton button)
{
    HWND hwnd = (owner == nullptr) ? reinterpret_cast<HWND>(NULL) : owner->Handle;
    return (MsgBoxResult)MessageBoxW(hwnd, text.c_str(), caption.c_str(), (UINT)button | MB_ICONQUESTION);
}

sw::MsgBoxResultHelper sw::MsgBox::ShowQuestion(const WndBase &owner, const std::wstring &text, const std::wstring &caption, MsgBoxButton button)
{
    return MsgBox::ShowQuestion(&owner, text, caption, button);
}

sw::MsgBoxResultHelper sw::MsgBox::ShowQuestion(const std::wstring &text, const std::wstring &caption, MsgBoxButton button)
{
    return MsgBox::ShowQuestion(Window::ActiveWindow, text, caption, button);
}

// Panel.cpp

namespace
{
    /**
     * @brief 面板的窗口类名
     */
    constexpr wchar_t _PanelClassName[] = L"sw::Panel";
}

sw::Panel::Panel()
    : BorderStyle(
          // get
          [this]() -> sw::BorderStyle {
              return this->_borderStyle;
          },
          // set
          [this](const sw::BorderStyle &value) {
              if (this->_borderStyle != value) {
                  this->_borderStyle = value;
                  this->UpdateBorder();
              }
          }),

      Padding(
          // get
          [this]() -> sw::Thickness {
              return this->_padding;
          },
          // set
          [this](const sw::Thickness &value) {
              if (this->_padding != value) {
                  this->_padding = value;
                  this->UpdateBorder();
              }
          })
{
    static thread_local ATOM panelClsAtom = 0;

    if (panelClsAtom == 0) {
        WNDCLASSEXW wc{};
        wc.cbSize        = sizeof(wc);
        wc.hInstance     = App::Instance;
        wc.lpfnWndProc   = DefWindowProcW;
        wc.lpszClassName = _PanelClassName;
        wc.hCursor       = CursorHelper::GetCursorHandle(StandardCursor::Arrow);
        panelClsAtom     = RegisterClassExW(&wc);
    }

    this->InitControl(_PanelClassName, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, WS_EX_NOACTIVATE);
    this->Rect             = sw::Rect(0, 0, 200, 200);
    this->Transparent      = true;
    this->InheritTextColor = true;
}

void sw::Panel::UpdateBorder()
{
    SetWindowPos(this->Handle, nullptr, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
}

LRESULT sw::Panel::WndProc(const ProcMsg &refMsg)
{
    switch (refMsg.uMsg) {
        case WM_NCCALCSIZE: {
            auto result = this->DefaultWndProc(refMsg);
            RECT *pRect = refMsg.wParam == FALSE
                              ? reinterpret_cast<RECT *>(refMsg.lParam)
                              : reinterpret_cast<NCCALCSIZE_PARAMS *>(refMsg.lParam)->rgrc;
            this->OnDrawBorder(NULL, *pRect);
            this->OnDrawPadding(NULL, *pRect);
            return result;
        }

        case WM_UpdateLayout: {
            if (this->IsInHierarchy)
                this->UpdateLayout();
            return 0;
        }

        default: {
            return this->WndBase::WndProc(refMsg);
        }
    }
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

    DeleteObject(hBrush);
    EndPaint(hwnd, &ps);
    return true;
}

void sw::Panel::OnEndNcPaint()
{
    HWND hwnd = this->Handle;
    HDC hdc   = GetWindowDC(hwnd);

    RECT rect;
    GetWindowRect(hwnd, &rect);

    rect.right -= rect.left;
    rect.bottom -= rect.top;
    rect.left = 0;
    rect.top  = 0;

    this->OnDrawBorder(hdc, rect);
    this->OnDrawPadding(hdc, rect);

    ReleaseDC(hwnd, hdc);
    return;
}

void sw::Panel::OnDrawBorder(HDC hdc, RECT &rect)
{
    if (this->_borderStyle == sw::BorderStyle::None) {
        return;
    }

    if (hdc != NULL) {
        DrawEdge(hdc, &rect, (UINT)this->_borderStyle, BF_RECT);
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
    if (this->_padding.left == 0 && this->_padding.top == 0 &&
        this->_padding.right == 0 && this->_padding.bottom == 0) {
        return;
    }

    RECT rtPaddingOuter  = rect;
    RECT &rtPaddingInner = rect;

    rtPaddingInner.left += Dip::DipToPxX(this->_padding.left);
    rtPaddingInner.top += Dip::DipToPxY(this->_padding.top);
    rtPaddingInner.right -= Dip::DipToPxX(this->_padding.right);
    rtPaddingInner.bottom -= Dip::DipToPxY(this->_padding.bottom);

    rtPaddingInner.right  = Utils::Max(rtPaddingInner.left, rtPaddingInner.right);
    rtPaddingInner.bottom = Utils::Max(rtPaddingInner.top, rtPaddingInner.bottom);

    if (hdc != NULL) {
        HRGN hRgnOuter = CreateRectRgnIndirect(&rtPaddingOuter);
        HRGN hRgnInner = CreateRectRgnIndirect(&rtPaddingInner);
        HRGN hRgnDiff  = CreateRectRgn(0, 0, 0, 0);
        CombineRgn(hRgnDiff, hRgnOuter, hRgnInner, RGN_DIFF);

        HBRUSH hBrush = CreateSolidBrush(this->GetRealBackColor());
        FillRgn(hdc, hRgnDiff, hBrush);

        DeleteObject(hRgnOuter);
        DeleteObject(hRgnInner);
        DeleteObject(hRgnDiff);
        DeleteObject(hBrush);
    }
}

// PanelBase.cpp

sw::PanelBase::PanelBase()
{
}

sw::PanelBase::~PanelBase()
{
}

sw::Size sw::PanelBase::MeasureOverride(const Size &availableSize)
{
    return this->Layer::MeasureOverride(availableSize);
}

void sw::PanelBase::ArrangeOverride(const Size &finalSize)
{
    this->Layer::ArrangeOverride(finalSize);
}

bool sw::PanelBase::OnVerticalScroll(int event, int pos)
{
    return this->Layer::OnVerticalScroll(event, pos);
}

bool sw::PanelBase::OnHorizontalScroll(int event, int pos)
{
    return this->Layer::OnHorizontalScroll(event, pos);
}

bool sw::PanelBase::OnNotified(NMHDR *pNMHDR, LRESULT &result)
{
    return this->Control::OnNotified(pNMHDR, result);
}

bool sw::PanelBase::OnKillFocus(HWND hNextFocus)
{
    return this->Control::OnKillFocus(hNextFocus);
}

void sw::PanelBase::OnTabStop()
{
    this->Control::OnTabStop();
}

void sw::PanelBase::OnEndPaint()
{
    this->Control::OnEndPaint();
}

bool sw::PanelBase::RequestBringIntoView(const sw::Rect &screenRect)
{
    return this->Layer::RequestBringIntoView(screenRect);
}

bool sw::PanelBase::OnRoutedEvent(RoutedEventArgs &eventArgs, const RoutedEventHandler &handler)
{
    return this->Layer::OnRoutedEvent(eventArgs, handler);
}

sw::Control *sw::PanelBase::ToControl()
{
    return this->Control::ToControl();
}

// PasswordBox.cpp

sw::PasswordBox::PasswordBox()
    : PasswordChar(
          // get
          [this]() -> wchar_t {
              return (wchar_t)this->SendMessageW(EM_GETPASSWORDCHAR, 0, 0);
          },
          // set
          [this](const wchar_t &value) {
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
    : x(Dip::PxToDipX(point.x)), y(Dip::PxToDipY(point.y))
{
}

sw::Point::operator POINT() const
{
    return {Dip::DipToPxX(this->x), Dip::DipToPxY(this->y)};
}

bool sw::Point::operator==(const Point &other) const
{
    return (this->x == other.x) && (this->y == other.y);
}

bool sw::Point::operator!=(const Point &other) const
{
    return (this->x != other.x) || (this->y != other.y);
}

std::wstring sw::Point::ToString() const
{
    return Utils::FormatStr(L"(%g, %g)", this->x, this->y);
}

// ProcMsg.cpp

sw::ProcMsg::ProcMsg()
    : ProcMsg(NULL, 0, 0, 0)
{
}

sw::ProcMsg::ProcMsg(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
          [this]() -> uint16_t {
              return (uint16_t)this->SendMessageW(PBM_GETRANGE, TRUE, 0);
          },
          // set
          [this](const uint16_t &value) {
              uint16_t maximum = this->Maximum;
              this->SendMessageW(PBM_SETRANGE, 0, MAKELPARAM(value, maximum));
          }),

      Maximum(
          // get
          [this]() -> uint16_t {
              return (uint16_t)this->SendMessageW(PBM_GETRANGE, FALSE, 0);
          },
          // set
          [this](const uint16_t &value) {
              uint16_t minimum = this->Minimum;
              this->SendMessageW(PBM_SETRANGE, 0, MAKELPARAM(minimum, value));
          }),

      Value(
          // get
          [this]() -> uint16_t {
              return (uint16_t)this->SendMessageW(PBM_GETPOS, 0, 0);
          },
          // set
          [this](const uint16_t &value) {
              this->SendMessageW(PBM_SETPOS, value, 0);
          }),

      State(
          // get
          [this]() -> ProgressBarState {
              return (ProgressBarState)this->SendMessageW(PBM_GETSTATE, 0, 0);
          },
          // set
          [this](const ProgressBarState &value) {
              this->SendMessageW(PBM_SETSTATE, (WPARAM)value, 0);
          }),

      Vertical(
          // get
          [this]() -> bool {
              return this->GetStyle(PBS_VERTICAL);
          },
          // set
          [this](const bool &value) {
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
    InitButtonBase(L"RadioButton", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | BS_NOTIFY | BS_AUTORADIOBUTTON, 0);
    Rect     = sw::Rect{0, 0, 100, 20};
    AutoSize = true;
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
    : left(Dip::PxToDipX(rect.left)),
      top(Dip::PxToDipY(rect.top)),
      width(Dip::PxToDipX(rect.right - rect.left)),
      height(Dip::PxToDipY(rect.bottom - rect.top))
{
}

sw::Rect::operator RECT() const
{
    return {Dip::DipToPxX(this->left),
            Dip::DipToPxY(this->top),
            Dip::DipToPxX(this->left + this->width),
            Dip::DipToPxY(this->top + this->height)};
}

sw::Point sw::Rect::GetPos() const
{
    return Point(this->left, this->top);
}

sw::Size sw::Rect::GetSize() const
{
    return Size(this->width, this->height);
}

bool sw::Rect::operator==(const Rect &other) const
{
    return (this->left == other.left) &&
           (this->top == other.top) &&
           (this->width == other.width) &&
           (this->height == other.height);
}

bool sw::Rect::operator!=(const Rect &other) const
{
    return (this->left != other.left) ||
           (this->top != other.top) ||
           (this->width != other.width) ||
           (this->height != other.height);
}

std::wstring sw::Rect::ToString() const
{
    return Utils::FormatStr(L"Rect{left=%g, top=%g, width=%g, height=%g}", this->left, this->top, this->width, this->height);
}

// RoutedEvent.cpp

sw::RoutedEventArgs::RoutedEventArgs(RoutedEventType eventType)
    : eventType(eventType)
{
}

// Screen.cpp

const sw::ReadOnlyProperty<double> sw::Screen::Width(
    []() -> double {
        return Dip::PxToDipX(GetSystemMetrics(SM_CXSCREEN));
    } //
);

const sw::ReadOnlyProperty<double> sw::Screen::Height(
    []() -> double {
        return Dip::PxToDipY(GetSystemMetrics(SM_CYSCREEN));
    } //
);

const sw::ReadOnlyProperty<sw::Point> sw::Screen::CursorPosition(
    []() -> sw::Point {
        POINT p;
        GetCursorPos(&p);
        return p;
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
    : width(Dip::PxToDipX(size.cx)), height(Dip::PxToDipY(size.cy))
{
}

sw::Size::operator SIZE() const
{
    return {Dip::DipToPxX(this->width), Dip::DipToPxY(this->height)};
}

bool sw::Size::operator==(const Size &other) const
{
    return (this->width == other.width) && (this->height == other.height);
}

bool sw::Size::operator!=(const Size &other) const
{
    return (this->width != other.width) || (this->height != other.height);
}

std::wstring sw::Size::ToString() const
{
    return Utils::FormatStr(L"Size{width=%g, height=%g}", this->width, this->height);
}

// Slider.cpp

sw::Slider::Slider()
    : Minimum(
          // get
          [this]() -> int {
              return (int)this->SendMessageW(TBM_GETRANGEMIN, 0, 0);
          },
          // set
          [this](const int &value) {
              this->SendMessageW(TBM_SETRANGEMIN, TRUE, value);
          }),

      Maximum(
          // get
          [this]() -> int {
              return (int)this->SendMessageW(TBM_GETRANGEMAX, 0, 0);
          },
          // set
          [this](const int &value) {
              this->SendMessageW(TBM_SETRANGEMAX, TRUE, value);
          }),

      Value(
          // get
          [this]() -> int {
              return (int)this->SendMessageW(TBM_GETPOS, 0, 0);
          },
          // set
          [this](const int &value) {
              this->SendMessageW(TBM_SETPOS, TRUE, value);
              this->OnValueChanged();
              this->OnEndTrack();
          }),

      Vertical(
          // get
          [this]() -> bool {
              return this->GetStyle(TBS_VERT);
          },
          // set
          [this](const bool &value) {
              this->SetStyle(TBS_VERT, value);
          }),

      ValueTooltips(
          // get
          [this]() -> bool {
              return this->GetStyle(TBS_TOOLTIPS);
          },
          // set
          [this](const bool &value) {
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

// Splitter.cpp

sw::Splitter::Splitter()
    : Orientation(
          // get
          [this]() -> sw::Orientation {
              return this->_orientation;
          },
          // set
          [this](const sw::Orientation &value) {
              if (this->_orientation != value) {
                  this->_orientation = value;
                  value == Orientation::Horizontal
                      ? this->SetAlignment(HorizontalAlignment::Stretch, VerticalAlignment::Center)
                      : this->SetAlignment(HorizontalAlignment::Center, VerticalAlignment::Stretch);
              }
          })
{
    this->Rect        = sw::Rect{0, 0, 10, 10};
    this->Transparent = true;
    this->SetAlignment(HorizontalAlignment::Stretch, VerticalAlignment::Center);
}

bool sw::Splitter::OnPaint()
{
    PAINTSTRUCT ps;

    HWND hwnd = this->Handle;
    HDC hdc   = BeginPaint(hwnd, &ps);

    RECT rect;
    GetClientRect(hwnd, &rect);

    HBRUSH hBrush = CreateSolidBrush(this->GetRealBackColor());
    FillRect(hdc, &rect, hBrush);

    if (this->_orientation == sw::Orientation::Horizontal) {
        // 在中间绘制横向分隔条
        rect.top += Utils::Max(0L, (rect.bottom - rect.top) / 2 - 1);
        DrawEdge(hdc, &rect, EDGE_ETCHED, BF_TOP);
    } else {
        // 在中间绘制纵向分隔条
        rect.left += Utils::Max(0L, (rect.right - rect.left) / 2 - 1);
        DrawEdge(hdc, &rect, EDGE_ETCHED, BF_LEFT);
    }

    DeleteObject(hBrush);
    EndPaint(hwnd, &ps);
    return true;
}

bool sw::Splitter::OnSize(Size newClientSize)
{
    InvalidateRect(this->Handle, NULL, FALSE);
    return this->UIElement::OnSize(newClientSize);
}

// StackLayout.cpp

sw::Size sw::StackLayout::MeasureOverride(const Size &availableSize)
{
    return this->orientation == Orientation::Horizontal
               ? this->StackLayoutH::MeasureOverride(availableSize)
               : this->StackLayoutV::MeasureOverride(availableSize);
}

void sw::StackLayout::ArrangeOverride(const Size &finalSize)
{
    this->orientation == Orientation::Horizontal
        ? this->StackLayoutH::ArrangeOverride(finalSize)
        : this->StackLayoutV::ArrangeOverride(finalSize);
}

// StackLayoutH.cpp

sw::Size sw::StackLayoutH::MeasureOverride(const Size &availableSize)
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

    return desireSize;
}

void sw::StackLayoutH::ArrangeOverride(const Size &finalSize)
{
    double width   = 0;
    int childCount = this->GetChildLayoutCount();

    for (int i = 0; i < childCount; ++i) {
        ILayout &item       = this->GetChildLayoutAt(i);
        Size itemDesireSize = item.GetDesireSize();
        item.Arrange(Rect{width, 0, itemDesireSize.width, finalSize.height});
        width += itemDesireSize.width;
    }
}

// StackLayoutV.cpp

sw::Size sw::StackLayoutV::MeasureOverride(const Size &availableSize)
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

    return desireSize;
}

void sw::StackLayoutV::ArrangeOverride(const Size &finalSize)
{
    double top     = 0;
    int childCount = this->GetChildLayoutCount();

    for (int i = 0; i < childCount; ++i) {
        ILayout &item       = this->GetChildLayoutAt(i);
        Size itemDesireSize = item.GetDesireSize();
        item.Arrange(Rect{0, top, finalSize.width, itemDesireSize.height});
        top += itemDesireSize.height;
    }
}

// StackPanel.cpp

sw::StackPanel::StackPanel()
    : Orientation(
          // get
          [this]() -> sw::Orientation {
              return this->_stackLayout.orientation;
          },
          // set
          [this](const sw::Orientation &value) {
              this->_stackLayout.orientation = value;
              this->InvalidateMeasure();
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
    : Notify(
          // get
          [this]() -> bool {
              return this->GetStyle(SS_NOTIFY);
          },
          // set
          [this](const bool &value) {
              this->SetStyle(SS_NOTIFY, value);
          })
{
    this->InitControl(L"STATIC", L"", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, 0);
}

// StatusBar.cpp

sw::StatusBar::StatusBar()
    : SizingGrip(
          // get
          [this]() -> bool {
              return this->GetStyle(SBARS_SIZEGRIP);
          },
          // set
          [this](const bool &value) {
              if (this->SizingGrip != value) {
                  auto style   = this->GetStyle();
                  auto exstyle = this->GetExtendedStyle();
                  this->ResetHandle(value ? (style | SBARS_SIZEGRIP) : (style & ~SBARS_SIZEGRIP), exstyle);
              }
          }),

      PartsCount(
          // get
          [this]() -> int {
              return (int)this->SendMessageW(SB_GETPARTS, 0, 0);
          }),

      UseUnicode(
          // get
          [this]() -> bool {
              return this->SendMessageW(SB_GETUNICODEFORMAT, 0, 0);
          },
          // set
          [this](const bool &value) {
              this->SendMessageW(SB_SETUNICODEFORMAT, value, 0);
          })
{
    this->InitControl(STATUSCLASSNAMEW, L"", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | CCS_NORESIZE, 0);
    this->Height = 25;
    this->SetAlignment(HorizontalAlignment::Stretch, VerticalAlignment::Bottom);
    this->Control::SetBackColor(KnownColor::Control, false);
}

bool sw::StatusBar::SetParts(std::initializer_list<double> parts)
{
    int count = (int)parts.size();

    if (count <= 0) {
        int tmp = -1;
        this->SendMessageW(SB_SETPARTS, 1, reinterpret_cast<LPARAM>(&tmp));
    }

    std::vector<int> vec;
    vec.reserve(count);

    int right = 0;
    for (double item : parts) {
        if (item == -1) {
            vec.push_back(-1);
            break;
        } else {
            right += Utils::Max(0, Dip::DipToPxX(item));
            vec.push_back(right);
            right += 2; // 分隔条
        }
    }

    return this->SendMessageW(SB_SETPARTS, vec.size(), reinterpret_cast<LPARAM>(vec.data()));
}

bool sw::StatusBar::GetTextAt(uint8_t index, std::wstring &out)
{
    int len = LOWORD(this->SendMessageW(SB_GETTEXTLENGTHW, index, 0));
    if (len <= 0) return false;

    out.resize(len + 1);
    this->SendMessageW(SB_GETTEXTW, index, reinterpret_cast<LPARAM>(&out[0]));
    out.resize(len);
    return true;
}

bool sw::StatusBar::SetTextAt(uint8_t index, const std::wstring &text)
{
    return this->SendMessageW(SB_SETTEXTW, index, reinterpret_cast<LPARAM>(text.c_str()));
}

bool sw::StatusBar::GetRectAt(uint8_t index, sw::Rect &out)
{
    RECT rect;
    if (this->SendMessageW(SB_GETRECT, index, reinterpret_cast<LPARAM>(&rect))) {
        out = rect;
        return true;
    }
    return false;
}

void sw::StatusBar::SetBackColor(Color color, bool redraw)
{
    this->Control::SetBackColor(color, false);
    this->SendMessageW(SB_SETBKCOLOR, 0, (COLORREF)color);
}

// SysLink.cpp

#if !defined(LM_GETIDEALSIZE)
#define LM_GETIDEALSIZE (LM_GETIDEALHEIGHT)
#endif

sw::SysLink::SysLink()
    : IgnoreReturn(
          // get
          [this]() -> bool {
              return this->GetStyle(LWS_IGNORERETURN);
          },
          // set
          [this](const bool &value) {
              this->SetStyle(LWS_IGNORERETURN, value);
          }),

      AutoSize(
          // get
          [this]() -> bool {
              return this->_autoSize;
          },
          // set
          [this](const bool &value) {
              if (this->_autoSize != value) {
                  this->_autoSize = value;
                  this->_UpdateLayoutFlags();
                  this->InvalidateMeasure();
              }
          })
{
    this->InitControl(WC_LINK, L"<a>SysLink</a>", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_TABSTOP, 0);
    this->_UpdateTextSize();
    this->_ResizeToTextSize();
    this->_UpdateLayoutFlags();
    this->Transparent      = true;
    this->InheritTextColor = true;
}

void sw::SysLink::OnTextChanged()
{
    this->_UpdateTextSize();
    this->Control::OnTextChanged();
}

void sw::SysLink::FontChanged(HFONT hfont)
{
    this->_UpdateTextSize();
    this->Control::FontChanged(hfont);
}

sw::Size sw::SysLink::MeasureOverride(const Size &availableSize)
{
    if (!this->_autoSize) {
        return this->UIElement::MeasureOverride(availableSize);
    }

    sw::Size desireSize = this->_textSize;

    if (availableSize.width < desireSize.width) {
        SIZE size;
        this->SendMessageW(LM_GETIDEALSIZE, Utils::Max(0, Dip::DipToPxX(availableSize.width)), reinterpret_cast<LPARAM>(&size));
        desireSize.width  = availableSize.width;
        desireSize.height = Dip::PxToDipY(size.cy);
    }
    return desireSize;
}

bool sw::SysLink::OnNotified(NMHDR *pNMHDR, LRESULT &result)
{
    switch (pNMHDR->code) {
        case NM_RETURN: {
            if (this->IgnoreReturn) {
                break;
            }
        }
        case NM_CLICK: {
            this->OnClicked(reinterpret_cast<NMLINK *>(pNMHDR));
            break;
        }
    }
    return this->Control::OnNotified(pNMHDR, result);
}

void sw::SysLink::OnClicked(NMLINK *pNMLink)
{
    SysLinkClickedEventArgs arg{pNMLink->item.szID, pNMLink->item.szUrl};
    this->RaiseRoutedEvent(arg);
}

void sw::SysLink::_UpdateTextSize()
{
    SIZE size;
    this->SendMessageW(LM_GETIDEALSIZE, INT_MAX, reinterpret_cast<LPARAM>(&size));
    this->_textSize = size;
}

void sw::SysLink::_ResizeToTextSize()
{
    this->Resize(this->_textSize);
}

void sw::SysLink::_UpdateLayoutFlags()
{
    constexpr auto flags =
        sw::LayoutUpdateCondition::TextChanged |
        sw::LayoutUpdateCondition::FontChanged;

    if (this->_autoSize) {
        this->LayoutUpdateCondition |= flags;
    } else {
        this->LayoutUpdateCondition &= ~flags;
    }
}

// TabControl.cpp

sw::TabControl::TabControl()
    : ContentRect(
          // get
          [this]() -> sw::Rect {
              RECT rect;
              GetClientRect(this->Handle, &rect);
              this->SendMessageW(TCM_ADJUSTRECT, FALSE, reinterpret_cast<LPARAM>(&rect));
              return rect;
          }),

      SelectedIndex(
          // get
          [this]() -> int {
              return (int)this->SendMessageW(TCM_GETCURSEL, 0, 0);
          },
          // set
          [this](const int &value) {
              if (this->SelectedIndex != value) {
                  this->SendMessageW(TCM_SETCURSEL, (WPARAM)value, 0);
                  this->OnSelectedIndexChanged();
              }
          }),

      Alignment(
          // get
          [this]() -> TabAlignment {
              auto style = this->GetStyle();
              if (style & TCS_VERTICAL) {
                  return (style & TCS_RIGHT) ? TabAlignment::Right : TabAlignment::Left;
              } else {
                  return (style & TCS_BOTTOM) ? TabAlignment::Bottom : TabAlignment::Top;
              }
          },
          // set
          [this](const TabAlignment &value) {
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
                      children.push_back(&this->GetChildAt(i));
                      this->RemoveChildAt(i);
                  }

                  this->ResetHandle();
                  for (int i = childCount - 1; i >= 0; --i) {
                      this->AddChild(children[i]);
                  }

                  this->SelectedIndex = selectedIndex;

              } else {
                  this->InvalidateMeasure();
              }
          }),

      MultiLine(
          // get
          [this]() -> bool {
              return this->GetStyle(TCS_MULTILINE);
          },
          // set
          [this](const bool &value) {
              this->SetStyle(TCS_MULTILINE, value);
              this->InvalidateMeasure();
          })
{
    this->InitControl(WC_TABCONTROLW, L"", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | TCS_TABS, 0);
    this->Rect = sw::Rect(0, 0, 200, 200);
    this->LayoutUpdateCondition |= sw::LayoutUpdateCondition::FontChanged;
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
        auto text    = this->GetChildAt(i).Text.Get();
        item.pszText = (LPWSTR)text.c_str();
        this->_SetItem(i, item);
    }

    this->Redraw();
}

void sw::TabControl::UpdateTabText(int index)
{
    if (index < 0) {
        return;
    }

    int childCount = this->ChildCount;
    int tabCount   = this->GetTabCount();

    if (index >= childCount || index >= tabCount) {
        return;
    }

    std::wstring text =
        this->GetChildAt(index).Text;

    TCITEMW item{};
    item.mask    = TCIF_TEXT;
    item.pszText = (LPWSTR)text.c_str();
    this->_SetItem(index, item);

    this->Redraw();
}

void sw::TabControl::OnAddedChild(UIElement &element)
{
    auto text = element.Text.Get();

    TCITEMW item{};
    item.mask    = TCIF_TEXT;
    item.pszText = (LPWSTR)text.c_str();

    int index = this->IndexOf(element);
    this->_InsertItem(index, item);
    ShowWindow(element.Handle, index == this->SelectedIndex ? SW_SHOW : SW_HIDE);

    this->UIElement::OnAddedChild(element);
}

void sw::TabControl::OnRemovedChild(UIElement &element)
{
    this->UpdateTab();
    this->_UpdateChildVisible();
    this->UIElement::OnRemovedChild(element);
}

void sw::TabControl::ArrangeOverride(const Size &finalSize)
{
    int selectedIndex = this->SelectedIndex;
    if (selectedIndex < 0 || selectedIndex >= this->ChildCount) return;

    UIElement &selectedItem = this->GetChildAt(selectedIndex);
    sw::Rect contentRect    = this->ContentRect;

    selectedItem.Measure(contentRect.GetSize());
    selectedItem.Arrange(contentRect);
}

bool sw::TabControl::OnNotified(NMHDR *pNMHDR, LRESULT &result)
{
    if (pNMHDR->code == TCN_SELCHANGE) {
        this->OnSelectedIndexChanged();
    }
    return this->Control::OnNotified(pNMHDR, result);
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
        auto &item = this->GetChildAt(i);
        HWND hwnd  = item.Handle;
        if (i != selectedIndex) {
            ShowWindow(hwnd, SW_HIDE);
        } else {
            sw::Rect contentRect = this->ContentRect;
            item.Measure(contentRect.GetSize());
            item.Arrange(contentRect);
            ShowWindow(hwnd, SW_SHOW);
        }
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
          [this]() -> bool {
              return this->_autoWrap;
          },
          // set
          [this](const bool &value) {
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
          [this]() -> bool {
              return this->GetStyle(ES_MULTILINE);
          },
          // set
          [this](const bool &value) {
              if (this->MultiLine != value) {
                  this->SetStyle(ES_MULTILINE, value);
                  this->SetStyle(ES_AUTOHSCROLL, !(value && this->_autoWrap));
                  this->ResetHandle();
              }
          }),

      HorizontalScrollBar(
          // get
          [this]() -> bool {
              return this->GetStyle(WS_HSCROLL);
          },
          // set
          [this](const bool &value) {
              if (this->HorizontalScrollBar != value) {
                  this->SetStyle(WS_HSCROLL, value);
                  this->ResetHandle();
              }
          }),

      VerticalScrollBar(
          // get
          [this]() -> bool {
              return this->GetStyle(WS_VSCROLL);
          },
          // set
          [this](const bool &value) {
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
          [this]() -> bool {
              return this->GetStyle(ES_READONLY);
          },
          // set
          [this](const bool &value) {
              this->SendMessageW(EM_SETREADONLY, value, 0);
          }),

      HorizontalContentAlignment(
          // get
          [this]() -> sw::HorizontalAlignment {
              LONG_PTR style = this->GetStyle();
              if (style & ES_CENTER) {
                  return sw::HorizontalAlignment::Center;
              } else if (style & ES_RIGHT) {
                  return sw::HorizontalAlignment::Right;
              } else {
                  return sw::HorizontalAlignment::Left;
              }
          },
          // set
          [this](const sw::HorizontalAlignment &value) {
              switch (value) {
                  case sw::HorizontalAlignment::Left: {
                      this->SetStyle(ES_CENTER | ES_RIGHT, false);
                      break;
                  }
                  case sw::HorizontalAlignment::Center: {
                      DWORD style = this->GetStyle();
                      style &= ~(ES_CENTER | ES_RIGHT);
                      style |= ES_CENTER;
                      this->SetStyle(style);
                      break;
                  }
                  case sw::HorizontalAlignment::Right: {
                      DWORD style = this->GetStyle();
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
          [this]() -> bool {
              return this->SendMessageW(EM_CANUNDO, 0, 0);
          }),

      AcceptTab(
          // get
          [this]() -> bool {
              return this->_acceptTab;
          },
          // set
          [this](const bool &value) {
              this->_acceptTab = value;
          })
{
    this->TabStop = true;
}

sw::TextBoxBase::~TextBoxBase()
{
}

void sw::TextBoxBase::InitTextBoxBase(DWORD dwStyle, DWORD dwExStyle)
{
    this->InitControl(L"EDIT", L"", dwStyle, dwExStyle);
}

std::wstring &sw::TextBoxBase::GetInternalText()
{
    if (this->_isTextChanged) {
        this->UpdateInternalText();
        this->_isTextChanged = false;
    }
    return this->WndBase::GetInternalText();
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
        bool shiftDown = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
        shiftDown ? this->SetPreviousTabStopFocus() : this->SetNextTabStopFocus();
    }

    return e.handledMsg;
}

void sw::TextBoxBase::OnDrawFocusRect(HDC hdc)
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
    this->Text = std::wstring{};
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

sw::Thickness::Thickness(const RECT &rect)
    : Thickness(
          Dip::PxToDipX(rect.left),
          Dip::PxToDipY(rect.top),
          Dip::PxToDipX(rect.right),
          Dip::PxToDipY(rect.bottom))
{
}

sw::Thickness::operator RECT() const
{
    return RECT{
        Dip::DipToPxX(this->left),
        Dip::DipToPxY(this->top),
        Dip::DipToPxX(this->right),
        Dip::DipToPxY(this->bottom)};
}

bool sw::Thickness::operator==(const Thickness &other) const
{
    return (this->left == other.left) &&
           (this->top == other.top) &&
           (this->right == other.right) &&
           (this->bottom == other.bottom);
}

bool sw::Thickness::operator!=(const Thickness &other) const
{
    return (this->left != other.left) ||
           (this->top != other.top) ||
           (this->right != other.right) ||
           (this->bottom != other.bottom);
}

std::wstring sw::Thickness::ToString() const
{
    return Utils::FormatStr(L"Thickness{left=%g, top=%g, right=%g, bottom=%g}", this->left, this->top, this->right, this->bottom);
}

// Timer.cpp

namespace
{
    /**
     * @brief 窗口句柄保存Timer指针的属性名称
     */
    constexpr wchar_t _TimerPtrProp[] = L"SWPROP_TimerPtr";
}

sw::Timer::Timer()
    : Interval(
          // get
          [this]() -> uint32_t {
              return this->_interval;
          },
          // set
          [this](const uint32_t &value) {
              this->_interval = value;
              if (this->_started) {
                  this->Stop();
                  this->Start();
              }
          })
{
    this->InitControl(L"STATIC", L"", WS_CHILD, 0);
    Timer::_SetTimerPtr(this->Handle, *this);
}

void sw::Timer::Start()
{
    if (!this->_started) {
        this->_started = true;
        SetTimer(this->Handle, 0, this->_interval, &Timer::_TimerProc);
    }
}

void sw::Timer::Stop()
{
    if (this->_started) {
        this->_started = false;
        KillTimer(this->Handle, 0);
    }
}

void sw::Timer::OnTick()
{
    if (this->Tick)
        this->Tick(*this);
}

sw::Timer *sw::Timer::_GetTimerPtr(HWND hwnd)
{
    return reinterpret_cast<Timer *>(GetPropW(hwnd, _TimerPtrProp));
}

void sw::Timer::_SetTimerPtr(HWND hwnd, Timer &timer)
{
    SetPropW(hwnd, _TimerPtrProp, reinterpret_cast<HANDLE>(&timer));
}

void sw::Timer::_TimerProc(HWND hwnd, UINT msg, UINT_PTR idTimer, DWORD time)
{
    if (msg == WM_TIMER) {
        auto timer = Timer::_GetTimerPtr(hwnd);
        if (timer) timer->OnTick();
    }
}

// ToolTip.cpp

sw::ToolTip::ToolTip()
    : ToolTip(WS_POPUP)
{
}

sw::ToolTip::ToolTip(DWORD style)
    : InitialDelay(
          // get
          [this]() -> int {
              return int(this->SendMessageW(TTM_GETDELAYTIME, TTDT_INITIAL, 0));
          },
          // set
          [this](const int &value) {
              this->SendMessageW(TTM_SETDELAYTIME, TTDT_AUTOMATIC, static_cast<LPARAM>(value));
          }),

      ToolTipIcon(
          // get
          [this]() -> sw::ToolTipIcon {
              return this->_icon;
          },
          // set
          [this](const sw::ToolTipIcon &value) {
              this->_icon = value;
              this->_UpdateIconAndTitle();
          }),

      ToolTipTitle(
          // get
          [this]() -> std::wstring {
              return this->_title;
          },
          // set
          [this](const std::wstring &value) {
              this->_title = value;
              this->_UpdateIconAndTitle();
          }),

      MaxTipWidth(
          // get
          [this]() -> double {
              int w = int(this->SendMessageW(TTM_GETMAXTIPWIDTH, 0, 0));
              return (w == -1) ? -1 : Dip::PxToDipX(w);
          },
          // set
          [this](const double &value) {
              int w = value < 0 ? -1 : Dip::DipToPxX(value);
              this->SendMessageW(TTM_SETMAXTIPWIDTH, 0, w);
          })
{
    this->InitControl(TOOLTIPS_CLASSW, L"", style, 0);
}

bool sw::ToolTip::SetToolTip(HWND hwnd, const std::wstring &tooltip)
{
    TOOLINFOW info{};
    info.cbSize = sizeof(info);
    info.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
    info.hwnd   = hwnd;
    info.uId    = reinterpret_cast<UINT_PTR>(hwnd);

    if (this->SendMessageW(TTM_GETTOOLINFOW, 0, reinterpret_cast<LPARAM>(&info))) {
        info.lpszText = const_cast<LPWSTR>(tooltip.c_str());
        this->SendMessageW(TTM_UPDATETIPTEXTW, 0, reinterpret_cast<LPARAM>(&info));
        return true;
    } else {
        info.lpszText = const_cast<LPWSTR>(tooltip.c_str());
        return this->SendMessageW(TTM_ADDTOOLW, 0, reinterpret_cast<LPARAM>(&info));
    }
}

bool sw::ToolTip::SetToolTip(const WndBase &wnd, const std::wstring &tooltip)
{
    return this->SetToolTip(wnd.Handle, tooltip);
}

void sw::ToolTip::RemoveAll()
{
    TOOLINFOW info{};
    info.cbSize = sizeof(info);
    while (this->SendMessageW(TTM_ENUMTOOLSW, 0, reinterpret_cast<LPARAM>(&info)))
        this->SendMessageW(TTM_DELTOOLW, 0, reinterpret_cast<LPARAM>(&info));
}

void sw::ToolTip::_UpdateIconAndTitle()
{
    this->SendMessageW(TTM_SETTITLEW, static_cast<WPARAM>(this->_icon), reinterpret_cast<LPARAM>(this->_title.c_str()));
}

sw::BallonToolTip::BallonToolTip()
    : ToolTip(WS_POPUP | TTS_BALLOON)
{
}

// UIElement.cpp

sw::UIElement::UIElement()
    : Margin(
          // get
          [this]() -> Thickness {
              return this->_margin;
          },
          // set
          [this](const Thickness &value) {
              this->_margin = value;
              this->InvalidateMeasure();
          }),

      HorizontalAlignment(
          // get
          [this]() -> sw::HorizontalAlignment {
              return this->_horizontalAlignment;
          },
          // set
          [this](const sw::HorizontalAlignment &value) {
              if (this->_SetHorzAlignment(value)) {
                  this->InvalidateMeasure();
              }
          }),

      VerticalAlignment(
          // get
          [this]() -> sw::VerticalAlignment {
              return this->_verticalAlignment;
          },
          // set
          [this](const sw::VerticalAlignment &value) {
              if (this->_SetVertAlignment(value)) {
                  this->InvalidateMeasure();
              }
          }),

      ChildCount(
          // get
          [this]() -> int {
              return (int)this->_children.size();
          }),

      CollapseWhenHide(
          // get
          [this]() -> bool {
              return this->_collapseWhenHide;
          },
          // set
          [this](const bool &value) {
              if (this->_collapseWhenHide != value) {
                  this->_collapseWhenHide = value;
                  if (this->_parent && !this->Visible) {
                      this->_parent->_UpdateLayoutVisibleChildren();
                      this->_parent->InvalidateMeasure();
                  }
              }
          }),

      Parent(
          // get
          [this]() -> UIElement * {
              return this->_parent;
          }),

      Tag(
          // get
          [this]() -> uint64_t {
              return this->_tag;
          },
          // set
          [this](const uint64_t &value) {
              this->_tag = value;
          }),

      LayoutTag(
          // get
          [this]() -> uint64_t {
              return this->_layoutTag;
          },
          // set
          [this](const uint64_t &value) {
              this->_layoutTag = value;
              this->InvalidateMeasure();
          }),

      ContextMenu(
          // get
          [this]() -> sw::ContextMenu * {
              return this->_contextMenu;
          },
          // set
          [this](sw::ContextMenu *value) {
              this->_contextMenu = value;
          }),

      Float(
          // get
          [this]() -> bool {
              return this->_float;
          },
          // set
          [this](const bool &value) {
              if (this->_float != value) {
                  this->_float = value;
                  this->UpdateSiblingsZOrder();
              }
          }),

      TabStop(
          // get
          [this]() -> bool {
              return this->_tabStop;
          },
          // set
          [this](const bool &value) {
              this->_tabStop = value;
          }),

      BackColor(
          // get
          [this]() -> Color {
              return this->_backColor;
          },
          // set
          [this](const Color &value) {
              this->_transparent = false;
              this->SetBackColor(value, true);
          }),

      TextColor(
          // get
          [this]() -> Color {
              return this->_textColor;
          },
          // set
          [this](const Color &value) {
              this->_inheritTextColor = false;
              this->SetTextColor(value, true);
          }),

      Transparent(
          // get
          [this]() -> bool {
              return this->_transparent;
          },
          // set
          [this](const bool &value) {
              this->_transparent = value;
              this->Redraw();
          }),

      InheritTextColor(
          // get
          [this]() -> bool {
              return this->_inheritTextColor;
          },
          // set
          [this](const bool &value) {
              this->_inheritTextColor = value;
              this->Redraw();
          }),

      LayoutUpdateCondition(
          // get
          [this]() -> sw::LayoutUpdateCondition {
              return this->_layoutUpdateCondition;
          },
          // set
          [this](const sw::LayoutUpdateCondition &value) {
              this->_layoutUpdateCondition = value;
          }),

      IsMeasureValid(
          // get
          [this]() -> bool {
              return !this->IsLayoutUpdateConditionSet(sw::LayoutUpdateCondition::MeasureInvalidated);
          }),

      MinWidth(
          // get
          [this]() -> double {
              return this->_minSize.width;
          },
          // set
          [this](const double &value) {
              if (this->_minSize.width != value) {
                  this->_minSize.width = value;
                  this->OnMinMaxSizeChanged();
              }
          }),

      MinHeight(
          // get
          [this]() -> double {
              return this->_minSize.height;
          },
          // set
          [this](const double &value) {
              if (this->_minSize.height != value) {
                  this->_minSize.height = value;
                  this->OnMinMaxSizeChanged();
              }
          }),

      MaxWidth(
          // get
          [this]() -> double {
              return this->_maxSize.width;
          },
          // set
          [this](const double &value) {
              if (this->_maxSize.width != value) {
                  this->_maxSize.width = value;
                  this->OnMinMaxSizeChanged();
              }
          }),

      MaxHeight(
          // get
          [this]() -> double {
              return this->_maxSize.height;
          },
          // set
          [this](const double &value) {
              if (this->_maxSize.height != value) {
                  this->_maxSize.height = value;
                  this->OnMinMaxSizeChanged();
              }
          })
{
}

sw::UIElement::~UIElement()
{
    // 将自己从父窗口的children中移除
    this->SetParent(nullptr);

    // 释放资源
    if (this->_hCtlColorBrush != NULL) {
        DeleteObject(this->_hCtlColorBrush);
    }
}

void sw::UIElement::RegisterRoutedEvent(RoutedEventType eventType, const RoutedEventHandler &handler)
{
    this->_eventMap[eventType] = handler;
}

void sw::UIElement::AddHandler(RoutedEventType eventType, const RoutedEventHandler &handler)
{
    if (handler) this->_eventMap[eventType] += handler;
}

bool sw::UIElement::RemoveHandler(RoutedEventType eventType, const RoutedEventHandler &handler)
{
    return handler == nullptr ? false : this->_eventMap[eventType].Remove(handler);
}

void sw::UIElement::UnregisterRoutedEvent(RoutedEventType eventType)
{
    this->_eventMap[eventType] = nullptr;
}

bool sw::UIElement::IsRoutedEventRegistered(RoutedEventType eventType)
{
    return this->_eventMap[eventType] != nullptr;
}

sw::UIElement &sw::UIElement::operator[](int index) const
{
    return *this->_children[index];
}

sw::UIElement &sw::UIElement::GetChildAt(int index) const
{
    return *this->_children.at(index);
}

bool sw::UIElement::AddChild(UIElement *element)
{
    if (element == nullptr || element == this) {
        return false;
    }

    if (!this->CheckAccess(*element)) {
        return false; // 父子元素必须在同一线程创建
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

    // 处理z轴顺序，确保悬浮的元素在最前
    if (!element->_float) {
        HDWP hdwp = BeginDeferWindowPos((int)this->_children.size());
        for (UIElement *child : this->_children) {
            if (child->_float)
                DeferWindowPos(hdwp, child->Handle, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        }
        EndDeferWindowPos(hdwp);
    }

    this->_children.push_back(element);
    this->_UpdateLayoutVisibleChildren();

    this->OnAddedChild(*element);
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
    this->_UpdateLayoutVisibleChildren();

    this->OnRemovedChild(*element);
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
    this->_UpdateLayoutVisibleChildren();

    this->OnRemovedChild(*element);
    return true;
}

bool sw::UIElement::RemoveChild(UIElement &element)
{
    return this->RemoveChild(&element);
}

void sw::UIElement::ClearChildren()
{
    if (this->_children.empty()) {
        return;
    }

    this->_layoutUpdateCondition |= sw::LayoutUpdateCondition::Supressed;

    while (!this->_children.empty()) {
        UIElement *item = this->_children.back();
        item->WndBase::SetParent(nullptr);
        this->_children.pop_back();
        this->OnRemovedChild(*item);
    }

    this->_layoutUpdateCondition &= ~sw::LayoutUpdateCondition::Supressed;
    this->_UpdateLayoutVisibleChildren();

    if (this->IsLayoutUpdateConditionSet(sw::LayoutUpdateCondition::ChildRemoved)) {
        this->InvalidateMeasure();
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

    HDWP hdwp = BeginDeferWindowPos((int)parent->_children.size());

    parent->_children.erase(parent->_children.begin() + index);
    parent->_children.push_back(this);
    DeferWindowPos(hdwp, this->Handle, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    if (!this->_float) {
        for (UIElement *item : parent->_children) {
            if (item->_float)
                DeferWindowPos(hdwp, item->Handle, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        }
    }
    EndDeferWindowPos(hdwp);
    parent->_UpdateLayoutVisibleChildren();
    parent->InvalidateMeasure();
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
        HDWP hdwp = BeginDeferWindowPos((int)parent->_children.size());
        for (UIElement *item : parent->_children) {
            if (item->_float)
                DeferWindowPos(hdwp, item->Handle, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        }
        EndDeferWindowPos(hdwp);
    } else {
        SetWindowPos(this->Handle, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }
    parent->_UpdateLayoutVisibleChildren();
    parent->InvalidateMeasure();
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

sw::UIElement *sw::UIElement::GetPreviousElement()
{
    return _GetPreviousElement(this);
}

sw::UIElement *sw::UIElement::GetNextTabStopElement()
{
    UIElement *element = this;
    do {
        element = element->GetNextElement();
    } while (element != nullptr && element != this &&
             !(element->_tabStop && element->IsVisible() && element->Enabled));
    return element;
}

sw::UIElement *sw::UIElement::GetPreviousTabStopElement()
{
    UIElement *element = this;
    do {
        element = element->GetPreviousElement();
    } while (element != nullptr && element != this &&
             !(element->_tabStop && element->IsVisible() && element->Enabled));
    return element;
}

sw::Color sw::UIElement::GetRealBackColor()
{
    UIElement *p = this;
    while (p->_transparent && p->_parent != nullptr) {
        p = p->_parent;
    }
    return p->_backColor;
}

sw::Color sw::UIElement::GetRealTextColor()
{
    UIElement *p = this;
    while (p->_inheritTextColor && p->_parent != nullptr) {
        p = p->_parent;
    }
    return p->_textColor;
}

void sw::UIElement::SetCursor(HCURSOR hCursor)
{
    this->_hCursor          = hCursor;
    this->_useDefaultCursor = false;
}

void sw::UIElement::SetCursor(StandardCursor cursor)
{
    this->SetCursor(CursorHelper::GetCursorHandle(cursor));
}

void sw::UIElement::ResetCursor()
{
    this->_hCursor          = NULL;
    this->_useDefaultCursor = true;
}

void sw::UIElement::SetAlignment(sw::HorizontalAlignment horz, sw::VerticalAlignment vert)
{
    bool changed = false;

    changed |= this->_SetHorzAlignment(horz);
    changed |= this->_SetVertAlignment(vert);

    if (changed) {
        this->InvalidateMeasure();
    }
}

void sw::UIElement::Resize(const Size &size)
{
    this->_origionalSize = size;

    SetWindowPos(this->Handle, NULL,
                 0, 0, Dip::DipToPxX(size.width), Dip::DipToPxY(size.height),
                 SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
}

bool sw::UIElement::IsLayoutUpdateConditionSet(sw::LayoutUpdateCondition condition)
{
    return (this->_layoutUpdateCondition & condition) == condition;
}

void sw::UIElement::InvalidateMeasure()
{
    if (this->IsLayoutUpdateConditionSet(sw::LayoutUpdateCondition::Supressed)) {
        return;
    }

    UIElement *root;
    UIElement *element = this;
    do {
        root = element;
        element->_SetMeasureInvalidated();
        element = element->_parent;
    } while (element != nullptr);

    root->SendMessageW(WM_UpdateLayout, 0, 0);
}

bool sw::UIElement::BringIntoView()
{
    UIElement *p = this->_parent;
    if (this->_float || p == nullptr) return false;

    sw::Rect rect = this->Rect;
    sw::Point pos = p->PointToScreen(rect.GetPos());

    rect.left = pos.x - this->_margin.left;
    rect.top  = pos.y - this->_margin.top;
    rect.width += this->_margin.left + this->_margin.right;
    rect.height += this->_margin.top + this->_margin.bottom;

    for (; p != nullptr; p = p->_parent) {
        rect.left -= p->_arrangeOffsetX;
        rect.top -= p->_arrangeOffsetY;
        if (p->RequestBringIntoView(rect)) return true;
        if (p->_float) return false;
    }
    return false;
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
    return (int)this->_layoutVisibleChildren.size();
}

sw::ILayout &sw::UIElement::GetChildLayoutAt(int index)
{
    return *this->_layoutVisibleChildren[index];
}

sw::Size sw::UIElement::GetDesireSize()
{
    return this->_desireSize;
}

void sw::UIElement::Measure(const Size &availableSize)
{
    if (!this->IsLayoutUpdateConditionSet(sw::LayoutUpdateCondition::MeasureInvalidated) &&
        this->_lastMeasureAvailableSize == availableSize) {
        return; // 若布局未失效且可用尺寸没有变化，则无需重新测量
    }

    Size measureSize    = availableSize;
    Thickness &margin   = this->_margin;
    sw::Rect windowRect = this->Rect;
    sw::Rect clientRect = this->ClientRect;

    // 考虑边框
    measureSize.width -= (windowRect.width - clientRect.width) + margin.left + margin.right;
    measureSize.height -= (windowRect.height - clientRect.height) + margin.top + margin.bottom;

    // 由子类实现MeasureOverride函数来计算内容所需的尺寸
    this->_desireSize = this->MeasureOverride(measureSize);
    this->_desireSize.width += windowRect.width - clientRect.width;
    this->_desireSize.height += windowRect.height - clientRect.height;

    // 限制尺寸在最小和最大尺寸之间
    this->ClampDesireSize(this->_desireSize);
    this->_desireSize.width += margin.left + margin.right;
    this->_desireSize.height += margin.top + margin.bottom;

    // 更新_lastMeasureAvailableSize
    this->_lastMeasureAvailableSize = availableSize;
}

void sw::UIElement::Arrange(const sw::Rect &finalPosition)
{
    // 为什么在Arrange阶段清除MeasureInvalidated标记：
    // 一些复杂的布局可能会在测量阶段多次调用Measure函数，
    // 比如Grid会调用两次Measure，若在测量阶段清除该标记，
    // 则在第二次调用时会认为布局没有失效，导致测量被跳过。
    this->_layoutUpdateCondition &= ~sw::LayoutUpdateCondition::MeasureInvalidated;
    this->_layoutUpdateCondition |= sw::LayoutUpdateCondition::Supressed;

    Size &desireSize  = this->_desireSize;
    Thickness &margin = this->_margin;

    sw::Rect rect;
    rect.width  = desireSize.width - margin.left - margin.right;
    rect.height = desireSize.height - margin.top - margin.bottom;

    if (this->_horizontalAlignment == HorizontalAlignment::Stretch) {
        rect.width = finalPosition.width - margin.left - margin.right;
    }

    if (this->_verticalAlignment == VerticalAlignment::Stretch) {
        rect.height = finalPosition.height - margin.top - margin.bottom;
    }

    this->ClampDesireSize(rect);

    switch (this->_horizontalAlignment) {
        case HorizontalAlignment::Center:
        case HorizontalAlignment::Stretch: {
            rect.left = finalPosition.left + (finalPosition.width - rect.width - margin.left - margin.right) / 2 + margin.left;
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
        case VerticalAlignment::Center:
        case VerticalAlignment::Stretch: {
            rect.top = finalPosition.top + (finalPosition.height - rect.height - margin.top - margin.bottom) / 2 + margin.top;
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

    rect.width  = Utils::Max(0.0, rect.width);
    rect.height = Utils::Max(0.0, rect.height);

    bool hasChildren = !this->_children.empty();
    HDWP hdwpCurrent = this->_parent ? this->_parent->_hdwpChildren : NULL;

    if (!hasChildren && hdwpCurrent != NULL) {
        DeferWindowPos(hdwpCurrent, this->Handle, NULL,
                       Dip::DipToPxX(rect.left), Dip::DipToPxY(rect.top),
                       Dip::DipToPxX(rect.width), Dip::DipToPxY(rect.height),
                       SWP_NOACTIVATE | SWP_NOZORDER);
    } else {
        SetWindowPos(this->Handle, NULL,
                     Dip::DipToPxX(rect.left), Dip::DipToPxY(rect.top),
                     Dip::DipToPxX(rect.width), Dip::DipToPxY(rect.height),
                     SWP_NOACTIVATE | SWP_NOZORDER);
    }

    if (hasChildren) {
        if (this->_children.size() >= 3) {
            this->_hdwpChildren = BeginDeferWindowPos((int)this->_children.size());
        }

        this->ArrangeOverride(this->ClientRect->GetSize());

        if (this->_hdwpChildren != NULL) {
            EndDeferWindowPos(this->_hdwpChildren);
            this->_hdwpChildren = NULL;
        }
    }

    this->_layoutUpdateCondition &= ~sw::LayoutUpdateCondition::Supressed;
}

sw::UIElement *sw::UIElement::ToUIElement()
{
    return this;
}

void sw::UIElement::RaiseRoutedEvent(RoutedEventType eventType)
{
    RoutedEventArgs eventArgs(eventType);
    this->RaiseRoutedEvent(eventArgs);
}

void sw::UIElement::RaiseRoutedEvent(RoutedEventArgs &eventArgs)
{
    eventArgs.originalSource = this;

    if (eventArgs.source == nullptr) {
        eventArgs.source = this;
    }

    UIElement *element = this;
    do {
        auto &handler = element->_eventMap[eventArgs.eventType];
        if (!element->OnRoutedEvent(eventArgs, handler)) {
            if (handler) handler(*element, eventArgs);
        }
        if (eventArgs.handled) {
            break;
        } else {
            element = element->_parent;
        }
    } while (element != nullptr);
}

double &sw::UIElement::GetInternalArrangeOffsetX()
{
    return this->_arrangeOffsetX;
}

double &sw::UIElement::GetInternalArrangeOffsetY()
{
    return this->_arrangeOffsetY;
}

double sw::UIElement::GetChildRightmost(bool update)
{
    if (update) {
        this->_childRightmost = 0;
        for (UIElement *item : this->_layoutVisibleChildren) {
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
        for (UIElement *item : this->_layoutVisibleChildren) {
            if (item->_float) continue;
            this->_childBottommost = Utils::Max(this->_childBottommost, item->Top + item->Height + item->_margin.bottom - this->_arrangeOffsetY);
        }
    }
    return this->_childBottommost;
}

void sw::UIElement::UpdateChildrenZOrder(bool invalidateMeasure)
{
    int childCount = (int)this->_children.size();
    if (childCount < 2) return;

    std::deque<HWND> floatingElements;
    HDWP hdwp = BeginDeferWindowPos(childCount);

    for (UIElement *child : this->_children) {
        HWND hwnd = child->Handle;
        if (child->_float) {
            floatingElements.push_back(hwnd);
        } else {
            DeferWindowPos(hdwp, hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        }
    }

    while (!floatingElements.empty()) {
        DeferWindowPos(hdwp, floatingElements.front(), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        floatingElements.pop_front();
    }

    EndDeferWindowPos(hdwp);

    if (invalidateMeasure) {
        this->InvalidateMeasure();
    }
}

void sw::UIElement::UpdateSiblingsZOrder(bool invalidateMeasure)
{
    if (this->_parent != nullptr) {
        this->_parent->UpdateChildrenZOrder(invalidateMeasure);
    }
}

void sw::UIElement::SetNextTabStopFocus()
{
    UIElement *next = this->GetNextTabStopElement();
    if (next && next != this) next->OnTabStop();
}

void sw::UIElement::SetPreviousTabStopFocus()
{
    UIElement *previous = this->GetPreviousTabStopElement();
    if (previous && previous != this) previous->OnTabStop();
}

void sw::UIElement::ClampDesireSize(sw::Size &size)
{
    if (this->_minSize.width > 0) {
        size.width = Utils::Max(size.width, this->_minSize.width);
    }
    if (this->_minSize.height > 0) {
        size.height = Utils::Max(size.height, this->_minSize.height);
    }
    if (this->_maxSize.width > 0) {
        size.width = Utils::Min(size.width, this->_maxSize.width);
    }
    if (this->_maxSize.height > 0) {
        size.height = Utils::Min(size.height, this->_maxSize.height);
    }
}

void sw::UIElement::ClampDesireSize(sw::Rect &rect)
{
    auto size = rect.GetSize();
    this->ClampDesireSize(size);
    rect.width  = size.width;
    rect.height = size.height;
}

bool sw::UIElement::QueryAllChildren(const Func<UIElement *, bool> &queryFunc)
{
    if (queryFunc == nullptr) {
        return true;
    }

    std::vector<UIElement *> children;
    _GetAllChildren(this, children);

    for (UIElement *child : children) {
        if (!queryFunc(child)) return false;
    }
    return true;
}

sw::Size sw::UIElement::MeasureOverride(const Size &availableSize)
{
    // 普通元素测量时，其本身用户区尺寸即为所需尺寸
    // 当元素的对齐方式为拉伸时，返回原始用户区尺寸
    sw::Size desireSize = this->ClientRect->GetSize();

    // 由于Measure中会自动处理边框和Margin而_origionalSize
    // 记录的是原始的窗口尺寸，因此当使用原始尺寸时需要减去边框
    if (this->_horizontalAlignment == HorizontalAlignment::Stretch ||
        this->_verticalAlignment == VerticalAlignment::Stretch) {
        sw::Rect windowRect = this->Rect;
        sw::Size clientSize = desireSize;
        if (this->_horizontalAlignment == HorizontalAlignment::Stretch) {
            desireSize.width = this->_origionalSize.width - (windowRect.width - clientSize.width);
        }
        if (this->_verticalAlignment == VerticalAlignment::Stretch) {
            desireSize.height = this->_origionalSize.height - (windowRect.height - clientSize.height);
        }
    }
    return desireSize;
}

void sw::UIElement::ArrangeOverride(const Size &finalSize)
{
}

void sw::UIElement::SetBackColor(Color color, bool redraw)
{
    this->_backColor = color;
    if (redraw) this->Redraw();
}

void sw::UIElement::SetTextColor(Color color, bool redraw)
{
    this->_textColor = color;
    if (redraw) this->Redraw();
}

bool sw::UIElement::RequestBringIntoView(const sw::Rect &screenRect)
{
    return false;
}

void sw::UIElement::OnAddedChild(UIElement &element)
{
    if (this->IsLayoutUpdateConditionSet(sw::LayoutUpdateCondition::ChildAdded)) {
        this->InvalidateMeasure();
    }
}

void sw::UIElement::OnRemovedChild(UIElement &element)
{
    if (this->IsLayoutUpdateConditionSet(sw::LayoutUpdateCondition::ChildRemoved)) {
        this->InvalidateMeasure();
    }
}

void sw::UIElement::OnTabStop()
{
    this->Focused = true;
    this->BringIntoView();
}

void sw::UIElement::OnMinMaxSizeChanged()
{
    this->InvalidateMeasure();
}

bool sw::UIElement::OnRoutedEvent(RoutedEventArgs &eventArgs, const RoutedEventHandler &handler)
{
    return false;
}

bool sw::UIElement::SetParent(WndBase *parent)
{
    UIElement *oldParentElement = this->_parent;
    UIElement *newParentElement = parent ? parent->ToUIElement() : nullptr;

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
                // 前面调用RemoveChild失败，当前元素仍在父元素的_layoutVisibleChildren中，此处手动调用更新
                oldParentElement->_UpdateLayoutVisibleChildren();
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
    this->_parent = newParent ? newParent->ToUIElement() : nullptr;
    this->_SetMeasureInvalidated();
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

    if (this->IsLayoutUpdateConditionSet(sw::LayoutUpdateCondition::PositionChanged)) {
        this->InvalidateMeasure();
    }
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

    if (this->IsLayoutUpdateConditionSet(sw::LayoutUpdateCondition::SizeChanged)) {
        this->InvalidateMeasure();
    }
    return args.handledMsg;
}

void sw::UIElement::OnTextChanged()
{
    this->RaiseRoutedEvent(UIElement_TextChanged);

    if (this->IsLayoutUpdateConditionSet(sw::LayoutUpdateCondition::TextChanged)) {
        this->InvalidateMeasure();
    }
}

void sw::UIElement::FontChanged(HFONT hfont)
{
    if (this->IsLayoutUpdateConditionSet(sw::LayoutUpdateCondition::FontChanged)) {
        this->InvalidateMeasure();
    }
}

void sw::UIElement::VisibleChanged(bool newVisible)
{
    if (this->_parent && this->_collapseWhenHide) {
        this->_parent->_UpdateLayoutVisibleChildren();
    }
    if (newVisible || this->_collapseWhenHide) {
        this->InvalidateMeasure(); // visible变为true，或者需要折叠隐藏时，更新布局
    }
}

bool sw::UIElement::OnSetFocus(HWND hPrevFocus)
{
    TypedRoutedEventArgs<UIElement_GotFocus> args;
    this->RaiseRoutedEvent(args);
    return args.handledMsg;
}

bool sw::UIElement::OnKillFocus(HWND hNextFocus)
{
    TypedRoutedEventArgs<UIElement_LostFocus> args;
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
        bool shiftDown = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
        shiftDown ? this->SetPreviousTabStopFocus() : this->SetNextTabStopFocus();
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
    TypedRoutedEventArgs<UIElement_MouseLeave> args;
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

bool sw::UIElement::OnColor(HDC hdc, HBRUSH &hRetBrush)
{
    COLORREF textColor = this->GetRealTextColor();
    COLORREF backColor = this->GetRealBackColor();

    ::SetTextColor(hdc, textColor);
    ::SetBkColor(hdc, backColor);

    if (this->_lastTextColor != textColor ||
        this->_lastBackColor != backColor) {
        if (this->_hCtlColorBrush != NULL)
            DeleteObject(this->_hCtlColorBrush);
        this->_hCtlColorBrush = NULL;
        this->_lastTextColor  = textColor;
        this->_lastBackColor  = backColor;
    }

    if (this->_hCtlColorBrush == NULL)
        this->_hCtlColorBrush = CreateSolidBrush(backColor);

    hRetBrush = this->_hCtlColorBrush;
    return true;
}

bool sw::UIElement::OnSetCursor(HWND hwnd, HitTestResult hitTest, int message, bool &result)
{
    if (this->_useDefaultCursor || hitTest != HitTestResult::HitClient) {
        return false;
    }
    ::SetCursor(this->_hCursor);
    result = true;
    return true;
}

bool sw::UIElement::OnDropFiles(HDROP hDrop)
{
    DropFilesEventArgs args(hDrop);
    this->RaiseRoutedEvent(args);
    return args.handledMsg;
}

bool sw::UIElement::_SetHorzAlignment(sw::HorizontalAlignment value)
{
    if (value == this->_horizontalAlignment) {
        return false;
    }

    if (value == sw::HorizontalAlignment::Stretch) {
        this->_horizontalAlignment = value;
        this->_origionalSize.width = this->Width;
    } else {
        this->_horizontalAlignment = value;
        this->Width                = this->_origionalSize.width;
    }

    return true;
}

bool sw::UIElement::_SetVertAlignment(sw::VerticalAlignment value)
{
    if (value == this->_verticalAlignment) {
        return false;
    }

    if (value == sw::VerticalAlignment::Stretch) {
        this->_verticalAlignment    = value;
        this->_origionalSize.height = this->Height;
    } else {
        this->_verticalAlignment = value;
        this->Height             = this->_origionalSize.height;
    }

    return true;
}

void sw::UIElement::_SetMeasureInvalidated()
{
    this->_layoutUpdateCondition |= sw::LayoutUpdateCondition::MeasureInvalidated;
}

void sw::UIElement::_UpdateLayoutVisibleChildren()
{
    this->_layoutVisibleChildren.clear();

    for (UIElement *item : this->_children) {
        if (!item->_collapseWhenHide || item->Visible)
            this->_layoutVisibleChildren.push_back(item);
    }
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

sw::UIElement *sw::UIElement::_GetDeepestLastElement(UIElement *element)
{
    while (!element->_children.empty()) {
        element = element->_children.back();
    }
    return element;
}

sw::UIElement *sw::UIElement::_GetPreviousElement(UIElement *element)
{
    UIElement *parent = element->_parent;

    if (parent == nullptr) {
        return _GetDeepestLastElement(element);
    }

    int index = parent->IndexOf(element);
    return index <= 0 ? parent : _GetDeepestLastElement(parent->_children[index - 1]);
}

void sw::UIElement::_GetAllChildren(UIElement *element, std::vector<UIElement *> &children)
{
    for (UIElement *child : element->_children) {
        children.push_back(child);
        _GetAllChildren(child, children);
    }
}

// UniformGrid.cpp

sw::UniformGrid::UniformGrid()
    : Rows(
          // get
          [this]() -> int {
              return this->_uniformGridLayout.rows;
          },
          // set
          [this](const int &value) {
              this->_uniformGridLayout.rows = value;
              this->InvalidateMeasure();
          }),

      Columns(
          // get
          [this]() -> int {
              return this->_uniformGridLayout.columns;
          },
          // set
          [this](const int &value) {
              this->_uniformGridLayout.columns = value;
              this->InvalidateMeasure();
          }),

      FirstColumn(
          // get
          [this]() -> int {
              return this->_uniformGridLayout.firstColumn;
          },
          // set
          [this](const int &value) {
              this->_uniformGridLayout.firstColumn = value;
              this->InvalidateMeasure();
          })
{
    this->_uniformGridLayout.Associate(this);
    this->HorizontalAlignment = HorizontalAlignment::Stretch;
    this->VerticalAlignment   = VerticalAlignment::Stretch;
}

sw::LayoutHost *sw::UniformGrid::GetDefaultLayout()
{
    return &this->_uniformGridLayout;
}

// UniformGridLayout.cpp

sw::Size sw::UniformGridLayout::MeasureOverride(const Size &availableSize)
{
    int rowCount = Utils::Max(1, this->rows);
    int colCount = Utils::Max(1, this->columns);

    double itemMaxDesireWidth  = 0;
    double itemMaxDesireHeight = 0;

    int childCount = this->GetChildLayoutCount();
    Size measureSize{availableSize.width / colCount, availableSize.height / rowCount};

    for (int i = 0; i < childCount; ++i) {
        ILayout &item = this->GetChildLayoutAt(i);
        item.Measure(measureSize);
        Size itemDesireSize = item.GetDesireSize();
        itemMaxDesireWidth  = Utils::Max(itemDesireSize.width, itemMaxDesireWidth);
        itemMaxDesireHeight = Utils::Max(itemDesireSize.height, itemMaxDesireHeight);
    }

    return Size{
        std::isinf(availableSize.width) ? (itemMaxDesireWidth * colCount) : (availableSize.width),
        std::isinf(availableSize.height) ? (itemMaxDesireHeight * rowCount) : (availableSize.height)};
}

void sw::UniformGridLayout::ArrangeOverride(const Size &finalSize)
{
    int rowCount = Utils::Max(1, this->rows);
    int colCount = Utils::Max(1, this->columns);
    int beginCol = Utils::Max(0, Utils::Min(rowCount - 1, this->firstColumn));

    double arrangeWidth  = finalSize.width / colCount;
    double arrangeHeight = finalSize.height / rowCount;

    int childCount = this->GetChildLayoutCount();

    for (int i = 0; i < childCount; ++i) {
        int arrangeRow = (i + beginCol) / colCount;
        int arrangeCol = (i + beginCol) % colCount;

        if (arrangeRow >= rowCount) {
            arrangeRow = rowCount - 1;
            arrangeCol = colCount - 1;
        }

        ILayout &item = this->GetChildLayoutAt(i);
        item.Arrange(Rect{arrangeCol * arrangeWidth, arrangeRow * arrangeHeight, arrangeWidth, arrangeHeight});
    }
}

// Utils.cpp

std::wstring sw::Utils::ToWideStr(const std::string &str, bool utf8)
{
    int code = utf8 ? CP_UTF8 : CP_ACP;
    int size = MultiByteToWideChar(code, 0, str.c_str(), -1, nullptr, 0);
    std::wstring wstr(size, L'\0');
    MultiByteToWideChar(code, 0, str.c_str(), -1, &wstr[0], size);
    wstr.resize(wcslen(wstr.data()));
    return wstr;
}

std::string sw::Utils::ToMultiByteStr(const std::wstring &wstr, bool utf8)
{
    int code = utf8 ? CP_UTF8 : CP_ACP;
    int size = WideCharToMultiByte(code, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string str(size, '\0');
    WideCharToMultiByte(code, 0, wstr.c_str(), -1, &str[0], size, nullptr, nullptr);
    str.resize(strlen(str.data()));
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

std::wstring sw::Utils::FormatStr(const wchar_t *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    size_t len = std::vswprintf(nullptr, 0, fmt, args);
    va_end(args);

    std::wstring result(len + 1, L'\0');
    va_start(args, fmt);
    result.resize(std::vswprintf(&result[0], result.size(), fmt, args));
    va_end(args);

    return result;
}

// Window.cpp

#if !defined(WM_DPICHANGED)
#define WM_DPICHANGED 0x02E0
#endif

namespace
{
    /**
     * @brief 记录当前创建的窗口数
     */
    thread_local int _windowCount = 0;

    /**
     * @brief 窗口句柄保存Window指针的属性名称
     */
    constexpr wchar_t _WindowPtrProp[] = L"SWPROP_WindowPtr";
}

/**
 * @brief 当前线程的活动窗口
 */
const sw::ReadOnlyProperty<sw::Window *> sw::Window::ActiveWindow(
    []() -> sw::Window * {
        HWND hwnd = GetActiveWindow();
        // return _GetWindowPtr(hwnd); // vs2015无法识别此处的作用域？
        return reinterpret_cast<sw::Window *>(GetPropW(hwnd, _WindowPtrProp));
    } //
);

/**
 * @brief 当前线程已创建的窗口数
 */
const sw::ReadOnlyProperty<int> sw::Window::WindowCount(
    []() -> int {
        return _windowCount;
    } //
);

sw::Window::Window()
    : StartupLocation(
          // get
          [this]() -> WindowStartupLocation {
              return _startupLocation;
          },
          // set
          [this](const WindowStartupLocation &value) {
              _startupLocation = value;
          }),

      State(
          // get
          [this]() -> WindowState {
              HWND hwnd = Handle;
              if (IsIconic(hwnd)) {
                  return WindowState::Minimized;
              } else if (IsZoomed(hwnd)) {
                  return WindowState::Maximized;
              } else {
                  return WindowState::Normal;
              }
          },
          // set
          [this](const WindowState &value) {
              HWND hwnd = Handle;
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
          [this]() -> bool {
              return GetStyle(WS_SIZEBOX);
          },
          // set
          [this](const bool &value) {
              SetStyle(WS_SIZEBOX, value);
          }),

      MaximizeBox(
          // get
          [this]() -> bool {
              return GetStyle(WS_MAXIMIZEBOX);
          },
          // set
          [this](const bool &value) {
              SetStyle(WS_MAXIMIZEBOX, value);
          }),

      MinimizeBox(
          // get
          [this]() -> bool {
              return GetStyle(WS_MINIMIZEBOX);
          },
          // set
          [this](const bool &value) {
              SetStyle(WS_MINIMIZEBOX, value);
          }),

      Topmost(
          // get
          [this]() -> bool {
              return GetExtendedStyle(WS_EX_TOPMOST);
          },
          // set
          [this](const bool &value) {
              /*SetExtendedStyle(WS_EX_TOPMOST, value);*/
              HWND hWndInsertAfter = value ? HWND_TOPMOST : HWND_NOTOPMOST;
              SetWindowPos(Handle, hWndInsertAfter, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
          }),

      ToolWindow(
          // get
          [this]() -> bool {
              return GetExtendedStyle(WS_EX_TOOLWINDOW);
          },
          // set
          [this](const bool &value) {
              SetExtendedStyle(WS_EX_TOOLWINDOW, value);
          }),

      Menu(
          // get
          [this]() -> sw::Menu * {
              return _menu;
          },
          // set
          [this](sw::Menu *value) {
              _menu = value;
              SetMenu(Handle, value != nullptr ? value->GetHandle() : NULL);
          }),

      IsModal(
          // get
          [this]() -> bool {
              return _isModal;
          }),

      Owner(
          // get
          [this]() -> Window * {
              HWND hOwner = reinterpret_cast<HWND>(GetWindowLongPtrW(Handle, GWLP_HWNDPARENT));
              return _GetWindowPtr(hOwner);
          },
          // set
          [this](Window *value) {
              SetWindowLongPtrW(Handle, GWLP_HWNDPARENT, reinterpret_cast<LONG_PTR>(value ? value->Handle.Get() : NULL));
          }),

      IsLayered(
          // get
          [this]() -> bool {
              return GetExtendedStyle(WS_EX_LAYERED);
          },
          // set
          [this](const bool &value) {
              SetExtendedStyle(WS_EX_LAYERED, value);
          }),

      Opacity(
          // get
          [this]() -> double {
              BYTE result;
              return GetLayeredWindowAttributes(Handle, NULL, &result, NULL) ? (result / 255.0) : 1.0;
          },
          // set
          [this](const double &value) {
              double opacity = Utils::Min(1.0, Utils::Max(0.0, value));
              SetLayeredWindowAttributes(Handle, 0, (BYTE)std::lround(255 * opacity), LWA_ALPHA);
          }),

      Borderless(
          // get
          [this]() -> bool {
              return _isBorderless;
          },
          // set
          [this](const bool &value) {
              if (_isBorderless != value) {
                  _isBorderless = value;
                  SetStyle(WS_CAPTION | WS_THICKFRAME, !value);
              }
          }),

      DialogResult(
          // get
          [this]() -> int {
              return _dialogResult;
          },
          // set
          [this](const int &value) {
              _dialogResult = value;
              Close();
          })
{
    InitWindow(L"Window", WS_OVERLAPPEDWINDOW, 0);
    _SetWindowPtr(Handle, *this);
    SetIcon(_GetWindowDefaultIcon());
}

LRESULT sw::Window::WndProc(const ProcMsg &refMsg)
{
    switch (refMsg.uMsg) {
        case WM_CREATE: {
            ++_windowCount;
            return WndBase::WndProc(refMsg);
        }

        case WM_DESTROY: {
            _isDestroying = true;
            auto result   = WndBase::WndProc(refMsg);
            bool quitted  = false;
            // 若当前窗口为模态窗口则在窗口关闭时退出消息循环
            if (_isModal) {
                App::QuitMsgLoop(_dialogResult);
                quitted = true;
            }
            // 所有窗口都关闭时若App::QuitMode为Auto则退出主消息循环
            if (--_windowCount <= 0 && App::QuitMode == AppQuitMode::Auto) {
                if (!quitted) App::QuitMsgLoop();
            }
            _isDestroying = false;
            return result;
        }

        case WM_SHOWWINDOW: {
            if (_isFirstShow) {
                _isFirstShow = false;
                OnFirstShow();
            }
            return WndBase::WndProc(refMsg);
        }

        case WM_GETMINMAXINFO: {
            auto pInfo = reinterpret_cast<PMINMAXINFO>(refMsg.lParam);
            Size minSize{MinWidth, MinHeight};
            Size maxSize{MaxWidth, MaxHeight};
            if (minSize.width > 0) {
                pInfo->ptMinTrackSize.x = Utils::Max<LONG>(pInfo->ptMinTrackSize.x, Dip::DipToPxX(minSize.width));
            }
            if (minSize.height > 0) {
                pInfo->ptMinTrackSize.y = Utils::Max<LONG>(pInfo->ptMinTrackSize.y, Dip::DipToPxY(minSize.height));
            }
            if (maxSize.width > 0) {
                pInfo->ptMaxTrackSize.x = Utils::Min<LONG>(pInfo->ptMaxTrackSize.x, Dip::DipToPxX(maxSize.width));
            }
            if (maxSize.height > 0) {
                pInfo->ptMaxTrackSize.y = Utils::Min<LONG>(pInfo->ptMaxTrackSize.y, Dip::DipToPxY(maxSize.height));
            }
            return 0;
        }

        case WM_DPICHANGED: {
            OnDpiChanged(LOWORD(refMsg.wParam), HIWORD(refMsg.wParam));
            return 0;
        }

        case WM_ACTIVATE: {
            (refMsg.wParam == WA_INACTIVE) ? OnInactived() : OnActived();
            return 0;
        }

        case WM_UpdateLayout: {
            if (!_isDestroying) {
                UpdateLayout();
            }
            return 0;
        }

        default: {
            return WndBase::WndProc(refMsg);
        }
    }
}

sw::LayoutHost *sw::Window::GetDefaultLayout()
{
    return _layout.get();
}

bool sw::Window::OnClose()
{
    WindowClosingEventArgs args;
    RaiseRoutedEvent(args);

    if (!args.cancel) {
        UIElement::OnClose();
    }
    return true;
}

bool sw::Window::OnDestroy()
{
    RaiseRoutedEvent(Window_Closed);
    return true;
}

bool sw::Window::OnEraseBackground(HDC hdc, LRESULT &result)
{
    result = 1; // 阻止擦除背景
    return true;
}

bool sw::Window::OnPaint()
{
    PAINTSTRUCT ps;
    HWND hwnd = Handle;
    HDC hdc   = BeginPaint(hwnd, &ps);

    RECT rtClient;
    GetClientRect(hwnd, &rtClient);

    // 创建内存 DC 和位图
    HDC hdcMem         = CreateCompatibleDC(hdc);
    HBITMAP hBitmap    = CreateCompatibleBitmap(hdc, rtClient.right - rtClient.left, rtClient.bottom - rtClient.top);
    HBITMAP hBitmapOld = (HBITMAP)SelectObject(hdcMem, hBitmap);

    // 在内存 DC 上进行绘制
    HBRUSH hBrush = CreateSolidBrush(GetRealBackColor());
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

void sw::Window::OnMenuCommand(int id)
{
    if (ContextMenu::IsContextMenuID(id)) {
        UIElement::OnMenuCommand(id);
        return;
    }
    if (_menu) {
        MenuItem *item = _menu->GetMenuItem(id);
        if (item) item->CallCommand();
    }
}

void sw::Window::OnMinMaxSizeChanged()
{
    if (!IsRootElement()) {
        UIElement::OnMinMaxSizeChanged();
    }

    HWND hwnd = Handle;

    RECT rect;
    GetWindowRect(hwnd, &rect);

    SetWindowPos(
        hwnd, NULL, 0, 0, rect.right - rect.left, rect.bottom - rect.top,
        SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
}

void sw::Window::OnFirstShow()
{
    // 若未设置焦点元素则默认第一个元素为焦点元素
    if (ChildCount && GetAncestor(GetFocus(), GA_ROOT) != Handle) {
        GetChildAt(0).Focused = true;
    }

    // 按照StartupLocation修改位置
    if (_startupLocation == WindowStartupLocation::CenterScreen) {
        auto rect = Rect.Get();
        rect.left = (Screen::Width - rect.width) / 2;
        rect.top  = (Screen::Height - rect.height) / 2;
        Rect      = rect;
    } else if (_startupLocation == WindowStartupLocation::CenterOwner) {
        Window *owner = Owner;
        if (owner) {
            auto windowRect = Rect.Get();
            auto ownerRect  = owner->Rect.Get();
            windowRect.left = ownerRect.left + (ownerRect.width - windowRect.width) / 2;
            windowRect.top  = ownerRect.top + (ownerRect.height - windowRect.height) / 2;
            Rect            = windowRect;
        }
    }
}

void sw::Window::OnActived()
{
    SetFocus(_hPrevFocused);
    RaiseRoutedEvent(Window_Actived);
}

void sw::Window::OnInactived()
{
    RaiseRoutedEvent(Window_Inactived);
    _hPrevFocused = GetFocus();
}

void sw::Window::OnDpiChanged(int dpiX, int dpiY)
{
    bool layoutDisabled = IsLayoutDisabled();

    Dip::Update(dpiX, dpiY);
    DisableLayout();

    {
        // Windows在DIP改变时会自动调整窗口大小，此时会先触发WM_WINDOWPOSCHANGED，再触发WM_DPICHANGED
        // 因此在先触发的WM_WINDOWPOSCHANGED消息中，（Dip类中）DPI信息未更新，从而导致窗口的Rect数据错误
        // 此处在更新DPI信息后手动发送一个WM_WINDOWPOSCHANGED以修正窗口的Rect数据

        HWND hwnd = Handle;

        RECT rect;
        GetWindowRect(hwnd, &rect);

        WINDOWPOS pos{};
        pos.hwnd  = hwnd;
        pos.x     = rect.left;
        pos.y     = rect.top;
        pos.cx    = rect.right - rect.left;
        pos.cy    = rect.bottom - rect.top;
        pos.flags = SWP_NOACTIVATE | SWP_NOZORDER;

        SendMessageW(WM_WINDOWPOSCHANGED, 0, reinterpret_cast<LPARAM>(&pos));
    }

    UpdateFont();

    QueryAllChildren([](UIElement *item) {
        return item->UpdateFont(), true;
    });

    if (!layoutDisabled) {
        EnableLayout();
    }
}

sw::Window *sw::Window::ToWindow()
{
    return this;
}

void sw::Window::Close()
{
    WndBase::Close();
}

void sw::Window::Show()
{
    WndBase::Show(SW_SHOW);
}

int sw::Window::ShowDialog(Window *owner)
{
    if (owner != nullptr) {
        return ShowDialog(*owner);
    }

    int result = -1;

    if (!CheckAccess()) {
        return result; // 只能在创建窗口的线程调用
    }

    if (_isModal || IsDestroyed) {
        return result;
    }

    HWND hOwner = NULL;
    HWND hwnd   = Handle;

    {
        Window *pOwner;
        pOwner = Owner;
        hOwner = pOwner ? pOwner->Handle : reinterpret_cast<HWND>(GetWindowLongPtrW(hwnd, GWLP_HWNDPARENT));
    }

    if (hOwner == NULL) {
        if ((hOwner = GetActiveWindow()) != NULL) {
            SetWindowLongPtrW(hwnd, GWLP_HWNDPARENT, reinterpret_cast<LONG_PTR>(hOwner));
        }
    }

    _isModal     = true;
    _hModalOwner = hOwner;

    if (hOwner == NULL) {
        Show();
        result = App::MsgLoop();
    } else {
        bool oldIsEnabled = IsWindowEnabled(hOwner);
        EnableWindow(hOwner, false);
        Show();
        result = App::MsgLoop();
        SetForegroundWindow(hOwner);
        EnableWindow(hOwner, oldIsEnabled);
    }
    return result;
}

int sw::Window::ShowDialog(Window &owner)
{
    int result = -1;

    if (!CheckAccess()) {
        return result; // 只能在创建窗口的线程调用
    }

    if (this == &owner || _isModal || IsDestroyed) {
        return result;
    }

    Owner        = &owner;
    _isModal     = true;
    _hModalOwner = owner.Handle;

    bool oldIsEnabled = owner.Enabled;
    owner.Enabled     = false;

    Show();
    result = App::MsgLoop();
    SetForegroundWindow(owner.Handle);

    if (oldIsEnabled) {
        owner.Enabled = true;
    }
    return result;
}

void sw::Window::SetIcon(HICON hIcon)
{
    SendMessageW(WM_SETICON, ICON_BIG, (LPARAM)hIcon);
    SendMessageW(WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
}

void sw::Window::DrawMenuBar()
{
    ::DrawMenuBar(Handle);
}

void sw::Window::SizeToContent()
{
    if (!IsRootElement()) {
        return; // 只对顶级窗口有效
    }

    // 该函数需要AutoSize为true，这里先备份其值以做后续恢复
    bool oldAutoSize = AutoSize;
    AutoSize         = true;

    // measure
    sw::Size measureSize(INFINITY, INFINITY);
    Measure(measureSize);

    // arrange
    sw::Size desireSize  = GetDesireSize();
    sw::Rect windowRect  = Rect;
    sw::Thickness margin = Margin;
    Arrange(sw::Rect{windowRect.left - margin.left, windowRect.top - margin.top, desireSize.width, desireSize.height});

    // 恢复AutoSize属性的值
    AutoSize = oldAutoSize;
}

sw::Window *sw::Window::_GetWindowPtr(HWND hwnd)
{
    return reinterpret_cast<sw::Window *>(GetPropW(hwnd, _WindowPtrProp));
}

void sw::Window::_SetWindowPtr(HWND hwnd, Window &wnd)
{
    SetPropW(hwnd, _WindowPtrProp, reinterpret_cast<HANDLE>(&wnd));
}

HICON sw::Window::_GetWindowDefaultIcon()
{
    static HICON hIcon = ExtractIconW(App::Instance, App::ExePath->c_str(), 0);
    return hIcon;
}

// WndBase.cpp

namespace
{
    /**
     * @brief _check字段的值，用于判断给定指针是否为指向WndBase的指针
     */
    constexpr uint32_t _WndBaseMagicNumber = 0x946dba7e;

    /**
     * @brief 窗口句柄保存WndBase指针的属性名称
     */
    constexpr wchar_t _WndBasePtrProp[] = L"SWPROP_WndBasePtr";

    /**
     * @brief 窗口类名
     */
    constexpr wchar_t _WindowClassName[] = L"sw::Window";

    /**
     * @brief 控件初始化时所在的窗口
     */
    thread_local struct : sw::WndBase{} *_controlInitContainer = nullptr;

    /**
     * @brief 控件id计数器
     */
    std::atomic<int> _controlIdCounter = 1073741827;
}

sw::WndBase::WndBase()
    : _check(_WndBaseMagicNumber),

      Handle(
          // get
          [this]() -> HWND {
              return this->_hwnd;
          }),

      Font(
          // get
          [this]() -> sw::Font {
              return this->_font;
          },
          // set
          [this](const sw::Font &value) {
              this->_font = value;
              this->UpdateFont();
          }),

      FontName(
          // get
          [this]() -> std::wstring {
              return this->_font.name;
          },
          // set
          [this](const std::wstring &value) {
              if (this->_font.name != value) {
                  this->_font.name = value;
                  this->UpdateFont();
              }
          }),

      FontSize(
          // get
          [this]() -> double {
              return this->_font.size;
          },
          // set
          [this](const double &value) {
              if (this->_font.size != value) {
                  this->_font.size = value;
                  this->UpdateFont();
              }
          }),

      FontWeight(
          // get
          [this]() -> sw::FontWeight {
              return this->_font.weight;
          },
          // set
          [this](const sw::FontWeight &value) {
              if (this->_font.weight != value) {
                  this->_font.weight = value;
                  this->UpdateFont();
              }
          }),

      Rect(
          // get
          [this]() -> sw::Rect {
              return this->_rect;
          },
          // set
          [this](const sw::Rect &value) {
              if (this->_rect != value) {
                  int left   = Dip::DipToPxX(value.left);
                  int top    = Dip::DipToPxY(value.top);
                  int width  = Dip::DipToPxX(value.width);
                  int height = Dip::DipToPxY(value.height);
                  SetWindowPos(this->_hwnd, NULL, left, top, width, height, SWP_NOACTIVATE | SWP_NOZORDER);
              }
          }),

      Left(
          // get
          [this]() -> double {
              return this->_rect.left;
          },
          // set
          [this](const double &value) {
              if (this->_rect.left != value) {
                  int x = Dip::DipToPxX(value);
                  int y = Dip::DipToPxY(this->_rect.top);
                  SetWindowPos(this->_hwnd, NULL, x, y, 0, 0, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
              }
          }),

      Top(
          // get
          [this]() -> double {
              return this->_rect.top;
          },
          // set
          [this](const double &value) {
              if (this->_rect.top != value) {
                  int x = Dip::DipToPxX(this->_rect.left);
                  int y = Dip::DipToPxY(value);
                  SetWindowPos(this->_hwnd, NULL, x, y, 0, 0, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
              }
          }),

      Width(
          // get
          [this]() -> double {
              return this->_rect.width;
          },
          // set
          [this](const double &value) {
              if (this->_rect.width != value) {
                  int cx = Dip::DipToPxX(value);
                  int cy = Dip::DipToPxY(this->_rect.height);
                  SetWindowPos(this->_hwnd, NULL, 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
              }
          }),

      Height(
          // get
          [this]() -> double {
              return this->_rect.height;
          },
          // set
          [this](const double &value) {
              if (this->_rect.height != value) {
                  int cx = Dip::DipToPxX(this->_rect.width);
                  int cy = Dip::DipToPxY(value);
                  SetWindowPos(this->_hwnd, NULL, 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
              }
          }),

      ClientRect(
          // get
          [this]() -> sw::Rect {
              RECT rect;
              GetClientRect(this->_hwnd, &rect);
              return rect;
          }),

      ClientWidth(
          // get
          [this]() -> double {
              return this->ClientRect->width;
          }),

      ClientHeight(
          // get
          [this]() -> double {
              return this->ClientRect->height;
          }),

      Enabled(
          // get
          [this]() -> bool {
              return IsWindowEnabled(this->_hwnd);
          },
          // set
          [this](const bool &value) {
              EnableWindow(this->_hwnd, value);
          }),

      Visible(
          // get
          [this]() -> bool {
              return this->GetStyle(WS_VISIBLE);
          },
          // set
          [this](const bool &value) {
              ShowWindow(this->_hwnd, value ? SW_SHOW : SW_HIDE);
              this->VisibleChanged(value);
          }),

      Text(
          // get
          [this]() -> std::wstring {
              return this->GetInternalText();
          },
          // set
          [this](const std::wstring &value) {
              this->SetInternalText(value);
          }),

      Focused(
          // get
          [this]() -> bool {
              return this->_focused;
          },
          // set
          [this](const bool &value) {
              SetFocus(value ? this->_hwnd : NULL);
          }),

      Parent(
          // get
          [this]() -> WndBase * {
              HWND hwnd = GetParent(this->_hwnd);
              return WndBase::GetWndBase(hwnd);
          }),

      IsDestroyed(
          // get
          [this]() -> bool {
              return this->_isDestroyed;
          }),

      AcceptFiles(
          // get
          [this]() -> bool {
              return this->GetExtendedStyle(WS_EX_ACCEPTFILES);
          },
          // set
          [this](const bool &value) {
              this->SetExtendedStyle(WS_EX_ACCEPTFILES, value);
          }),

      IsControl(
          // get
          [this]() -> bool {
              return this->_isControl;
          }),

      ClassName(
          // get
          [this]() -> std::wstring {
              std::wstring result(256, L'\0');
              result.resize(GetClassNameW(this->_hwnd, &result[0], (int)result.size()));
              return result;
          }),

      GroupStart(
          // get
          [this]() -> bool {
              return this->GetStyle(WS_GROUP);
          },
          // set
          [this](const bool &value) {
              this->SetStyle(WS_GROUP, value);
          })
{
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

bool sw::WndBase::operator==(const WndBase &other) const
{
    return this == &other;
}

bool sw::WndBase::operator!=(const WndBase &other) const
{
    return this != &other;
}

sw::UIElement *sw::WndBase::ToUIElement()
{
    return nullptr;
}

sw::Control *sw::WndBase::ToControl()
{
    return nullptr;
}

sw::Window *sw::WndBase::ToWindow()
{
    return nullptr;
}

std::wstring sw::WndBase::ToString() const
{
    return L"WndBase{ClassName=" + this->ClassName + L", Handle=" + std::to_wstring(reinterpret_cast<uintptr_t>(this->_hwnd)) + L"}";
}

void sw::WndBase::InitWindow(LPCWSTR lpWindowName, DWORD dwStyle, DWORD dwExStyle)
{
    static thread_local ATOM wndClsAtom = 0;

    if (this->_hwnd != NULL) {
        return;
    }

    if (wndClsAtom == 0) {
        WNDCLASSEXW wc{};
        wc.cbSize        = sizeof(wc);
        wc.hInstance     = App::Instance;
        wc.lpfnWndProc   = WndBase::_WndProc;
        wc.lpszClassName = _WindowClassName;
        wc.hCursor       = CursorHelper::GetCursorHandle(StandardCursor::Arrow);
        wndClsAtom       = RegisterClassExW(&wc);
    }

    if (lpWindowName) {
        this->_text = lpWindowName;
    }

    this->_hwnd = CreateWindowExW(
        dwExStyle,           // Optional window styles
        _WindowClassName,    // Window class
        this->_text.c_str(), // Window text
        dwStyle,             // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,          // Parent window
        NULL,          // Menu
        App::Instance, // Instance handle
        this           // Additional application data
    );

    WndBase::_SetWndBase(this->_hwnd, *this);

    RECT rect;
    GetWindowRect(this->_hwnd, &rect);
    this->_rect = rect;

    this->HandleInitialized(this->_hwnd);
    this->UpdateFont();
}

void sw::WndBase::InitControl(LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, DWORD dwExStyle, LPVOID lpParam)
{
    WndBase::_InitControlContainer();

    if (this->_hwnd != NULL) {
        return;
    }

    if (lpWindowName) {
        this->_text = lpWindowName;
    }

    HMENU id = reinterpret_cast<HMENU>(
        static_cast<uintptr_t>(WndBase::_NextControlId()));

    this->_hwnd = CreateWindowExW(
        dwExStyle,                    // Optional window styles
        lpClassName,                  // Window class
        this->_text.c_str(),          // Window text
        dwStyle,                      // Window style
        0, 0, 0, 0,                   // Size and position
        _controlInitContainer->_hwnd, // Parent window
        id,                           // Control id
        App::Instance,                // Instance handle
        lpParam                       // Additional application data
    );

    this->_isControl = true;
    WndBase::_SetWndBase(this->_hwnd, *this);

    this->_originalWndProc =
        reinterpret_cast<WNDPROC>(SetWindowLongPtrW(this->_hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndBase::_WndProc)));

    this->HandleInitialized(this->_hwnd);
    this->UpdateFont();
}

LRESULT sw::WndBase::DefaultWndProc(const ProcMsg &refMsg)
{
    if (this->_originalWndProc == nullptr) {
        return DefWindowProcW(refMsg.hwnd, refMsg.uMsg, refMsg.wParam, refMsg.lParam);
    } else {
        return CallWindowProcW(this->_originalWndProc, refMsg.hwnd, refMsg.uMsg, refMsg.wParam, refMsg.lParam);
    }
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
            LRESULT result     = this->OnDestroy() ? 0 : this->DefaultWndProc(refMsg);
            this->_isDestroyed = true;
            return result;
        }

        case WM_PAINT: {
            LRESULT result = this->OnPaint() ? 0 : this->DefaultWndProc(refMsg);
            this->OnEndPaint();
            return result;
        }

        case WM_NCPAINT: {
            LRESULT result = this->OnNcPaint(reinterpret_cast<HRGN>(refMsg.wParam)) ? 0 : this->DefaultWndProc(refMsg);
            this->OnEndNcPaint();
            return result;
        }

        case WM_WINDOWPOSCHANGED: {
            auto pWndPos = reinterpret_cast<PWINDOWPOS>(refMsg.lParam);
            if ((pWndPos->flags & SWP_NOMOVE) == 0) {
                this->_rect.left = Dip::PxToDipX(pWndPos->x);
                this->_rect.top  = Dip::PxToDipY(pWndPos->y);
            }
            if ((pWndPos->flags & SWP_NOSIZE) == 0) {
                this->_rect.width  = Dip::PxToDipX(pWndPos->cx);
                this->_rect.height = Dip::PxToDipY(pWndPos->cy);
            }
            return this->DefaultWndProc(refMsg);
        }

        case WM_MOVE: {
            int xPos = (int)(short)LOWORD(refMsg.lParam); // horizontal position
            int yPos = (int)(short)HIWORD(refMsg.lParam); // vertical position
            return this->OnMove(POINT{xPos, yPos}) ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_SIZE: {
            int width  = LOWORD(refMsg.lParam); // the new width of the client area
            int height = HIWORD(refMsg.lParam); // the new height of the client area
            return this->OnSize(SIZE{width, height}) ? 0 : this->DefaultWndProc(refMsg);
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
            LRESULT result = 0;

            NMHDR *pNMHDR = reinterpret_cast<NMHDR *>(refMsg.lParam);
            bool handled  = this->OnNotify(pNMHDR, result);

            if (!handled) {
                WndBase *pWnd = WndBase::GetWndBase(pNMHDR->hwndFrom);
                if (pWnd) handled = pWnd->OnNotified(pNMHDR, result);
            }

            return handled ? result : this->DefaultWndProc(refMsg);
        }

        case WM_CTLCOLORMSGBOX:
        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORLISTBOX:
        case WM_CTLCOLORBTN:
        case WM_CTLCOLORDLG:
        case WM_CTLCOLORSCROLLBAR:
        case WM_CTLCOLORSTATIC: {
            HWND hControl     = (HWND)refMsg.lParam;
            WndBase *pControl = WndBase::GetWndBase(hControl);
            if (pControl == nullptr) {
                return this->DefaultWndProc(refMsg);
            } else {
                HDC hdc       = (HDC)refMsg.wParam;
                HBRUSH hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
                return this->OnCtlColor(pControl, hdc, hBrush) ? (LRESULT)hBrush : this->DefaultWndProc(refMsg);
            }
        }

        case WM_SETCURSOR: {
            HWND hwnd   = (HWND)refMsg.wParam;
            int hitTest = LOWORD(refMsg.lParam);
            int message = HIWORD(refMsg.lParam);
            bool result = false;
            return this->OnSetCursor(hwnd, (HitTestResult)hitTest, message, result) ? result : this->DefaultWndProc(refMsg);
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

        case WM_NCHITTEST: {
            Point testPoint      = POINT{GET_X_LPARAM(refMsg.lParam), GET_Y_LPARAM(refMsg.lParam)};
            HitTestResult result = (HitTestResult)this->DefaultWndProc(refMsg);
            this->OnNcHitTest(testPoint, result);
            return (LRESULT)result;
        }

        case WM_ERASEBKGND: {
            LRESULT result = 0;
            return this->OnEraseBackground(reinterpret_cast<HDC>(refMsg.wParam), result) ? result : this->DefaultWndProc(refMsg);
        }

        case WM_DRAWITEM: {
            auto pDrawItem = reinterpret_cast<DRAWITEMSTRUCT *>(refMsg.lParam);
            if (this->OnDrawItem((int)refMsg.wParam, pDrawItem)) {
                return TRUE;
            }
            if (pDrawItem->CtlType != ODT_MENU && pDrawItem->hwndItem != NULL) {
                WndBase *pWnd = GetWndBase(pDrawItem->hwndItem);
                if (pWnd && pWnd->OnDrawItemSelf(pDrawItem)) return TRUE;
            }
            return this->DefaultWndProc(refMsg);
        }

        case WM_MEASUREITEM: {
            auto pMeasure = reinterpret_cast<MEASUREITEMSTRUCT *>(refMsg.lParam);
            if (this->OnMeasureItem((int)refMsg.wParam, pMeasure)) {
                return TRUE;
            }
            if (pMeasure->CtlType != ODT_MENU) {
                WndBase *pWnd = GetWndBase(GetDlgItem(this->_hwnd, (int)refMsg.wParam));
                if (pWnd && pWnd->OnMeasureItemSelf(pMeasure)) return TRUE;
            }
            return this->DefaultWndProc(refMsg);
        }

        case WM_DROPFILES: {
            return this->OnDropFiles(reinterpret_cast<HDROP>(refMsg.wParam)) ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_InvokeAction: {
            auto pAction = reinterpret_cast<Action<> *>(refMsg.lParam);
            if (pAction && *pAction) pAction->Invoke();
            if (refMsg.wParam) delete pAction;
            return 0;
        }

        default: {
            return this->DefaultWndProc(refMsg);
        }
    }
}

void sw::WndBase::UpdateInternalText()
{
    int len = GetWindowTextLengthW(this->_hwnd);

    if (len <= 0) {
        this->_text.clear();
        return;
    }

    // wchar_t *buf = new wchar_t[len + 1];
    // GetWindowTextW(this->_hwnd, buf, len + 1);
    // this->_text = buf;
    // delete[] buf;

    this->_text.resize(len + 1);
    GetWindowTextW(this->_hwnd, &this->_text[0], len + 1);
    this->_text.resize(len);
}

std::wstring &sw::WndBase::GetInternalText()
{
    return this->_text;
}

void sw::WndBase::SetInternalText(const std::wstring &value)
{
    SetWindowTextW(this->_hwnd, value.c_str());
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

bool sw::WndBase::OnNcPaint(HRGN hRgn)
{
    return false;
}

void sw::WndBase::OnEndNcPaint()
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
        hParent = this->_isControl ? _controlInitContainer->_hwnd : NULL;
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

bool sw::WndBase::OnSetCursor(HWND hwnd, HitTestResult hitTest, int message, bool &result)
{
    return false;
}

bool sw::WndBase::OnContextMenu(bool isKeyboardMsg, Point mousePosition)
{
    return false;
}

bool sw::WndBase::OnNotify(NMHDR *pNMHDR, LRESULT &result)
{
    return false;
}

bool sw::WndBase::OnNotified(NMHDR *pNMHDR, LRESULT &result)
{
    return false;
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

bool sw::WndBase::OnCtlColor(WndBase *pControl, HDC hdc, HBRUSH &hRetBrush)
{
    return pControl->OnColor(hdc, hRetBrush);
}

bool sw::WndBase::OnColor(HDC hdc, HBRUSH &hRetBrush)
{
    return false;
}

void sw::WndBase::OnNcHitTest(const Point &testPoint, HitTestResult &result)
{
}

bool sw::WndBase::OnEraseBackground(HDC hdc, LRESULT &result)
{
    return false;
}

bool sw::WndBase::OnDrawItem(int id, DRAWITEMSTRUCT *pDrawItem)
{
    return false;
}

bool sw::WndBase::OnDrawItemSelf(DRAWITEMSTRUCT *pDrawItem)
{
    return false;
}

bool sw::WndBase::OnMeasureItem(int id, MEASUREITEMSTRUCT *pMeasure)
{
    return false;
}

bool sw::WndBase::OnMeasureItemSelf(MEASUREITEMSTRUCT *pMeasure)
{
    return false;
}

bool sw::WndBase::OnDropFiles(HDROP hDrop)
{
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

HFONT sw::WndBase::GetFontHandle()
{
    return this->_hfont;
}

void sw::WndBase::Redraw(bool erase, bool updateWindow)
{
    InvalidateRect(this->_hwnd, NULL, erase);
    if (updateWindow) UpdateWindow(this->_hwnd);
}

bool sw::WndBase::IsVisible()
{
    return IsWindowVisible(this->_hwnd);
}

DWORD sw::WndBase::GetStyle()
{
    return DWORD(GetWindowLongPtrW(this->_hwnd, GWL_STYLE));
}

void sw::WndBase::SetStyle(DWORD style)
{
    SetWindowLongPtrW(this->_hwnd, GWL_STYLE, LONG_PTR(style));
}

bool sw::WndBase::GetStyle(DWORD mask)
{
    return (DWORD(GetWindowLongPtrW(this->_hwnd, GWL_STYLE)) & mask) == mask;
}

void sw::WndBase::SetStyle(DWORD mask, bool value)
{
    DWORD newstyle =
        value ? (DWORD(GetWindowLongPtrW(this->_hwnd, GWL_STYLE)) | mask)
              : (DWORD(GetWindowLongPtrW(this->_hwnd, GWL_STYLE)) & ~mask);
    SetWindowLongPtrW(this->_hwnd, GWL_STYLE, LONG_PTR(newstyle));
}

DWORD sw::WndBase::GetExtendedStyle()
{
    return DWORD(GetWindowLongPtrW(this->_hwnd, GWL_EXSTYLE));
}

void sw::WndBase::SetExtendedStyle(DWORD style)
{
    SetWindowLongPtrW(this->_hwnd, GWL_EXSTYLE, LONG_PTR(style));
}

bool sw::WndBase::GetExtendedStyle(DWORD mask)
{
    return (DWORD(GetWindowLongPtrW(this->_hwnd, GWL_EXSTYLE)) & mask) == mask;
}

void sw::WndBase::SetExtendedStyle(DWORD mask, bool value)
{
    DWORD newstyle =
        value ? (DWORD(GetWindowLongPtrW(this->_hwnd, GWL_EXSTYLE)) | mask)
              : (DWORD(GetWindowLongPtrW(this->_hwnd, GWL_EXSTYLE)) & ~mask);
    SetWindowLongPtrW(this->_hwnd, GWL_EXSTYLE, LONG_PTR(newstyle));
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

LRESULT sw::WndBase::SendMessageA(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return ::SendMessageA(this->_hwnd, uMsg, wParam, lParam);
}

LRESULT sw::WndBase::SendMessageW(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return ::SendMessageW(this->_hwnd, uMsg, wParam, lParam);
}

BOOL sw::WndBase::PostMessageA(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return ::PostMessageA(this->_hwnd, uMsg, wParam, lParam);
}

BOOL sw::WndBase::PostMessageW(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return ::PostMessageW(this->_hwnd, uMsg, wParam, lParam);
}

sw::HitTestResult sw::WndBase::NcHitTest(const Point &testPoint)
{
    POINT point = testPoint;
    return (HitTestResult)this->SendMessageW(WM_NCHITTEST, 0, MAKELPARAM(point.x, point.y));
}

void sw::WndBase::Invoke(const SimpleAction &action)
{
    if (action == nullptr)
        return;

    if (this->CheckAccess())
        action();
    else {
        Action<> &a = const_cast<Action<> &>(action); // safe here
        this->SendMessageW(WM_InvokeAction, false, reinterpret_cast<LPARAM>(&a));
    }
}

void sw::WndBase::InvokeAsync(const SimpleAction &action)
{
    if (action == nullptr)
        return;
    else {
        Action<> *p = new Action<>(action);
        this->PostMessageW(WM_InvokeAction, true, reinterpret_cast<LPARAM>(p));
    }
}

DWORD sw::WndBase::GetThreadId() const
{
    return GetWindowThreadProcessId(this->_hwnd, NULL);
}

bool sw::WndBase::CheckAccess() const
{
    return this->GetThreadId() == GetCurrentThreadId();
}

bool sw::WndBase::CheckAccess(const WndBase &other) const
{
    return this == &other || this->GetThreadId() == other.GetThreadId();
}

LRESULT sw::WndBase::_WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    WndBase *pWnd = nullptr;

    if (hwnd != NULL) {
        pWnd = WndBase::GetWndBase(hwnd);
    }

    if (pWnd == nullptr && (uMsg == WM_NCCREATE || uMsg == WM_CREATE)) {
        auto temp = reinterpret_cast<WndBase *>(
            reinterpret_cast<LPCREATESTRUCTW>(lParam)->lpCreateParams);
        if (temp != nullptr && temp->_check == _WndBaseMagicNumber) pWnd = temp;
    }

    if (pWnd != nullptr) {
        ProcMsg msg{hwnd, uMsg, wParam, lParam};
        return pWnd->WndProc(msg);
    }

    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

void sw::WndBase::_InitControlContainer()
{
    if (_controlInitContainer == nullptr || _controlInitContainer->_isDestroyed) {
        delete _controlInitContainer;
        _controlInitContainer = new std::remove_reference<decltype(*_controlInitContainer)>::type;
        _controlInitContainer->InitWindow(L"", WS_POPUP, 0);
    }
}

sw::WndBase *sw::WndBase::_GetControlInitContainer()
{
    _InitControlContainer();
    return _controlInitContainer;
}

int sw::WndBase::_NextControlId()
{
    return _controlIdCounter.fetch_add(1);
}

void sw::WndBase::_SetWndBase(HWND hwnd, WndBase &wnd)
{
    SetPropW(hwnd, _WndBasePtrProp, reinterpret_cast<HANDLE>(&wnd));
}

sw::WndBase *sw::WndBase::GetWndBase(HWND hwnd)
{
    auto p = reinterpret_cast<WndBase *>(GetPropW(hwnd, _WndBasePtrProp));
    return (p == nullptr || p->_check != _WndBaseMagicNumber) ? nullptr : p;
}

// WrapLayout.cpp

sw::Size sw::WrapLayout::MeasureOverride(const Size &availableSize)
{
    return this->orientation == Orientation::Horizontal
               ? this->WrapLayoutH::MeasureOverride(availableSize)
               : this->WrapLayoutV::MeasureOverride(availableSize);
}

void sw::WrapLayout::ArrangeOverride(const Size &finalSize)
{
    this->orientation == Orientation::Horizontal
        ? this->WrapLayoutH::ArrangeOverride(finalSize)
        : this->WrapLayoutV::ArrangeOverride(finalSize);
}

// WrapLayoutH.cpp

sw::Size sw::WrapLayoutH::MeasureOverride(const Size &availableSize)
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

    return size;
}

void sw::WrapLayoutH::ArrangeOverride(const Size &finalSize)
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

sw::Size sw::WrapLayoutV::MeasureOverride(const Size &availableSize)
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

    return size;
}

void sw::WrapLayoutV::ArrangeOverride(const Size &finalSize)
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
          [this]() -> sw::Orientation {
              return this->_wrapLayout.orientation;
          },
          // set
          [this](const sw::Orientation &value) {
              this->_wrapLayout.orientation = value;
              this->InvalidateMeasure();
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
