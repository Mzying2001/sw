#include "Timer.h"

namespace
{
    /**
     * @brief 窗口句柄保存Timer指针的属性名称
     */
    constexpr wchar_t _TimerPtrProp[] = L"SWPROP_TimerPtr";
}

sw::Timer::Timer()
    : Interval(
          Property<uint32_t>::Init(this)
              .Getter([](Timer *self) -> uint32_t {
                  return self->_interval;
              })
              .Setter([](Timer *self, uint32_t value) {
                  self->_interval = value;
                  if (self->_started) {
                      self->Stop();
                      self->Start();
                  }
              }))
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

void sw::Timer::OnTick()
{
    if (this->Tick) {
        this->Tick(*this);
    }
}

sw::Timer *sw::Timer::_GetTimerPtr(HWND hwnd)
{
    // clang-format off
    static struct _InternalRaiiAtomHelper {
        ATOM value;
        _InternalRaiiAtomHelper() : value(GlobalAddAtomW(_TimerPtrProp)) {}
        ~_InternalRaiiAtomHelper() { GlobalDeleteAtom(value); }
    } _atom;
    // clang-format on

    auto ptr = GetProp(hwnd, MAKEINTATOM(_atom.value));
    return reinterpret_cast<Timer *>(ptr);
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
