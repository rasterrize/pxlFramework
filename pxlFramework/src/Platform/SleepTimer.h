#pragma once

namespace pxl
{
    class SleepTimer
    {
    public:
        virtual ~SleepTimer() = default;

        virtual void Sleep(uint64_t nsDuration) = 0;
    };
}