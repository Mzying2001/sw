#include "Test.h"

#include "List.h"
#include "ObservableCollection.h"

#include <stdexcept>
#include <string>
#include <typeindex>
#include <utility>
#include <vector>

namespace
{
    struct TestObservableObject : sw::ObservableObject {
        int number = 0;

        sw::Property<int> Number{
            sw::Property<int>::Init(this).Getter<&TestObservableObject::number>().Setter<&TestObservableObject::number>()};

        void NotifyNumber()
        {
            RaisePropertyChanged(&TestObservableObject::Number);
        }
    };

    struct CollectionChangeRecord {
        sw::NotifyCollectionChangedAction action;
        const sw::IList *list;
        int index;
        int oldIndex;
    };

    template <typename T>
    void CaptureCollectionChanges(
        sw::ObservableCollection<T> &collection,
        std::vector<CollectionChangeRecord> &records)
    {
        collection.CollectionChanged +=
            [&](sw::INotifyCollectionChanged &, sw::NotifyCollectionChangedEventArgs &args) {
                records.push_back(CollectionChangeRecord{args.action, args.list, args.index, args.oldIndex});
            };
    }
}

TEST_CASE("List supports common mutations and queries")
{
    sw::List<int> list{1, 2, 4};

    CHECK_EQ(3, list.Count());
    list.Insert(2, 3);
    CHECK_EQ(4, list.Count());
    CHECK_EQ(3, list[2]);
    CHECK(list.Contains(4));
    CHECK_EQ(3, list.IndexOf(4));
    CHECK_EQ(3, list.LastIndexOf(4));

    list.SetAt(0, 9);
    CHECK_EQ(9, list.GetAt(0));

    REQUIRE(list.Remove(2));
    CHECK_EQ(3, list.Count());
    list.RemoveAt(0);
    CHECK_EQ(3, list.GetAt(0));
}

TEST_CASE("List supports construction capacity copy and move")
{
    sw::List<int> empty;
    CHECK_EQ(0, empty.Count());

    sw::List<int> reserved(4);
    CHECK_EQ(0, reserved.Count());
    CHECK_GE(reserved.Capacity(), 4);

    reserved.Add(1);
    reserved.Add(2);

    const int initialCapacity = reserved.Capacity();
    reserved.Reserve(initialCapacity - 1);
    CHECK_EQ(initialCapacity, reserved.Capacity());
    reserved.Reserve(initialCapacity + 3);
    CHECK_GE(reserved.Capacity(), initialCapacity + 3);

    sw::List<int> copied = reserved;
    copied.SetAt(0, 9);
    CHECK_EQ(1, reserved.GetAt(0));
    CHECK_EQ(9, copied.GetAt(0));

    sw::List<int> moved = std::move(copied);
    CHECK_EQ(2, moved.Count());
    CHECK_EQ(9, moved.GetAt(0));

    sw::List<int> assigned;
    assigned = moved;
    assigned.SetAt(1, 8);
    CHECK_EQ(2, moved.GetAt(1));
    CHECK_EQ(8, assigned.GetAt(1));

    sw::List<int> moveAssigned;
    moveAssigned = std::move(assigned);
    CHECK_EQ(2, moveAssigned.Count());
    CHECK_EQ(8, moveAssigned.GetAt(1));
}

TEST_CASE("List handles duplicate values endpoints and misses")
{
    sw::List<int> list{2, 1, 2, 3, 2};

    CHECK_EQ(0, list.IndexOf(2));
    CHECK_EQ(4, list.LastIndexOf(2));
    CHECK_EQ(-1, list.IndexOf(4));
    CHECK_EQ(-1, list.LastIndexOf(4));
    CHECK_FALSE(list.Contains(4));

    CHECK_FALSE(list.Remove(4));
    CHECK_EQ(5, list.Count());

    list.Insert(0, 0);
    list.Insert(list.Count(), 4);
    REQUIRE(list.Remove(2));

    CHECK_EQ(6, list.Count());
    CHECK_EQ(0, list.GetAt(0));
    CHECK_EQ(1, list.GetAt(1));
    CHECK_EQ(2, list.GetAt(2));
    CHECK_EQ(3, list.GetAt(3));
    CHECK_EQ(2, list.GetAt(4));
    CHECK_EQ(4, list.GetAt(5));
}

