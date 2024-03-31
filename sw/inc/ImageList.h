#pragma once

#include "Dip.h"
#include "Point.h"
#include <CommCtrl.h>

namespace sw
{
    /**
     * @brief 图像列表
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
         * @brief            初始化图像列表
         * @param hImageList 图像列表的句柄
         * @param isWrap     是否为包装对象
         */
        ImageList(HIMAGELIST hImageList, bool isWrap);

    public:
        /**
         * @brief 创建图像列表，参数与ImageList_Create相同
         */
        ImageList(int cx, int cy, UINT flags, int cInitial, int cGrow);

        /**
         * @brief 拷贝构造
         */
        ImageList(const ImageList &value);

        /**
         * @brief 移动构造
         */
        ImageList(ImageList &&rvalue);

        /**
         * @brief 析构函数
         */
        virtual ~ImageList();

        /**
         * @brief 拷贝赋值
         */
        ImageList &operator=(const ImageList &value);

        /**
         * @brief 移动赋值
         */
        ImageList &operator=(ImageList &&rvalue);

        /**
         * @brief  创建一个图像列表，该函数调用ImageList_Create
         * @return 图像列表对象
         */
        static ImageList Create(int cx, int cy, UINT flags, int cInitial, int cGrow);

        /**
         * @brief            包装一个图像列表句柄为ImageList对象，通过该函数创建的对象析构时不会销毁句柄
         * @param hImageList 要包装的句柄
         * @return           包装原有句柄的对象
         */
        static ImageList Wrap(HIMAGELIST hImageList);

        /**
         * @brief 复制图像，该函数调用ImageList_Copy
         */
        static bool Copy(const ImageList &dst, int iDst, const ImageList &src, int iSrc, UINT uFlags);

        /**
         * @brief 锁定窗口并在指定窗口内显示拖拽图像，该函数调用ImageList_DragEnter
         */
        static bool DragEnter(HWND hwndLock, double x, double y);

        /**
         * @brief 解除窗口锁定并隐藏显示的拖拽图像，该函数调用ImageList_DragLeave
         */
        static bool DragLeave(HWND hwndLock);

        /**
         * @brief 拖拽移动，一般在WM_MOUSEMOVE函数中调用，该函数调用ImageList_DragMove
         */
        static bool DragMove(double x, double y);

        /**
         * @brief 拖拽时显示或隐藏图像，该函数调用ImageList_DragShowNolock
         */
        static bool DragShowNolock(bool fShow);

        /**
         * @brief 结束拖拽，该函数调用ImageList_EndDrag
         */
        static void EndDrag();

        /**
         * @brief 获取拖拽中图像的列表，该函数调用ImageList_GetDragImage
         */
        static ImageList GetDragImage(Point &pt, Point &ptHotspot);

        /**
         * @brief 加载图像列表，该函数调用ImageList_LoadImageA
         */
        static ImageList LoadImageA(HINSTANCE hi, LPCSTR lpbmp, int cx, int cGrow, COLORREF crMask, UINT uType, UINT uFlags);

        /**
         * @brief 加载图像列表，该函数调用ImageList_LoadImageW
         */
        static ImageList LoadImageW(HINSTANCE hi, LPCWSTR lpbmp, int cx, int cGrow, COLORREF crMask, UINT uType, UINT uFlags);

        /**
         * @brief 合并两个图像列表，该函数调用ImageList_Merge
         */
        static ImageList Merge(const ImageList &iml1, int i1, const ImageList &iml2, int i2, int dx, int dy);

        /**
         * @brief 读取图像列表，该函数调用ImageList_Read
         */
        static ImageList Read(IStream *pstm);

    public:
        /**
         * @brief 获取图像列表的句柄
         */
        HIMAGELIST GetHandle();

        /**
         * @brief 判断当前对象是否为包装对象
         */
        bool IsWrap();

        /**
         * @brief 添加图像，该函数调用ImageList_Add
         */
        int Add(HBITMAP hbmImage, HBITMAP hbmMask);

