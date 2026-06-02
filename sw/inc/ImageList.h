#pragma once

#include <windows.h>

// commctrl.h需要在windows.h之后包含
#include <commctrl.h>

namespace sw
{
    /**
     * @brief Win32图像列表句柄的RAII包装
     * @note 非包装对象在析构时会调用ImageList_Destroy；通过Wrap创建的包装对象不负责销毁句柄。
     */
    class ImageList
    {
    private:
        /**
         * @brief 图像列表的句柄
         */
        HIMAGELIST _hImageList;

        /**
         * @brief 记录是否为包装对象
         */
        bool _isWrap;

    protected:
        /**
         * @brief 使用已有句柄初始化图像列表对象
         * @param hImageList 图像列表句柄
         * @param isWrap 是否仅包装句柄，若为true则析构时不销毁句柄
         */
        ImageList(HIMAGELIST hImageList, bool isWrap) noexcept;

    public:
        /**
         * @brief 创建图像列表，该函数调用ImageList_Create
         * @param cx 每个图像的宽度，单位为像素
         * @param cy 每个图像的高度，单位为像素
         * @param flags 创建标志，与ImageList_Create的flags参数相同
         * @param cInitial 图像列表的初始容量
         * @param cGrow 容量不足时每次增长的图像数量
         */
        ImageList(int cx, int cy, UINT flags, int cInitial, int cGrow) noexcept;

        /**
         * @brief 拷贝构造图像列表对象
         * @param other 要拷贝的对象
         * @note 包装对象会共享原句柄；非包装对象会调用ImageList_Duplicate复制句柄。
         */
        ImageList(const ImageList &other) noexcept;

        /**
         * @brief 移动构造图像列表对象
         * @param other 要移动的对象，移动后其句柄被置空
         */
        ImageList(ImageList &&other) noexcept;

        /**
         * @brief 析构图像列表对象
         * @note 若当前对象不是包装对象且句柄有效，则调用ImageList_Destroy销毁句柄。
         */
        ~ImageList() noexcept;

        /**
         * @brief 拷贝赋值图像列表对象
         * @param other 要拷贝的对象
         * @return 当前对象
         * @note 原有非包装句柄会先被销毁；包装对象会共享原句柄，非包装对象会复制句柄。
         */
        ImageList &operator=(const ImageList &other) noexcept;

        /**
         * @brief 移动赋值图像列表对象
         * @param other 要移动的对象，移动后其句柄被置空
         * @return 当前对象
         */
        ImageList &operator=(ImageList &&other) noexcept;

        /**
         * @brief 创建图像列表，该函数调用ImageList_Create
         * @param cx 每个图像的宽度，单位为像素
         * @param cy 每个图像的高度，单位为像素
         * @param flags 创建标志，与ImageList_Create的flags参数相同
         * @param cInitial 图像列表的初始容量
         * @param cGrow 容量不足时每次增长的图像数量
         * @return 拥有新图像列表句柄的对象，创建失败时句柄为NULL
         */
        static ImageList Create(int cx, int cy, UINT flags, int cInitial, int cGrow) noexcept;

        /**
         * @brief 将已有图像列表句柄包装为ImageList对象
         * @param hImageList 要包装的图像列表句柄
         * @return 包装原有句柄的对象，该对象析构时不会销毁句柄
         */
        static ImageList Wrap(HIMAGELIST hImageList) noexcept;

        /**
         * @brief 复制图像，该函数调用ImageList_Copy
         * @param dst 目标图像列表
         * @param iDst 目标图像索引
         * @param src 源图像列表
         * @param iSrc 源图像索引
         * @param uFlags 复制方式标志，与ImageList_Copy的uFlags参数相同
         * @return 操作成功返回true，否则返回false
         */
        static bool Copy(const ImageList &dst, int iDst, const ImageList &src, int iSrc, UINT uFlags) noexcept;

        /**
         * @brief 锁定窗口并在指定窗口内显示拖拽图像，该函数调用ImageList_DragEnter
         * @param hwndLock 要锁定更新的窗口句柄
         * @param x 拖拽图像相对锁定窗口左上角的水平位置，单位为像素
         * @param y 拖拽图像相对锁定窗口左上角的垂直位置，单位为像素
         * @return 操作成功返回true，否则返回false
         * @note hwndLock为NULL时，坐标相对屏幕左上角。
         */
        static bool DragEnter(HWND hwndLock, int x, int y) noexcept;

