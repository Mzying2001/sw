#include "Test.h"

#include "Keys.h"
#include "RoutedEvent.h"
#include "RoutedEventArgs.h"

#include <type_traits>
#include <windows.h>

TEST_CASE("KeyFlags parses lParam bit fields")
{
    LPARAM value = 0;
    value |= 3;
    value |= static_cast<LPARAM>(0x45) << 16;
    value |= static_cast<LPARAM>(1) << 24;
    value |= static_cast<LPARAM>(1) << 29;
    value |= static_cast<LPARAM>(1) << 30;
    value |= static_cast<LPARAM>(1) << 31;

    sw::KeyFlags flags(value);

    CHECK_EQ(3, flags.repeatCount);
    CHECK_EQ(0x45, flags.scanCode);
    CHECK(flags.isExtendedKey);
    CHECK(flags.contextCode);
    CHECK(flags.previousKeyState);
    CHECK(flags.transitionState);
}

TEST_CASE("Opted-in input enums support bit operations")
{
    static_assert(sw::_EnumSupportBitOperations<sw::MouseKey>::value, "MouseKey should support bit operations");
    static_assert(sw::_EnumSupportBitOperations<sw::HotKeyModifier>::value, "HotKeyModifier should support bit operations");

    auto mouse = sw::MouseKey::Ctrl | sw::MouseKey::MouseLeft;
    CHECK(static_cast<int>(mouse & sw::MouseKey::Ctrl) == static_cast<int>(sw::MouseKey::Ctrl));

    auto hotkey = sw::HotKeyModifier::Ctrl | sw::HotKeyModifier::Alt;
    CHECK(static_cast<int>(hotkey) ==
          (static_cast<int>(sw::HotKeyModifier::Ctrl) | static_cast<int>(sw::HotKeyModifier::Alt)));
}

TEST_CASE("RoutedEventArgs defaults and explicit constructor initialize routing state")
{
    sw::RoutedEventArgs args;
    CHECK(args.eventType == sw::RoutedEventType_Null);
    CHECK_FALSE(args.handled);
    CHECK_FALSE(args.handledMsg);
    CHECK(args.source == nullptr);
    CHECK(args.originalSource == nullptr);

    sw::RoutedEventArgs clicked(sw::ButtonBase_Clicked);
    CHECK(clicked.eventType == sw::ButtonBase_Clicked);
    CHECK_FALSE(clicked.handled);
}

TEST_CASE("Typed routed event traits identify event argument types")
{
    static_assert(sw::_IsTypedRoutedEventArgs<sw::SizeChangedEventArgs>::value, "Typed args should be detected");
    static_assert(!sw::_IsTypedRoutedEventArgs<sw::RoutedEventArgs>::value, "Base args should not be typed");
    static_assert(std::is_base_of<sw::RoutedEventArgs, sw::WindowClosingEventArgs>::value, "Cancelable typed args derive from routed args");

    sw::TypedRoutedEventArgs<sw::ButtonBase_Clicked> args;
    CHECK(args.eventType == sw::ButtonBase_Clicked);
    CHECK(args.EventType == sw::ButtonBase_Clicked);
}

