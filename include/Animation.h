#pragma once

#include "Control.h"
#include <CommCtrl.h>

namespace sw
{
    /**
     * @brief 动画控件，仅支持无音频的avi动画
     */
    class Animation : public Control
    {
    public:
        /**
         * @brief 是否将动画居中显示
         */
        const Property<bool> Center;

        /**
         * @brief 是否在打开avi文件后自动播放
         */
        const Property<bool> AutoPlay;

        /**
         * @brief 动画是否正在播放
         */
        const ReadOnlyProperty<bool> IsPlaying;

    public:
        /**
         * @brief 初始化动画控件
         */
        Animation();

        /**
         * @brief            从指定模块中打开avi动画
         * @param hInstance  DLL或EXE的模块句柄
         * @param resourceId 动画的资源序号
         * @return           若函数成功则返回true，否则返回false
         */
        bool Open(HINSTANCE hInstance, int resourceId);

        /**
         * @brief          从本地文件打开avi动画
         * @param fileName 动画的文件路径
         * @return         若函数成功则返回true，否则返回false
         */
        bool Open(const std::wstring &fileName);

        /**
         * @brief          从本地文件打开avi动画，传入nullptr可以关闭打开的动画
         * @param fileName 动画的文件路径
         * @return         若函数成功则返回true，否则返回false
         */
        bool Open(const wchar_t *fileName);

        /**
         * @brief            播放动画
         * @param times      循环播放次数，设为-1表示无限循环播放
         * @param beginFrame 从第几帧开始播放，值必须小于65536
         * @param endFrame   播放到第几帧，值必须小于65536，设为-1表示播放到动画结尾
         * @return           若函数成功则返回true，否则返回false
         */
        bool Play(int times, int beginFrame, int endFrame);

        /**
         * @brief            播放动画
         * @param times      循环播放次数，设为-1表示无限循环播放
         * @return           若函数成功则返回true，否则返回false
         */
        bool Play(int times = -1);

        /**
         * @brief  停止播放
         * @return 若函数成功则返回true，否则返回false
         */
        bool Stop();
    };
}
