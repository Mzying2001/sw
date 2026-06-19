#include "Test.h"

#include "Binding.h"
#include "Converters.h"
#include "ObservableObject.h"
#include "SelfBinding.h"

#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace
{
    enum class SmallEnum {
        Zero = 0,
        Two  = 2,
    };

    struct NoCast {
    };

    struct BindableObject : sw::ObservableObject {
        int value = 0;
        int other = 0;
        std::wstring text;
        sw::Variant variant;

        sw::Property<int> Value{
            sw::Property<int>::Init(this).Getter<&BindableObject::value>().Setter<&BindableObject::SetValue>()};

        sw::Property<int> Other{
            sw::Property<int>::Init(this).Getter<&BindableObject::other>().Setter<&BindableObject::SetOther>()};

        sw::Property<std::wstring> Text{
            sw::Property<std::wstring>::Init(this).Getter<&BindableObject::text>().Setter<&BindableObject::SetText>()};

        sw::Property<sw::Variant> VariantValue{
            sw::Property<sw::Variant>::Init(this).Getter<&BindableObject::variant>().Setter<&BindableObject::SetVariant>()};

        void SetValue(int newValue)
        {
            if (value != newValue) {
                value = newValue;
                RaisePropertyChanged(&BindableObject::Value);
            }
        }

        void SetOther(int newValue)
        {
            if (other != newValue) {
                other = newValue;
                RaisePropertyChanged(&BindableObject::Other);
            }
        }

        void SetText(const std::wstring &newText)
        {
            if (text != newText) {
                text = newText;
                RaisePropertyChanged(&BindableObject::Text);
            }
        }

        void SetVariant(const sw::Variant &newValue)
        {
            variant = newValue;
            RaisePropertyChanged(&BindableObject::VariantValue);
        }
    };

    class CountingStringIntConverter : public sw::IValueConverter<std::wstring, int>
    {
    public:
        explicit CountingStringIntConverter(int *convertCalls, int *convertBackCalls, int *destructCalls)
            : _convertCalls(convertCalls), _convertBackCalls(convertBackCalls), _destructCalls(destructCalls)
        {
        }

        ~CountingStringIntConverter() override
        {
            ++*_destructCalls;
        }

        int Convert(const std::wstring &source) override
        {
            ++*_convertCalls;
            return static_cast<int>(source.size());
        }

        std::wstring ConvertBack(int target) override
        {
            ++*_convertBackCalls;
            return std::wstring(static_cast<size_t>(target), L'x');
        }

    private:
        int *_convertCalls;
        int *_convertBackCalls;
        int *_destructCalls;
    };
}

TEST_CASE("Converters handle numeric boolean enum and reverse conversions")
{
    sw::IntToDoubleConverter intToDouble;
    CHECK_EQ(5.0, intToDouble.Convert(5));
    CHECK_EQ(3, intToDouble.ConvertBack(3.75));

    sw::NumericNegationConverter<int> negation;
    CHECK_EQ(-9, negation.Convert(9));
    CHECK_EQ(9, negation.ConvertBack(-9));

    sw::BoolNegationConverter boolNegation;
    CHECK_FALSE(boolNegation.Convert(true));
    CHECK(boolNegation.ConvertBack(false));

    sw::IntToBoolConverter intToBool;
    CHECK_FALSE(intToBool.Convert(0));
    CHECK(intToBool.Convert(-2));
    CHECK_EQ(1, intToBool.ConvertBack(true));
    CHECK_EQ(0, intToBool.ConvertBack(false));

    sw::BoolToIntConverter boolToInt;
    CHECK_EQ(1, boolToInt.Convert(true));
    CHECK_FALSE(boolToInt.ConvertBack(0));

    sw::EnumToIntConverter<SmallEnum> enumToInt;
    CHECK_EQ(2, enumToInt.Convert(SmallEnum::Two));
    CHECK(enumToInt.ConvertBack(2) == SmallEnum::Two);

    sw::IntToEnumConverter<SmallEnum> intToEnum;
    CHECK(intToEnum.Convert(2) == SmallEnum::Two);
    CHECK_EQ(2, intToEnum.ConvertBack(SmallEnum::Two));

    sw::ReverseConverter<sw::IntToStringConverter> reversed(7);
    CHECK_EQ(42, reversed.Convert(L"42"));
    CHECK_EQ(std::wstring(L"5"), reversed.ConvertBack(5));
    CHECK_EQ(7, reversed.Convert(L"bad"));
}

