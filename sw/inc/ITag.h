#pragma once

#include <type_traits>

namespace sw
{
    /**
     * @brief Tag接口
     */
    template <typename T>
    class ITag
    {
    public:
        /**
         * @brief 默认虚析构函数
         */
        virtual ~ITag() = default;

    public:
        /**
         * @brief 获取Tag
         */
        virtual T GetTag() const = 0;

        /**
         * @brief 设置Tag
         */
        virtual void SetTag(typename std::conditional<std::is_scalar<T>::value, T, const T &>::type tag) = 0;
    };
}
