#pragma once

#include <time.h>

using namespace std::literals;

namespace pxl
{
    class SleepTimer
    {
    public:
        void Sleep(uint64_t nsDuration)
        {
            timespec time = {};
            time.tv_nsec = static_cast<long>(nsDuration);
            nanosleep(&time, NULL);
        }

        std::chrono::nanoseconds GetAccuracy() const { return 100us; }
    };
}