        /**
         * @brief 解除窗口锁定并隐藏显示的拖拽图像，该函数调用ImageList_DragLeave
         * @param hwndLock 先前传给DragEnter的窗口句柄
         * @return 操作成功返回true，否则返回false
         */
        static bool DragLeave(HWND hwndLock) noexcept;

        /**
         * @brief 拖拽移动，一般在WM_MOUSEMOVE函数中调用，该函数调用ImageList_DragMove
         * @param x 拖拽图像相对锁定窗口左上角的新水平位置，单位为像素
         * @param y 拖拽图像相对锁定窗口左上角的新垂直位置，单位为像素
         * @return 操作成功返回true，否则返回false
         * @note 拖拽锁定窗口为NULL时，坐标相对屏幕左上角。
         */
        static bool DragMove(int x, int y) noexcept;

        /**
         * @brief 拖拽时显示或隐藏图像，该函数调用ImageList_DragShowNolock
         * @param fShow 为true时显示拖拽图像，为false时隐藏拖拽图像
         * @return 操作成功返回true，否则返回false
         */
        static bool DragShowNolock(bool fShow) noexcept;

        /**
         * @brief 结束拖拽操作并销毁临时拖拽图像列表，该函数调用ImageList_EndDrag
         */
        static void EndDrag() noexcept;

        /**
         * @brief 获取拖拽中的临时图像列表，该函数调用ImageList_GetDragImage
         * @param ppt 接收当前拖拽位置，可为NULL
         * @param pptHotspot 接收拖拽图像热点相对拖拽位置的偏移，可为NULL
         * @return 包装临时图像列表句柄的对象；该句柄由拖拽过程管理，EndDrag后失效
         */
        static ImageList GetDragImage(POINT *ppt, POINT *pptHotspot) noexcept;

        /**
         * @brief 加载图像列表，该函数调用ImageList_LoadImageA
         * @param hi 包含资源的模块实例句柄
         * @param lpbmp 位图资源名、资源ID或文件路径
         * @param cx 每个图像的宽度，单位为像素
         * @param cGrow 容量不足时每次增长的图像数量
         * @param crMask 用作透明掩码的颜色
         * @param uType 图像类型，与LoadImage的uType参数相同
         * @param uFlags 加载标志，与LoadImage的fuLoad参数相同
         * @return 拥有新图像列表句柄的对象，加载失败时句柄为NULL
         */
        static ImageList LoadImageA(HINSTANCE hi, LPCSTR lpbmp, int cx, int cGrow, COLORREF crMask, UINT uType, UINT uFlags) noexcept;

        /**
         * @brief 加载图像列表，该函数调用ImageList_LoadImageW
         * @param hi 包含资源的模块实例句柄
         * @param lpbmp 位图资源名、资源ID或文件路径
         * @param cx 每个图像的宽度，单位为像素
         * @param cGrow 容量不足时每次增长的图像数量
         * @param crMask 用作透明掩码的颜色
         * @param uType 图像类型，与LoadImage的uType参数相同
         * @param uFlags 加载标志，与LoadImage的fuLoad参数相同
         * @return 拥有新图像列表句柄的对象，加载失败时句柄为NULL
         */
        static ImageList LoadImageW(HINSTANCE hi, LPCWSTR lpbmp, int cx, int cGrow, COLORREF crMask, UINT uType, UINT uFlags) noexcept;

        /**
         * @brief 合并两个图像列表，该函数调用ImageList_Merge
         * @param iml1 第一个图像列表
         * @param i1 第一个图像列表中的图像索引
         * @param iml2 第二个图像列表
         * @param i2 第二个图像列表中的图像索引
         * @param dx 第二个图像相对第一个图像的水平偏移，单位为像素
         * @param dy 第二个图像相对第一个图像的垂直偏移，单位为像素
         * @return 拥有合并结果句柄的对象，合并失败时句柄为NULL
         */
        static ImageList Merge(const ImageList &iml1, int i1, const ImageList &iml2, int i2, int dx, int dy) noexcept;

        /**
         * @brief 读取图像列表，该函数调用ImageList_Read
         * @param pstm 要读取的流对象
         * @return 拥有读取结果句柄的对象，读取失败时句柄为NULL
         */
        static ImageList Read(IStream *pstm) noexcept;

