#pragma once

namespace sw
{
    /**
     * @brief 相等性比较接口
     */
    template <
        typename TDerived,
        typename TOther = const TDerived &>
    class IEqualityComparable
    {
    public:
        /**
         * @brief 判断当前对象与另一个对象是否相等
         */
        bool Equals(TOther other) const
        {
            static_assert(&IEqualityComparable::Equals != &TDerived::Equals,
                          "Derived class must implement Equals method.");
            return static_cast<const TDerived *>(this)->Equals(other);
        }

        /**
         * @brief 判断当前对象是否与另一个对象相等
         */
        bool operator==(TOther other) const
        {
            return Equals(other);
        }

        /**
         * @brief 判断当前对象是否与另一个对象不相等
         */
        bool operator!=(TOther other) const
        {
            return !Equals(other);
        }
    };

    /**
     * @brief 全序比较接口
     */
    template <
        typename TDerived,
        typename TOther = const TDerived &>
    class IComparable : public IEqualityComparable<TDerived, TOther>
    {
    public:
        /**
         * @brief  比较当前对象与另一个对象的大小关系
         * @return 若当前对象小于另一个对象，返回负数；若等于，返回0；若大于，返回正数
         */
        int CompareTo(TOther other) const
        {
            static_assert(&IComparable::CompareTo != &TDerived::CompareTo,
                          "Derived class must implement CompareTo method.");
            return static_cast<const TDerived *>(this)->CompareTo(other);
        }

        /**
         * @brief 判断当前对象与另一个对象是否相等
         */
        bool Equals(TOther other) const
        {
            return CompareTo(other) == 0;
        }

        /**
         * @brief 判断当前对象是否小于另一个对象
         */
        bool operator<(TOther other) const
        {
            return CompareTo(other) < 0;
        }

        /**
         * @brief 判断当前对象是否小于或等于另一个对象
         */
        bool operator<=(TOther other) const
        {
            return CompareTo(other) <= 0;
        }

        /**
         * @brief 判断当前对象是否大于另一个对象
         */
        bool operator>(TOther other) const
        {
            return CompareTo(other) > 0;
        }

        /**
         * @brief 判断当前对象是否大于或等于另一个对象
         */
        bool operator>=(TOther other) const
        {
            return CompareTo(other) >= 0;
        }
    };
}
