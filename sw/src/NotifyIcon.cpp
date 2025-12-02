#include "NotifyIcon.h"
#include "Icon.h"
#include "Screen.h"
#include <strsafe.h>

namespace
{
    /**
     * @brief 通知图标ID
     */
    constexpr int _NotifyIconId = 1;

    /**
     * @brief 自定义消息：通知图标消息
     */
    constexpr int _NotifyIconMsg = WM_USER + 1;
}

sw::NotifyIcon::NotifyIcon()
    : Icon(
          Property<HICON>::Init(this)
              .Getter([](NotifyIcon *self) -> HICON {
                  return self->_nid.hIcon;
              })
              .Setter([](NotifyIcon *self, HICON value) {
                  self->_nid.hIcon = value;
                  self->_nid.uFlags |= NIF_ICON;
                  self->_ModifyIcon();
                  self->_nid.uFlags &= ~NIF_ICON;
              })),

      ToolTip(
          Property<std::wstring>::Init(this)
              .Getter([](NotifyIcon *self) -> std::wstring {
                  return self->_nid.szTip;
              })
              .Setter([](NotifyIcon *self, const std::wstring &value) {
                  if (value.empty()) {
                      self->_nid.szTip[0] = L'\0';
                  } else {
                      constexpr size_t bufsize = sizeof(_nid.szTip) / sizeof(*_nid.szTip);
                      StringCchCopyNW(self->_nid.szTip, bufsize, value.c_str(), bufsize - 1);
                  }
                  self->_nid.uFlags |= NIF_TIP;
                  self->_ModifyIcon();
                  self->_nid.uFlags &= ~NIF_TIP;
              })),

      Visible(
          Property<bool>::Init(this)
              .Getter([](NotifyIcon *self) -> bool {
                  return (self->_nid.dwState & NIS_HIDDEN) == 0;
              })
              .Setter([](NotifyIcon *self, bool value) {
                  value ? self->Show() : self->Hide();
              })),

      ContextMenu(
          Property<sw::ContextMenu *>::Init(this)
              .Getter([](NotifyIcon *self) -> sw::ContextMenu * {
                  return self->_contextMenu;
              })
              .Setter([](NotifyIcon *self, sw::ContextMenu *value) {
                  self->_contextMenu = value;
              })),

      Rect(
          Property<sw::Rect>::Init(this)
              .Getter([](NotifyIcon *self) -> sw::Rect {
                  NOTIFYICONIDENTIFIER iconId{};
                  iconId.cbSize   = sizeof(NOTIFYICONIDENTIFIER);
                  iconId.hWnd     = self->_nid.hWnd;
                  iconId.uID      = self->_nid.uID;
                  iconId.guidItem = GUID_NULL;
                  RECT iconRect{};
                  if (FAILED(Shell_NotifyIconGetRect(&iconId, &iconRect))) {
                      return sw::Rect{};
                  } else {
                      return static_cast<sw::Rect>(iconRect);
                  }
              }))
{
    _nid.cbSize           = sizeof(NOTIFYICONDATAW);
    _nid.uID              = _NotifyIconId;
    _nid.uFlags           = NIF_STATE | NIF_ICON | NIF_MESSAGE;
    _nid.hIcon            = _GetDefaultIcon();
    _nid.dwState          = NIS_HIDDEN;
    _nid.dwStateMask      = NIS_HIDDEN;
    _nid.uCallbackMessage = _NotifyIconMsg;

    InitControl(L"STATIC", NULL, WS_CHILD, 0);

    _nid.hWnd = Handle;
    _AddIcon();
    _nid.uFlags = 0;

    // _nid.uVersion = NOTIFYICON_VERSION_4;
    // _ShellNotifyIcon(NIM_SETVERSION);
}

sw::NotifyIcon::~NotifyIcon()
{
    if (!IsDestroyed) {
        Destroy(); // 确保图标被删除
    }
}

LRESULT sw::NotifyIcon::WndProc(ProcMsg &refMsg)
{
    switch (refMsg.uMsg) {
        case _NotifyIconMsg: {
            OnNotyfyIconMessage(refMsg.wParam, refMsg.lParam);
            return 0;
        }
        case WM_NCDESTROY: {
            _DeleteIcon();
            return TBase::WndProc(refMsg);
        }
        default: {
            return TBase::WndProc(refMsg);
        }
    }
}

void sw::NotifyIcon::OnMenuCommand(int id)
{
    if (_contextMenu) {
        MenuItem *item = _contextMenu->GetMenuItem(id);
        if (item) item->CallCommand();
    }
}

