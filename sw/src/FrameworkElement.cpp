#include "FrameworkElement.h"
#include "DataBinding.h"

sw::FrameworkElement::FrameworkElement()
    : DataContextChanged(
          Event<DataContextChangedEventHandler>::Init(this)
              .Delegate([](FrameworkElement *self) -> DataContextChangedEventHandler & {
                  return self->_dataContextChanged;
              })),

      Tag(
          Property<Variant>::Init(this)
              .Getter<&FrameworkElement::GetTag>()
              .Setter<&FrameworkElement::SetTag>()),

      DataContext(
          Property<Variant>::Init(this)
              .Getter([](FrameworkElement *self) -> Variant {
                  return self->_dataContext;
              })
              .Setter([](FrameworkElement *self, const Variant &value) {
                  if (!self->_dataContext.ReferenceEquals(value)) {
                      auto oldDataContext = self->CurrentDataContext.Get();
                      self->_dataContext  = value;
                      self->RaisePropertyChanged(&FrameworkElement::DataContext);
                      if (oldDataContext != self->_dataContext.Object()) {
                          self->OnCurrentDataContextChanged(oldDataContext);
                      }
                  }
              })),

      CurrentDataContext(
          Property<DynamicObject *>::Init(this)
              .Getter([](FrameworkElement *self) -> DynamicObject * {
                  DynamicObject *result     = nullptr;
                  FrameworkElement *element = self;
                  do {
                      result  = element->_dataContext.Object();
                      element = element->GetParent();
                  } while (result == nullptr && element != nullptr);
                  return result;
              }))
{
}

bool sw::FrameworkElement::AddBinding(BindingBase *binding)
{
    if (binding == nullptr) {
        return false;
    } else {
        this->_bindings[binding->GetTargetPropertyId()].reset(binding);
        return true;
    }
}

bool sw::FrameworkElement::AddBinding(Binding *binding)
{
    if (binding == nullptr) {
        return false;
    }
    if (binding->GetSourceObject() == nullptr) {
        auto dataBinding = DataBinding::Create(this, binding);
        return this->AddBinding(static_cast<BindingBase *>(dataBinding));
    } else {
        binding->SetTargetObject(this);
        return this->AddBinding(static_cast<BindingBase *>(binding));
    }
}

bool sw::FrameworkElement::AddBinding(DataBinding *binding)
{
    if (binding == nullptr) {
        return false;
    } else {
        binding->SetTargetElement(this);
        return this->AddBinding(static_cast<BindingBase *>(binding));
    }
}

bool sw::FrameworkElement::RemoveBinding(FieldId propertyId)
{
    if (this->_bindings.count(propertyId) == 0) {
        return false;
    } else {
        this->_bindings.erase(propertyId);
        return true;
    }
}

sw::Variant sw::FrameworkElement::GetTag() const
{
    return this->_tag;
}

void sw::FrameworkElement::SetTag(const Variant &tag)
{
    if (!this->_tag.ReferenceEquals(tag)) {
        this->_tag = tag;
        this->RaisePropertyChanged(&FrameworkElement::Tag);
    }
}

void sw::FrameworkElement::OnCurrentDataContextChanged(DynamicObject *oldDataContext)
{
    std::vector<FrameworkElement *> stack;
    stack.push_back(this);

    while (!stack.empty()) //
    {
        auto current = stack.back();
        stack.pop_back();

        current->RaisePropertyChanged(
            &FrameworkElement::CurrentDataContext);

        if (current->_dataContextChanged) {
            DataContextChangedEventArgs args{};
            args.oldDataContext = oldDataContext;
            current->_dataContextChanged(*current, args);
        }

        int childCount = current->GetChildCount();

        for (int i = 0; i < childCount; ++i) //
        {
            auto &child = current->GetChildAt(i);

            if (child._dataContext.IsNull()) {
                stack.push_back(&child);
            }
        }
    }
}
