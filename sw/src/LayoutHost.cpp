#include "LayoutHost.h"

void sw::LayoutHost::Associate(ILayout *obj)
{
    this->_associatedObj = obj;
}

bool sw::LayoutHost::IsAssociated(ILayout *obj)
{
    return obj == nullptr
               ? (this->_associatedObj != nullptr)
               : (this->_associatedObj == obj);
}

int sw::LayoutHost::GetChildLayoutCount()
{
    return this->_associatedObj->GetChildLayoutCount();
}

sw::ILayout &sw::LayoutHost::GetChildLayoutAt(int index)
{
    return this->_associatedObj->GetChildLayoutAt(index);
}
