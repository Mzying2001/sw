#include "DateTimePicker.h"

sw::DateTimePicker::DateTimePicker()
    : ShowUpDownButton(
          // get
          [&]() -> const bool & {
              static bool result;
              result = this->GetStyle(DTS_UPDOWN);
              return result;
          },
          // set
          [&](const bool &value) {
              if (this->ShowUpDownButton != value) {
                  this->_UpdateStyle(
                      value ? (this->GetStyle() | DTS_UPDOWN)
                            : (this->GetStyle() & ~DTS_UPDOWN));
              }
          }),

      Format(
          // get
          [&]() -> const DateTimePickerFormat & {
              return this->_format;
          },
          // set
          [&](const DateTimePickerFormat &value) {
              if (this->_format == value) {
                  return;
              }
              DWORD style = this->GetStyle();
              style &= ~(DTS_SHORTDATEFORMAT | DTS_LONGDATEFORMAT);
              switch (value) {
                  case DateTimePickerFormat::Short:
                  case DateTimePickerFormat::Custom:
                      style |= DTS_SHORTDATEFORMAT;
                      break;
                  case DateTimePickerFormat::Long:
                      style |= DTS_LONGDATEFORMAT;
                      break;
              }
              this->_format = value;
              this->_UpdateStyle(style);
          }),

      CustomFormat(
          // get
          [&]() -> const std::wstring & {
              return this->_customFormat;
          },
          // set
          [&](const std::wstring &value) {
              this->_format       = DateTimePickerFormat::Custom;
              this->_customFormat = value;
              this->_SetFormat(this->_customFormat);
          })
{
    this->InitControl(DATETIMEPICK_CLASSW, L"", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | DTS_SHORTDATEFORMAT, 0);
    this->Rect    = sw::Rect{0, 0, 100, 24};
    this->TabStop = true;
}

bool sw::DateTimePicker::GetTime(SYSTEMTIME &out)
{
    return this->SendMessageW(DTM_GETSYSTEMTIME, 0, reinterpret_cast<LPARAM>(&out)) == GDT_VALID;
}

bool sw::DateTimePicker::SetTime(const SYSTEMTIME &time)
{
    bool result = this->SendMessageW(DTM_SETSYSTEMTIME, GDT_VALID, reinterpret_cast<LPARAM>(&time));
    if (result) {
        SYSTEMTIME time{};
        this->GetTime(time);
        DateTimePickerTimeChangedEventArgs arg{time};
        this->RaiseRoutedEvent(arg);
    }
    return result;
}

bool sw::DateTimePicker::SetRange(const SYSTEMTIME &minTime, const SYSTEMTIME &maxTime)
{
    SYSTEMTIME minmax[] = {minTime, maxTime};
    return this->SendMessageW(DTM_SETRANGE, GDTR_MIN | GDTR_MAX, reinterpret_cast<LPARAM>(minmax));
}

void sw::DateTimePicker::OnNotified(NMHDR *pNMHDR)
{
    if (pNMHDR->code == DTN_DATETIMECHANGE) {
        this->OnTimeChanged(reinterpret_cast<NMDATETIMECHANGE *>(pNMHDR));
    }
}

void sw::DateTimePicker::OnTimeChanged(NMDATETIMECHANGE *pInfo)
{
    DateTimePickerTimeChangedEventArgs arg{pInfo->st};
    this->RaiseRoutedEvent(arg);
}

void sw::DateTimePicker::_SetFormat(const std::wstring &value)
{
    LPCWSTR fmt = value.empty() ? nullptr : value.c_str();
    this->SendMessageW(DTM_SETFORMATW, 0, reinterpret_cast<LPARAM>(fmt));
}

void sw::DateTimePicker::_UpdateStyle(DWORD style)
{
    SYSTEMTIME time;
    this->GetTime(time);

    SYSTEMTIME range[2];
    DWORD flag = (DWORD)this->SendMessageW(DTM_GETRANGE, 0, reinterpret_cast<LPARAM>(range));

    this->ResetHandle(style, this->GetExtendedStyle());

    this->SendMessageW(DTM_SETSYSTEMTIME, GDT_VALID, reinterpret_cast<LPARAM>(&time));
    this->SendMessageW(DTM_SETRANGE, flag, reinterpret_cast<LPARAM>(range));

    if (this->_format == DateTimePickerFormat::Custom) {
        this->_SetFormat(this->_customFormat);
    }
}