TEST_CASE("Common routed event args store constructor data")
{
    sw::SizeChangedEventArgs size(sw::Size(80, 25));
    CHECK(size.eventType == sw::UIElement_SizeChanged);
    CHECK_EQ(sw::Size(80, 25), size.newClientSize);

    sw::PositionChangedEventArgs position(sw::Point(3, 4));
    CHECK(position.eventType == sw::UIElement_PositionChanged);
    CHECK_EQ(sw::Point(3, 4), position.newClientPosition);

    sw::KeyFlags keyFlags(static_cast<LPARAM>(1));
    sw::GotCharEventArgs gotChar(L'A', keyFlags);
    CHECK(gotChar.eventType == sw::UIElement_GotChar);
    CHECK_EQ(L'A', gotChar.ch);
    CHECK_EQ(1, gotChar.flags.repeatCount);

    sw::KeyDownEventArgs keyDown(sw::VirtualKey::A, keyFlags);
    CHECK(keyDown.eventType == sw::UIElement_KeyDown);
    CHECK(keyDown.key == sw::VirtualKey::A);

    sw::MouseMoveEventArgs mouseMove(sw::Point(5, 6), sw::MouseKey::Ctrl);
    CHECK(mouseMove.eventType == sw::UIElement_MouseMove);
    CHECK_EQ(sw::Point(5, 6), mouseMove.mousePosition);
    CHECK(mouseMove.keyState == sw::MouseKey::Ctrl);

    sw::MouseWheelEventArgs wheel(120, sw::Point(1, 2), sw::MouseKey::MouseLeft);
    CHECK(wheel.eventType == sw::UIElement_MouseWheel);
    CHECK_EQ(120, wheel.wheelDelta);

    sw::MouseButtonDownEventArgs button(sw::MouseKey::MouseRight, sw::Point(7, 8), sw::MouseKey::MouseRight);
    CHECK(button.eventType == sw::UIElement_MouseButtonDown);
    CHECK(button.key == sw::MouseKey::MouseRight);
}

TEST_CASE("Cancelable routed event args default to not canceled")
{
    sw::WindowClosingEventArgs closing;
    CHECK(closing.eventType == sw::Window_Closing);
    CHECK_FALSE(closing.cancel);

    sw::ShowContextMenuEventArgs menu(true, sw::Point(10, 20));
    CHECK(menu.eventType == sw::UIElement_ShowContextMenu);
    CHECK_FALSE(menu.cancel);
    CHECK(menu.isKeyboardMsg);
    CHECK_EQ(sw::Point(10, 20), menu.mousePosition);

    sw::ScrollingEventArgs scrolling(sw::ScrollOrientation::Vertical, sw::ScrollEvent::ThumbPosition, 33.5);
    CHECK(scrolling.eventType == sw::Layer_Scrolling);
    CHECK_FALSE(scrolling.cancel);
    CHECK(scrolling.scrollbar == sw::ScrollOrientation::Vertical);
    CHECK(scrolling.event == sw::ScrollEvent::ThumbPosition);
    CHECK_EQ(33.5, scrolling.pos);
}

TEST_CASE("List and control routed event args preserve indices text time and keys")
{
    sw::ListViewItemClickedEventArgs item(2, 3);
    CHECK(item.eventType == sw::ListView_ItemClicked);
    CHECK_EQ(2, item.index);
    CHECK_EQ(3, item.subIndex);

    sw::ListViewEndEditEventArgs edit(4, L"new");
    CHECK(edit.eventType == sw::ListView_EndEdit);
    CHECK_FALSE(edit.cancel);
    CHECK_EQ(4, edit.index);
    CHECK_EQ(std::wstring(L"new"), edit.newText);

    SYSTEMTIME time{};
    time.wYear  = 2026;
    time.wMonth = 6;
    time.wDay   = 19;
    sw::DateTimePickerTimeChangedEventArgs date(time);
    CHECK(date.eventType == sw::DateTimePicker_TimeChanged);
    CHECK_EQ(2026, date.time.wYear);
    CHECK_EQ(6, date.time.wMonth);
    CHECK_EQ(19, date.time.wDay);

    sw::SysLinkClickedEventArgs link(L"id", L"https://example.test");
    CHECK(link.eventType == sw::SysLink_Clicked);
    CHECK_EQ(std::wstring(L"id"), link.id);
    CHECK_EQ(std::wstring(L"https://example.test"), link.url);

    sw::HotKeyValueChangedEventArgs hotkey(sw::VirtualKey::K, sw::HotKeyModifier::Ctrl);
    CHECK(hotkey.eventType == sw::HotKeyControl_ValueChanged);
    CHECK(hotkey.key == sw::VirtualKey::K);
    CHECK(hotkey.modifier == sw::HotKeyModifier::Ctrl);
}
