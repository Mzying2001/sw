#include "ItemsControl.h"

sw::ItemsControl::ItemsControl()
    : ItemsSource(
          Property<IList *>::Init(this)
              .Getter([](ItemsControl *self) -> IList * {
                  return self->_itemsSource;
              })
              .Setter([](ItemsControl *self, IList *value) {
                  if (self->_itemsSource != value) {
                      self->_itemsSource = value;
                      self->UpdateCurrentItemsSource();
                      self->RaisePropertyChanged(&ItemsControl::ItemsSource);
                  }
              })),

      ItemsCount(
          Property<int>::Init(this)
              .Getter([](ItemsControl *self) -> int {
                  IList *currentItemsSource = self->_current.itemsSource;
                  return currentItemsSource ? currentItemsSource->Count() : 0;
              })),

      SelectedIndex(
          Property<int>::Init(this)
              .Getter([](ItemsControl *self) -> int {
                  return self->GetSelectedIndex();
              })
              .Setter([](ItemsControl *self, int value) {
                  self->SetSelectedIndex(value);
              })),

      SelectedItem(
          Property<Variant>::Init(this)
              .Getter([](ItemsControl *self) -> Variant {
                  if (IList *currentItemsSource = self->GetCurrentItemsSource()) {
                      int index = self->GetSelectedIndex();
                      if (index >= 0 && index < currentItemsSource->Count()) {
                          return currentItemsSource->GetVariantAt(index);
                      }
                  }
                  return Variant{};
              }))
{
}

sw::IList *sw::ItemsControl::GetCurrentItemsSource()
{
    return _current.itemsSource;
}

void sw::ItemsControl::UpdateCurrentItemsSource()
{
    IList *oldItemsSource = _current.itemsSource;
    IList *newItemsSource = _itemsSource ? _itemsSource : GetDefaultItemsSource();

    if (newItemsSource == oldItemsSource) {
        return;
    }

    if (_current.notifyCollectionChanged != nullptr) {
        _current.notifyCollectionChanged->CollectionChanged -=
            NotifyCollectionChangedEventHandler(*this, &ItemsControl::_CollectionChangedEventHandler);
    }

    _current.itemsSource             = newItemsSource;
    _current.notifyCollectionChanged = dynamic_cast<INotifyCollectionChanged *>(newItemsSource);

    if (_current.notifyCollectionChanged != nullptr) {
        _current.notifyCollectionChanged->CollectionChanged +=
            NotifyCollectionChangedEventHandler(*this, &ItemsControl::_CollectionChangedEventHandler);
    }

    OnCurrentItemsSourceChanged(oldItemsSource, newItemsSource);
}

void sw::ItemsControl::OnSelectionChanged()
{
    RaisePropertyChanged(&ItemsControl::SelectedIndex);
    RaisePropertyChanged(&ItemsControl::SelectedItem);
    RaiseRoutedEvent(ItemsControl_SelectionChanged);
}

void sw::ItemsControl::_CollectionChangedEventHandler(
    INotifyCollectionChanged &sender, NotifyCollectionChangedEventArgs &args)
{
    OnCurrentItemsSourceCollectionChanged(args);
}
