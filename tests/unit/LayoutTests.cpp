#include "Test.h"

#include "LayoutTestHelpers.h"

#include "CanvasLayout.h"
#include "DockLayout.h"
#include "FillLayout.h"
#include "GridLayout.h"
#include "StackLayout.h"
#include "UniformGridLayout.h"
#include "WrapLayout.h"

#include <limits>
#include <vector>

namespace
{
    constexpr double kInf = std::numeric_limits<double>::infinity();

    template <typename TLayout>
    struct LayoutFixture {
        std::vector<std::string> calls;
        swtest::layouttest::ContainerLayout container;
        TLayout host;

        LayoutFixture()
            : calls(),
              container(&calls),
              host()
        {
            this->host.Associate(&this->container);
        }
    };
}

TEST_CASE("RecordingLayout records measure arrange calls and returns configured state")
{
    std::vector<std::string> calls;
    swtest::layouttest::RecordingLayout item(
        "item",
        sw::Size(24, 12),
        42,
        &calls);

    CHECK_EQ(42u, item.GetLayoutTag());
    CHECK_EQ(sw::Size(24, 12), item.GetDesireSize());
    CHECK_EQ(0, item.GetChildLayoutCount());

    item.Measure(sw::Size(100, 50));
    item.Measure(sw::Size(80, 40));
    item.Arrange(sw::Rect(3, 4, 24, 12));

    CHECK_EQ(2, item.measureCount);
    CHECK_EQ(1, item.arrangeCount);
    CHECK_EQ(sw::Size(80, 40), item.lastMeasureAvailableSize);
    CHECK_EQ(sw::Rect(3, 4, 24, 12), item.lastArrangeRect);
    REQUIRE_EQ(2, static_cast<int>(item.measureAvailableSizes.size()));
    CHECK_EQ(sw::Size(100, 50), item.measureAvailableSizes[0]);
    CHECK_EQ(sw::Size(80, 40), item.measureAvailableSizes[1]);
    REQUIRE_EQ(1, static_cast<int>(item.arrangeRects.size()));
    CHECK_EQ(sw::Rect(3, 4, 24, 12), item.arrangeRects[0]);

    std::vector<std::string> expectedCalls{
        "item.Measure",
        "item.Measure",
        "item.Arrange"};
    CHECK_EQ(expectedCalls, calls);
}

TEST_CASE("RecordingLayout rejects child access because it is a leaf")
{
    swtest::layouttest::RecordingLayout item;

    CHECK_EQ(0, item.GetChildLayoutCount());
    CHECK_THROWS_AS(item.GetChildLayoutAt(0), std::out_of_range);
}

TEST_CASE("ContainerLayout owns children and exposes them through ILayout")
{
    std::vector<std::string> calls;
    swtest::layouttest::ContainerLayout container(&calls);
    container.layoutTag = 9;
    container.desiredSize = sw::Size(50, 20);

    auto &first = container.EmplaceChild("first", sw::Size(10, 5), 1);
    auto &second = container.EmplaceChild("second", sw::Size(20, 8), 2);

    CHECK_EQ(9u, container.GetLayoutTag());
    CHECK_EQ(sw::Size(50, 20), container.GetDesireSize());
    CHECK_EQ(2, container.GetChildLayoutCount());
    CHECK(&first == &container.GetChildLayoutAt(0));
    CHECK(&second == &container.GetChildLayoutAt(1));

    sw::ILayout &child = container.GetChildLayoutAt(1);
    child.Measure(sw::Size(70, 30));
    child.Arrange(sw::Rect(1, 2, 20, 8));

    CHECK_EQ(1, second.measureCount);
    CHECK_EQ(1, second.arrangeCount);
    CHECK_EQ(sw::Size(70, 30), second.lastMeasureAvailableSize);
    CHECK_EQ(sw::Rect(1, 2, 20, 8), second.lastArrangeRect);

    std::vector<std::string> expectedCalls{
        "second.Measure",
        "second.Arrange"};
    CHECK_EQ(expectedCalls, calls);
    CHECK_THROWS_AS(container.GetChildLayoutAt(2), std::out_of_range);
}

