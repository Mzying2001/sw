#include "CommandLink.h"

sw::CommandLink::CommandLink()
    : NoteText(
          Property<std::wstring>::Init(this)
              .Getter([](CommandLink *self) -> std::wstring {
                  int len = (int)self->SendMessageW(BCM_GETNOTELENGTH, 0, 0);
                  if (len <= 0)
                      return std::wstring{};
                  else {
                      std::wstring result;
                      DWORD buflen = len + 1;
                      result.resize(buflen);
                      self->SendMessageW(BCM_GETNOTE, reinterpret_cast<WPARAM>(&buflen), reinterpret_cast<LPARAM>(&result[0]));
                      result.resize(len);
                      return result;
                  }
              })
              .Setter([](CommandLink *self, const std::wstring &value) {
                  self->SendMessageW(BCM_SETNOTE, 0, reinterpret_cast<LPARAM>(value.c_str()));
                  if (self->AutoSize) {
                      self->InvalidateMeasure();
                  }
              }))
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

bool sw::CommandLink::OnKeyDown(VirtualKey key, const KeyFlags &flags)
{
    bool result = UIElement::OnKeyDown(key, flags);

    if (!result && key == VirtualKey::Enter) {
        OnClicked();
    }
    return result;
}
