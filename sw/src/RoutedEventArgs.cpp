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
