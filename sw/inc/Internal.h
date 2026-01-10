#pragma once

#include <type_traits>

namespace sw
{
    /**
     * @brief 最佳参数类型，标量类型使用值传递，复杂类型使用常量引用传递
     */
    template <typename T>
    using _OptimalParamType = typename std::conditional<
        std::is_scalar<typename std::decay<T>::type>::value,
        typename std::decay<T>::type, const typename std::decay<T>::type &>::type;
}
