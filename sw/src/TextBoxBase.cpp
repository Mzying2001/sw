#include "TextBoxBase.h"

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
