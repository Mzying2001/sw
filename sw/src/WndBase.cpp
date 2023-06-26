#include "WndBase.h"
#include <cmath>

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
    WndBase *pWnd;
    pWnd = reinterpret_cast<WndBase *>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));

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
              ShowWindow(this->_hwnd, value ? SW_SHOW : SW_HIDE);
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

      Parent(
          // get
          [&]() -> WndBase *const & {
              static WndBase *pWndBase;
              HWND hwnd = GetParent(this->_hwnd);
              pWndBase  = reinterpret_cast<WndBase *>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
              return pWndBase;
          },
          // set
          [&](WndBase *const &value) {
              HWND hwnd = value == NULL ? NULL : value->_hwnd;
              SetParent(this->_hwnd, hwnd);
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
}

void sw::WndBase::InitWindow(LPCWSTR lpWindowName, DWORD dwStyle, HWND hWndParent, HMENU hMenu)
{
    if (this->_hwnd == NULL) {

        this->_text = lpWindowName;

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

        RECT rect;
        GetWindowRect(this->_hwnd, &rect);
        this->_rect = rect;

        SetWindowLongPtrW(this->_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    }
}

void sw::WndBase::InitControl(LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle)
{
    if (_controlInitContainer == NULL) {
        _controlInitContainer = CreateWindowExW(0, WINDOW_CLASS_NAME, L"", WS_POPUP, 0, 0, 0, 0, NULL, NULL, NULL, NULL);
    }

    if (this->_hwnd == NULL) {

        this->_text = lpWindowName;

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

        RECT rect;
        GetWindowRect(this->_hwnd, &rect);
        this->_rect = rect;

        SetWindowLongPtrW(this->_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

        this->_controlOldWndProc =
            reinterpret_cast<WNDPROC>(SetWindowLongPtrW(this->_hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndBase::_WndProc)));
    }
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
    if (this->_controlOldWndProc != NULL) {
        return this->_controlOldWndProc(refMsg.hwnd, refMsg.uMsg, refMsg.wParam, refMsg.lParam);
    } else {
        return DefWindowProcW(refMsg.hwnd, refMsg.uMsg, refMsg.wParam, refMsg.lParam);
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
            return this->OnDestroy() ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_PAINT: {
            return this->OnPaint() ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_WINDOWPOSCHANGED: {
            PWINDOWPOS pWndPos = reinterpret_cast<PWINDOWPOS>(refMsg.lParam);
            double dpiScaleX   = Dip::ScaleX;
            double dpiScaleY   = Dip::ScaleY;
            this->_rect.left   = dpiScaleX * pWndPos->x;
            this->_rect.top    = dpiScaleY * pWndPos->y;
            this->_rect.width  = dpiScaleX * pWndPos->cx;
            this->_rect.height = dpiScaleY * pWndPos->cy;
            return this->DefaultWndProc(refMsg);
        }

        case WM_MOVE: {
            int xPos = LOWORD(refMsg.lParam); // horizontal position
            int yPos = HIWORD(refMsg.lParam); // vertical position
            return this->OnMove(xPos * Dip::ScaleX, yPos * Dip::ScaleY) ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_SIZE: {
            int width  = LOWORD(refMsg.lParam); // the new width of the client area
            int height = HIWORD(refMsg.lParam); // the new height of the client area
            return this->OnSize(width * Dip::ScaleX, height * Dip::ScaleY) ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_SETTEXT: {
            LRESULT result = this->DefaultWndProc(refMsg);
            if (result == TRUE)
                this->_text = reinterpret_cast<PCWSTR>(refMsg.lParam);
            return result;
        }

        default: {
            return this->DefaultWndProc(refMsg);
        }
    }
}

bool sw::WndBase::OnCreate()
{
    return false;
}

bool sw::WndBase::OnClose()
{
    DestroyWindow(this->_hwnd);
    return true;
}

bool sw::WndBase::OnDestroy()
{
    return false;
}

bool sw::WndBase::OnPaint()
{
    return false;
}

bool sw::WndBase::OnMove(double newLeft, double newTop)
{
    return false;
}

bool sw::WndBase::OnSize(double newClientWidth, double newClientHeight)
{
    return false;
}

bool sw::WndBase::OnSetText(std::wstring &newText)
{
    return true;
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
