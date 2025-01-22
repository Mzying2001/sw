#include "Timer.h"

/**
 * @brief 窗口句柄保存Timer指针的属性名称
 */
static constexpr wchar_t _TimerPtrProp[] = L"SWPROP_TimerPtr";

/**
 */

sw::Timer::Timer()
    : Interval(
          // get
          [this]() -> uint32_t {
              return this->_interval;
          },
          // set
          [this](const uint32_t &value) {
              this->_interval = value;
              if (this->_started) {
                  this->Stop();
                  this->Start();
              }
          })
{
    this->InitControl(L"STATIC", L"", WS_CHILD, 0);
    Timer::_SetTimerPtr(this->Handle, *this);
}

void sw::Timer::Start()
{
    if (!this->_started) {
        this->_started = true;
        SetTimer(this->Handle, 0, this->_interval, &Timer::_TimerProc);
    }
}

void sw::Timer::Stop()
{
    if (this->_started) {
        this->_started = false;
        KillTimer(this->Handle, 0);
    }
}

void sw::Timer::SetTickHandler(const TimerTickHandler &handler)
{
    this->_handler = handler;
}

void sw::Timer::OnTick()
{
    if (this->_handler)
        this->_handler(*this);
}

sw::Timer *sw::Timer::_GetTimerPtr(HWND hwnd)
{
    return reinterpret_cast<Timer *>(GetPropW(hwnd, _TimerPtrProp));
}

void sw::Timer::_SetTimerPtr(HWND hwnd, Timer &timer)
{
    SetPropW(hwnd, _TimerPtrProp, reinterpret_cast<HANDLE>(&timer));
}

void sw::Timer::_TimerProc(HWND hwnd, UINT msg, UINT_PTR idTimer, DWORD time)
{
    if (msg == WM_TIMER) {
        auto timer = Timer::_GetTimerPtr(hwnd);
        if (timer) timer->OnTick();
    }
}
