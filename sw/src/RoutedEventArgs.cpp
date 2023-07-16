#include "RoutedEventArgs.h"

sw::SizeChangedEventArgs::SizeChangedEventArgs(const Size &newClientSize)
    : RoutedEventArgs(UIElement_SizeChanged), newClientSize(newClientSize)
{
}

sw::PositionChangedEventArgs::PositionChangedEventArgs(const Point &newClientPosition)
    : RoutedEventArgs(UIElement_PositionChanged), newClientPosition(newClientPosition)
{
}

sw::TextChangedEventArgs::TextChangedEventArgs(const wchar_t *newText)
    : RoutedEventArgs(UIElement_TextChanged), newText(newText)
{
}

sw::WindowClosingEventArgs::WindowClosingEventArgs()
    : RoutedEventArgs(Window_Closing)
{
}