TEST_CASE("StackLayoutH handles an empty container")
{
    LayoutFixture<sw::StackLayoutH> fx;

    CHECK_EQ(sw::Size(0, 0), fx.host.MeasureOverride(sw::Size(100, 60)));

    CHECK_NOTHROW(fx.host.ArrangeOverride(sw::Size(100, 60)));
    CHECK_EQ(0, fx.container.GetChildLayoutCount());
    CHECK_EQ(0, static_cast<int>(fx.calls.size()));
}

TEST_CASE("StackLayoutH measures and arranges children from left to right")
{
    LayoutFixture<sw::StackLayoutH> fx;
    auto &first  = fx.container.EmplaceChild("first", sw::Size(30, 10));
    auto &second = fx.container.EmplaceChild("second", sw::Size(20, 15));
    auto &third  = fx.container.EmplaceChild("third", sw::Size(5, 8));

    CHECK_EQ(sw::Size(55, 15), fx.host.MeasureOverride(sw::Size(100, 40)));
    CHECK_EQ(1, first.measureCount);
    CHECK_EQ(1, second.measureCount);
    CHECK_EQ(1, third.measureCount);
    CHECK_EQ(sw::Size(kInf, 40), first.lastMeasureAvailableSize);
    CHECK_EQ(sw::Size(kInf, 40), second.lastMeasureAvailableSize);
    CHECK_EQ(sw::Size(kInf, 40), third.lastMeasureAvailableSize);

    fx.host.ArrangeOverride(sw::Size(55, 20));
    CHECK_EQ(sw::Rect(0, 0, 30, 20), first.lastArrangeRect);
    CHECK_EQ(sw::Rect(30, 0, 20, 20), second.lastArrangeRect);
    CHECK_EQ(sw::Rect(50, 0, 5, 20), third.lastArrangeRect);

    std::vector<std::string> expectedCalls{
        "first.Measure",
        "second.Measure",
        "third.Measure",
        "first.Arrange",
        "second.Arrange",
        "third.Arrange"};
    CHECK_EQ(expectedCalls, fx.calls);
}

TEST_CASE("StackLayoutV handles a single child with infinite available width")
{
    LayoutFixture<sw::StackLayoutV> fx;
    auto &child = fx.container.EmplaceChild("child", sw::Size(18, 24));

    CHECK_EQ(sw::Size(18, 24), fx.host.MeasureOverride(sw::Size(kInf, 50)));
    CHECK_EQ(sw::Size(kInf, kInf), child.lastMeasureAvailableSize);

    fx.host.ArrangeOverride(sw::Size(40, 24));
    CHECK_EQ(sw::Rect(0, 0, 40, 24), child.lastArrangeRect);
}

TEST_CASE("StackLayoutV measures and arranges children from top to bottom")
{
    LayoutFixture<sw::StackLayoutV> fx;
    auto &first  = fx.container.EmplaceChild("first", sw::Size(12, 20));
    auto &second = fx.container.EmplaceChild("second", sw::Size(18, 15));
    auto &third  = fx.container.EmplaceChild("third", sw::Size(8, 30));

    CHECK_EQ(sw::Size(18, 65), fx.host.MeasureOverride(sw::Size(60, 50)));
    CHECK_EQ(sw::Size(60, kInf), first.lastMeasureAvailableSize);
    CHECK_EQ(sw::Size(60, kInf), second.lastMeasureAvailableSize);
    CHECK_EQ(sw::Size(60, kInf), third.lastMeasureAvailableSize);

    fx.host.ArrangeOverride(sw::Size(25, 65));
    CHECK_EQ(sw::Rect(0, 0, 25, 20), first.lastArrangeRect);
    CHECK_EQ(sw::Rect(0, 20, 25, 15), second.lastArrangeRect);
    CHECK_EQ(sw::Rect(0, 35, 25, 30), third.lastArrangeRect);
}

