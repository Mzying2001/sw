#include "Timer.h"

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
    this->InitControl(L"STATIC", L"", WS_POPUP, 0);
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

void sw::Timer::_TimerProc(HWND hwnd, UINT msg, UINT_PTR idTimer, DWORD time)
{
    if (msg == WM_TIMER) {
        auto timer = dynamic_cast<Timer *>(WndBase::GetWndBase(hwnd));
        if (timer) timer->OnTick();
    }
}
