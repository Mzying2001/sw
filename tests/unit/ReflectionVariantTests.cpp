#include "Test.h"

#include "Reflection.h"
#include "Variant.h"

#include <stdexcept>
#include <string>
#include <typeindex>

namespace
{
    struct PlainObject {
        int number = 0;
        std::wstring name;

        int Add(int value)
        {
            return number + value;
        }

        int AddConst(int value) const
        {
            return number + value;
        }
    };

    struct ReflectiveObject : sw::DynamicObject {
        int number = 0;

        sw::Property<int> Number{
            sw::Property<int>::Init(this).Getter<&ReflectiveObject::number>().Setter<&ReflectiveObject::number>()};

        int Add(int value)
        {
            return number + value;
        }
    };

    struct DerivedReflectiveObject : ReflectiveObject {
        int extra = 0;
    };

    struct MoveOnly {
        int value;

        explicit MoveOnly(int value)
            : value(value)
        {
        }

        MoveOnly(const MoveOnly &) = delete;
        MoveOnly(MoveOnly &&)      = default;
    };
}

TEST_CASE("DynamicObject reports type and supports casts")
{
    DerivedReflectiveObject obj;
    obj.number = 7;

    sw::DynamicObject &base = obj;
    ReflectiveObject *out  = nullptr;

    CHECK(base.IsType<ReflectiveObject>(&out));
    REQUIRE(out != nullptr);
    CHECK_EQ(7, out->number);
    CHECK(base.GetType() == std::type_index(typeid(DerivedReflectiveObject)));
    CHECK_EQ(7, base.DynamicCast<ReflectiveObject>().number);
    CHECK_EQ(7, base.UnsafeCast<ReflectiveObject>().number);
}

TEST_CASE("BoxedObject supports value and reference semantics")
{
    sw::BoxedObject<int> boxed(5);

    CHECK(boxed.IsBoxedObject());
    CHECK(boxed.IsType<int>());
    CHECK_EQ(5, boxed.Unbox());

    int value = 9;
    auto ref  = sw::BoxedObject<int>::MakeRef(value);
    ref.Unbox() = 10;

    CHECK_EQ(10, value);
    CHECK(ref.ReferenceEquals(ref));
}

TEST_CASE("BoxedObject null references behave as empty boxes")
{
    auto ref = sw::BoxedObject<int>::MakeRef(static_cast<int *>(nullptr));
    int fallback = 0;
    int *out = &fallback;

    CHECK(ref.IsBoxedObject());
    CHECK(ref.GetType() == std::type_index(typeid(int)));
    CHECK_FALSE(ref.IsType<int>(&out));
    CHECK(out == nullptr);
    REQUIRE_THROWS_AS(ref.DynamicCast<int>(), std::bad_cast);
}

TEST_CASE("Variant handles null value storage and reset")
{
    sw::Variant value;
    CHECK(value.IsNull());
    CHECK(!value.HasValue());
    CHECK(value.GetType() == std::type_index(typeid(void)));
    CHECK_FALSE(static_cast<bool>(value));

    value.Reset(12);
    CHECK(value.HasValue());
    CHECK(static_cast<bool>(value));
    CHECK(value.IsType<int>());
    CHECK_EQ(12, value.DynamicCast<int>());

    value.Reset(nullptr);
    CHECK(value.IsNull());
}

TEST_CASE("Variant treats null object references as empty")
{
    sw::Variant value(sw::ObjectRef{nullptr});
    int fallback = 0;
    int *out = &fallback;

    CHECK(value.IsNull());
    CHECK_FALSE(value.HasValue());
    CHECK_FALSE(value.IsType<int>(&out));
    CHECK(out == nullptr);

    value.Reset(12);
    value.Reset(sw::ObjectRef{nullptr});

    CHECK(value.IsNull());
    CHECK(value.GetType() == std::type_index(typeid(void)));
}

TEST_CASE("Variant supports copy move and reference equality")
{
    sw::Variant value = std::wstring(L"hello");
    sw::Variant copied = value;

    CHECK(copied.IsType<std::wstring>());
    CHECK_EQ(std::wstring(L"hello"), copied.DynamicCast<std::wstring>());
    CHECK(!value.ReferenceEquals(copied));

    sw::Variant moved = std::move(copied);
    CHECK(moved.IsType<std::wstring>());
    CHECK(copied.IsNull());

    ReflectiveObject obj;
    obj.number = 33;
    sw::Variant ref = sw::Variant::MakeRef(obj);
    CHECK(ref.IsType<ReflectiveObject>());
    CHECK(ref.Object() == &obj);
    CHECK(ref.ReferenceEquals(sw::Variant::MakeRef(obj)));
}

