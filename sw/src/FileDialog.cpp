#include "FileDialog.h"
#include "Path.h"
#include "Utils.h"
#include "Window.h"

namespace
{
    /**
     * @brief FileDialog缓冲区默认大小
     */
    constexpr int _FileDialogInitialBufferSize = 1024;
}

sw::FileDialog::FileDialog()
    : BufferSize(
          Property<int>::Init(this)
              .Getter([](FileDialog *self) -> int {
                  return (int)self->_buffer.size();
              })
              .Setter([](FileDialog *self, int value) {
                  int size = Utils::Max(MAX_PATH, value);
                  self->_buffer.resize(size);
                  self->_ofn.lpstrFile = self->_buffer.data();
                  self->_ofn.nMaxFile  = (DWORD)self->_buffer.size();
                  self->ClearBuffer(); // 清空缓冲区，防止BufferSize比原来小时获取FileName访问到缓冲区外的内存
              })),

      Flags(
          Property<FileDialogFlags>::Init(this)
              .Getter([](FileDialog *self) -> FileDialogFlags {
                  return static_cast<FileDialogFlags>(self->_ofn.Flags);
              })
              .Setter([](FileDialog *self, FileDialogFlags value) {
                  self->_ofn.Flags = static_cast<DWORD>(value);
              })),

      Title(
          Property<std::wstring>::Init(this)
              .Getter([](FileDialog *self) -> std::wstring {
                  return self->_title;
              })
              .Setter([](FileDialog *self, const std::wstring &value) {
                  self->_title = value;
                  if (self->_title.empty()) {
                      self->_ofn.lpstrTitle = nullptr;
                  } else {
                      self->_ofn.lpstrTitle = self->_title.c_str();
                  }
              })),

      InitialDir(
          Property<std::wstring>::Init(this)
              .Getter([](FileDialog *self) -> std::wstring {
                  return self->_initialDir;
              })
              .Setter([](FileDialog *self, const std::wstring &value) {
                  self->_initialDir = value;
                  if (self->_initialDir.empty()) {
                      self->_ofn.lpstrInitialDir = nullptr;
                  } else {
                      self->_ofn.lpstrInitialDir = self->_initialDir.c_str();
                  }
              })),

      Filters(
          Property<ObservableCollection<FileFilterItem> *>::Init(this)
              .Getter([](FileDialog *self) -> ObservableCollection<FileFilterItem> * {
                  return &self->_filters;
              })),

      FilterIndex(
          Property<int>::Init(this)
              .Getter([](FileDialog *self) -> int {
                  return (int)self->_ofn.nFilterIndex - 1;
              })
              .Setter([](FileDialog *self, int value) {
                  self->_ofn.nFilterIndex = Utils::Max(0, value) + 1;
              })),

      FileName(
          Property<std::wstring>::Init(this)
              .Getter([](FileDialog *self) -> std::wstring {
                  if (!self->MultiSelect) {
                      std::wstring result(self->GetBuffer());
                      self->ProcessFileName(result);
                      return result;
                  } else {
                      std::wstring path(self->GetBuffer());
                      wchar_t *pFile = self->GetBuffer() + path.size() + 1;
                      std::wstring result(*pFile ? Path::Combine({path, pFile}) : path);
                      self->ProcessFileName(result);
                      return result;
                  }
              })),

      MultiSelect(
          Property<bool>::Init(this)
              .Getter([](FileDialog *self) -> bool {
                  return (self->Flags & FileDialogFlags::AllowMultiSelect) == FileDialogFlags::AllowMultiSelect;
              })
              .Setter([](FileDialog *self, bool value) {
                  if (value) {
                      self->Flags |= FileDialogFlags::AllowMultiSelect;
                  } else {
                      self->Flags &= ~FileDialogFlags::AllowMultiSelect;
                  }
              })),

      FileNames(
          ReadOnlyProperty<sw::List<std::wstring>>::Init(this)
              .Getter([](FileDialog *self) -> List<std::wstring> {
                  List<std::wstring> result;

                  if (!self->MultiSelect) {
                      auto fileName = self->FileName.Get();
                      if (!fileName.empty())
                          result.Add(fileName);
                      return result;
                  }

                  std::wstring path(self->GetBuffer());
                  wchar_t *pFile = self->GetBuffer() + path.size() + 1;

                  if (*pFile == 0) { // 多选状态下只选中一项时，buffer中存放的就是选择的文件路径
                      if (!path.empty()) {
                          result.Add(path);
                          self->ProcessFileName(result.GetAt(result.Count() - 1));
                      }
                      return result;
                  }

                  while (*pFile) {
                      std::wstring file = pFile;
                      result.Add(Path::Combine({path, file}));
                      self->ProcessFileName(result.GetAt(result.Count() - 1));
                      pFile += file.size() + 1;
                  }
                  return result;
              }))
{
    _ofn.lStructSize       = sizeof(_ofn);
    _ofn.lpstrFileTitle    = nullptr;
    _ofn.nMaxFileTitle     = 0;
    _ofn.lpstrCustomFilter = nullptr; // 直接设置Filter即可，不提供CustomFilter支持
    _ofn.nMaxCustFilter    = 0;

    BufferSize  = _FileDialogInitialBufferSize;
    Flags       = FileDialogFlags::Explorer;
    Title       = L"";
    InitialDir  = L"";
    FilterIndex = 0;

    _filters.CollectionChanged +=
        NotifyCollectionChangedEventHandler(*this, &FileDialog::_FiltersCollectionChangedHandler);
}

