#include "Test.h"

#include "Property.h"

#include <cstdint>
#include <string>
#include <type_traits>
#include <vector>

namespace
{
    int g_staticValue = 0;

    int GetStaticValue()
    {
        return g_staticValue;
    }

    void SetStaticValue(int value)
    {
        g_staticValue = value;
    }

    struct NestedValue {
        int value = 0;
    };

    struct PropertyOwner {
        int number = 0;
        NestedValue nested{7};
        int values[3]{1, 2, 3};

        sw::Property<int> Number{
            sw::Property<int>::Init(this).Getter<&PropertyOwner::number>().Setter<&PropertyOwner::number>()};

        sw::ReadOnlyProperty<int> ReadOnlyNumber{
            sw::ReadOnlyProperty<int>::Init(this).Getter<&PropertyOwner::number>()};

        sw::WriteOnlyProperty<int> WriteOnlyNumber{
            sw::WriteOnlyProperty<int>::Init(this).Setter<&PropertyOwner::number>()};

        sw::Property<NestedValue> Nested{
            sw::Property<NestedValue>::Init(this).Getter<&PropertyOwner::GetNested>().Setter<&PropertyOwner::SetNested>()};

        sw::Property<int *> Values{
            sw::Property<int *>::Init(this).Getter<&PropertyOwner::GetValues>().Setter<&PropertyOwner::SetValues>()};

        int GetNumber() const
        {
            return number;
        }

        void SetNumber(int value)
        {
            number = value;
        }

        NestedValue GetNested() const
        {
            return nested;
        }

        void SetNested(const NestedValue &value)
        {
            nested = value;
        }

        int *GetValues()
        {
            return values;
        }

        void SetValues(int *newValues)
        {
            for (int i = 0; i < 3; ++i) {
                values[i] = newValues[i];
            }
        }
    };

    struct MethodOwner {
        int backing = 0;

        sw::Property<int> Value{
            sw::Property<int>::Init(this).Getter<&MethodOwner::GetValue>().Setter<&MethodOwner::SetValue>()};

        int GetValue() const
        {
            return backing;
        }

        void SetValue(int value)
        {
            backing = value;
        }
    };
}

TEST_CASE("Property supports member field getter and setter")
{
    PropertyOwner owner;

    CHECK_EQ(0, owner.Number.Get());
    owner.Number = 42;

    CHECK_EQ(42, owner.number);
    CHECK_EQ(42, static_cast<int>(owner.Number));
}

TEST_CASE("Property supports member function getter and setter")
{
    MethodOwner owner;

    owner.Value = 9;
    CHECK_EQ(9, owner.backing);
    CHECK_EQ(9, owner.Value.Get());
}

TEST_CASE("Property supports static getter and setter")
{
    g_staticValue = 0;
    sw::Property<int> value{sw::Property<int>::Init().Getter(GetStaticValue).Setter(SetStaticValue)};

    value = 31;

    CHECK_EQ(31, g_staticValue);
    CHECK_EQ(31, value.Get());
}

TEST_CASE("Property read and write traits classify property kinds")
{
    static_assert(sw::_IsProperty<sw::Property<int>>::value, "Property must be recognized");
    static_assert(sw::_IsReadableProperty<sw::Property<int>>::value, "Property must be readable");
    static_assert(sw::_IsWritableProperty<sw::Property<int>>::value, "Property must be writable");
    static_assert(sw::_IsReadableProperty<sw::ReadOnlyProperty<int>>::value, "ReadOnlyProperty must be readable");
    static_assert(!sw::_IsWritableProperty<sw::ReadOnlyProperty<int>>::value, "ReadOnlyProperty must not be writable");
    static_assert(!sw::_IsReadableProperty<sw::WriteOnlyProperty<int>>::value, "WriteOnlyProperty must not be readable");
    static_assert(sw::_IsWritableProperty<sw::WriteOnlyProperty<int>>::value, "WriteOnlyProperty must be writable");

    CHECK(true);
}

TEST_CASE("Property supports arithmetic comparison and compound assignment")
{
    PropertyOwner owner;
    owner.Number = 10;

    CHECK_EQ(15, owner.Number + 5);
    CHECK_EQ(15, 5 + owner.Number);
    CHECK(owner.Number == 10);
    CHECK(owner.Number != 11);
    CHECK(owner.Number < 11);

    owner.Number += 5;
    CHECK_EQ(15, owner.Number.Get());
    owner.Number -= 3;
    CHECK_EQ(12, owner.Number.Get());
    owner.Number *= 2;
    CHECK_EQ(24, owner.Number.Get());
    owner.Number /= 6;
    CHECK_EQ(4, owner.Number.Get());
}

TEST_CASE("Property supports arrow and pointer index access")
{
    PropertyOwner owner;

    CHECK_EQ(7, owner.Nested->value);
    owner.Nested = NestedValue{12};
    CHECK_EQ(12, owner.Nested->value);

    CHECK_EQ(1, owner.Values[0]);
    CHECK_EQ(2, owner.Values[1]);

    int newValues[3]{8, 9, 10};
    owner.Values = newValues;
    CHECK_EQ(10, owner.Values[2]);
}

TEST_CASE("ReadOnlyProperty and WriteOnlyProperty expose intended operations")
{
    PropertyOwner owner;
    owner.number = 5;

    CHECK_EQ(5, owner.ReadOnlyNumber.Get());
    owner.WriteOnlyNumber = 17;
    CHECK_EQ(17, owner.number);
}