TEST_CASE("StackLayout dispatches through the generic host orientation")
{
    LayoutFixture<sw::StackLayout> fx;
    auto &first  = fx.container.EmplaceChild("first", sw::Size(30, 10));
    auto &second = fx.container.EmplaceChild("second", sw::Size(20, 15));

    CHECK_EQ(sw::Size(30, 25), fx.host.MeasureOverride(sw::Size(100, 40)));
    CHECK_EQ(sw::Size(100, kInf), first.lastMeasureAvailableSize);
    CHECK_EQ(sw::Size(100, kInf), second.lastMeasureAvailableSize);

    fx.host.ArrangeOverride(sw::Size(40, 25));
    CHECK_EQ(sw::Rect(0, 0, 40, 10), first.lastArrangeRect);
    CHECK_EQ(sw::Rect(0, 10, 40, 15), second.lastArrangeRect);

    fx.host.orientation = sw::Orientation::Horizontal;
    CHECK_EQ(sw::Size(50, 15), fx.host.MeasureOverride(sw::Size(100, 40)));
    CHECK_EQ(sw::Size(kInf, 40), first.lastMeasureAvailableSize);
    CHECK_EQ(sw::Size(kInf, 40), second.lastMeasureAvailableSize);

    fx.host.ArrangeOverride(sw::Size(50, 20));
    CHECK_EQ(sw::Rect(0, 0, 30, 20), first.lastArrangeRect);
    CHECK_EQ(sw::Rect(30, 0, 20, 20), second.lastArrangeRect);
}

TEST_CASE("FillLayout measures all children with the same constraint and fills the final rect")
{
    LayoutFixture<sw::FillLayout> fx;
    auto &first  = fx.container.EmplaceChild("first", sw::Size(30, 10));
    auto &second = fx.container.EmplaceChild("second", sw::Size(40, 25));

    CHECK_EQ(sw::Size(40, 25), fx.host.MeasureOverride(sw::Size(80, 60)));
    CHECK_EQ(sw::Size(80, 60), first.lastMeasureAvailableSize);
    CHECK_EQ(sw::Size(80, 60), second.lastMeasureAvailableSize);

    fx.host.ArrangeOverride(sw::Size(120, 70));
    CHECK_EQ(sw::Rect(0, 0, 120, 70), first.lastArrangeRect);
    CHECK_EQ(sw::Rect(0, 0, 120, 70), second.lastArrangeRect);
}

TEST_CASE("FillLayout handles an empty container")
{
    LayoutFixture<sw::FillLayout> fx;

    CHECK_EQ(sw::Size(0, 0), fx.host.MeasureOverride(sw::Size(80, 60)));

    CHECK_NOTHROW(fx.host.ArrangeOverride(sw::Size(80, 60)));
    CHECK_EQ(0, fx.container.GetChildLayoutCount());
    CHECK_EQ(0, static_cast<int>(fx.calls.size()));
}

TEST_CASE("CanvasLayout uses layout tags as absolute positions")
{
    LayoutFixture<sw::CanvasLayout> fx;
    sw::CanvasLayoutTag tag(12.5f, 7.25f);
    sw::CanvasLayoutTag roundTrip(static_cast<uint64_t>(tag));
    CHECK_EQ(tag.left, roundTrip.left);
    CHECK_EQ(tag.top, roundTrip.top);

    auto &first = fx.container.EmplaceChild(
        "first",
        sw::Size(20, 8),
        static_cast<uint64_t>(sw::CanvasLayoutTag(10.f, 5.f)));
    auto &second = fx.container.EmplaceChild(
        "second",
        sw::Size(4, 10),
        static_cast<uint64_t>(sw::CanvasLayoutTag(2.f, 7.f)));

    CHECK_EQ(sw::Size(30, 17), fx.host.MeasureOverride(sw::Size(200, 100)));
    CHECK_EQ(sw::Size(kInf, kInf), first.lastMeasureAvailableSize);
    CHECK_EQ(sw::Size(kInf, kInf), second.lastMeasureAvailableSize);

    fx.host.ArrangeOverride(sw::Size(30, 17));
    CHECK_EQ(sw::Rect(10, 5, 20, 8), first.lastArrangeRect);
    CHECK_EQ(sw::Rect(2, 7, 4, 10), second.lastArrangeRect);
}