void sw::FileDialog::Close()
{
}

void sw::FileDialog::Show()
{
}

OPENFILENAMEW *sw::FileDialog::GetOFN()
{
    if (_filterBuffer.empty()) {
        _ofn.lpstrFilter = nullptr;
    } else {
        _ofn.lpstrFilter = _filterBuffer.data();
    }
    return &_ofn;
}

wchar_t *sw::FileDialog::GetBuffer()
{
    return _buffer.data();
}

void sw::FileDialog::ClearBuffer()
{
    _buffer.at(0) = 0;
    _buffer.at(1) = 0; // 两个'\0'表示结束，防止多选时FileName的意外拼接
}

void sw::FileDialog::ProcessFileName(std::wstring &fileName)
{
}

void sw::FileDialog::_AppendFilterToBuffer(const FileFilterItem &filter)
{
    auto &buffer = _filterBuffer;

    if (!buffer.empty()) {
        buffer.pop_back();
    }

    buffer.insert(buffer.end(), filter.name.begin(), filter.name.end());
    buffer.push_back(0);

    buffer.insert(buffer.end(), filter.filter.begin(), filter.filter.end());
    buffer.push_back(0);
    buffer.push_back(0);
}

void sw::FileDialog::_ResetFilterBuffer()
{
    _filterBuffer.clear();

    for (auto &item : _filters.GetInternalVector()) {
        _AppendFilterToBuffer(item);
    }
}

void sw::FileDialog::_FiltersCollectionChangedHandler(
    INotifyCollectionChanged &sender, NotifyCollectionChangedEventArgs &args)
{
    if (args.action == NotifyCollectionChangedAction::Add &&
        args.index == _filters.Count() - 1) {
        _AppendFilterToBuffer(_filters.GetAt(args.index));
    } else {
        _ResetFilterBuffer();
    }
}

sw::OpenFileDialog::OpenFileDialog()
{
    Flags |= FileDialogFlags::PathMustExist |
             FileDialogFlags::FileMustExist;
}

int sw::OpenFileDialog::ShowDialog(Window *owner)
{
    if (owner == nullptr) {
        owner = Window::ActiveWindow;
    }

    HWND hOwner     = owner ? owner->Handle.Get() : NULL;
    auto pOFN       = GetOFN();
    pOFN->hwndOwner = hOwner;

    bool result   = false;
    DWORD errcode = 0;
    do {
        if (errcode == FNERR_BUFFERTOOSMALL) {
            BufferSize = *reinterpret_cast<uint16_t *>(GetBuffer());
        }
        ClearBuffer();
        result = GetOpenFileNameW(pOFN);
    } while (!result && ((errcode = CommDlgExtendedError()) == FNERR_BUFFERTOOSMALL));

    return result;
}

int sw::OpenFileDialog::ShowDialog(Window &owner)
{
    return ShowDialog(&owner);
}

sw::SaveFileDialog::SaveFileDialog()
    : InitialFileName(
          Property<std::wstring>::Init(this)
              .Getter(
                  [](SaveFileDialog *self) -> std::wstring {
                      return self->_initialFileName;
                  })
              .Setter(
                  [](SaveFileDialog *self, const std::wstring &value) {
                      self->_initialFileName = value;
                  }))
{
    Flags |= FileDialogFlags::PathMustExist |
             FileDialogFlags::FileMustExist |
             FileDialogFlags::OverwritePrompt;
}

int sw::SaveFileDialog::ShowDialog(Window *owner)
{
    if (owner == nullptr) {
        owner = Window::ActiveWindow;
    }

    HWND hOwner     = owner ? owner->Handle.Get() : NULL;
    auto pOFN       = GetOFN();
    pOFN->hwndOwner = hOwner;

    bool result   = false;
    DWORD errcode = 0;
    do {
        if (errcode == FNERR_BUFFERTOOSMALL) {
            BufferSize = *reinterpret_cast<uint16_t *>(GetBuffer());
        }
        if (_initialFileName.empty()) {
            ClearBuffer();
        } else {
            _SetInitialFileName();
        }
        result = GetSaveFileNameW(pOFN);
    } while (!result && ((errcode = CommDlgExtendedError()) == FNERR_BUFFERTOOSMALL));

    if (!result) {
        ClearBuffer();
    }
    return result;
}

int sw::SaveFileDialog::ShowDialog(Window &owner)
{
    return ShowDialog(&owner);
}

void sw::SaveFileDialog::ProcessFileName(std::wstring &fileName)
{
    int index     = FilterIndex;
    auto *filters = Filters.Get();

    if (index < 0 || index >= filters->Count()) {
        return;
    }

    auto &ext = filters->GetAt(index).defaultExt;
    if (ext.empty()) return;

    size_t indexSlash = fileName.find_last_of(L"\\/");
    size_t indexDot   = fileName.find_last_of(L'.');

    static const auto npos = std::wstring::npos;

    if (indexDot == npos || (indexSlash != npos && indexSlash > indexDot)) {
        fileName += ext;
    }
}

void sw::SaveFileDialog::_SetInitialFileName()
{
    auto &str = _initialFileName;
    int size  = (int)str.size();

    if (BufferSize < size + 2) {
        BufferSize = size + 2;
    }

    wchar_t *buffer = GetBuffer();
    memcpy(buffer, &str[0], sizeof(wchar_t) * size);
    buffer[size] = buffer[size + 1] = 0;
}
