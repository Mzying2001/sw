#include "ButtonBase.h"
#include <cmath>

sw::ButtonBase::ButtonBase()
    : AutoSize(
          Property<bool>::Init(this)
              .Getter([](ButtonBase *self) -> bool {
                  return self->_autoSize;
              })
              .Setter([](ButtonBase *self, bool value) {
                  if (self->_autoSize != value) {
                      self->_autoSize = value;
                      self->_UpdateLayoutFlags();
                      self->RaisePropertyChanged(&ButtonBase::AutoSize);
                      self->InvalidateMeasure();
                  }
              })),

      MultiLine(
          Property<bool>::Init(this)
              .Getter([](ButtonBase *self) -> bool {
                  return self->GetStyle(BS_MULTILINE);
              })
              .Setter([](ButtonBase *self, bool value) {
                  if (self->MultiLine != value) {
                      self->SetStyle(BS_MULTILINE, value);
                      self->RaisePropertyChanged(&ButtonBase::MultiLine);
                      if (self->_autoSize) {
                          self->InvalidateMeasure();
                      }
                  }
              })),

      TextMargin(
          Property<Thickness>::Init(this)
              .Getter([](ButtonBase *self) -> Thickness {
                  RECT rect{};
                  self->_GetTextMargin(rect);
                  return rect;
              })
              .Setter([](ButtonBase *self, const Thickness &value) {
                  if (self->TextMargin != value) {
                      RECT rect = value;
                      self->_SetTextMargin(rect);
                      self->RaisePropertyChanged(&ButtonBase::TextMargin);
                      if (self->_autoSize) {
                          self->InvalidateMeasure();
                      }
                  }
              }))
{
    TabStop          = true;
    Transparent      = true;
    InheritTextColor = true;
}

sw::ButtonBase::~ButtonBase()
{
}

void sw::ButtonBase::InitButtonBase(LPCWSTR lpWindowName, DWORD dwStyle, DWORD dwExStyle)
{
    InitControl(L"BUTTON", lpWindowName, dwStyle, dwExStyle);
}

void sw::ButtonBase::OnClicked()
{
    RaiseRoutedEvent(ButtonBase_Clicked);
}

void sw::ButtonBase::OnDoubleClicked()
{
    RaiseRoutedEvent(ButtonBase_DoubleClicked);
}

void sw::ButtonBase::OnCommand(int code)
{
    switch (code) {
        case BN_CLICKED:
            OnClicked();
            break;

        case BN_DOUBLECLICKED:
            OnDoubleClicked();
            break;

        default:
            break;
    }
}

sw::Size sw::ButtonBase::MeasureOverride(const Size &availableSize)
{
    if (!_autoSize) {
        return UIElement::MeasureOverride(availableSize);
    }

    SIZE szIdeal = {0, 0};

    if (!_GetIdealSize(szIdeal)) {
        return UIElement::MeasureOverride(availableSize);
    }

    Size desireSize = szIdeal;

    if (szIdeal.cx == 0 && std::isinf(availableSize.width)) {
        desireSize = UIElement::MeasureOverride(availableSize);
        szIdeal.cx = Dip::DipToPxX(desireSize.width);
        szIdeal.cy = 0;
        _GetIdealSize(szIdeal);
        desireSize.height = Dip::PxToDipY(szIdeal.cy);
    } else if (availableSize.width < desireSize.width ||
               (szIdeal.cx == 0 && !std::isinf(availableSize.width))) {
        szIdeal.cx = Dip::DipToPxX(availableSize.width);
        szIdeal.cy = 0;
        _GetIdealSize(szIdeal);
        desireSize.width  = availableSize.width;
        desireSize.height = Dip::PxToDipY(szIdeal.cy);
    }
    return desireSize;
}

void sw::ButtonBase::_UpdateLayoutFlags()
{
    constexpr auto flags =
        sw::LayoutUpdateCondition::TextChanged |
        sw::LayoutUpdateCondition::FontChanged;

    if (_autoSize) {
        LayoutUpdateCondition |= flags;
    } else {
        LayoutUpdateCondition &= ~flags;
    }
}

bool sw::ButtonBase::_GetIdealSize(SIZE &size)
{
    return SendMessageW(BCM_GETIDEALSIZE, 0, reinterpret_cast<LPARAM>(&size)) == TRUE;
}

bool sw::ButtonBase::_GetTextMargin(RECT &rect)
{
    return SendMessageW(BCM_GETTEXTMARGIN, 0, reinterpret_cast<LPARAM>(&rect)) == TRUE;
}

bool sw::ButtonBase::_SetTextMargin(RECT &rect)
{
    return SendMessageW(BCM_SETTEXTMARGIN, 0, reinterpret_cast<LPARAM>(&rect)) == TRUE;
}
