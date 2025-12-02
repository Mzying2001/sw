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
          Property<int>::Init(this)
              .Getter([](FolderBrowserDialog *self) -> int {
                  return (int)self->_buffer.size();
              })
              .Setter([](FolderBrowserDialog *self, int value) {
                  self->_buffer.resize(Utils::Max(MAX_PATH, value));
                  self->ClearBuffer();
              })),

      Flags(
          Property<FolderDialogFlags>::Init(this)
              .Getter([](FolderBrowserDialog *self) -> FolderDialogFlags {
                  return static_cast<FolderDialogFlags>(self->_bi.ulFlags);
              })
              .Setter([](FolderBrowserDialog *self, FolderDialogFlags value) {
                  self->_bi.ulFlags = static_cast<UINT>(value);
              })),

      Description(
          Property<std::wstring>::Init(this)
              .Getter([](FolderBrowserDialog *self) -> std::wstring {
                  return self->_description;
              })
              .Setter([](FolderBrowserDialog *self, const std::wstring &value) {
                  self->_description = value;
                  if (self->_description.empty()) {
                      self->_bi.lpszTitle = nullptr;
                  } else {
                      self->_bi.lpszTitle = self->_description.c_str();
                  }
              })),

      SelectedPath(
          Property<std::wstring>::Init(this)
              .Getter([](FolderBrowserDialog *self) -> std::wstring {
                  return self->GetBuffer();
              })),

      NewFolderButton(
          Property<bool>::Init(this)
              .Getter([](FolderBrowserDialog *self) -> bool {
                  return !((self->Flags & FolderDialogFlags::NoNewFolderButton) == FolderDialogFlags::NoNewFolderButton);
              })
              .Setter([](FolderBrowserDialog *self, bool value) {
                  if (value) {
                      self->Flags &= ~FolderDialogFlags::NoNewFolderButton;
                  } else {
                      self->Flags |= FolderDialogFlags::NoNewFolderButton;
                  }
              }))
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
