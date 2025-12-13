#pragma once

#include "SimpleWindow.h"

/**
 * @brief 数据模型类，继承自sw::ObservableObject可以实现属性变更通知
 */
class MyViewModel : public sw::ObservableObject
{
public:
    // 自定义一个整数属性Value
    sw::Property<int> Value{
        sw::Property<int>::Init(this)
            .Getter<&MyViewModel::GetValue>()
            .Setter<&MyViewModel::SetValue>()};

    // Value属性的getter方法
    int GetValue() const
    {
        return _value;
    }

    // Value属性的setter方法
    void SetValue(int value)
    {
        if (_value != value) {
            _value = value;
            RaisePropertyChanged(&MyViewModel::Value);
        }
    }

private:
    // 存储Value属性的私有成员变量
    int _value = 0;
};
