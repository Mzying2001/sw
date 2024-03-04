#pragma once

#include "Icon.h"
#include "StaticControl.h"

namespace sw
{
    /**
     * @brief 用于显示一个图标的控件
     */
    class IconBox : public StaticControl
    {
    private:
        /**
         * @brief 图标句柄
         */
        HICON _hIcon{NULL};

    public:
        /**
         * @brief 当前控件显示的图标句柄，使用Load函数可以加载图标
         */
        const ReadOnlyProperty<HICON> IconHandle;

    public:
        /**
         * @brief 初始化IconBox
         */
        IconBox();

        /**
         * @brief       加载图标，该函数会复制一个图标句柄作为显示的图标
         * @param hIcon 要加载的图标
         * @return      加载到IconBox的图标句柄，若加载失败则返回NULL，该资源由IconBox内部管理，在加载新图标或控件销毁时会自动释放
         */
        HICON Load(HICON hIcon);

        /**
         * @brief      加载系统标准图标
         * @param icon 图标样式
         * @return     加载到IconBox的图标句柄，若加载失败则返回NULL，该资源由IconBox内部管理，在加载新图标或控件销毁时会自动释放
         */
        HICON Load(StandardIcon icon);

        /**
         * @brief            从指定模块中加载图标
         * @param hInstance  DLL或EXE的模块句柄
         * @param resourceId 图标的资源序号
         * @return           加载到IconBox的图标句柄，若加载失败则返回NULL，该资源由IconBox内部管理，在加载新图标或控件销毁时会自动释放
         */
        HICON Load(HINSTANCE hInstance, int resourceId);

        /**
         * @brief          从文件加载图标
         * @param fileName 图标文件的路径
         * @return         加载到IconBox的图标句柄，若加载失败则返回NULL，该资源由IconBox内部管理，在加载新图标或控件销毁时会自动释放
         */
        HICON Load(const std::wstring &fileName);

        /**
         * @brief 清除加载的图标
         */
        void Clear();

    protected:
        /**
         * @brief  接收到WM_DESTROY时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnDestroy() override;

    private:
        /**
         * @brief       设置图标
         * @param hIcon 图标句柄
         */
        void _SetIcon(HICON hIcon);
    };
}
