#pragma once

#include <functional>
#include <iostream>

namespace sw
{
    /**
     * @brief 只读属性
     */
    template <typename T>
    class ReadOnlyProperty
    {
    private:
        /**
         * @brief 读取属性的函数
         */
        std::function<const T &()> _funcGet;

    public:
        /**
         * @brief 初始化ReadOnlyProperty
         */
        ReadOnlyProperty(const std::function<const T &()> &funcGet)
            : _funcGet(funcGet)
        {
        }

        /**
         * @brief 读属性
         */
        const T &Get() const
        {
            return this->_funcGet();
        }

        /**
         * @brief 读属性
         */
        operator const T &() const
        {
            return this->_funcGet();
        }

        /**
         * @brief 取属性成员
         */
        const T *operator->() const
        {
            return &this->_funcGet();
        }
    };

    /**
     * @brief 只写属性
     */
    template <typename T>
    class WriteOnlyProperty
    {
    private:
        /**
         * @brief 写属性的函数
         */
        std::function<void(const T &)> _funcSet;

    public:
        /**
         * @brief 初始化WriteOnlyProperty
         */
        WriteOnlyProperty(const std::function<void(const T &)> &funcSet)
            : _funcSet(funcSet)
        {
        }

        /**
         * @brief 写属性
         */
        void Set(const T &value) const
        {
            this->_funcSet(value);
        }

        /**
         * @brief 写属性
         */
        const WriteOnlyProperty &operator=(const T &value) const
        {
            this->_funcSet(value);
            return *this;
        }
    };

    /**
     * @brief 属性
     */
    template <typename T>
    class Property : public ReadOnlyProperty<T>, public WriteOnlyProperty<T>
    {
    public:
        /**
         * @brief 初始化Property
         */
        Property(const std::function<const T &()> &funcGet, const std::function<void(const T &)> &funcSet)
            : ReadOnlyProperty<T>(funcGet), WriteOnlyProperty<T>(funcSet)
        {
        }

        /**
         * @brief 写属性
         */
        const Property &operator=(const T &value) const
        {
            this->Set(value);
            return *this;
        }

        /**
         * @brief 取属性成员
         */
        T *operator->() const
        {
            const T &value = this->Get();
            return const_cast<T *>(&value);
        }
    };

    /*================================================================================*/

    template <typename T>
    inline std::wostream &operator<<(std::wostream &wos, const ReadOnlyProperty<T> &prop)
    {
        return wos << prop.Get();
    }
}