TEST_CASE("List exposes const access internal vector and string form")
{
    sw::List<int> list{1, 2};

    list.GetInternalVector().push_back(3);

    const sw::List<int> &constList = list;
    CHECK(constList.GetElementType() == std::type_index(typeid(int)));
    CHECK_EQ(3, constList.Count());
    CHECK_EQ(3, constList[2]);
    CHECK_EQ(std::wstring(L"[1, 2, 3]"), constList.ToString());

    const std::vector<int> &items = constList.GetInternalVector();
    CHECK_EQ(3, static_cast<int>(items.size()));
    CHECK_EQ(2, items[1]);
}

TEST_CASE("List supports IList variant access")
{
    sw::List<int> list{1, 2};
    sw::IList &base = list;

    sw::Variant ref = base.GetVariantAt(0);
    ref.DynamicCast<int>() = 7;
    CHECK_EQ(7, list.GetAt(0));

    sw::Variant value = 12;
    base.SetVariantAt(1, value);
    CHECK_EQ(12, list.GetAt(1));

    sw::Variant moved = 30;
    base.MoveVariantAt(1, moved);
    CHECK_EQ(30, list.GetAt(1));

    const sw::IList &constBase = list;
    sw::Variant copy = constBase.GetVariantAt(0);
    copy.DynamicCast<int>() = 9;
    CHECK_EQ(7, list.GetAt(0));
}

TEST_CASE("List throws on invalid indexes")
{
    sw::List<int> list{1, 2};

    REQUIRE_THROWS_AS(list.GetAt(-1), std::out_of_range);
    REQUIRE_THROWS_AS(list.GetAt(2), std::out_of_range);
    REQUIRE_THROWS_AS(list.Insert(3, 4), std::out_of_range);
    REQUIRE_THROWS_AS(list.RemoveAt(2), std::out_of_range);
    REQUIRE_THROWS_AS(list.SetAt(2, 4), std::out_of_range);
}

TEST_CASE("ObservableObject raises property changed and object dead")
{
    sw::FieldId raisedId{};
    bool dead = false;

    {
        TestObservableObject obj;
        obj.PropertyChanged += [&](sw::INotifyPropertyChanged &, sw::PropertyChangedEventArgs &args) {
            raisedId = args.propertyId;
        };
        obj.ObjectDead += [&](sw::INotifyObjectDead &, sw::EventArgs &) {
            dead = true;
        };

        obj.NotifyNumber();
        CHECK(raisedId == sw::Reflection::GetFieldId(&TestObservableObject::Number));
    }

    CHECK(dead);
}

TEST_CASE("ObservableCollection reports add replace remove and reset")
{
    sw::ObservableCollection<int> collection;
    std::vector<CollectionChangeRecord> changes;

    CaptureCollectionChanges(collection, changes);

    collection.Clear();
    CHECK(changes.empty());

    collection.Add(10);
    collection.Add(20);
    collection.SetAt(1, 30);
    REQUIRE(collection.Remove(10));
    collection.Refresh();
    collection.Clear();

    REQUIRE_EQ(6, static_cast<int>(changes.size()));
    CHECK(changes[0].action == sw::NotifyCollectionChangedAction::Add);
    CHECK_EQ(0, changes[0].index);
    CHECK(changes[0].list == &collection);
    CHECK(changes[1].action == sw::NotifyCollectionChangedAction::Add);
    CHECK_EQ(1, changes[1].index);
    CHECK(changes[2].action == sw::NotifyCollectionChangedAction::Replace);
    CHECK_EQ(1, changes[2].index);
    CHECK(changes[3].action == sw::NotifyCollectionChangedAction::Remove);
    CHECK_EQ(0, changes[3].index);
    CHECK(changes[4].action == sw::NotifyCollectionChangedAction::Reset);
    CHECK_EQ(-1, changes[4].index);
    CHECK(changes[5].action == sw::NotifyCollectionChangedAction::Reset);
    CHECK_EQ(0, collection.Count());
}

TEST_CASE("ObservableCollection reports move and skips no-op move")
{
    sw::ObservableCollection<int> collection{1, 2, 3};
    int notifications = 0;
    int oldIndex = -1;
    int newIndex = -1;

    collection.CollectionChanged += [&](sw::INotifyCollectionChanged &, sw::NotifyCollectionChangedEventArgs &args) {
        ++notifications;
        CHECK(args.action == sw::NotifyCollectionChangedAction::Move);
        oldIndex = args.oldIndex;
        newIndex = args.index;
    };

    collection.Move(0, 2);

    CHECK_EQ(1, notifications);
    CHECK_EQ(0, oldIndex);
    CHECK_EQ(2, newIndex);
    CHECK_EQ(2, collection.GetAt(0));
    CHECK_EQ(3, collection.GetAt(1));
    CHECK_EQ(1, collection.GetAt(2));

    collection.Move(1, 1);
    CHECK_EQ(1, notifications);
}

