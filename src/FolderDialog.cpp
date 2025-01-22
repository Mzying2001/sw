#include "FolderDialog.h"
#include "Utils.h"

/**
 * @brief FolderBrowserDialog缓冲区默认大小
 */
static constexpr int _FolderBrowserDialogInitialBufferSize = MAX_PATH;

/**
 */

sw::FolderBrowserDialog::FolderBrowserDialog()
    : BufferSize(
          // get
          [this]() -> int {
              return (int)this->_buffer.size();
          },
          // set
          [this](const int &value) {
              this->_buffer.resize(Utils::Max(MAX_PATH, value));
              this->ClearBuffer();
          }),

      Flags(
          // get
          [this]() -> FolderDialogFlags {
              return static_cast<FolderDialogFlags>(this->_bi.ulFlags);
          },
          // set
          [this](const FolderDialogFlags &value) {
              this->_bi.ulFlags = static_cast<UINT>(value);
          }),

      Description(
          // get
          [this]() -> std::wstring {
              return this->_description;
          },
          // set
          [this](const std::wstring &value) {
              this->_description = value;
              if (this->_description.empty()) {
                  this->_bi.lpszTitle = nullptr;
              } else {
                  this->_bi.lpszTitle = this->_description.c_str();
              }
          }),

      SelectedPath(
          // get
          [this]() -> std::wstring {
              return this->GetBuffer();
          }),

      NewFolderButton(
          // get
          [this]() -> bool {
              return !((this->Flags.Get() & FolderDialogFlags::NoNewFolderButton) == FolderDialogFlags::NoNewFolderButton);
          },
          // set
          [this](const bool &value) {
              if (value) {
                  this->Flags = this->Flags.Get() & ~FolderDialogFlags::NoNewFolderButton;
              } else {
                  this->Flags = this->Flags.Get() | FolderDialogFlags::NoNewFolderButton;
              }
          })
{
    this->BufferSize  = _FolderBrowserDialogInitialBufferSize;
    this->Flags       = FolderDialogFlags::NewDialogStyle | FolderDialogFlags::ReturnOnlyFileSystemDirs;
    this->Description = L"";
}

bool sw::FolderBrowserDialog::ShowDialog()
{
    return this->ShowDialog(Window::ActiveWindow);
}

bool sw::FolderBrowserDialog::ShowDialog(const Window &owner)
{
    return this->ShowDialog(&owner);
}

bool sw::FolderBrowserDialog::ShowDialog(const Window *owner)
{
    HWND hOwner    = owner ? owner->Handle.Get() : NULL;
    auto pBI       = this->GetBI();
    pBI->hwndOwner = hOwner;

    auto pidl = SHBrowseForFolderW(pBI);
    if (pidl == nullptr) {
        return false;
    }

    bool result = SHGetPathFromIDListW(pidl, this->GetBuffer());
    CoTaskMemFree(pidl);
    return result;
}

BROWSEINFOW *sw::FolderBrowserDialog::GetBI()
{
    return &this->_bi;
}

wchar_t *sw::FolderBrowserDialog::GetBuffer()
{
    return this->_buffer.data();
}

void sw::FolderBrowserDialog::ClearBuffer()
{
    this->_buffer.at(0) = 0;
}
