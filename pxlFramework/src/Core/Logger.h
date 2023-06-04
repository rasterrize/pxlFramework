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

        // Confusing, will change
        static void LogInfo(std::string text);
        static void LogWarn(std::string text);
        static void LogError(std::string text);
        static void LogCritical(std::string text);
    private:
        static bool s_Enabled;

        static std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> s_Sink;
        static std::shared_ptr<spdlog::logger> s_Logger;
    };
}