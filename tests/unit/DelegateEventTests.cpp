#include "Test.h"

#include "Delegate.h"
#include "Event.h"

#include <stdexcept>
#include <vector>

namespace
{
    int FreeAddOne(int value)
    {
        return value + 1;
    }

    int FreeAddTwo(int value)
    {
        return value + 2;
    }

    struct Functor {
        int delta;

        int operator()(int value) const
        {
            return value + delta;
        }

        bool operator==(const Functor &other) const
        {
            return delta == other.delta;
        }
    };

    struct PlainFunctor {
        int delta;

        int operator()(int value) const
        {
            return value + delta;
        }
    };

    struct Receiver {
        int factor = 1;
        std::vector<int> calls;

        int Multiply(int value)
        {
            calls.push_back(value);
            return value * factor;
        }

        int AddFactor(int value) const
        {
            return value + factor;
        }
    };

    struct RecordingFunctor {
        std::vector<int> *calls;
        int delta;

        int operator()(int value) const
        {
            calls->push_back(value + delta);
            return value + delta;
        }

        bool operator==(const RecordingFunctor &other) const
        {
            return calls == other.calls && delta == other.delta;
        }
    };

    struct VoidRecorder {
        int *value;

        void operator()(int input) const
        {
            *value += input;
        }

        bool operator==(const VoidRecorder &other) const
        {
            return value == other.value;
        }
    };

    using IntHandler = sw::Delegate<void(int)>;

    struct EventOwner {
        IntHandler handler;

        const sw::Event<IntHandler> Changed{
            sw::Event<IntHandler>::Init(this).Delegate<&EventOwner::handler>()};
    };

    IntHandler &StaticEventHandler()
    {
        static IntHandler handler;
        return handler;
    }
}

TEST_CASE("Delegate empty invocation throws")
{
    sw::Delegate<int(int)> func;
    sw::Delegate<int(int)> empty;
    int (*nullFunc)(int) = nullptr;

    CHECK(func == nullptr);
    CHECK(nullptr == func);
    CHECK_FALSE(static_cast<bool>(func));
    CHECK_FALSE(func != nullptr);
    CHECK_FALSE(nullptr != func);

    func += empty;
    func += nullFunc;
    CHECK(func == nullptr);
    CHECK_FALSE(func.Remove(empty));
    CHECK_FALSE(func.Remove(nullFunc));

    REQUIRE_THROWS_AS(func(1), std::runtime_error);
    REQUIRE_THROWS_AS(func.InvokeAll(1), std::runtime_error);
}

TEST_CASE("Delegate invokes free functions lambdas functors and member functions")
{
    sw::Delegate<int(int)> func;

    func += FreeAddOne;
    CHECK_EQ(4, func(3));

    auto lambda = [](int value) {
        return value * 2;
    };
    func += lambda;
    CHECK_EQ(6, func(3));

    func += Functor{4};
    CHECK_EQ(7, func(3));

    func += PlainFunctor{5};
    CHECK_EQ(8, func(3));

    Receiver receiver;
    receiver.factor = 5;
    func.Add(receiver, &Receiver::Multiply);
    CHECK_EQ(15, func(3));

    const Receiver constReceiver{6, {}};
    sw::Delegate<int(int)> constMember(constReceiver, &Receiver::AddFactor);
    CHECK_EQ(9, constMember(3));
}

TEST_CASE("Delegate removes handlers from the back and preserves multicast order")
{
    std::vector<int> calls;
    RecordingFunctor first{&calls, 10};
    RecordingFunctor second{&calls, 20};

    sw::Delegate<int(int)> func;
    func += first;
    func += second;
    func += first;

    CHECK_EQ(13, func(3));
    CHECK_EQ(3, static_cast<int>(calls.size()));
    CHECK_EQ(13, calls[0]);
    CHECK_EQ(23, calls[1]);
    CHECK_EQ(13, calls[2]);

    REQUIRE(func.Remove(first));
    calls.clear();
    CHECK_EQ(23, func(3));
    CHECK_EQ(2, static_cast<int>(calls.size()));
    CHECK_EQ(13, calls[0]);
    CHECK_EQ(23, calls[1]);

    CHECK_FALSE(func.Remove(RecordingFunctor{&calls, 30}));
    calls.clear();
    CHECK_EQ(23, func(3));
    CHECK_EQ(2, static_cast<int>(calls.size()));
    CHECK_EQ(13, calls[0]);
    CHECK_EQ(23, calls[1]);
}