TEST_CASE("DockLayout docks children on each edge")
{
    LayoutFixture<sw::DockLayout> fx;
    fx.host.lastChildFill = false;

    auto &left   = fx.container.EmplaceChild("left", sw::Size(10, 6), static_cast<uint64_t>(sw::DockLayoutTag::Left));
    auto &top    = fx.container.EmplaceChild("top", sw::Size(20, 7), static_cast<uint64_t>(sw::DockLayoutTag::Top));
    auto &right  = fx.container.EmplaceChild("right", sw::Size(5, 8), static_cast<uint64_t>(sw::DockLayoutTag::Right));
    auto &bottom = fx.container.EmplaceChild("bottom", sw::Size(12, 4), static_cast<uint64_t>(sw::DockLayoutTag::Bottom));

    CHECK_EQ(sw::Size(30, 15), fx.host.MeasureOverride(sw::Size(100, 80)));
    CHECK_EQ(sw::Size(100, 80), left.lastMeasureAvailableSize);
    CHECK_EQ(sw::Size(90, 80), top.lastMeasureAvailableSize);
    CHECK_EQ(sw::Size(90, 73), right.lastMeasureAvailableSize);
    CHECK_EQ(sw::Size(85, 73), bottom.lastMeasureAvailableSize);

    fx.host.ArrangeOverride(sw::Size(100, 80));
    CHECK_EQ(sw::Rect(0, 0, 10, 80), left.lastArrangeRect);
    CHECK_EQ(sw::Rect(10, 0, 90, 7), top.lastArrangeRect);
    CHECK_EQ(sw::Rect(95, 7, 5, 73), right.lastArrangeRect);
    CHECK_EQ(sw::Rect(10, 76, 85, 4), bottom.lastArrangeRect);
}

TEST_CASE("DockLayout can let the last child fill the remaining area")
{
    LayoutFixture<sw::DockLayout> fx;
    fx.host.lastChildFill = true;

    auto &left = fx.container.EmplaceChild("left", sw::Size(10, 5), static_cast<uint64_t>(sw::DockLayoutTag::Left));
    auto &top  = fx.container.EmplaceChild("top", sw::Size(20, 6), static_cast<uint64_t>(sw::DockLayoutTag::Top));
    auto &fill = fx.container.EmplaceChild("fill", sw::Size(7, 8), static_cast<uint64_t>(sw::DockLayoutTag::Right));

    CHECK_NOTHROW(fx.host.MeasureOverride(sw::Size(100, 60)));
    fx.host.ArrangeOverride(sw::Size(100, 60));

    CHECK_EQ(sw::Rect(0, 0, 10, 60), left.lastArrangeRect);
    CHECK_EQ(sw::Rect(10, 0, 90, 6), top.lastArrangeRect);
    CHECK_EQ(sw::Rect(10, 6, 90, 54), fill.lastArrangeRect);
}

TEST_CASE("DockLayout treats unknown layout tags as left docks")
{
    LayoutFixture<sw::DockLayout> fx;
    fx.host.lastChildFill = false;

    auto &fallback = fx.container.EmplaceChild("fallback", sw::Size(10, 6), 99);

    CHECK_EQ(sw::Size(10, 6), fx.host.MeasureOverride(sw::Size(100, 80)));
    CHECK_EQ(sw::Size(100, 80), fallback.lastMeasureAvailableSize);

    fx.host.ArrangeOverride(sw::Size(100, 80));
    CHECK_EQ(sw::Rect(0, 0, 10, 80), fallback.lastArrangeRect);
}