TEST_CASE("Converters parse strings and return defaults for invalid input")
{
    sw::IntToStringConverter intToString(99);
    CHECK_EQ(std::wstring(L"12"), intToString.Convert(12));
    CHECK_EQ(12, intToString.ConvertBack(L"12"));
    CHECK_EQ(99, intToString.ConvertBack(L"bad"));

    sw::StringToIntConverter stringToInt(77);
    CHECK_EQ(42, stringToInt.Convert(L"42"));
    CHECK_EQ(77, stringToInt.Convert(L"bad"));
    CHECK_EQ(std::wstring(L"42"), stringToInt.ConvertBack(42));

    sw::FloatToStringConverter floatToString(1.5f);
    CHECK_EQ(std::wstring(L"2.5"), floatToString.Convert(2.5f));
    CHECK_EQ(2.5f, floatToString.ConvertBack(L"2.5"));
    CHECK_EQ(1.5f, floatToString.ConvertBack(L"bad"));

    sw::StringToDoubleConverter stringToDouble(9.25);
    CHECK_EQ(3.5, stringToDouble.Convert(L"3.5"));
    CHECK_EQ(9.25, stringToDouble.Convert(L"bad"));
    CHECK_EQ(std::wstring(L"3.5"), stringToDouble.ConvertBack(3.5));

    sw::AnsiStringToIntConverter ansiToInt(8);
    CHECK_EQ(11, ansiToInt.Convert("11"));
    CHECK_EQ(8, ansiToInt.Convert("bad"));
    CHECK_EQ(std::string("11"), ansiToInt.ConvertBack(11));

    sw::IntToAnsiStringConverter intToAnsi(6);
    CHECK_EQ(std::string("15"), intToAnsi.Convert(15));
    CHECK_EQ(15, intToAnsi.ConvertBack("15"));
    CHECK_EQ(6, intToAnsi.ConvertBack("bad"));
}

TEST_CASE("Converters translate narrow wide and UTF-8 strings")
{
    sw::Utils::UseUtf8Encoding(true);

    sw::StringToAnsiStringConverter wideToAnsi;
    CHECK_EQ(std::string("hello"), wideToAnsi.Convert(L"hello"));
    CHECK_EQ(std::wstring(L"hello"), wideToAnsi.ConvertBack("hello"));

    sw::AnsiStringToStringConverter ansiToWide;
    CHECK_EQ(std::wstring(L"hello"), ansiToWide.Convert("hello"));
    CHECK_EQ(std::string("hello"), ansiToWide.ConvertBack(L"hello"));

    sw::StringToUtf8StringConverter wideToUtf8;
    sw::Utf8StringToStringConverter utf8ToWide;
    CHECK_EQ(std::string("hello"), wideToUtf8.Convert(L"hello"));
    CHECK_EQ(std::wstring(L"hello"), wideToUtf8.ConvertBack("hello"));
    CHECK_EQ(std::wstring(L"hello"), utf8ToWide.Convert("hello"));
    CHECK_EQ(std::string("hello"), utf8ToWide.ConvertBack(L"hello"));

    sw::Utils::UseUtf8Encoding(true);
}

TEST_CASE("BindingCastHelper supports default casts and Variant bridges")
{
    static_assert(sw::BindingCastHelper<int, int>::value, "Same type should be supported");
    static_assert(sw::BindingCastHelper<int, double>::value, "Bidirectional static casts should be supported");
    static_assert(sw::BindingCastHelper<int, sw::Variant>::value, "Value to Variant should be supported");
    static_assert(sw::BindingCastHelper<sw::Variant, int>::value, "Variant to value should be supported");
    static_assert(!sw::BindingCastHelper<NoCast, int>::value, "Uncastable values should not be supported");

    const int sameType = sw::BindingCastHelper<int, int>::Convert(4);
    const double castToDouble = sw::BindingCastHelper<int, double>::Convert(4);
    const int castBackToInt = sw::BindingCastHelper<int, double>::ConvertBack(4.5);

    CHECK_EQ(4, sameType);
    CHECK_EQ(4.0, castToDouble);
    CHECK_EQ(4, castBackToInt);

    sw::Variant boxed = sw::BindingCastHelper<int, sw::Variant>::Convert(9);
    CHECK(boxed.IsType<int>());
    CHECK_EQ(9, boxed.DynamicCast<int>());

    const int unboxed = sw::BindingCastHelper<sw::Variant, int>::Convert(boxed);
    CHECK_EQ(9, unboxed);
    REQUIRE_THROWS_AS((sw::BindingCastHelper<sw::Variant, int>::Convert(sw::Variant{})), std::bad_cast);
}

TEST_CASE("Binding performs initial updates for all modes")
{
    BindableObject target;
    BindableObject source;
    target.value = 1;
    source.value = 9;

    {
        std::unique_ptr<sw::Binding> binding(
            sw::Binding::Create(&target, &BindableObject::Value, &source, &BindableObject::Value, sw::BindingMode::OneTime));
        CHECK_EQ(9, target.value);
        source.Value = 10;
        CHECK_EQ(9, target.value);
    }

    target.value = 1;
    source.value = 9;
    {
        std::unique_ptr<sw::Binding> binding(
            sw::Binding::Create(&target, &BindableObject::Value, &source, &BindableObject::Value, sw::BindingMode::OneWay));
        CHECK_EQ(9, target.value);
    }

    target.value = 3;
    source.value = 9;
    {
        std::unique_ptr<sw::Binding> binding(
            sw::Binding::Create(&target, &BindableObject::Value, &source, &BindableObject::Value, sw::BindingMode::OneWayToSource));
        CHECK_EQ(3, source.value);
    }

    target.value = 1;
    source.value = 8;
    {
        std::unique_ptr<sw::Binding> binding(
            sw::Binding::Create(&target, &BindableObject::Value, &source, &BindableObject::Value, sw::BindingMode::TwoWay));
        CHECK_EQ(8, target.value);
    }
}

