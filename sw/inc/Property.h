#pragma once

#include <functional>
#include <iostream>

namespace sw
{
    template <class T>
    class ReadOnlyProperty
    {
    private:
        std::function<const T &()> _funcGet;

    public:
        ReadOnlyProperty(const std::function<const T &()> &funcGet);
        const T &Get() const;
        operator const T &() const;
        const T *operator->() const;
    };

    template <class T>
    class WriteOnlyProperty
    {
    private:
        std::function<void(const T &)> _funcSet;

    public:
        WriteOnlyProperty(const std::function<void(const T &)> &funcSet);
        void Set(const T &value) const;
        const WriteOnlyProperty &operator=(const T &value) const;
    };

    template <class T>
    class Property : public ReadOnlyProperty<T>,
                     public WriteOnlyProperty<T>
    {
    public:
        Property(const std::function<const T &()> &funcGet, const std::function<void(const T &)> &funcSet);
        const Property &operator=(const T &value) const;
        T *operator->() const;
    };

    template <class T>
    std::wostream &operator<<(std::wostream &wos, const ReadOnlyProperty<T> &prop);

    /*================================================================================*/

    template <class T>
    inline ReadOnlyProperty<T>::ReadOnlyProperty(const std::function<const T &()> &funcGet)
        : _funcGet(funcGet)
    {
    }

    template <class T>
    inline const T &ReadOnlyProperty<T>::Get() const
    {
        return this->_funcGet();
    }

    template <class T>
    inline ReadOnlyProperty<T>::operator const T &() const
    {
        return this->_funcGet();
    }

    template <class T>
    inline const T *ReadOnlyProperty<T>::operator->() const
    {
        return &this->_funcGet();
    }

    template <class T>
    inline WriteOnlyProperty<T>::WriteOnlyProperty(const std::function<void(const T &)> &funcSet)
        : _funcSet(funcSet)
    {
    }

    template <class T>
    inline void WriteOnlyProperty<T>::Set(const T &value) const
    {
        this->_funcSet(value);
    }

    template <class T>
    inline const WriteOnlyProperty<T> &WriteOnlyProperty<T>::operator=(const T &value) const
    {
        this->_funcSet(value);
        return *this;
    }

    template <class T>
    inline Property<T>::Property(const std::function<const T &()> &funcGet, const std::function<void(const T &)> &funcSet)
        : ReadOnlyProperty<T>(funcGet), WriteOnlyProperty<T>(funcSet)
    {
    }

    template <class T>
    inline const Property<T> &Property<T>::operator=(const T &value) const
    {
        this->Set(value);
        return *this;
    }

    template <class T>
    inline T *Property<T>::operator->() const
    {
        const T &value = this->Get();
        return const_cast<T *>(&value);
    }

    template <class T>
    inline std::wostream &operator<<(std::wostream &wos, const ReadOnlyProperty<T> &prop)
    {
        return wos << prop.Get();
    }
}
