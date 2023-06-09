#include "Logger.h"

namespace pxl
{
    bool Logger::s_Enabled = false;

    std::shared_ptr<spdlog::logger> Logger::s_Logger;
    std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> Logger::s_Sink;

    void Logger::Init()
    {
        s_Sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        s_Logger = std::make_shared<spdlog::logger>("Logger", s_Sink);

        s_Enabled = true;
    }

    void Logger::Log(LogLevel level, const std::string& text)
    {
        if (!s_Enabled)
            return;
        
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

    void Logger::LogInfo(const std::string& text)
    {
        Logger::Log(LogLevel::Info, text);
    }

    void Logger::LogWarn(const std::string& text)
    {
        Logger::Log(LogLevel::Warn, text);
    }

    void Logger::LogError(const std::string& text)
    {
        Logger::Log(LogLevel::Error, text);
    }

    void Logger::LogCritical(const std::string& text)
    {
        Logger::Log(LogLevel::Critical, text);
    }
}