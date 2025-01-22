#include "FileDialog.h"
#include "Path.h"
#include "Utils.h"

namespace
{
    /**
     * @brief FileDialog缓冲区默认大小
     */
    constexpr int _FileDialogInitialBufferSize = 1024;
}

sw::FileFilter::FileFilter(std::initializer_list<FileFilterItem> filters)
{
    this->SetFilter(filters);
}

bool sw::FileFilter::AddFilter(const std::wstring &name, const std::wstring &filter, const std::wstring &defaultExt)
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

    this->_defaultExts.emplace_back(defaultExt);
    return true;
}

int sw::FileFilter::SetFilter(std::initializer_list<FileFilterItem> filters)
{
    int result = 0;
    this->Clear();
    for (auto &item : filters) {
        result += this->AddFilter(item.name, item.filter, item.defaultExt);
    }
    return result;
}

void sw::FileFilter::Clear()
{
    this->_buffer.clear();
    this->_defaultExts.clear();
}

wchar_t *sw::FileFilter::GetFilterStr()
{
    return this->_buffer.empty() ? nullptr : this->_buffer.data();
}

const wchar_t *sw::FileFilter::GetDefaultExt(int index)
{
    return (index >= 0 && index < (int)_defaultExts.size()) ? _defaultExts[index].c_str() : L"";
}

sw::FileDialog::FileDialog()
    : BufferSize(
          // get
          [this]() -> int {
              return (int)this->_buffer.size();
          },
          // set
          [this](const int &value) {
              int size = Utils::Max(MAX_PATH, value);
              this->_buffer.resize(size);
              this->_ofn.lpstrFile = this->_buffer.data();
              this->_ofn.nMaxFile  = (DWORD)this->_buffer.size();
              this->ClearBuffer(); // 清空缓冲区，防止BufferSize比原来小时获取FileName访问到缓冲区外的内存
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
          // get
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
              if (!this->MultiSelect) {
                  std::wstring result(this->GetBuffer());
                  this->ProcessFileName(result);
                  return result;
              } else {
                  std::wstring path(this->GetBuffer());
                  wchar_t *pFile = this->GetBuffer() + path.size() + 1;
                  std::wstring result(*pFile ? Path::Combine({path, pFile}) : path);
                  this->ProcessFileName(result);
                  return result;
              }
          }),

      MultiSelect(
          // get
          [this]() -> bool {
              return (this->Flags.Get() & FileDialogFlags::AllowMultiSelect) == FileDialogFlags::AllowMultiSelect;
          },
          // set
          [this](const bool &value) {
              if (value) {
                  this->Flags = this->Flags.Get() | FileDialogFlags::AllowMultiSelect;
              } else {
                  this->Flags = this->Flags.Get() & ~FileDialogFlags::AllowMultiSelect;
              }
          }),

      FileNames(
          // get
          [this]() -> List<std::wstring> {
              List<std::wstring> result;

              if (!this->MultiSelect) {
                  auto fileName = this->FileName.Get();
                  if (!fileName.empty())
                      result.Append(fileName);
                  return result;
              }

              std::wstring path(this->GetBuffer());
              wchar_t *pFile = this->GetBuffer() + path.size() + 1;

              if (*pFile == 0) { // 多选状态下只选中一项时，buffer中存放的就是选择的文件路径
                  if (!path.empty()) {
                      result.Append(path);
                      this->ProcessFileName(result[result.Count() - 1]);
                  }
                  return result;
              }

              while (*pFile) {
                  std::wstring file = pFile;
                  result.Append(Path::Combine({path, file}));
                  this->ProcessFileName(result[result.Count() - 1]);
                  pFile += file.size() + 1;
              }
              return result;
          })
{
    this->_ofn.lStructSize       = sizeof(this->_ofn);
    this->_ofn.lpstrFileTitle    = nullptr;
    this->_ofn.nMaxFileTitle     = 0;
    this->_ofn.lpstrCustomFilter = nullptr; // 直接设置Filter即可，不提供CustomFilter支持
    this->_ofn.nMaxCustFilter    = 0;

    this->BufferSize  = _FileDialogInitialBufferSize;
    this->Flags       = FileDialogFlags::Explorer;
    this->Title       = L"";
    this->InitialDir  = L"";
    this->FilterIndex = 0;
}

void sw::FileDialog::SetFilter(const FileFilter &filter)
{
    this->_filter = filter;
}

bool sw::FileDialog::ShowDialog()
{
    return this->ShowDialog(Window::ActiveWindow);
}

bool sw::FileDialog::ShowDialog(const Window &owner)
{
    return this->ShowDialog(&owner);
}

OPENFILENAMEW *sw::FileDialog::GetOFN()
{
    this->_ofn.lpstrFilter = this->_filter.GetFilterStr();
    return &this->_ofn;
}

wchar_t *sw::FileDialog::GetBuffer()
{
    return this->_buffer.data();
}

void sw::FileDialog::ClearBuffer()
{
    this->_buffer.at(0) = 0;
    this->_buffer.at(1) = 0; // 两个'\0'表示结束，防止多选时FileName的意外拼接
}

void sw::FileDialog::ProcessFileName(std::wstring &fileName)
{
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
    : InitialFileName(
          // get
          [this]() -> std::wstring {
              return this->_initialFileName;
          },
          // set
          [this](const std::wstring &value) {
              this->_initialFileName = value;
          })
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
        if (this->_initialFileName.empty()) {
            this->ClearBuffer();
        } else {
            this->_SetInitialFileName();
        }
        result = GetSaveFileNameW(pOFN);
    } while (!result && ((errcode = CommDlgExtendedError()) == FNERR_BUFFERTOOSMALL));

    if (!result) {
        this->ClearBuffer();
    }
    return result;
}

void sw::SaveFileDialog::ProcessFileName(std::wstring &fileName)
{
    const wchar_t *ext = this->Filter->GetDefaultExt(this->FilterIndex);
    if (ext == nullptr || ext[0] == L'\0') return;

    size_t indexSlash = fileName.find_last_of(L"\\/");
    size_t indexDot   = fileName.find_last_of(L'.');

    static const auto npos = std::wstring::npos;
    if (indexDot == npos || (indexSlash != npos && indexSlash > indexDot)) {
        fileName += ext;
    }
}

void sw::SaveFileDialog::_SetInitialFileName()
{
    auto &str = this->_initialFileName;
    int size  = (int)str.size();

    if (this->BufferSize < size + 2) {
        this->BufferSize = size + 2;
    }

    wchar_t *buffer = this->GetBuffer();
    memcpy(buffer, &str[0], sizeof(wchar_t) * size);
    buffer[size] = buffer[size + 1] = 0;
}