TEST_CASE("WrapLayout wraps horizontally when width is exhausted")
{
    LayoutFixture<sw::WrapLayout> fx;
    fx.host.orientation = sw::Orientation::Horizontal;

    auto &first  = fx.container.EmplaceChild("first", sw::Size(20, 10));
    auto &second = fx.container.EmplaceChild("second", sw::Size(30, 8));
    auto &third  = fx.container.EmplaceChild("third", sw::Size(40, 12));

    CHECK_EQ(sw::Size(50, 22), fx.host.MeasureOverride(sw::Size(60, 50)));
    CHECK_EQ(sw::Size(60, kInf), first.lastMeasureAvailableSize);
    CHECK_EQ(sw::Size(60, kInf), second.lastMeasureAvailableSize);
    CHECK_EQ(sw::Size(60, kInf), third.lastMeasureAvailableSize);

    fx.host.ArrangeOverride(sw::Size(60, 50));
    CHECK_EQ(sw::Rect(0, 0, 20, 10), first.lastArrangeRect);
    CHECK_EQ(sw::Rect(20, 0, 30, 10), second.lastArrangeRect);
    CHECK_EQ(sw::Rect(0, 10, 40, 12), third.lastArrangeRect);
}

TEST_CASE("WrapLayout measures horizontally without a width constraint")
{
    LayoutFixture<sw::WrapLayout> fx;
    fx.host.orientation = sw::Orientation::Horizontal;

    auto &first  = fx.container.EmplaceChild("first", sw::Size(20, 10));
    auto &second = fx.container.EmplaceChild("second", sw::Size(30, 8));
    auto &third  = fx.container.EmplaceChild("third", sw::Size(40, 12));

    CHECK_EQ(sw::Size(90, 12), fx.host.MeasureOverride(sw::Size(kInf, 50)));
    CHECK_EQ(sw::Size(kInf, kInf), first.lastMeasureAvailableSize);
    CHECK_EQ(sw::Size(kInf, kInf), second.lastMeasureAvailableSize);
    CHECK_EQ(sw::Size(kInf, kInf), third.lastMeasureAvailableSize);

    fx.host.ArrangeOverride(sw::Size(90, 20));
    CHECK_EQ(sw::Rect(0, 0, 20, 12), first.lastArrangeRect);
    CHECK_EQ(sw::Rect(20, 0, 30, 12), second.lastArrangeRect);
    CHECK_EQ(sw::Rect(50, 0, 40, 12), third.lastArrangeRect);
}

TEST_CASE("WrapLayout wraps vertically when height is exhausted")
{
    LayoutFixture<sw::WrapLayout> fx;
    fx.host.orientation = sw::Orientation::Vertical;

    auto &first  = fx.container.EmplaceChild("first", sw::Size(10, 20));
    auto &second = fx.container.EmplaceChild("second", sw::Size(12, 15));
    auto &third  = fx.container.EmplaceChild("third", sw::Size(14, 30));

    CHECK_EQ(sw::Size(26, 35), fx.host.MeasureOverride(sw::Size(50, 40)));
    CHECK_EQ(sw::Size(kInf, 40), first.lastMeasureAvailableSize);
    CHECK_EQ(sw::Size(kInf, 40), second.lastMeasureAvailableSize);
    CHECK_EQ(sw::Size(kInf, 40), third.lastMeasureAvailableSize);

    fx.host.ArrangeOverride(sw::Size(50, 40));
    CHECK_EQ(sw::Rect(0, 0, 12, 20), first.lastArrangeRect);
    CHECK_EQ(sw::Rect(0, 20, 12, 15), second.lastArrangeRect);
    CHECK_EQ(sw::Rect(12, 0, 14, 30), third.lastArrangeRect);
}

