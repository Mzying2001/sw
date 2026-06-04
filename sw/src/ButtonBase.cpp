#include "ButtonBase.h"
#include "Dip.h"
#include <cmath>
#include <limits>

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
                  return Thickness(rect);
              })
              .Setter([](ButtonBase *self, const Thickness &value) {
                  if (self->TextMargin != value) {
                      RECT rect = static_cast<RECT>(value);
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
        return TBase::MeasureOverride(availableSize);
    }

    SIZE szIdeal = {0, 0};

    if (!_GetIdealSize(szIdeal)) {
        return TBase::MeasureOverride(availableSize);
    }

    Size desireSize = szIdeal;

    constexpr auto unlimitedWidth =
        (std::numeric_limits<decltype(szIdeal.cx)>::max)();

    if (szIdeal.cx == 0 && std::isinf(availableSize.width)) {
        szIdeal.cx = unlimitedWidth;
        szIdeal.cy = 0;
        if (_GetIdealSize(szIdeal)) {
            desireSize = szIdeal;
        } else {
            desireSize = TBase::MeasureOverride(availableSize);
        }
    } else if (availableSize.width < desireSize.width ||
               (szIdeal.cx == 0 && !std::isinf(availableSize.width))) {
        szIdeal.cx = Dip::DipToPxX(availableSize.width);
        szIdeal.cy = 0;
        if (_GetIdealSize(szIdeal)) {
            desireSize = szIdeal;
        } else {
            desireSize = TBase::MeasureOverride(availableSize);
        }
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
