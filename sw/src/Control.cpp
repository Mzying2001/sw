#include "Control.h"
#include "App.h"

sw::Control::Control()
    : ControlId(
          Property<int>::Init(this)
              .Getter([](Control *self) -> int {
                  return GetDlgCtrlID(self->_hwnd);
              })),

      IsInHierarchy(
          Property<bool>::Init(this)
              .Getter([](Control *self) -> bool {
                  if (self->_hwnd == NULL || self->_isDestroyed) {
                      return false;
                  }
                  auto container = WndBase::_GetControlInitContainer();
                  return container == nullptr || ::GetParent(self->_hwnd) != container->_hwnd;
              }))
{
}

sw::Control::~Control()
{
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

    if (_hwnd == NULL || _isDestroyed) {
        return false;
    }

    RECT rect = Rect;
    auto text = GetInternalText().c_str();

    HWND oldHwnd = _hwnd;
    HWND hParent = ::GetParent(oldHwnd);

    wchar_t className[256];
    GetClassNameW(oldHwnd, className, 256);

    HMENU id = reinterpret_cast<HMENU>(
        static_cast<uintptr_t>(GetDlgCtrlID(oldHwnd)));

    // 用 CBT 钩子让新 HWND 在 WM_NCCREATE 之前完成绑定与子类化，
    // 流程与 WndBase::InitControl 完全一致。
    WndBase::_pendingInit = this;
    HHOOK hHook           = SetWindowsHookExW(WH_CBT, WndBase::_CbtProc, NULL, GetCurrentThreadId());

    if (hHook == NULL) {
        WndBase::_pendingInit = nullptr;
        return false;
    }

    CreateWindowExW(
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

    UnhookWindowsHookEx(hHook);
    WndBase::_pendingInit = nullptr;

    // CBT 钩子触发时已把 _hwnd 切换到新句柄；CreateWindowExW 失败时钩子不会
    // 触发，_hwnd 仍为 oldHwnd，可据此判断是否成功。
    if (_hwnd == oldHwnd) {
        return false;
    }

    // CBT 钩子已经把 _hwnd 切到新句柄，并刷新了 _originalWndProc（旧窗口和新窗口
    // 同类，原始 WndProc 一致）。旧 HWND 仍指向 _WndProc 与本对象的 prop，必须把
    // WndProc 还原为原始类 WndProc 后再销毁，让销毁路径走原生清理而不再回到框架。
    SetWindowLongPtrW(oldHwnd, GWLP_WNDPROC,
                      reinterpret_cast<LONG_PTR>(_originalWndProc));
    DestroyWindow(oldHwnd);

    SendMessageW(WM_SETFONT, (WPARAM)GetFontHandle(), TRUE);
    UpdateSiblingsZOrder();
    OnHandleChanged(_hwnd);
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

bool sw::Control::OnCustomDraw(NMCUSTOMDRAW *pNMCD, LRESULT &result)
{
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
    return false;
}

void sw::Control::OnHandleChanged(HWND hwnd)
{
    RaisePropertyChanged(&Control::Handle);
}