        /**
         * @brief 添加图像，指定颜色为mask，该函数调用ImageList_AddMasked
         */
        int AddMasked(HBITMAP hbmImage, COLORREF crMask);

        /**
         * @brief 开始拖拽图像，该函数调用ImageList_BeginDrag
         */
        bool BeginDrag(int iTrack, double dxHotspot, double dyHotspot);

        /**
         * @brief 在指定上下文DC下绘制图像，该函数调用ImageList_Draw
         */
        bool Draw(int i, HDC hdcDst, double x, double y, UINT fStyle);

        /**
         * @brief 在指定上下文DC下绘制图像，该函数调用ImageList_DrawEx
         */
        bool Draw(int i, HDC hdcDst, double x, double y, double dx, double dy, COLORREF rgbBk, COLORREF rgbFg, UINT fStyle);

        /**
         * @brief 以像素为单位，在指定上下文DC下绘制图像，该函数调用ImageList_Draw
         */
        bool DrawPx(int i, HDC hdcDst, int x, int y, UINT fStyle);

        /**
         * @brief 以像素为单位，在指定上下文DC下绘制图像，该函数调用ImageList_DrawEx
         */
        bool DrawPx(int i, HDC hdcDst, int x, int y, int dx, int dy, COLORREF rgbBk, COLORREF rgbFg, UINT fStyle);

        /**
         * @brief 复制当前图像列表，该函数调用ImageList_Duplicate
         */
        ImageList Duplicate();

        /**
         * @brief 获取背景颜色，该函数调用ImageList_GetBkColor
         */
        COLORREF GetBkColor();

        /**
         * @brief 通过指定位置的图像创建图标句柄，该函数调用ImageList_GetIcon
         */
        HICON GetIcon(int i, UINT flags);

        /**
         * @brief 获取图标大小，该函数调用ImageList_GetIconSize
         */
        bool GetIconSize(int &cx, int &cy);

        /**
         * @brief 获取图像个数，该函数调用ImageList_GetImageCount
         */
        int GetImageCount();

        /**
         * @brief 获取图像信息，该函数调用ImageList_GetImageInfo
         */
        bool GetImageInfo(int i, IMAGEINFO *pImageInfo);

        /**
         * @brief 移除指定图像，该函数调用ImageList_Remove
         */
        bool Remove(int i);

        /**
         * @brief 移除所有图像，该函数调用ImageList_Remove
         */
        bool RemoveAll();

        /**
         * @brief 更换指定位置的图像，该函数调用ImageList_Replace
         */
        bool Replace(int i, HBITMAP hbmImage, HBITMAP hbmMask);

        /**
         * @brief 更换图标，该函数调用ImageList_ReplaceIcon
         */
        int ReplaceIcon(int i, HICON hicon);

        /**
         * @brief 设置背景颜色，该函数调用ImageList_SetBkColor
         */
        COLORREF SetBkColor(COLORREF clrBk);

        /**
         * @brief 设置拖拽图标为指定图标与当前拖拽图标的结合，该函数调用ImageList_SetDragCursorImage
         */
        bool SetDragCursorImage(int iDrag, int dxHotspot, int dyHotspot);

        /**
         * @brief 设置图像大小并移除所有图像，该函数调用ImageList_SetIconSize
         */
        bool SetIconSize(int cx, int cy);

        /**
         * @brief 设置图像个数，该函数调用ImageList_SetImageCount
         */
        bool SetImageCount(UINT uNewCount);

        /**
         * @brief 将指定的图像添加到要用作覆盖遮罩的图像列表中，该函数调用ImageList_SetOverlayImage
         */
        bool SetOverlayImage(int iImage, int iOverlay);

        /**
         * @brief 写图像列表，该函数调用ImageList_Write
         */
        bool Write(IStream *pstm);

    private:
        /**
         * @brief 若_isWrap为false时调用ImageList_Destroy
         */
        void _DestroyIfNotWrap();
    };
}