TEST_CASE("Delegate removes function objects and member handlers")
{
    sw::Delegate<int(int)> func;

    func += PlainFunctor{5};
    CHECK_EQ(8, func(3));
    REQUIRE(func.Remove(PlainFunctor{5}));
    CHECK(func == nullptr);

    Receiver receiver;
    receiver.factor = 4;
    const Receiver constReceiver{6, {}};

    func.Add(receiver, &Receiver::Multiply);
    func.Add(constReceiver, &Receiver::AddFactor);
    CHECK_EQ(9, func(3));
    CHECK_EQ(1, static_cast<int>(receiver.calls.size()));

    REQUIRE(func.Remove(constReceiver, &Receiver::AddFactor));
    CHECK_EQ(12, func(3));
    CHECK_EQ(2, static_cast<int>(receiver.calls.size()));

    REQUIRE(func.Remove(receiver, &Receiver::Multiply));
    CHECK(func == nullptr);
    CHECK_FALSE(func.Remove(receiver, &Receiver::Multiply));
}

TEST_CASE("Delegate invokes all void handlers in order")
{
    std::vector<int> calls;
    sw::Delegate<void(int)> action;

    action += [&](int value) {
        calls.push_back(value + 1);
    };
    action += [&](int value) {
        calls.push_back(value + 2);
    };

    action(10);
    CHECK_EQ(2, static_cast<int>(calls.size()));
    CHECK_EQ(11, calls[0]);
    CHECK_EQ(12, calls[1]);
}

TEST_CASE("Delegate InvokeAll returns every non-void result")
{
    sw::Delegate<int(int)> func;
    func += FreeAddOne;
    func += FreeAddTwo;

    auto results = func.InvokeAll(10);

    CHECK_EQ(2, static_cast<int>(results.size()));
    CHECK_EQ(11, results[0]);
    CHECK_EQ(12, results[1]);
}

TEST_CASE("Delegate assignment replaces handlers and keeps copies independent")
{
    sw::Delegate<int(int)> source;
    source += FreeAddTwo;

    sw::Delegate<int(int)> target;
    target += FreeAddOne;

    target = source;
    CHECK(target == source);
    CHECK_EQ(7, target(5));

    source.Clear();
    CHECK(source == nullptr);
    CHECK_EQ(7, target(5));

    sw::Delegate<int(int)> movedTarget;
    movedTarget += Functor{1};
    movedTarget = std::move(target);

    CHECK_EQ(7, movedTarget(5));
    CHECK(target == nullptr);
}

TEST_CASE("Delegate copy move equality and clear work")
{
    sw::Delegate<int(int)> original;
    original += FreeAddOne;

    sw::Delegate<int(int)> copied = original;
    CHECK(copied == original);
    CHECK_EQ(6, copied(5));

    sw::Delegate<int(int)> moved = std::move(copied);
    CHECK(moved != nullptr);
    CHECK(copied == nullptr);
    CHECK_EQ(6, moved(5));

    moved.Clear();
    CHECK(moved == nullptr);
}

TEST_CASE("Delegate supports adding and removing nested delegates")
{
    sw::Delegate<int(int)> single;
    single += FreeAddOne;

    sw::Delegate<int(int)> target;
    target += single;
    CHECK(target == single);
    CHECK_EQ(6, target(5));

    target -= single;
    CHECK(target == nullptr);

    sw::Delegate<int(int)> source;
    source += FreeAddOne;
    source += FreeAddTwo;

    target += source;
    CHECK(target != source);
    CHECK_EQ(7, target(5));

    target -= source;
    CHECK(target == nullptr);
}

TEST_CASE("Event forwards member handlers to backing delegate")
{
    EventOwner owner;
    int seen = 0;
    VoidRecorder handler{&seen};

    owner.Changed += handler;
    owner.handler(4);
    CHECK_EQ(4, seen);

    owner.Changed -= handler;
    CHECK(owner.handler == nullptr);
}

TEST_CASE("Event forwards static handlers to backing delegate")
{
    StaticEventHandler().Clear();
    sw::Event<IntHandler> changed{
        sw::Event<IntHandler>::Init().Delegate(StaticEventHandler)};

    int seen = 0;
    VoidRecorder handler{&seen};

    changed += handler;
    StaticEventHandler()(12);
    CHECK_EQ(12, seen);

    changed -= handler;
    CHECK(StaticEventHandler() == nullptr);
}