TEST_CASE("Variant keeps DynamicObject references transparent across copy and const access")
{
    ReflectiveObject obj;
    obj.number = 12;

    sw::Variant ref = sw::Variant::MakeRef(obj);
    sw::Variant copied = ref;

    CHECK(copied.Object() == &obj);
    CHECK(copied.ReferenceEquals(ref));

    copied.DynamicCast<ReflectiveObject>().number = 18;
    CHECK_EQ(18, obj.number);

    const sw::Variant constRef = copied;
    const ReflectiveObject *out = nullptr;

    CHECK(constRef.IsType<ReflectiveObject>(&out));
    REQUIRE(out != nullptr);
    CHECK(out == &obj);
    CHECK_EQ(18, constRef.DynamicCast<ReflectiveObject>().number);
    CHECK(constRef.Object() == &obj);
}

TEST_CASE("Variant supports reference semantics for non dynamic values")
{
    int value = 5;
    sw::Variant ref = sw::Variant::MakeRef(value);
    int *out = nullptr;

    CHECK(ref.IsType<int>(&out));
    REQUIRE(out != nullptr);
    CHECK(out == &value);
    CHECK(ref.Object() != nullptr);

    ref.DynamicCast<int>() = 6;
    CHECK_EQ(6, value);

    sw::Variant copied = ref;
    CHECK(copied.ReferenceEquals(ref));

    copied.DynamicCast<int>() = 7;
    CHECK_EQ(7, value);

    const sw::Variant constRef = copied;
    const int *constOut = nullptr;

    CHECK(constRef.IsType<int>(&constOut));
    REQUIRE(constOut != nullptr);
    CHECK(constOut == &value);
    CHECK_EQ(7, constRef.DynamicCast<int>());
}

TEST_CASE("Variant MakeRef forwards existing variant storage")
{
    sw::Variant empty;
    CHECK(sw::Variant::MakeRef(empty).IsNull());

    sw::Variant value = 41;
    sw::Variant valueRef = sw::Variant::MakeRef(value);

    CHECK(valueRef.ReferenceEquals(value));

    valueRef.DynamicCast<int>() = 42;
    CHECK_EQ(42, value.DynamicCast<int>());

    ReflectiveObject obj;
    obj.number = 8;

    sw::Variant externalRef = sw::Variant::MakeRef(obj);
    sw::Variant forwardedRef = sw::Variant::MakeRef(externalRef);

    CHECK(forwardedRef.Object() == &obj);
    CHECK(forwardedRef.ReferenceEquals(externalRef));

    forwardedRef.DynamicCast<ReflectiveObject>().number = 9;
    CHECK_EQ(9, obj.number);
}

TEST_CASE("Variant casts throw for null and mismatched types")
{
    sw::Variant empty;
    REQUIRE_THROWS_AS(empty.DynamicCast<int>(), std::bad_cast);

    sw::Variant value = 3;
    REQUIRE_THROWS_AS(value.DynamicCast<std::wstring>(), std::bad_cast);
}

TEST_CASE("Variant preserves move-only values and rejects copy")
{
    sw::Variant value = sw::Variant::MakeVal<MoveOnly>(7);
    CHECK(value.IsType<MoveOnly>());
    CHECK_EQ(7, value.DynamicCast<MoveOnly>().value);

    REQUIRE_THROWS_AS(sw::Variant(value), std::runtime_error);
}

TEST_CASE("Reflection field ids are stable and distinguish fields")
{
    auto numberA = sw::Reflection::GetFieldId(&PlainObject::number);
    auto numberB = sw::Reflection::GetFieldId(&PlainObject::number);
    auto name    = sw::Reflection::GetFieldId(&PlainObject::name);

    CHECK(numberA == numberB);
    CHECK(numberA != name);
    CHECK_EQ(std::to_wstring(numberA.value), numberA.ToString());
}

TEST_CASE("Reflection method and field accessors work for plain and dynamic objects")
{
    PlainObject plain;
    plain.number = 4;

    auto plainMethod = sw::Reflection::GetMethod(&PlainObject::Add);
    CHECK_EQ(10, sw::Reflection::InvokeMethod(plainMethod, plain, 6));

    auto plainConstMethod = sw::Reflection::GetMethod(&PlainObject::AddConst);
    CHECK_EQ(11, sw::Reflection::InvokeMethod(plainConstMethod, plain, 7));

    auto field = sw::Reflection::GetFieldAccessor(&PlainObject::number);
    sw::Reflection::AccessField(field, plain) = 15;
    CHECK_EQ(15, plain.number);

    ReflectiveObject dyn;
    dyn.number = 3;
    auto dynMethod = sw::Reflection::GetMethod(&ReflectiveObject::Add);
    CHECK_EQ(8, sw::Reflection::InvokeMethod(dynMethod, dyn, 5));
}

TEST_CASE("Reflection property getters and setters work")
{
    ReflectiveObject obj;
    obj.number = 21;

    auto getter = sw::Reflection::GetPropertyGetter(&ReflectiveObject::Number);
    auto setter = sw::Reflection::GetPropertySetter(&ReflectiveObject::Number);

    CHECK(getter != nullptr);
    CHECK(setter != nullptr);
    CHECK_EQ(21, sw::Reflection::GetProperty(getter, obj));

    sw::Reflection::SetProperty(setter, obj, 44);
    CHECK_EQ(44, obj.number);
}
