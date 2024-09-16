#include "FileDialog.h"
#include "Utils.h"

sw::FileFilter::FileFilter(std::initializer_list<std::pair<std::wstring, std::wstring>> filters)
{
    this->SetFilter(filters);
}

bool sw::FileFilter::AddFilter(const std::wstring &name, const std::wstring &filter)
{
    if (name.empty() || filter.empty()) {
        return false;
    }

    if (!this->_buffer.empty()) {
        this->_buffer.pop_back();
    }

    for (auto c : name) {
        this->_buffer.push_back(c);
    }
    this->_buffer.push_back(0);

    for (auto c : filter) {
        this->_buffer.push_back(c);
    }
    this->_buffer.push_back(0);
    this->_buffer.push_back(0);

    return true;
}

int sw::FileFilter::SetFilter(std::initializer_list<std::pair<std::wstring, std::wstring>> filters)
{
    int result = 0;
    this->Clear();
    for (auto &pair : filters) {
        result += this->AddFilter(pair.first, pair.second);
    }
    return result;
}

void sw::FileFilter::Clear()
{
    this->_buffer.clear();
}

wchar_t *sw::FileFilter::GetFilterStr()
{
    return this->_buffer.empty() ? nullptr : this->_buffer.data();
}

sw::FileDialogBase::FileDialogBase()
    : BufferSize(
          // ger
          [this]() -> int {
              return (int)this->_buffer.size();
          },
          // set
          [this](const int &value) {
              int size = Utils::Max(MAX_PATH, value);
              this->_buffer.resize(size);
              this->_ofn.lpstrFile = this->_buffer.data();
              this->_ofn.nMaxFile  = this->_buffer.size();
          }),

      Flags(
          // get
          [this]() -> FileDialogFlags {
              return static_cast<FileDialogFlags>(this->_ofn.Flags);
          },
          // set
          [this](const FileDialogFlags &value) {
              this->_ofn.Flags = static_cast<DWORD>(value);
          }),

      Title(
          // get
          [this]() -> std::wstring {
              return this->_title;
          },
          // set
          [this](const std::wstring &value) {
              this->_title = value;
              if (this->_title.empty()) {
                  this->_ofn.lpstrTitle = nullptr;
              } else {
                  this->_ofn.lpstrTitle = this->_title.c_str();
              }
          }),

      InitialDir(
          // get
          [this]() -> std::wstring {
              return this->_initialDir;
          },
          // set
          [this](const std::wstring &value) {
              this->_initialDir = value;
              if (this->_initialDir.empty()) {
                  this->_ofn.lpstrInitialDir = nullptr;
              } else {
                  this->_ofn.lpstrInitialDir = this->_initialDir.c_str();
              }
          }),

      Filter(
          // get
          [this]() -> FileFilter * {
              return &this->_filter;
          }),

      FilterIndex(
          // ger
          [this]() -> int {
              return (int)this->_ofn.nFilterIndex - 1;
          },
          // set
          [this](const int &value) {
              this->_ofn.nFilterIndex = Utils::Max(0, value) + 1;
          }),

      FileName(
          // get
          [this]() -> std::wstring {
              return this->_buffer.data();
          })
{
    this->_ofn.lStructSize       = sizeof(this->_ofn);
    this->_ofn.lpstrFileTitle    = nullptr;
    this->_ofn.nMaxFileTitle     = 0;
    this->_ofn.lpstrCustomFilter = nullptr; // 直接设置Filter即可，不提供CustomFilter支持
    this->_ofn.nMaxCustFilter    = 0;

    this->BufferSize  = MAX_PATH;
    this->Title       = L"";
    this->InitialDir  = L"";
    this->FilterIndex = 0;
}

void sw::FileDialogBase::SetFilter(const FileFilter &filter)
{
    this->_filter = filter;
}

bool sw::FileDialogBase::ShowDialog()
{
    return this->ShowDialog(Window::ActiveWindow);
}

bool sw::FileDialogBase::ShowDialog(const Window &owner)
{
    return this->ShowDialog(&owner);
}

OPENFILENAMEW *sw::FileDialogBase::GetOFN()
{
    this->_ofn.lpstrFilter = this->_filter.GetFilterStr();
    return &this->_ofn;
}

wchar_t *sw::FileDialogBase::GetBuffer()
{
    return this->_buffer.data();
}

void sw::FileDialogBase::ClearBuffer()
{
    this->_buffer.at(0) = 0;
}

sw::OpenFileDialog::OpenFileDialog()
{
    this->Flags = this->Flags.Get() |
                  FileDialogFlags::PathMustExist |
                  FileDialogFlags::FileMustExist;
}

bool sw::OpenFileDialog::ShowDialog(const Window *owner)
{
    HWND hOwner     = owner ? owner->Handle.Get() : NULL;
    auto pOFN       = this->GetOFN();
    pOFN->hwndOwner = hOwner;

    bool result   = false;
    DWORD errcode = 0;
    do {
        if (errcode == FNERR_BUFFERTOOSMALL) {
            this->BufferSize = *reinterpret_cast<uint16_t *>(this->GetBuffer());
        }
        this->ClearBuffer();
        result = GetOpenFileNameW(pOFN);
    } while (!result && ((errcode = CommDlgExtendedError()) == FNERR_BUFFERTOOSMALL));

    return result;
}

sw::SaveFileDialog::SaveFileDialog()
{
    this->Flags = this->Flags.Get() |
                  FileDialogFlags::PathMustExist |
                  FileDialogFlags::FileMustExist |
                  FileDialogFlags::OverwritePrompt;
}

bool sw::SaveFileDialog::ShowDialog(const Window *owner)
{
    HWND hOwner     = owner ? owner->Handle.Get() : NULL;
    auto pOFN       = this->GetOFN();
    pOFN->hwndOwner = hOwner;

    bool result   = false;
    DWORD errcode = 0;
    do {
        if (errcode == FNERR_BUFFERTOOSMALL) {
            this->BufferSize = *reinterpret_cast<uint16_t *>(this->GetBuffer());
        }
        this->ClearBuffer();
        result = GetSaveFileNameW(pOFN);
    } while (!result && ((errcode = CommDlgExtendedError()) == FNERR_BUFFERTOOSMALL));

    return result;
}