    public:
        /**
         * @brief 获取图像列表句柄
         * @return 当前对象持有的HIMAGELIST句柄，所有权不转移
         */
        HIMAGELIST GetHandle() const noexcept;

        /**
         * @brief 判断当前对象是否为包装对象
         * @return 当前对象仅包装句柄且析构时不销毁句柄则返回true，否则返回false
         */
        bool IsWrap() const noexcept;

        /**
         * @brief 获取图像列表句柄并取消对句柄的托管
         * @return 原图像列表句柄，调用方负责在需要时销毁该句柄
         * @note 调用后当前对象的句柄被置空，析构时不会销毁原句柄。
         */
        HIMAGELIST ReleaseHandle() noexcept;

        /**
         * @brief 添加图像，该函数调用ImageList_Add
         * @param hbmImage 包含一个或多个图像的位图句柄
         * @param hbmMask 包含掩码的位图句柄，可为NULL
         * @return 新增图像中第一个图像的索引，失败返回-1
         */
        int Add(HBITMAP hbmImage, HBITMAP hbmMask) noexcept;

        /**
         * @brief 添加图标，该函数调用ImageList_AddIcon
         * @param hIcon 要添加的图标句柄
         * @return 新增图标的索引，失败返回-1
         */
        int AddIcon(HICON hIcon) noexcept;

        /**
         * @brief 添加图像，指定颜色为mask，该函数调用ImageList_AddMasked
         * @param hbmImage 包含一个或多个图像的位图句柄
         * @param crMask 用作透明掩码的颜色
         * @return 新增图像中第一个图像的索引，失败返回-1
         */
        int AddMasked(HBITMAP hbmImage, COLORREF crMask) noexcept;

        /**
         * @brief 开始拖拽图像，该函数调用ImageList_BeginDrag
         * @param iTrack 要拖拽的图像索引
         * @param dxHotspot 拖拽热点相对图像左上角的水平偏移，单位为像素
         * @param dyHotspot 拖拽热点相对图像左上角的垂直偏移，单位为像素
         * @return 操作成功返回true，否则返回false
         */
        bool BeginDrag(int iTrack, int dxHotspot, int dyHotspot) noexcept;

        /**
         * @brief 在指定上下文DC下绘制图像，该函数调用ImageList_Draw
         * @param i 要绘制的图像索引
         * @param hdcDst 目标设备上下文句柄
         * @param x 绘制位置的水平坐标，单位为像素
         * @param y 绘制位置的垂直坐标，单位为像素
         * @param fStyle 绘制样式标志，与ImageList_Draw的fStyle参数相同
         * @return 操作成功返回true，否则返回false
         */
        bool Draw(int i, HDC hdcDst, int x, int y, UINT fStyle) noexcept;

        /**
         * @brief 在指定上下文DC下绘制图像，该函数调用ImageList_DrawEx
         * @param i 要绘制的图像索引
         * @param hdcDst 目标设备上下文句柄
         * @param x 绘制位置的水平坐标，单位为像素
         * @param y 绘制位置的垂直坐标，单位为像素
         * @param dx 绘制宽度，单位为像素
         * @param dy 绘制高度，单位为像素
         * @param rgbBk 背景颜色，与ImageList_DrawEx的rgbBk参数相同
         * @param rgbFg 前景颜色，与ImageList_DrawEx的rgbFg参数相同
         * @param fStyle 绘制样式标志，与ImageList_DrawEx的fStyle参数相同
         * @return 操作成功返回true，否则返回false
         */
        bool Draw(int i, HDC hdcDst, int x, int y, int dx, int dy, COLORREF rgbBk, COLORREF rgbFg, UINT fStyle) noexcept;

        /**
         * @brief 复制当前图像列表，该函数调用ImageList_Duplicate
         * @return 拥有复制结果句柄的对象，复制失败时句柄为NULL
         */
        ImageList Duplicate() noexcept;

        /**
         * @brief 获取背景颜色，该函数调用ImageList_GetBkColor
         * @return 当前背景颜色，返回值语义与ImageList_GetBkColor相同
         */
        COLORREF GetBkColor() noexcept;

        /**
         * @brief 通过指定位置的图像创建图标句柄，该函数调用ImageList_GetIcon
         * @param i 图像索引
         * @param flags 绘制样式标志，与ImageList_GetIcon的flags参数相同
         * @return 新创建的图标句柄，失败返回NULL；调用方负责在不再使用时销毁该图标
         */
        HICON GetIcon(int i, UINT flags) noexcept;