TEST_CASE("Binding responds to matching property notifications according to mode")
{
    BindableObject target;
    BindableObject source;
    target.value = 1;
    source.value = 2;

    std::unique_ptr<sw::Binding> binding(
        sw::Binding::Create(&target, &BindableObject::Value, &source, &BindableObject::Value, sw::BindingMode::OneWay));

    CHECK_EQ(2, target.value);

    source.Value = 5;
    CHECK_EQ(5, target.value);

    target.Value = 7;
    CHECK_EQ(5, source.value);

    source.Other = 99;
    CHECK_EQ(7, target.value);

    binding->SetBindingMode(sw::BindingMode::OneWayToSource);
    target.Value = 11;
    CHECK_EQ(11, source.value);

    source.Value = 13;
    CHECK_EQ(11, target.value);

    binding->SetBindingMode(sw::BindingMode::TwoWay);
    source.Value = 17;
    CHECK_EQ(17, target.value);
    target.Value = 19;
    CHECK_EQ(19, source.value);
}

TEST_CASE("Binding handles null and object death without stale notifications")
{
    BindableObject target;
    target.value = 1;

    std::unique_ptr<sw::Binding> binding;
    {
        auto source = std::make_unique<BindableObject>();
        source->value = 5;
        binding.reset(sw::Binding::Create(
            &target, &BindableObject::Value, source.get(), &BindableObject::Value, sw::BindingMode::OneWay));

        CHECK_EQ(5, target.value);
        CHECK(binding->GetSourceObject() == source.get());
        source.reset();
    }

    CHECK(binding->GetSourceObject() == nullptr);
    CHECK_FALSE(binding->UpdateTarget());

    target.value = 2;
    BindableObject newSource;
    newSource.value = 8;
    binding->SetSourceObject(&newSource);
    CHECK_EQ(8, target.value);

    binding->SetTargetObject(nullptr);
    newSource.Value = 10;
    CHECK_EQ(nullptr, binding->GetTargetObject());
    CHECK_EQ(8, target.value);
    CHECK_FALSE(binding->UpdateTarget());
}

TEST_CASE("Binding uses custom converters and owns converter lifetime")
{
    int convertCalls = 0;
    int convertBackCalls = 0;
    int destructCalls = 0;

    BindableObject target;
    BindableObject source;
    source.text = L"abcd";

    {
        std::unique_ptr<sw::Binding> binding(sw::Binding::Create(
            &target,
            &BindableObject::Value,
            &source,
            &BindableObject::Text,
            sw::BindingMode::TwoWay,
            new CountingStringIntConverter(&convertCalls, &convertBackCalls, &destructCalls)));

        CHECK_EQ(4, target.value);
        CHECK_GE(convertCalls, 1);

        const int initialConvertCalls = convertCalls;
        source.Text = L"xy";
        CHECK_EQ(2, target.value);
        CHECK_GT(convertCalls, initialConvertCalls);

        const int initialConvertBackCalls = convertBackCalls;
        target.Value = 3;
        CHECK_EQ(std::wstring(L"xxx"), source.text);
        CHECK_GT(convertBackCalls, initialConvertBackCalls);
    }

    CHECK_EQ(1, destructCalls);
}

TEST_CASE("Binding can bridge Variant properties by default")
{
    BindableObject target;
    BindableObject source;
    source.variant = sw::Variant(42);

    std::unique_ptr<sw::Binding> binding(
        sw::Binding::Create(&target, &BindableObject::Value, &source, &BindableObject::VariantValue, sw::BindingMode::TwoWay));

    CHECK_EQ(42, target.value);

    target.Value = 7;
    CHECK(source.variant.IsType<int>());
    CHECK_EQ(7, source.variant.DynamicCast<int>());
}

TEST_CASE("SelfBinding synchronizes properties on one object and can retarget")
{
    BindableObject first;
    first.value = 3;
    first.other = 1;

    std::unique_ptr<sw::SelfBinding> binding(
        sw::SelfBinding::Create(&BindableObject::Other, &BindableObject::Value, sw::BindingMode::TwoWay));

    CHECK(binding->GetTargetObject() == nullptr);
    CHECK_FALSE(binding->UpdateTarget());

    binding->SetTargetObject(&first);
    CHECK_EQ(3, first.other);

    first.Value = 8;
    CHECK_EQ(8, first.other);

    first.Other = 10;
    CHECK_EQ(10, first.value);

    BindableObject second;
    second.value = 12;
    second.other = 0;
    binding->SetTargetObject(&second);
    CHECK_EQ(12, second.other);

    first.Value = 20;
    CHECK_EQ(10, first.other);
}
