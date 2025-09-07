#pragma once

#include "TextBoxBase.h"

namespace sw
{
    /**
     * @brief 数值调节框
     */
    class SpinBox : public TextBoxBase
    {
    private:
        /**
         * @brief UpDown控件的句柄
         */
        HWND _hUpDown = NULL;

        /**
         * @brief 记录UpDown控件的宽度
         */
        int _upDownWidth = 0;

        /**
         * @brief 储存加速信息
         */
        std::vector<UDACCEL> _accels;

    public:
        /**
         * @brief 最小值，默认为0
         */
        const Property<int> Minimum;

        /**
         * @brief 最大值，默认为100
         */
        const Property<int> Maximum;

        /**
         * @brief 当前值，默认为0
         */
        const Property<int> Value;

        /**
         * @brief 是否显示为十六进制，默认为false
         */
        const Property<bool> Hexadecimal;

        /**
         * @brief 每次点击UpDown控件时增量，默认为1
         */
        const Property<uint32_t> Increment;

    public:
        /**
         * @brief 初始化数值调节框
         */
        SpinBox();

        /**
         * @brief           添加加速信息
         * @param seconds   按住按钮多少秒后开始加速
         * @param increment 达到seconds秒后每次增加的值
         * @return          返回当前对象以支持链式调用
         */
        SpinBox &AddAccel(uint32_t seconds, uint32_t increment);

        /**
         * @brief 清除所有加速信息
         */
        void ClearAccels();

    protected:
        /**
         * @brief      控件句柄发生改变时调用该函数
         * @param hwnd 新的控件句柄
         */
        virtual void OnHandleChanged(HWND hwnd) override;

        /**
         * @brief               接收到WM_SIZE时调用该函数
         * @param newClientSize 改变后的用户区尺寸
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSize(Size newClientSize) override;

        /**
         * @brief            接收到WM_KILLFOCUS时调用该函数
         * @param hNextFocus 接收到焦点的hwnd，可能为NULL
         * @return           若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnKillFocus(HWND hNextFocus) override;

    private:
        /**
         * @brief 初始化
         */
        void _InitSpinBox();

        /**
         * @brief 创建UpDown控件
         */
        void _InitUpDownControl();

        /**
         * @brief 初始化加速信息
         */
        void _InitAccels();

        /**
         * @brief  获取当前值
         * @return 当前值
         */
        int _GetPos32();

        /**
         * @brief     设置当前值
         * @param pos 新的值
         */
        void _SetPos32(int pos);

        /**
         * @brief     获取最小值和最大值
         * @param min 用于接收最小值的指针
         * @param max 用于接收最大值的指针
         * @note      若不需要获取某个值则传入nullptr
         */
        void _GetRange32(int *min, int *max);

        /**
         * @brief     设置最小值和最大值
         * @param min 新的最小值
         * @param max 新的最大值
         */
        void _SetRange32(int min, int max);

        /**
         * @brief         设置加速信息
         * @param count   加速信息的数量
         * @param pAccels 指向UDACCEL数组的指针
         */
        void _SetAccel(size_t count, UDACCEL *pAccels);

        /**
         * @brief 更新UpDown控件的位置
         */
        void _UpdateUpDownPos();
    };
};
