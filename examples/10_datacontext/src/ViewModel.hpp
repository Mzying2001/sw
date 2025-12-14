#pragma once

#include "SimpleWindow.h"

/**
 * @brief 数据模型类
 */
class MyViewModel : public sw::ObservableObject
{
public:
    // 自定义一个整数属性Value
    sw::Property<int> Value{
        sw::Property<int>::Init(this)
            .Getter<&MyViewModel::GetValue>()
            .Setter<&MyViewModel::SetValue>()
    };

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
            RaisePropertyChanged(&MyViewModel::DisplayText);
        }
    }

    // 只读属性，标签显示文本
    sw::ReadOnlyProperty<std::wstring> DisplayText{
        sw::Property<std::wstring>::Init(this)
            .Getter([](MyViewModel *self) -> std::wstring {
                return L"Current Value: " + std::to_wstring(self->_value);
            })
    };

private:
    // 存储Value属性的私有成员变量
    int _value = 0;
};
