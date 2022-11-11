#include "Logger.h"

namespace pxl
{
    std::shared_ptr<spdlog::logger> Logger::s_Logger;
    std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> Logger::s_Sink;

    void Logger::Init()
    {
        s_Sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        s_Logger = std::make_shared<spdlog::logger>("Logger", s_Sink);
    }

    void Logger::Log(LogLevel level, std::string text)
    {
        switch(level)
        {
            case LogLevel::Info:
                s_Logger->info(text);
            break;
            case LogLevel::Warn:
                s_Logger->warn(text);
            break;
            case LogLevel::Error:
                s_Logger->error(text);
            break;
            case LogLevel::Critical:
                s_Logger->critical(text);
            break;

        }
    }

    void Logger::LogInfo(std::string text)
    {
        s_Logger->info(text);
    }

    void Logger::LogWarn(std::string text)
    {
        s_Logger->warn(text);
    }

    void Logger::LogError(std::string text)
    {
        s_Logger->error(text);
    }

    void Logger::LogCritical(std::string text)
    {
        s_Logger->critical(text);
    }
}