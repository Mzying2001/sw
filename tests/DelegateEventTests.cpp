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

    CHECK(func == nullptr);
    CHECK(nullptr == func);
    REQUIRE_THROWS_AS(func(1), std::runtime_error);
}

TEST_CASE("Delegate invokes free functions lambdas functors and member functions")
{
    sw::Delegate<int(int)> func;

    func += FreeAddOne;
    CHECK_EQ(4, func(3));

    func += Functor{4};
    CHECK_EQ(7, func(3));

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

    CHECK_EQ(23, func(3));
    CHECK_EQ(2, static_cast<int>(calls.size()));
    CHECK_EQ(13, calls[0]);
    CHECK_EQ(23, calls[1]);

    REQUIRE(func.Remove(second));
    calls.clear();
    CHECK_EQ(13, func(3));
    CHECK_EQ(1, static_cast<int>(calls.size()));
    CHECK_EQ(13, calls[0]);
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
    sw::Delegate<int(int)> source;
    source += FreeAddOne;
    source += FreeAddTwo;

    sw::Delegate<int(int)> target;
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
