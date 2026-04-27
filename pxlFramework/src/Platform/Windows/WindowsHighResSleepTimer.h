#pragma once
#ifdef _WIN64

    #include <Windows.h>
    #include <timeapi.h>

    #include "Platform/SleepTimer.h"

namespace pxl::Platform::Windows
{
    class HighResSleepTimer : public SleepTimer
    {
    public:
        HighResSleepTimer()
        {
            m_Timer = CreateWaitableTimerEx(nullptr, nullptr, CREATE_WAITABLE_TIMER_MANUAL_RESET | CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, TIMER_ALL_ACCESS);
        }

        virtual void Sleep(uint64_t nsDuration) override
        {
            if (nsDuration <= 0)
                return;

            LARGE_INTEGER dueTime;
            // A negative integer here means the time will be relative, which is we what we want
            // We must also divide the duration since Win32 expects it to be in 100ns intervals
            dueTime.QuadPart = -static_cast<int64_t>(nsDuration) / 100;
            SetWaitableTimer(m_Timer, &dueTime, 0, nullptr, nullptr, false);
            WaitForSingleObject(m_Timer, INFINITE);
        }

    private:
        HANDLE m_Timer = nullptr;
    };
}

#endif