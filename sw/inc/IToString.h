#pragma once

#include <string>
#include <type_traits>

namespace sw
{
    /**
     * @brief 为支持ToString方法的类提供统一接口
     */
    template <typename TDerived>
    class IToString
    {
    public:
        /**
         * @brief 获取描述当前对象的字符串
         */
        std::wstring ToString() const
        {
            static_assert(
                !std::is_same<
                    decltype(&IToString<TDerived>::ToString),
                    decltype(&TDerived::ToString)>::value,
                "Derived class must implement ToString method.");

            return static_cast<const TDerived *>(this)->ToString();
        }
    };
}