void sw::NotifyIcon::OnNotyfyIconMessage(WPARAM wParam, LPARAM lParam)
{
    // int event = LOWORD(lParam);
    int event = static_cast<int>(lParam);

    switch (event) {
        case WM_LBUTTONUP: {
            OnClicked(Screen::CursorPosition);
            break;
        }
        case WM_LBUTTONDBLCLK: {
            OnDoubleClicked(Screen::CursorPosition);
            break;
        }
        case WM_RBUTTONUP: {
            OnContextMenuOpening(Screen::CursorPosition);
            break;
        }
    }
}

void sw::NotifyIcon::OnClicked(const Point &mousePos)
{
    if (Clicked) {
        Clicked(*this, mousePos);
    }
}

void sw::NotifyIcon::OnDoubleClicked(const Point &mousePos)
{
    if (DoubleClicked) {
        DoubleClicked(*this, mousePos);
    }
}

void sw::NotifyIcon::OnContextMenuOpening(const Point &mousePos)
{
    bool handled = false;

    if (ContextMenuOpening) {
        handled = ContextMenuOpening(*this, mousePos);
    }

    if (!handled) {
        ShowContextMenu(mousePos);
    }
}

NOTIFYICONDATAW &sw::NotifyIcon::GetNotifyIconData()
{
    return _nid;
}

bool sw::NotifyIcon::Show()
{
    return _ModifyState(0, NIS_HIDDEN);
}

bool sw::NotifyIcon::Hide()
{
    return _ModifyState(NIS_HIDDEN, NIS_HIDDEN);
}

void sw::NotifyIcon::Destroy()
{
    DestroyWindow(Handle);
}

bool sw::NotifyIcon::ShowContextMenu(const Point &point, sw::HorizontalAlignment horz, sw::VerticalAlignment vert)
{
    UINT uFlags = 0;
    HMENU hMenu = NULL;

    if (_contextMenu) {
        hMenu = _contextMenu->GetHandle();
    }
    if (hMenu == NULL) {
        return false;
    }

    switch (horz) {
        case sw::HorizontalAlignment::Left: {
            uFlags |= TPM_LEFTALIGN;
            break;
        }
        case sw::HorizontalAlignment::Right: {
            uFlags |= TPM_RIGHTALIGN;
            break;
        }
        case sw::HorizontalAlignment::Center:
        case sw::HorizontalAlignment::Stretch: {
            uFlags |= TPM_CENTERALIGN;
            break;
        }
    }

    switch (vert) {
        case sw::VerticalAlignment::Top: {
            uFlags |= TPM_TOPALIGN;
            break;
        }
        case sw::VerticalAlignment::Bottom: {
            uFlags |= TPM_BOTTOMALIGN;
            break;
        }
        case sw::VerticalAlignment::Center:
        case sw::VerticalAlignment::Stretch: {
            uFlags |= TPM_VCENTERALIGN;
            break;
        }
    }

    POINT pos = point;
    HWND hwnd = Handle;
    SetForegroundWindow(hwnd); // 没这句的话菜单无法正确关闭
    return TrackPopupMenu(hMenu, uFlags, pos.x, pos.y, 0, hwnd, nullptr);
}

bool sw::NotifyIcon::_ShellNotifyIcon(DWORD dwMessage)
{
    return Shell_NotifyIconW(dwMessage, &_nid);
}

bool sw::NotifyIcon::_AddIcon()
{
    return _ShellNotifyIcon(NIM_ADD);
}

bool sw::NotifyIcon::_DeleteIcon()
{
    return _ShellNotifyIcon(NIM_DELETE);
}

bool sw::NotifyIcon::_ModifyIcon()
{
    return _ShellNotifyIcon(NIM_MODIFY);
}

bool sw::NotifyIcon::_ModifyState(DWORD dwState, DWORD dwStateMask)
{
    _nid.uFlags |= NIF_STATE;
    _nid.dwState     = dwState;
    _nid.dwStateMask = dwStateMask;

    bool result = _ModifyIcon();
    _nid.uFlags &= ~NIF_STATE;
    return result;
}

HICON sw::NotifyIcon::_GetDefaultIcon()
{
    static HICON hDefIcon = []() -> HICON {
        HICON hIcon = ExtractIconW(App::Instance, App::ExePath->c_str(), 0);
        return hIcon != NULL ? hIcon : IconHelper::GetIconHandle(StandardIcon::Application);
    }();
    return hDefIcon;
}
