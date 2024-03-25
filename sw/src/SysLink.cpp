#include "SysLink.h"
#include "Utils.h"
#include <climits>

#if !defined(LM_GETIDEALSIZE)
#define LM_GETIDEALSIZE (LM_GETIDEALHEIGHT)
#endif

sw::SysLink::SysLink()
    : IgnoreReturn(
          // get
          [&]() -> const bool & {
              static bool result;
              result = this->GetStyle(LWS_IGNORERETURN);
              return result;
          },
          // set
          [&](const bool &value) {
              this->SetStyle(LWS_IGNORERETURN, value);
          }),

      AutoSize(
          // get
          [&]() -> const bool & {
              return this->_autoSize;
          },
          // set
          [&](const bool &value) {
              this->_autoSize = value;
              if (value) {
                  this->NotifyLayoutUpdated();
              }
          })
{
    this->InitControl(WC_LINK, L"<a>SysLink</a>", WS_CHILD | WS_VISIBLE | WS_TABSTOP, 0);
    this->_UpdateTextSize();
    this->_ResizeToTextSize();
    this->Transparent      = true;
    this->InheritTextColor = true;
}

void sw::SysLink::OnTextChanged()
{
    this->Control::OnTextChanged();
    this->_UpdateTextSize();
    if (this->_autoSize) {
        this->NotifyLayoutUpdated();
    }
}

void sw::SysLink::FontChanged(HFONT hfont)
{
    this->_UpdateTextSize();
    if (this->_autoSize) {
        this->NotifyLayoutUpdated();
    }
}

void sw::SysLink::Measure(const Size &availableSize)
{
    if (!this->_autoSize) {
        this->Control::Measure(availableSize);
        return;
    }

    sw::Thickness margin = this->Margin;

    sw::Size desireSize{
        this->_textSize.width + margin.left + margin.right,
        this->_textSize.height + margin.top + margin.bottom};

    if (availableSize.width < desireSize.width) {
        SIZE size;
        this->SendMessageW(LM_GETIDEALSIZE,
                           Utils::Max(0, Dip::DipToPxX(availableSize.width - margin.left - margin.right)),
                           reinterpret_cast<LPARAM>(&size));
        desireSize.width  = availableSize.width;
        desireSize.height = Dip::PxToDipY(size.cy) + margin.top + margin.bottom;
    }

    this->SetDesireSize(desireSize);
}

void sw::SysLink::OnNotified(NMHDR *pNMHDR)
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
    sw::Rect rect = this->Rect;
    rect.width    = this->_textSize.width;
    rect.height   = this->_textSize.height;
    this->Rect    = rect;
}