TEST_CASE("WrapLayout measures vertically without a height constraint")
{
    LayoutFixture<sw::WrapLayout> fx;
    fx.host.orientation = sw::Orientation::Vertical;

    auto &first  = fx.container.EmplaceChild("first", sw::Size(10, 20));
    auto &second = fx.container.EmplaceChild("second", sw::Size(12, 15));
    auto &third  = fx.container.EmplaceChild("third", sw::Size(14, 30));

    CHECK_EQ(sw::Size(14, 65), fx.host.MeasureOverride(sw::Size(50, kInf)));
    CHECK_EQ(sw::Size(kInf, kInf), first.lastMeasureAvailableSize);
    CHECK_EQ(sw::Size(kInf, kInf), second.lastMeasureAvailableSize);
    CHECK_EQ(sw::Size(kInf, kInf), third.lastMeasureAvailableSize);

    fx.host.ArrangeOverride(sw::Size(20, 65));
    CHECK_EQ(sw::Rect(0, 0, 14, 20), first.lastArrangeRect);
    CHECK_EQ(sw::Rect(0, 20, 14, 15), second.lastArrangeRect);
    CHECK_EQ(sw::Rect(0, 35, 14, 30), third.lastArrangeRect);
}

TEST_CASE("UniformGridLayout measures evenly and respects firstColumn")
{
    LayoutFixture<sw::UniformGridLayout> fx;
    fx.host.rows = 2;
    fx.host.columns = 3;
    fx.host.firstColumn = 1;

    auto &first  = fx.container.EmplaceChild("first", sw::Size(10, 8));
    auto &second = fx.container.EmplaceChild("second", sw::Size(12, 9));
    auto &third  = fx.container.EmplaceChild("third", sw::Size(14, 10));
    auto &fourth = fx.container.EmplaceChild("fourth", sw::Size(16, 11));
    auto &fifth  = fx.container.EmplaceChild("fifth", sw::Size(18, 12));
    auto &sixth  = fx.container.EmplaceChild("sixth", sw::Size(20, 13));

    CHECK_EQ(sw::Size(120, 60), fx.host.MeasureOverride(sw::Size(120, 60)));
    CHECK_EQ(sw::Size(40, 30), first.lastMeasureAvailableSize);
    CHECK_EQ(sw::Size(40, 30), second.lastMeasureAvailableSize);
    CHECK_EQ(sw::Size(40, 30), third.lastMeasureAvailableSize);
    CHECK_EQ(sw::Size(40, 30), fourth.lastMeasureAvailableSize);
    CHECK_EQ(sw::Size(40, 30), fifth.lastMeasureAvailableSize);
    CHECK_EQ(sw::Size(40, 30), sixth.lastMeasureAvailableSize);

    fx.host.ArrangeOverride(sw::Size(120, 60));
    CHECK_EQ(sw::Rect(40, 0, 40, 30), first.lastArrangeRect);
    CHECK_EQ(sw::Rect(80, 0, 40, 30), second.lastArrangeRect);
    CHECK_EQ(sw::Rect(0, 30, 40, 30), third.lastArrangeRect);
    CHECK_EQ(sw::Rect(40, 30, 40, 30), fourth.lastArrangeRect);
    CHECK_EQ(sw::Rect(80, 30, 40, 30), fifth.lastArrangeRect);
    CHECK_EQ(sw::Rect(80, 30, 40, 30), sixth.lastArrangeRect);
}

TEST_CASE("UniformGridLayout clamps firstColumn to the last column")
{
    LayoutFixture<sw::UniformGridLayout> fx;
    fx.host.rows = 2;
    fx.host.columns = 3;
    fx.host.firstColumn = 8;

    auto &first  = fx.container.EmplaceChild("first", sw::Size(10, 8));
    auto &second = fx.container.EmplaceChild("second", sw::Size(12, 9));
    auto &third  = fx.container.EmplaceChild("third", sw::Size(14, 10));
    auto &fourth = fx.container.EmplaceChild("fourth", sw::Size(16, 11));

    CHECK_EQ(sw::Size(120, 60), fx.host.MeasureOverride(sw::Size(120, 60)));
    CHECK_EQ(sw::Size(40, 30), first.lastMeasureAvailableSize);
    CHECK_EQ(sw::Size(40, 30), second.lastMeasureAvailableSize);
    CHECK_EQ(sw::Size(40, 30), third.lastMeasureAvailableSize);
    CHECK_EQ(sw::Size(40, 30), fourth.lastMeasureAvailableSize);

    fx.host.ArrangeOverride(sw::Size(120, 60));
    CHECK_EQ(sw::Rect(80, 0, 40, 30), first.lastArrangeRect);
    CHECK_EQ(sw::Rect(0, 30, 40, 30), second.lastArrangeRect);
    CHECK_EQ(sw::Rect(40, 30, 40, 30), third.lastArrangeRect);
    CHECK_EQ(sw::Rect(80, 30, 40, 30), fourth.lastArrangeRect);
}