        /**
         * @brief 获取图标大小，该函数调用ImageList_GetIconSize
         * @param cx 接收图像宽度，单位为像素
         * @param cy 接收图像高度，单位为像素
         * @return 操作成功返回true，否则返回false
         */
        bool GetIconSize(int &cx, int &cy) noexcept;

        /**
         * @brief 获取图像个数，该函数调用ImageList_GetImageCount
         * @return 图像列表中的图像数量
         */
        int GetImageCount() noexcept;

        /**
         * @brief 获取图像信息，该函数调用ImageList_GetImageInfo
         * @param i 图像索引
         * @param pImageInfo 接收图像信息的IMAGEINFO结构指针
         * @return 操作成功返回true，否则返回false
         */
        bool GetImageInfo(int i, IMAGEINFO *pImageInfo) noexcept;

        /**
         * @brief 移除指定图像，该函数调用ImageList_Remove
         * @param i 要移除的图像索引，传入-1时移除所有图像
         * @return 操作成功返回true，否则返回false
         */
        bool Remove(int i) noexcept;

        /**
         * @brief 移除所有图像，该函数调用ImageList_Remove
         * @return 操作成功返回true，否则返回false
         */
        bool RemoveAll() noexcept;

        /**
         * @brief 更换指定位置的图像，该函数调用ImageList_Replace
         * @param i 要更换的图像索引
         * @param hbmImage 新图像位图句柄
         * @param hbmMask 新掩码位图句柄，可为NULL
         * @return 操作成功返回true，否则返回false
         */
        bool Replace(int i, HBITMAP hbmImage, HBITMAP hbmMask) noexcept;

        /**
         * @brief 更换图标，该函数调用ImageList_ReplaceIcon
         * @param i 要更换的图像索引，传入-1时追加图标
         * @param hicon 新图标句柄
         * @return 被更换或追加图标的索引，失败返回-1
         */
        int ReplaceIcon(int i, HICON hicon) noexcept;

        /**
         * @brief 设置背景颜色，该函数调用ImageList_SetBkColor
         * @param clrBk 新背景颜色
         * @return 先前的背景颜色，返回值语义与ImageList_SetBkColor相同
         */
        COLORREF SetBkColor(COLORREF clrBk) noexcept;

        /**
         * @brief 设置拖拽图标为指定图标与当前拖拽图标的结合，该函数调用ImageList_SetDragCursorImage
         * @param iDrag 当前图像列表中要组合到拖拽图像上的图像索引
         * @param dxHotspot 新图像相对拖拽热点的水平偏移，单位为像素
         * @param dyHotspot 新图像相对拖拽热点的垂直偏移，单位为像素
         * @return 操作成功返回true，否则返回false
         */
        bool SetDragCursorImage(int iDrag, int dxHotspot, int dyHotspot) noexcept;

        /**
         * @brief 设置图像大小并移除所有图像，该函数调用ImageList_SetIconSize
         * @param cx 新图像宽度，单位为像素
         * @param cy 新图像高度，单位为像素
         * @return 操作成功返回true，否则返回false
         */
        bool SetIconSize(int cx, int cy) noexcept;

        /**
         * @brief 设置图像个数，该函数调用ImageList_SetImageCount
         * @param uNewCount 新图像数量
         * @return 操作成功返回true，否则返回false
         */
        bool SetImageCount(UINT uNewCount) noexcept;

        /**
         * @brief 将指定的图像添加到要用作覆盖遮罩的图像列表中，该函数调用ImageList_SetOverlayImage
         * @param iImage 要用作覆盖图像的图像索引
         * @param iOverlay 覆盖图像编号，用于INDEXTOOVERLAYMASK
         * @return 操作成功返回true，否则返回false
         */
        bool SetOverlayImage(int iImage, int iOverlay) noexcept;

        /**
         * @brief 写图像列表，该函数调用ImageList_Write
         * @param pstm 要写入的流对象
         * @return 操作成功返回true，否则返回false
         */
        bool Write(IStream *pstm) noexcept;

    private:
        /**
         * @brief 在当前对象拥有句柄时调用ImageList_Destroy
         */
        void _DestroyIfNotWrap() noexcept;
    };
}
