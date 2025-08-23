#include "FolderDialog.h"
#include "Utils.h"

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