TEST_CASE("GridLayout uses fixed rows and columns without GUI involvement")
{
    LayoutFixture<sw::GridLayout> fx;
    fx.host.rows.Add(sw::FixSizeGridRow(20));
    fx.host.rows.Add(sw::FixSizeGridRow(30));
    fx.host.columns.Add(sw::FixSizeGridColumn(40));
    fx.host.columns.Add(sw::FixSizeGridColumn(60));

    auto &first  = fx.container.EmplaceChild("first", sw::Size(1, 1), static_cast<uint64_t>(sw::GridLayoutTag(0, 0)));
    auto &second = fx.container.EmplaceChild("second", sw::Size(1, 1), static_cast<uint64_t>(sw::GridLayoutTag(0, 1)));
    auto &third  = fx.container.EmplaceChild("third", sw::Size(1, 1), static_cast<uint64_t>(sw::GridLayoutTag(1, 0)));
    auto &fourth = fx.container.EmplaceChild("fourth", sw::Size(1, 1), static_cast<uint64_t>(sw::GridLayoutTag(1, 1)));

    CHECK_EQ(sw::Size(100, 50), fx.host.MeasureOverride(sw::Size(100, 50)));
    CHECK_EQ(1, first.measureCount);
    CHECK_EQ(1, second.measureCount);
    CHECK_EQ(1, third.measureCount);
    CHECK_EQ(1, fourth.measureCount);
    CHECK_EQ(sw::Size(40, 20), first.lastMeasureAvailableSize);
    CHECK_EQ(sw::Size(60, 20), second.lastMeasureAvailableSize);
    CHECK_EQ(sw::Size(40, 30), third.lastMeasureAvailableSize);
    CHECK_EQ(sw::Size(60, 30), fourth.lastMeasureAvailableSize);

    fx.host.ArrangeOverride(sw::Size(100, 50));
    CHECK_EQ(sw::Rect(0, 0, 40, 20), first.lastArrangeRect);
    CHECK_EQ(sw::Rect(40, 0, 60, 20), second.lastArrangeRect);
    CHECK_EQ(sw::Rect(0, 20, 40, 30), third.lastArrangeRect);
    CHECK_EQ(sw::Rect(40, 20, 60, 30), fourth.lastArrangeRect);
}

TEST_CASE("GridLayout uses a default one-by-one grid when rows and columns are omitted")
{
    LayoutFixture<sw::GridLayout> fx;
    auto &child = fx.container.EmplaceChild("child", sw::Size(16, 9));

    CHECK_EQ(sw::Size(120, 80), fx.host.MeasureOverride(sw::Size(120, 80)));
    CHECK_EQ(1, child.measureCount);
    CHECK_EQ(sw::Size(120, 80), child.lastMeasureAvailableSize);

    fx.host.ArrangeOverride(sw::Size(120, 80));
    CHECK_EQ(sw::Rect(0, 0, 120, 80), child.lastArrangeRect);
}

