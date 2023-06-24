#include "WndBase.h"

const PCWSTR sw::WndBase::WndBaseClassName = L"sw::WndBase";

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

LRESULT sw::WndBase::DefaultWndProc(const ProcMsg &refMsg)
{
    return DefWindowProcW(refMsg.hwnd, refMsg.uMsg, refMsg.wParam, refMsg.lParam);
}

sw::WndBase::WndBase()
    : _hwnd(NULL),
      Handle([&]() -> const HWND & { return this->_hwnd; }),

      _rect(),
      Rect(
          // get
          [&]() -> const sw::Rect & {
              return this->_rect;
          },
          // set
          [&](const sw::Rect &value) {
              RECT rect = value.GetRECT();
              MoveWindow(this->_hwnd, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, TRUE);
          }),

      Left(
          // get
          [&]() -> const double & {
              return this->Rect->left;
          },
          // set
          [&](const double &value) {
              sw::Rect rect = this->Rect;
              rect.left     = value;
              this->Rect    = rect;
          }),

      Top(
          // get
          [&]() -> const double & {
              return this->Rect->top;
          },
          // set
          [&](const double &value) {
              sw::Rect rect = this->Rect;
              rect.top      = value;
              this->Rect    = rect;
          }),

      Width(
          // get
          [&]() -> const double & {
              return this->Rect->width;
          },
          // set
          [&](const double &value) {
              sw::Rect rect = this->Rect;
              rect.width    = value;
              this->Rect    = rect;
          }),

      Height(
          // get
          [&]() -> const double & {
              return this->Rect->height;
          },
          // set
          [&](const double &value) {
              sw::Rect rect = this->Rect;
              rect.height   = value;
              this->Rect    = rect;
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

      _text(),
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
    static bool isClassRegistered = false;

    if (!isClassRegistered) {

        WNDCLASSEXW wc{};
        wc.cbSize        = sizeof(wc);
        wc.hCursor       = LoadCursorW(NULL, IDC_ARROW);
        wc.hInstance     = App::Instance;
        wc.lpfnWndProc   = _WndProc;
        wc.lpszClassName = WndBaseClassName;

        RegisterClassExW(&wc);
        isClassRegistered = true;
    }
}

void sw::WndBase::InitWndBase(DWORD dwExStyle, LPCWSTR lpWindowName, DWORD dwStyle, HWND hWndParent, HMENU hMenu)
{
    if (this->_hwnd == NULL) {

        this->_text = lpWindowName;

        this->_hwnd = CreateWindowExW(
            dwExStyle,        // Optional window styles
            WndBaseClassName, // Window class
            lpWindowName,     // Window text
            dwStyle,          // Window style

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

LRESULT sw::WndBase::WndProc(const ProcMsg &refMsg)
{
    switch (refMsg.uMsg) {
        case WM_CREATE: {
            return this->OnCreate() ? 0 : DefaultWndProc(refMsg);
        }

        case WM_CLOSE: {
            return this->OnClose() ? 0 : DefaultWndProc(refMsg);
        }

        case WM_DESTROY: {
            return this->OnDestroy() ? 0 : DefaultWndProc(refMsg);
        }

        case WM_PAINT: {
            return this->OnPaint() ? 0 : DefaultWndProc(refMsg);
        }

        case WM_WINDOWPOSCHANGED: {
            PWINDOWPOS pWndPos = reinterpret_cast<PWINDOWPOS>(refMsg.lParam);
            double dpiScaleX   = Dpi::ScaleX;
            double dpiScaleY   = Dpi::ScaleY;
            this->_rect.left   = dpiScaleX * pWndPos->x;
            this->_rect.top    = dpiScaleY * pWndPos->y;
            this->_rect.width  = dpiScaleX * pWndPos->cx;
            this->_rect.height = dpiScaleY * pWndPos->cy;
            return DefaultWndProc(refMsg);
        }

        case WM_MOVE: {
            int xPos = LOWORD(refMsg.lParam); // horizontal position
            int yPos = HIWORD(refMsg.lParam); // vertical position
            return this->OnMove(xPos * Dpi::ScaleX, yPos * Dpi::ScaleY) ? 0 : DefaultWndProc(refMsg);
        }

        case WM_SIZE: {
            int width  = LOWORD(refMsg.lParam); // the new width of the client area
            int height = HIWORD(refMsg.lParam); // the new height of the client area
            return this->OnSize(width * Dpi::ScaleX, height * Dpi::ScaleY) ? 0 : DefaultWndProc(refMsg);
        }

        case WM_SETTEXT: {
            LRESULT result = DefaultWndProc(refMsg);
            if (result == TRUE)
                this->_text = reinterpret_cast<PCWSTR>(refMsg.lParam);
            return result;
        }

        default: {
            return DefaultWndProc(refMsg);
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
