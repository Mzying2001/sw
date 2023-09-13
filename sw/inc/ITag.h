#pragma once

#include <stdint.h>

namespace sw
{
    /**
     * @brief Tag接口
     */
    class ITag
    {
    public:
        /**
         * @brief 获取Tag
         */
        virtual uint64_t GetTag() = 0;

        /**
         * @brief 设置Tag
         */
        virtual void SetTag(uint64_t tag) = 0;
    };
}
