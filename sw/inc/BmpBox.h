#pragma once

#include "StaticControl.h"

namespace sw
{
    /**
     * @brief 指定位图在BmpBox中的定位方式
     */
    enum class BmpBoxSizeMode {
        Normal,       // 位图置于BmpBox左上角
        StretchImage, // 拉伸位图填充BmpBox
        AutoSize,     // 调整BmpBox大小，使其等于所包含位图的大小
        CenterImage,  // 将位图居中显示
        Zoom,         // 图像按照原来的比例缩放
    };

    /**
     * @brief 用于显示一个位图的控件
     */
    class BmpBox : public StaticControl
    {
    private:
        /**
         * @brief 位图句柄
         */
        HBITMAP _hBitmap{NULL};

        /**
         * @brief 位图尺寸（以像素为单位），调用_UpdateBmpSize更新
         */
        SIZE _bmpSize{0, 0};

        /**
         * @brief 位图定位方式
         */
        BmpBoxSizeMode _sizeMode{BmpBoxSizeMode::Normal};

    public:
        /**
         * @brief 当前控件显示的位图句柄，使用Load函数可以加载位图
         */
        const ReadOnlyProperty<HBITMAP> BmpHandle;

        /**
         * @brief 位图在BmpBox中的定位方式
         */
        const Property<BmpBoxSizeMode> SizeMode;

    public:
        /**
         * @brief 初始化BmpBox
         */
        BmpBox();

        /**
         * @brief         加载位图，该函数会复制一个位图句柄作为显示的位图
         * @param hBitmap 要加载的位图
         * @return        加载到BmpBox的位图句柄，若加载失败则返回NULL，该资源由BmpBox内部管理，在加载新位图或控件销毁时会自动释放
         */
        HBITMAP Load(HBITMAP hBitmap);

        /**
         * @brief            从指定模块中加载位图
         * @param hInstance  DLL或EXE的模块句柄
         * @param resourceId 位图的资源序号
         * @return           加载到BmpBox的位图句柄，若加载失败则返回NULL，该资源由BmpBox内部管理，在加载新位图或控件销毁时会自动释放
         */
        HBITMAP Load(HINSTANCE hInstance, int resourceId);

        /**
         * @brief          从文件加载位图
         * @param fileName 位图文件的路径
         * @return         加载到BmpBox的位图句柄，若加载失败则返回NULL，该资源由BmpBox内部管理，在加载新位图或控件销毁时会自动释放
         */
        HBITMAP Load(const std::wstring &fileName);

        /**
         * @brief 清除加载的位图
         */
        void Clear();

        /**
         * @brief 调整控件尺寸为位图尺寸，未加载位图时该函数不生效
         */
        void SizeToImage();

    protected:
        /**
         * @brief  接收到WM_DESTROY时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnDestroy() override;

        /**
         * @brief  接收到WM_PAINT时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnPaint() override;

        /**
         * @brief               接收到WM_SIZE时调用该函数
         * @param newClientSize 改变后的用户区尺寸
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSize(Size newClientSize) override;
        /**
         * @brief               测量元素所需尺寸，无需考虑边框和边距
         * @param availableSize 可用的尺寸
         * @return              返回元素需要占用的尺寸
         */
        virtual Size MeasureOverride(const Size &availableSize) override;

    private:
        /**
         * @brief 更新_bmpSize
         */
        void _UpdateBmpSize();

        /**
         * @brief         设置位图
         * @param hBitmap 位图句柄
         */
        void _SetBmp(HBITMAP hBitmap);

        /**
         * @brief         传入的位图不为NULL时调用_SetBmp
         * @param hBitmap 位图句柄
         * @return        传入的位图
         */
        HBITMAP _SetBmpIfNotNull(HBITMAP hBitmap);
    };
}
