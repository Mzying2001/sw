#include "Test.h"

#include "List.h"
#include "ObservableCollection.h"

#include <stdexcept>
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
    std::vector<sw::NotifyCollectionChangedAction> actions;
    std::vector<int> indexes;

    collection.CollectionChanged += [&](sw::INotifyCollectionChanged &, sw::NotifyCollectionChangedEventArgs &args) {
        actions.push_back(args.action);
        indexes.push_back(args.index);
        CHECK(args.list == &collection);
    };

    collection.Clear();
    CHECK(actions.empty());

    collection.Add(10);
    collection.Add(20);
    collection.SetAt(1, 30);
    REQUIRE(collection.Remove(10));
    collection.Refresh();
    collection.Clear();

    CHECK_EQ(6, static_cast<int>(actions.size()));
    CHECK(actions[0] == sw::NotifyCollectionChangedAction::Add);
    CHECK_EQ(0, indexes[0]);
    CHECK(actions[1] == sw::NotifyCollectionChangedAction::Add);
    CHECK_EQ(1, indexes[1]);
    CHECK(actions[2] == sw::NotifyCollectionChangedAction::Replace);
    CHECK_EQ(1, indexes[2]);
    CHECK(actions[3] == sw::NotifyCollectionChangedAction::Remove);
    CHECK_EQ(0, indexes[3]);
    CHECK(actions[4] == sw::NotifyCollectionChangedAction::Reset);
    CHECK(actions[5] == sw::NotifyCollectionChangedAction::Reset);
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
