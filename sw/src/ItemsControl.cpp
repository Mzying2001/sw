#include "ItemsControl.h"

sw::ItemsControl::ItemsControl()
    : ItemsCount(
          // get
          [&]() -> const int & {
              static int result;
              result = this->GetItemsCount();
              return result;
          }),

      SelectedIndex(
          // get
          [&]() -> const int & {
              static int result;
              result = this->GetSelectedIndex();
              return result;
          },
          // set
          [&](const int &value) {
              this->SetSelectedIndex(value);
          }),

      SelectedItem(
          // get
          [&]() -> const std::wstring & {
              static std::wstring result;
              result = this->GetSelectedItem();
              return result;
          })
{
}

void sw::ItemsControl::OnSelectionChanged()
{
    this->RaiseRoutedEvent(ItemsControl_SelectionChanged);
}
