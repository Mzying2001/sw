#include "Test.h"

#include "Macros.h"
#include "ObservableObject.h"

#include <string>
#include <vector>

namespace
{
    struct FieldMacroOwner {
        int raw = 1;
        int readonly = 2;
        int writeonly = 3;

        SW_DEFINE_PROPERTY(Value, raw);
        SW_DEFINE_READONLY_PROPERTY(ReadOnly, readonly);
        SW_DEFINE_WRITEONLY_PROPERTY(WriteOnly, writeonly);
    };

    struct CustomMacroOwner {
        int raw = 1;
        int getCalls = 0;
        int setCalls = 0;

        int Get_raw()
        {
            ++getCalls;
            return raw + 10;
        }

        void Set_raw(int value)
        {
            ++setCalls;
            raw = value - 10;
        }

        SW_DEFINE_PROPERTY(Value, raw);
    };

    struct NotifyMacroOwner : sw::ObservableObject {
        int raw = 1;
        std::vector<sw::FieldId> ids;

        SW_DEFINE_NOTIFY_PROPERTY(Value, raw);

        NotifyMacroOwner()
        {
            PropertyChanged += [&](sw::INotifyPropertyChanged &, sw::PropertyChangedEventArgs &args) {
                ids.push_back(args.propertyId);
            };
        }
    };

    struct CustomNotifyMacroOwner : sw::ObservableObject {
        int raw = 1;
        int setCalls = 0;
        std::vector<sw::FieldId> ids;

        void Set_raw(int value)
        {
            ++setCalls;
            raw = value;
        }

        SW_DEFINE_NOTIFY_PROPERTY(Value, raw);

        CustomNotifyMacroOwner()
        {
            PropertyChanged += [&](sw::INotifyPropertyChanged &, sw::PropertyChangedEventArgs &args) {
                ids.push_back(args.propertyId);
            };
        }
    };

    struct NonComparableValue {
        int value = 0;
    };

    struct NonComparableNotifyMacroOwner : sw::ObservableObject {
        NonComparableValue raw{1};
        std::vector<sw::FieldId> ids;

        SW_DEFINE_NOTIFY_PROPERTY(Value, raw);

        NonComparableNotifyMacroOwner()
        {
            PropertyChanged += [&](sw::INotifyPropertyChanged &, sw::PropertyChangedEventArgs &args) {
                ids.push_back(args.propertyId);
            };
        }
    };

    struct ExprMacroOwner : sw::ObservableObject {
        int first = 1;
        int second = 2;
        int backing = 3;
        std::vector<sw::FieldId> ids;

        sw::Property<int> Backing{
            sw::Property<int>::Init(this).Getter<&ExprMacroOwner::backing>().Setter<&ExprMacroOwner::backing>()};

        SW_DEFINE_EXPR_PROPERTY(FirstValue, first);
        SW_DEFINE_EXPR_READONLY_PROPERTY(ReadOnlyBacking, Backing);
        SW_DEFINE_EXPR_WRITEONLY_PROPERTY(SecondWriter, second);
        SW_DEFINE_EXPR_NOTIFY_PROPERTY(BackingAlias, Backing);
        SW_DEFINE_EXPR_NOTIFY_PROPERTY(SecondAlias, second);

        ExprMacroOwner()
        {
            PropertyChanged += [&](sw::INotifyPropertyChanged &, sw::PropertyChangedEventArgs &args) {
                ids.push_back(args.propertyId);
            };
        }
    };

    struct ForwardedPropertyOwner : sw::ObservableObject {
        int raw = 1;
        int setCalls = 0;
        std::vector<sw::FieldId> ids;

        sw::Property<int> Backing{
            sw::Property<int>::Init(this).Getter<&ForwardedPropertyOwner::raw>().Setter<&ForwardedPropertyOwner::SetRaw>()};

        SW_DEFINE_EXPR_PROPERTY(Alias, Backing);
        SW_DEFINE_EXPR_NOTIFY_PROPERTY(NotifyAlias, Backing);

        void SetRaw(int value)
        {
            ++setCalls;
            raw = value + 10;
        }

        ForwardedPropertyOwner()
        {
            PropertyChanged += [&](sw::INotifyPropertyChanged &, sw::PropertyChangedEventArgs &args) {
                ids.push_back(args.propertyId);
            };
        }
    };
}

