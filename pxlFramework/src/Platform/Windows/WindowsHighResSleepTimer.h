#pragma once

#include <Windows.h>
#include <timeapi.h>

using namespace std::literals;

namespace pxl
{
    class SleepTimer
    {
    public:
        SleepTimer()
        {
            m_Timer = CreateWaitableTimerEx(nullptr, nullptr, CREATE_WAITABLE_TIMER_MANUAL_RESET | CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, TIMER_ALL_ACCESS);
        }

        void Sleep(uint64_t nsDuration)
        {
            // A negative integer here means the time will be relative, which is we what we want
            // We must also divide the duration since Win32 expects it to be in 100ns intervals
            LARGE_INTEGER dueTime;
            dueTime.QuadPart = -static_cast<int64_t>(nsDuration) / 100;
            SetWaitableTimer(m_Timer, &dueTime, 0, nullptr, nullptr, false);
            WaitForSingleObject(m_Timer, INFINITE);
        }

        std::chrono::nanoseconds GetAccuracy() const { return 1ms; }

    private:
        HANDLE m_Timer = nullptr;
    };
}