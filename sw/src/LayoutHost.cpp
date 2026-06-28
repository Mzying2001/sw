#include "LayoutHost.h"

void sw::LayoutHost::Associate(ILayout *obj)
{
    _associatedObj = obj;
}

bool sw::LayoutHost::IsAssociated(ILayout *obj) const
{
    if (obj != nullptr) {
        return _associatedObj == obj;
    } else {
        return _associatedObj != nullptr;
    }
}

int sw::LayoutHost::GetChildLayoutCount() const
{
    return _associatedObj->GetChildLayoutCount();
}

sw::ILayout &sw::LayoutHost::GetChildLayoutAt(int index) const
{
    return _associatedObj->GetChildLayoutAt(index);
}