TEST_CASE("Field property macros expose read write and restricted access")
{
    FieldMacroOwner owner;

    CHECK_EQ(1, owner.Value.Get());
    owner.Value = 9;
    CHECK_EQ(9, owner.raw);
    CHECK_EQ(9, static_cast<int>(owner.Value));

    CHECK_EQ(2, owner.ReadOnly.Get());

    owner.WriteOnly = 11;
    CHECK_EQ(11, owner.writeonly);
}

TEST_CASE("Field property macros prefer custom accessors when present")
{
    CustomMacroOwner owner;

    CHECK_EQ(11, owner.Value.Get());
    CHECK_EQ(1, owner.getCalls);

    owner.Value = 25;
    CHECK_EQ(15, owner.raw);
    CHECK_EQ(1, owner.setCalls);

    CHECK_EQ(25, owner.Value.Get());
    CHECK_EQ(2, owner.getCalls);
}

TEST_CASE("Notify property macro raises only for changed comparable values")
{
    NotifyMacroOwner owner;

    owner.Value = 1;
    CHECK(owner.ids.empty());

    owner.Value = 4;

    REQUIRE_EQ(1, static_cast<int>(owner.ids.size()));
    CHECK(owner.ids[0] == sw::Reflection::GetFieldId(&NotifyMacroOwner::Value));
    CHECK_EQ(4, owner.raw);
}

TEST_CASE("Notify property macro leaves notification to custom setters")
{
    CustomNotifyMacroOwner owner;

    owner.Value = 5;

    CHECK_EQ(5, owner.raw);
    CHECK_EQ(1, owner.setCalls);
    CHECK(owner.ids.empty());
}

TEST_CASE("Notify property macro raises for every non comparable assignment")
{
    NonComparableNotifyMacroOwner owner;

    owner.Value = NonComparableValue{1};
    owner.Value = NonComparableValue{1};

    REQUIRE_EQ(2, static_cast<int>(owner.ids.size()));
    CHECK(owner.ids[0] == sw::Reflection::GetFieldId(&NonComparableNotifyMacroOwner::Value));
    CHECK(owner.ids[1] == sw::Reflection::GetFieldId(&NonComparableNotifyMacroOwner::Value));
    CHECK_EQ(1, owner.raw.value);
}

TEST_CASE("Expression property macros read write and forward through properties")
{
    ExprMacroOwner owner;

    CHECK_EQ(1, owner.FirstValue.Get());
    owner.FirstValue = 8;
    CHECK_EQ(8, owner.first);

    CHECK_EQ(3, owner.ReadOnlyBacking.Get());

    owner.SecondWriter = 12;
    CHECK_EQ(12, owner.second);

    owner.BackingAlias = 20;
    CHECK_EQ(20, owner.backing);

    owner.SecondAlias = 14;
    CHECK_EQ(14, owner.second);

    REQUIRE_EQ(2, static_cast<int>(owner.ids.size()));
    CHECK(owner.ids[0] == sw::Reflection::GetFieldId(&ExprMacroOwner::BackingAlias));
    CHECK(owner.ids[1] == sw::Reflection::GetFieldId(&ExprMacroOwner::SecondAlias));
}

TEST_CASE("Expression notify macro suppresses unchanged comparable values")
{
    ExprMacroOwner owner;

    owner.BackingAlias = 3;
    owner.SecondAlias = 2;

    CHECK(owner.ids.empty());
}

TEST_CASE("Expression macros forward property expressions through target setters")
{
    ForwardedPropertyOwner owner;

    owner.Alias = 5;
    CHECK_EQ(15, owner.raw);
    CHECK_EQ(1, owner.setCalls);
    CHECK_EQ(15, owner.Alias.Get());

    owner.NotifyAlias = 15;
    CHECK_EQ(1, owner.setCalls);
    CHECK(owner.ids.empty());

    owner.NotifyAlias = 20;

    CHECK_EQ(30, owner.raw);
    CHECK_EQ(2, owner.setCalls);
    REQUIRE_EQ(1, static_cast<int>(owner.ids.size()));
    CHECK(owner.ids[0] == sw::Reflection::GetFieldId(&ForwardedPropertyOwner::NotifyAlias));
}