TEST_CASE("GridLayout clamps out-of-range tags and zero spans")
{
    LayoutFixture<sw::GridLayout> fx;
    fx.host.rows.Add(sw::FixSizeGridRow(10));
    fx.host.rows.Add(sw::FixSizeGridRow(20));
    fx.host.columns.Add(sw::FixSizeGridColumn(30));
    fx.host.columns.Add(sw::FixSizeGridColumn(40));

    auto &clamped = fx.container.EmplaceChild(
        "clamped",
        sw::Size(2, 3),
        static_cast<uint64_t>(sw::GridLayoutTag(9, 8, 0, 0)));
    auto &spanned = fx.container.EmplaceChild(
        "spanned",
        sw::Size(4, 5),
        static_cast<uint64_t>(sw::GridLayoutTag(0, 0, 5, 5)));

    CHECK_EQ(sw::Size(70, 30), fx.host.MeasureOverride(sw::Size(100, 50)));
    CHECK_EQ(1, clamped.measureCount);
    CHECK_EQ(1, spanned.measureCount);
    CHECK_EQ(sw::Size(40, 20), clamped.lastMeasureAvailableSize);
    CHECK_EQ(sw::Size(70, 30), spanned.lastMeasureAvailableSize);

    fx.host.ArrangeOverride(sw::Size(100, 50));
    CHECK_EQ(sw::Rect(30, 10, 40, 20), clamped.lastArrangeRect);
    CHECK_EQ(sw::Rect(0, 0, 70, 30), spanned.lastArrangeRect);
}

TEST_CASE("GridLayout distributes span sizes across auto rows and columns")
{
    LayoutFixture<sw::GridLayout> fx;
    fx.host.rows.Add(sw::AutoSizeGridRow());
    fx.host.rows.Add(sw::AutoSizeGridRow());
    fx.host.columns.Add(sw::AutoSizeGridColumn());
    fx.host.columns.Add(sw::AutoSizeGridColumn());

    auto &span = fx.container.EmplaceChild(
        "span",
        sw::Size(100, 80),
        static_cast<uint64_t>(sw::GridLayoutTag(0, 0, 2, 2)));

    CHECK_EQ(sw::Size(100, 80), fx.host.MeasureOverride(sw::Size(kInf, kInf)));
    CHECK_EQ(2, span.measureCount);
    CHECK_EQ(sw::Size(kInf, kInf), span.measureAvailableSizes[0]);
    CHECK_EQ(sw::Size(100, kInf), span.measureAvailableSizes[1]);

    fx.host.ArrangeOverride(sw::Size(100, 80));
    CHECK_EQ(sw::Rect(0, 0, 100, 80), span.lastArrangeRect);
}

TEST_CASE("GridLayout uses fill remain rows and columns in finite space")
{
    LayoutFixture<sw::GridLayout> fx;
    fx.host.rows.Add(sw::AutoSizeGridRow());
    fx.host.rows.Add(sw::FillRemainGridRow(1));
    fx.host.columns.Add(sw::AutoSizeGridColumn());
    fx.host.columns.Add(sw::FillRemainGridColumn(1));

    auto &first = fx.container.EmplaceChild(
        "first",
        sw::Size(30, 10),
        static_cast<uint64_t>(sw::GridLayoutTag(0, 0)));
    auto &second = fx.container.EmplaceChild(
        "second",
        sw::Size(20, 40),
        static_cast<uint64_t>(sw::GridLayoutTag(1, 1)));

    CHECK_EQ(sw::Size(120, 100), fx.host.MeasureOverride(sw::Size(120, 100)));
    CHECK_EQ(2, first.measureCount);
    CHECK_EQ(1, second.measureCount);
    CHECK_EQ(sw::Size(kInf, kInf), first.measureAvailableSizes[0]);
    CHECK_EQ(sw::Size(30, kInf), first.measureAvailableSizes[1]);
    CHECK_EQ(sw::Size(90, 90), second.measureAvailableSizes[0]);

    fx.host.ArrangeOverride(sw::Size(120, 100));
    CHECK_EQ(sw::Rect(0, 0, 30, 10), first.lastArrangeRect);
    CHECK_EQ(sw::Rect(30, 10, 90, 90), second.lastArrangeRect);
}
