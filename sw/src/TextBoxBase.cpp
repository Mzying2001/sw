#include "TextBoxBase.h"

sw::TextBoxBase::TextBoxBase()
    : ReadOnly(
          Property<bool>::Init(this)
              .Getter([](TextBoxBase *self) -> bool {
                  return self->GetStyle(ES_READONLY);
              })
              .Setter([](TextBoxBase *self, bool value) {
                  self->SendMessageW(EM_SETREADONLY, value, 0);
              })),

      HorizontalContentAlignment(
          Property<sw::HorizontalAlignment>::Init(this)
              .Getter([](TextBoxBase *self) -> sw::HorizontalAlignment {
                  LONG_PTR style = self->GetStyle();
                  if (style & ES_CENTER) {
                      return sw::HorizontalAlignment::Center;
                  } else if (style & ES_RIGHT) {
                      return sw::HorizontalAlignment::Right;
                  } else {
                      return sw::HorizontalAlignment::Left;
                  }
              })
              .Setter([](TextBoxBase *self, sw::HorizontalAlignment value) {
                  switch (value) {
                      case sw::HorizontalAlignment::Left: {
                          self->SetStyle(ES_CENTER | ES_RIGHT, false);
                          break;
                      }
                      case sw::HorizontalAlignment::Center: {
                          DWORD style = self->GetStyle();
                          style &= ~(ES_CENTER | ES_RIGHT);
                          style |= ES_CENTER;
                          self->SetStyle(style);
                          break;
                      }
                      case sw::HorizontalAlignment::Right: {
                          DWORD style = self->GetStyle();
                          style &= ~(ES_CENTER | ES_RIGHT);
                          style |= ES_RIGHT;
                          self->SetStyle(style);
                          break;
                      }
                      default: {
                          break;
                      }
                  }
                  self->Redraw();
              })),

      CanUndo(
          Property<bool>::Init(this)
              .Getter([](TextBoxBase *self) -> bool {
                  return self->SendMessageW(EM_CANUNDO, 0, 0);
              })),

      AcceptTab(
          Property<bool>::Init(this)
              .Getter([](TextBoxBase *self) -> bool {
                  return self->_acceptTab;
              })
              .Setter([](TextBoxBase *self, bool value) {
                  self->_acceptTab = value;
              }))
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

bool sw::TextBoxBase::OnChar(wchar_t ch, const KeyFlags &flags)
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

bool sw::TextBoxBase::OnKeyDown(VirtualKey key, const KeyFlags &flags)
{
    KeyDownEventArgs e(key, flags);
    this->RaiseRoutedEvent(e);

    if (!e.handledMsg && key == VirtualKey::Tab && (!this->_acceptTab || this->ReadOnly)) {
        bool shiftDown = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
        this->OnTabMove(!shiftDown);
    }

    return e.handledMsg;
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