TEST_CASE("ObservableCollection supports construction capacity queries and string form")
{
    sw::ObservableCollection<std::wstring> collection{L"alpha", L"beta", L"alpha"};

    CHECK(collection.GetElementType() == std::type_index(typeid(std::wstring)));
    CHECK_EQ(3, collection.Count());
    CHECK_EQ(0, collection.IndexOf(L"alpha"));
    CHECK_EQ(2, collection.LastIndexOf(L"alpha"));
    CHECK(collection.Contains(L"beta"));
    CHECK_FALSE(collection.Contains(L"gamma"));
    CHECK_EQ(std::wstring(L"[alpha, beta, alpha]"), collection.ToString());

    sw::ObservableCollection<int> reserved(3);
    CHECK_EQ(0, reserved.Count());
    CHECK_GE(reserved.Capacity(), 3);

    const int capacity = reserved.Capacity();
    reserved.Reserve(capacity + 2);
    CHECK_GE(reserved.Capacity(), capacity + 2);
}

TEST_CASE("ObservableCollection reports insert removeAt and move indices")
{
    sw::ObservableCollection<int> collection{1, 3, 4};
    std::vector<CollectionChangeRecord> changes;
    CaptureCollectionChanges(collection, changes);

    collection.Insert(1, 2);
    collection.RemoveAt(2);
    collection.Move(2, 0);

    REQUIRE_EQ(3, static_cast<int>(changes.size()));
    CHECK(changes[0].action == sw::NotifyCollectionChangedAction::Add);
    CHECK_EQ(1, changes[0].index);
    CHECK(changes[0].list == &collection);
    CHECK(changes[1].action == sw::NotifyCollectionChangedAction::Remove);
    CHECK_EQ(2, changes[1].index);
    CHECK(changes[2].action == sw::NotifyCollectionChangedAction::Move);
    CHECK_EQ(2, changes[2].oldIndex);
    CHECK_EQ(0, changes[2].index);

    CHECK_EQ(3, collection.Count());
    CHECK_EQ(4, collection.GetAt(0));
    CHECK_EQ(1, collection.GetAt(1));
    CHECK_EQ(2, collection.GetAt(2));
}

TEST_CASE("ObservableCollection supports IList variant updates with replace notifications")
{
    sw::ObservableCollection<int> collection{1, 2};
    std::vector<CollectionChangeRecord> changes;
    CaptureCollectionChanges(collection, changes);

    sw::IList &base = collection;
    sw::Variant value = 9;
    base.SetVariantAt(1, value);

    sw::Variant moved = 7;
    base.MoveVariantAt(0, moved);

    REQUIRE_EQ(2, static_cast<int>(changes.size()));
    CHECK(changes[0].action == sw::NotifyCollectionChangedAction::Replace);
    CHECK_EQ(1, changes[0].index);
    CHECK(changes[1].action == sw::NotifyCollectionChangedAction::Replace);
    CHECK_EQ(0, changes[1].index);
    CHECK_EQ(7, collection.GetAt(0));
    CHECK_EQ(9, collection.GetAt(1));
}

TEST_CASE("ObservableCollection does not notify failed or no-op mutations")
{
    sw::ObservableCollection<int> collection{1, 2, 3};
    int notifications = 0;

    collection.CollectionChanged += [&](sw::INotifyCollectionChanged &, sw::NotifyCollectionChangedEventArgs &) {
        ++notifications;
    };

    REQUIRE_THROWS_AS(collection.GetAt(-1), std::out_of_range);
    REQUIRE_THROWS_AS(collection.GetAt(3), std::out_of_range);
    REQUIRE_THROWS_AS(collection.Insert(-1, 0), std::out_of_range);
    REQUIRE_THROWS_AS(collection.Insert(4, 0), std::out_of_range);
    REQUIRE_THROWS_AS(collection.RemoveAt(3), std::out_of_range);
    REQUIRE_THROWS_AS(collection.SetAt(3, 0), std::out_of_range);
    REQUIRE_THROWS_AS(collection.Move(-1, 0), std::out_of_range);
    REQUIRE_THROWS_AS(collection.Move(0, 3), std::out_of_range);

    CHECK_FALSE(collection.Remove(4));
    collection.Move(1, 1);

    CHECK_EQ(0, notifications);
    CHECK_EQ(3, collection.Count());
    CHECK_EQ(1, collection.GetAt(0));
    CHECK_EQ(2, collection.GetAt(1));
    CHECK_EQ(3, collection.GetAt(2));
}
