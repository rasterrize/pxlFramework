#pragma once

#include <spdlog/logger.h>

namespace pxl
{
    class Logger
    {
    public:
        static void Init();

        static std::unique_ptr<spdlog::logger>& GetFrameworkLogger() { return s_FrameworkLogger; }
        static std::unique_ptr<spdlog::logger>& GetApplicationLogger() { return s_ApplicationLogger; }

    private:
        static inline std::unique_ptr<spdlog::logger> s_FrameworkLogger;
        static inline std::unique_ptr<spdlog::logger> s_ApplicationLogger;
    };
}