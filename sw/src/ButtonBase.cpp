#include "ButtonBase.h"

sw::ButtonBase::ButtonBase()
    : AutoSize(
          // get
          [this]() -> bool {
              return _autoSize;
          },
          // set
          [this](const bool &value) {
              if (_autoSize != value) {
                  _autoSize = value;
                  _UpdateLayoutFlags();
                  InvalidateMeasure();
              }
          }),

      MultiLine(
          // get
          [this]() -> bool {
              return GetStyle(BS_MULTILINE);
          },
          // set
          [this](const bool &value) {
              SetStyle(BS_MULTILINE, value);
              if (_autoSize) InvalidateMeasure();
          })
{
    TabStop = true;
}

sw::ButtonBase::~ButtonBase()
{
}

void sw::ButtonBase::InitButtonBase(LPCWSTR lpWindowName, DWORD dwStyle, DWORD dwExStyle)
{
    InitControl(L"BUTTON", lpWindowName, dwStyle, dwExStyle);
    Transparent      = true;
    InheritTextColor = true;
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

    if (availableSize.width < desireSize.width) {
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
