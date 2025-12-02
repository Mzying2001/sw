#include "DateTimePicker.h"

sw::DateTimePicker::DateTimePicker()
    : ShowUpDownButton(
          Property<bool>::Init(this)
              .Getter([](DateTimePicker *self) -> bool {
                  return self->GetStyle(DTS_UPDOWN);
              })
              .Setter([](DateTimePicker *self, bool value) {
                  if (self->ShowUpDownButton != value) {
                      self->_UpdateStyle(
                          value ? (self->GetStyle() | DTS_UPDOWN)
                                : (self->GetStyle() & ~DTS_UPDOWN));
                  }
              })),

      Format(
          Property<DateTimePickerFormat>::Init(this)
              .Getter([](DateTimePicker *self) -> DateTimePickerFormat {
                  return self->_format;
              })
              .Setter([](DateTimePicker *self, DateTimePickerFormat value) {
                  if (self->_format == value) {
                      return;
                  }
                  DWORD style = self->GetStyle();
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
                  self->_format = value;
                  self->_UpdateStyle(style);
              })),

      CustomFormat(
          Property<std::wstring>::Init(this)
              .Getter([](DateTimePicker *self) -> std::wstring {
                  return self->_customFormat;
              })
              .Setter([](DateTimePicker *self, const std::wstring &value) {
                  self->_format       = DateTimePickerFormat::Custom;
                  self->_customFormat = value;
                  self->_SetFormat(self->_customFormat);
              }))
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
    SYSTEMTIME range[2] = {minTime, maxTime};
    return this->SendMessageW(DTM_SETRANGE, GDTR_MIN | GDTR_MAX, reinterpret_cast<LPARAM>(range));
}

bool sw::DateTimePicker::OnNotified(NMHDR *pNMHDR, LRESULT &result)
{
    if (pNMHDR->code == DTN_DATETIMECHANGE) {
        this->OnTimeChanged(reinterpret_cast<NMDATETIMECHANGE *>(pNMHDR));
    }
    return this->Control::OnNotified(pNMHDR, result);
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
