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
              RECT rect;
              GetWindowRect(this->_hwnd, &rect);
              this->_rect = rect;
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

      _text(),
      Text(
          // get
          [&]() -> const std::wstring & {
              return this->_text;
          },
          // set
          [&](const std::wstring &value) {
              SetWindowTextW(this->_hwnd, value.c_str());
              this->_text = value;
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

        SetWindowLongPtrW(this->_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    }
}

LRESULT sw::WndBase::WndProc(const ProcMsg &refMsg)
{
    switch (refMsg.uMsg) {
        case WM_CREATE: {
            return this->OnCreate() ? 0 : DefaultWndProc(refMsg);
        }

        case WM_DESTROY: {
            return this->OnDestroy() ? 0 : DefaultWndProc(refMsg);
        }

        case WM_PAINT: {
            return this->OnPaint() ? 0 : DefaultWndProc(refMsg);
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

bool sw::WndBase::OnDestroy()
{
    return false;
}

bool sw::WndBase::OnPaint()
{
    return false;
}

void sw::WndBase::Show(int nCmdShow)
{
    ShowWindow(this->_hwnd, nCmdShow);
}

void sw::WndBase::Close()
{
    CloseWindow(this->_hwnd);
}
