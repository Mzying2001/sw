#include "SysLink.h"
#include "Utils.h"
#include <climits>

#if !defined(LM_GETIDEALSIZE)
#define LM_GETIDEALSIZE (LM_GETIDEALHEIGHT)
#endif

sw::SysLink::SysLink()
    : IgnoreReturn(
          Property<bool>::Init(this)
              .Getter([](SysLink *self) -> bool {
                  return self->GetStyle(LWS_IGNORERETURN);
              })
              .Setter([](SysLink *self, bool value) {
                  self->SetStyle(LWS_IGNORERETURN, value);
              })),

      AutoSize(
          Property<bool>::Init(this)
              .Getter([](SysLink *self) -> bool {
                  return self->_autoSize;
              })
              .Setter([](SysLink *self, bool value) {
                  if (self->_autoSize != value) {
                      self->_autoSize = value;
                      self->_UpdateLayoutFlags();
                      self->InvalidateMeasure();
                  }
              }))
{
    this->InitControl(WC_LINK, L"<a>SysLink</a>", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_TABSTOP, 0);
    this->_UpdateTextSize();
    this->_ResizeToTextSize();
    this->_UpdateLayoutFlags();
    this->Transparent      = true;
    this->InheritTextColor = true;
}

void sw::SysLink::OnTextChanged()
{
    this->_UpdateTextSize();
    this->Control::OnTextChanged();
}

void sw::SysLink::FontChanged(HFONT hfont)
{
    this->_UpdateTextSize();
    this->Control::FontChanged(hfont);
}

sw::Size sw::SysLink::MeasureOverride(const Size &availableSize)
{
    if (!this->_autoSize) {
        return this->UIElement::MeasureOverride(availableSize);
    }

    sw::Size desireSize = this->_textSize;

    if (availableSize.width < desireSize.width) {
        SIZE size;
        this->SendMessageW(LM_GETIDEALSIZE, Utils::Max(0, Dip::DipToPxX(availableSize.width)), reinterpret_cast<LPARAM>(&size));
        desireSize.width  = availableSize.width;
        desireSize.height = Dip::PxToDipY(size.cy);
    }
    return desireSize;
}

bool sw::SysLink::OnNotified(NMHDR *pNMHDR, LRESULT &result)
{
    switch (pNMHDR->code) {
        case NM_RETURN: {
            if (this->IgnoreReturn) {
                break;
            }
        }
        case NM_CLICK: {
            this->OnClicked(reinterpret_cast<NMLINK *>(pNMHDR));
            break;
        }
    }
    return this->Control::OnNotified(pNMHDR, result);
}

void sw::SysLink::OnClicked(NMLINK *pNMLink)
{
    SysLinkClickedEventArgs arg{pNMLink->item.szID, pNMLink->item.szUrl};
    this->RaiseRoutedEvent(arg);
}

void sw::SysLink::_UpdateTextSize()
{
    SIZE size;
    this->SendMessageW(LM_GETIDEALSIZE, INT_MAX, reinterpret_cast<LPARAM>(&size));
    this->_textSize = size;
}

void sw::SysLink::_ResizeToTextSize()
{
    this->Resize(this->_textSize);
}

void sw::SysLink::_UpdateLayoutFlags()
{
    constexpr auto flags =
        sw::LayoutUpdateCondition::TextChanged |
        sw::LayoutUpdateCondition::FontChanged;

    if (this->_autoSize) {
        this->LayoutUpdateCondition |= flags;
    } else {
        this->LayoutUpdateCondition &= ~flags;
    }
}
