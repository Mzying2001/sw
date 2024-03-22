#include "MonthCalendar.h"

sw::MonthCalendar::MonthCalendar()
    : ShowToday(
          // get
          [&]() -> const bool & {
              static bool result;
              result = !this->GetStyle(MCS_NOTODAY);
              return result;
          },
          // set
          [&](const bool &value) {
              this->SetStyle(MCS_NOTODAY, !value);
          })
{
    this->InitControl(MONTHCAL_CLASSW, L"", WS_CHILD | WS_VISIBLE, 0);
    this->Rect    = {0, 0, 250, 200};
    this->TabStop = true;
}

bool sw::MonthCalendar::GetToday(SYSTEMTIME &out)
{
    return this->SendMessageW(MCM_GETTODAY, 0, reinterpret_cast<LPARAM>(&out));
}

bool sw::MonthCalendar::SetToday(const SYSTEMTIME &today)
{
    return this->SendMessageW(MCM_SETTODAY, 0, reinterpret_cast<LPARAM>(&today));
}

bool sw::MonthCalendar::GetTime(SYSTEMTIME &out)
{
    return this->SendMessageW(MCM_GETCURSEL, 0, reinterpret_cast<LPARAM>(&out));
}

bool sw::MonthCalendar::SetTime(const SYSTEMTIME &time)
{
    return this->SendMessageW(MCM_SETCURSEL, 0, reinterpret_cast<LPARAM>(&time));
}

bool sw::MonthCalendar::SetRange(const SYSTEMTIME &minTime, const SYSTEMTIME &maxTime)
{
    SYSTEMTIME range[2] = {minTime, maxTime};
    return this->SendMessageW(MCM_SETRANGE, GDTR_MIN | GDTR_MAX, reinterpret_cast<LPARAM>(range));
}

void sw::MonthCalendar::OnDrawFocusRect()
{
    // 不绘制虚线框
}

void sw::MonthCalendar::SetBackColor(Color color, bool redraw)
{
    this->Control::SetBackColor(color, false);
    this->SendMessageW(MCM_SETCOLOR, MCSC_BACKGROUND, (COLORREF)color);
}

void sw::MonthCalendar::SetTextColor(Color color, bool redraw)
{
    this->Control::SetTextColor(color, false);
    this->SendMessageW(MCM_SETCOLOR, MCSC_TEXT, (COLORREF)color);
}
