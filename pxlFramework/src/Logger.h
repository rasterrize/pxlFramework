#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace pxl
{
    enum class LogLevel
    {
        Info, Warn, Error, Critical
    };

    class Logger
    {
    public:
        static void Init();
        static void Log(LogLevel level, std::string text);
    private:
        static std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> s_Sink;
        static std::shared_ptr<spdlog::logger> s_Logger;
    };
}