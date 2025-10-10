#pragma once

#include <string>

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
            static_assert(&IToString::ToString != &TDerived::ToString,
                          "Derived class must implement ToString method.");
            return static_cast<const TDerived *>(this)->ToString();
        }
    };
}
