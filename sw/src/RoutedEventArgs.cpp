#include "RoutedEventArgs.h"

sw::SizeChangedEventArgs::SizeChangedEventArgs(const Size &newClientSize)
    : newClientSize(newClientSize)
{
}

sw::PositionChangedEventArgs::PositionChangedEventArgs(const Point &newClientPosition)
    : newClientPosition(newClientPosition)
{
}

sw::TextChangedEventArgs::TextChangedEventArgs(const wchar_t *newText)
    : newText(newText)
{
}

sw::GotCharEventArgs::GotCharEventArgs(wchar_t ch, KeyFlags flags)
    : ch(ch), flags(flags)
{
}

sw::KeyDownEventArgs::KeyDownEventArgs(VirtualKey key, KeyFlags falgs)
    : key(key), flags(flags)
{
}

sw::KeyUpEventArgs::KeyUpEventArgs(VirtualKey key, KeyFlags falgs)
    : key(key), flags(flags)
{
}

sw::MouseMoveEventArgs::MouseMoveEventArgs(Point mousePosition, MouseKey keyState)
    : mousePosition(mousePosition), keyState(keyState)
{
}

sw::MouseWheelEventArgs::MouseWheelEventArgs(int wheelDelta, Point mousePosition, MouseKey keyState)
    : wheelDelta(wheelDelta), mousePosition(mousePosition), keyState(keyState)
{
}
