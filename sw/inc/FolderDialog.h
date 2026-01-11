#pragma once

#include "EnumBit.h"
#include "Window.h"
#include <shlobj.h>

namespace sw
{
    /**
     * @brief https://learn.microsoft.com/en-us/windows/win32/api/shlobj_core/ns-shlobj_core-browseinfoa
     */
    enum class FolderDialogFlags : UINT {
        // Only return file system directories. If the user selects folders that are not part of the
        // file system, the OK button is grayed.
        // Note  The OK button remains enabled for "\\server" items, as well as "\\server\share" and
        // directory items. However, if the user selects a "\\server" item, passing the PIDL returned
        // by SHBrowseForFolder to SHGetPathFromIDList fails.
        ReturnOnlyFileSystemDirs = 0x00000001,

        // Do not include network folders below the domain level in the dialog box's tree view control.
        DontGoBelowDomain = 0x00000002,

        // Include a status area in the dialog box. The callback function can set the status text by
        // sending messages to the dialog box. This flag is not supported when BIF_NEWDIALOGSTYLE is
        // specified.
        StatusText = 0x00000004,

        // Only return file system ancestors. An ancestor is a subfolder that is beneath the root folder
        // in the namespace hierarchy. If the user selects an ancestor of the root folder that is not
        // part of the file system, the OK button is grayed.
        ReturnFileSystemAncestors = 0x00000008,

        // Version 4.71. Include an edit control in the browse dialog box that allows the user to type
        // the name of an item.
        EditBox = 0x00000010,

        // Version 4.71. If the user types an invalid name into the edit box, the browse dialog box calls
        // the application's BrowseCallbackProc with the BFFM_VALIDATEFAILED message. This flag is ignored
        // if BIF_EDITBOX is not specified.
        Validate = 0x00000020,

        // Version 5.0. Use the new user interface. Setting this flag provides the user with a larger
        // dialog box that can be resized. The dialog box has several new capabilities, including:
        // drag-and-drop capability within the dialog box, reordering, shortcut menus, new folders,
        // delete, and other shortcut menu commands.
        // Note  If COM is initialized through CoInitializeEx with the COINIT_MULTITHREADED flag set,
        // SHBrowseForFolder fails if BIF_NEWDIALOGSTYLE is passed.
        NewDialogStyle = 0x00000040,

        // Version 5.0. The browse dialog box can display URLs. The BIF_USENEWUI and BIF_BROWSEINCLUDEFILES
        // flags must also be set. If any of these three flags are not set, the browser dialog box rejects
        // URLs. Even when these flags are set, the browse dialog box displays URLs only if the folder that
        // contains the selected item supports URLs. When the folder's IShellFolder::GetAttributesOf method
        // is called to request the selected item's attributes, the folder must set the SFGAO_FOLDER attribute
        // flag. Otherwise, the browse dialog box will not display the URL.
        BrowseIncludeUrls = 0x00000080,

        // Version 5.0. Use the new user interface, including an edit box. This flag is equivalent to
        // BIF_EDITBOX | BIF_NEWDIALOGSTYLE.
        // Note  If COM is initialized through CoInitializeEx with the COINIT_MULTITHREADED flag set,
        // SHBrowseForFolder fails if BIF_USENEWUI is passed.
        UseNewUI = 0x00000010 | 0x00000040,

        // Version 6.0. When combined with BIF_NEWDIALOGSTYLE, adds a usage hint to the dialog box, in place
        // of the edit box. BIF_EDITBOX overrides this flag.
        UsageHint = 0x00000100,

        // Version 6.0. Do not include the New Folder button in the browse dialog box.
        NoNewFolderButton = 0x00000200,

        // Version 6.0. When the selected item is a shortcut, return the PIDL of the shortcut itself rather
        // than its target.
        NoTranslateTargets = 0x00000400,

        // Only return computers. If the user selects anything other than a computer, the OK button is grayed.
        BrowseForComputer = 0x00001000,

        // Only allow the selection of printers. If the user selects anything other than a printer, the OK
        // button is grayed.
        // In Windows XP and later systems, the best practice is to use a Windows XP-style dialog, setting
        // the root of the dialog to the Printers and Faxes folder (CSIDL_PRINTERS).
        BrowseForPrinter = 0x00002000,

        // Version 4.71. The browse dialog box displays files as well as folders.
        BrowseIncludeFiles = 0x00004000,

        // Version 5.0. The browse dialog box can display sharable resources on remote systems. This is intended
        // for applications that want to expose remote shares on a local system. The BIF_NEWDIALOGSTYLE flag must
        // also be set.
        Sharable = 0x00008000,

        // Windows 7 and later. Allow folder junctions such as a library or a compressed file with a .zip file
        // name extension to be browsed.
        BrowseFileJunctions = 0x00010000,
    };

    /**
     * @brief 标记FolderDialogFlags枚举支持位运算
     */
    _SW_ENUM_ENABLE_BIT_OPERATIONS(FolderDialogFlags);

    /**
     * @brief 选择文件夹对话框
     */
    class FolderBrowserDialog : public IDialog
    {
    private:
        /**
         * @brief BROWSEINFOW结构体
         */
        BROWSEINFOW _bi{};

        /**
         * @brief 储存文件名的缓冲区
         */
        std::vector<wchar_t> _buffer;

        /**
         * @brief 对话框上方显示的描述性文本
         */
        std::wstring _description;

    public:
        /**
         * @brief 储存文件名的缓冲区大小，值不能小于MAX_PATH
         */
        const Property<int> BufferSize;

        /**
         * @brief 对话框标志
         */
        const Property<FolderDialogFlags> Flags;

        /**
         * @brief 对话框上方显示的描述性文本
         */
        const Property<std::wstring> Description;

        /**
         * @brief 选中文件夹的路径
         */
        const ReadOnlyProperty<std::wstring> SelectedPath;

        /**
         * @brief 是否显示“新建文件夹”按钮
         */
        const Property<bool> NewFolderButton;

    public:
        /**
         * @brief 初始化FolderBrowserDialog
         */
        FolderBrowserDialog();

        /**
         * @brief FolderBrowserDialog默认不支持该函数，调用该函数不会执行任何操作
         */
        virtual void Close() override;

        /**
         * @brief FolderBrowserDialog默认不支持该函数，调用该函数不会执行任何操作
         */
        virtual void Show() override;

        /**
         * @brief  显示对话框，并指定所有者窗口
         * @return 若用户选择了文件夹则返回true，否则返回false
         */
        virtual int ShowDialog(Window *owner = nullptr) override;

        /**
         * @brief  显示对话框，并指定所有者窗口
         * @return 若用户选择了文件夹则返回true，否则返回false
         */
        virtual int ShowDialog(Window &owner);

    protected:
        /**
         * @brief 获取BROWSEINFOW指针
         */
        BROWSEINFOW *GetBI();

        /**
         * @brief 获取指向缓冲区的指针
         */
        wchar_t *GetBuffer();

        /**
         * @brief 清空缓冲区
         */
        void ClearBuffer();
    };
}
