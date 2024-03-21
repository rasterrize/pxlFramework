#include "Logger.h"

namespace pxl
{
    std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> Logger::s_FrameworkSink;
    std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> Logger::s_ApplicationSink;
    std::shared_ptr<spdlog::logger> Logger::s_FrameworkLogger; // uses special flag for system areas
    std::shared_ptr<spdlog::logger> Logger::s_ApplicationLogger;

    LogArea Logger::s_CurrentArea = LogArea::None;

    void Logger::Init()
    {
        s_FrameworkSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        s_ApplicationSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        s_FrameworkLogger = std::make_shared<spdlog::logger>("FRAMEWORK", s_FrameworkSink);
        s_ApplicationLogger = std::make_shared<spdlog::logger>("APPLICATION", s_ApplicationSink);
        s_ApplicationLogger->set_pattern("[%I:%M:%S] [%n] [%^%l%$] %v");

        // Set framework logger/sink formatter (custom flag)
        auto formatter = std::make_unique<spdlog::pattern_formatter>();
        formatter->add_flag<SystemAreaFlag>('*');
        formatter->set_pattern("[%I:%M:%S] [%n] [%*] [%^%l%$] %v");
        s_FrameworkLogger->set_formatter(std::move(formatter));
    }

    // LEGACY
    void Logger::Log(LogLevel level, const std::string& text)
    {   
        if (s_FrameworkLogger == nullptr)
            return;

        s_CurrentArea = LogArea::None;
        
        switch(level)
        {
            case LogLevel::Info:
                s_FrameworkLogger->info(text);
                break;
            case LogLevel::Warn:
                s_FrameworkLogger->warn(text);
                break;
            case LogLevel::Error:
                s_FrameworkLogger->error(text);
                break;
            case LogLevel::Critical:
                s_FrameworkLogger->critical(text);
                break;
        }
    }

    // LEGACY
    void Logger::LogInfo(const std::string& text)
    {
        Logger::Log(LogLevel::Info, text);
    }

    // LEGACY
    void Logger::LogWarn(const std::string& text)
    {
        Logger::Log(LogLevel::Warn, text);
    }

    // LEGACY
    void Logger::LogError(const std::string& text)
    {
        Logger::Log(LogLevel::Error, text);
    }

    // LEGACY
    void Logger::LogCritical(const std::string& text)
    {
        Logger::Log(LogLevel::Critical, text);
    }

    std::string Logger::LogAreaToString(LogArea area)
    {
        switch (area)
        {
            case LogArea::None:
                return "None";
            case LogArea::Core:
                // return "\033[1;97mCore\033[0m";
                return "Core";
            case LogArea::Window:
                // return "\033[1;96mWindow\033[0m";
                return "Window";
            case LogArea::Input:
                // return "\033[1;95mInput\033[0m";
                return "Input";
            case LogArea::Renderer:
                // return "\033[1;93mRenderer\033[0m";
                return "Renderer";
            case LogArea::OpenGL:
                // return "\033[1;94mOpenGL\033[0m";
                return "OpenGL";
            case LogArea::Vulkan:
                // return "\033[1;31mVulkan\033[0m";
                return "Vulkan";
            case LogArea::Audio:
                // return "\033[1;92mAudio\033[0m";
                return "Audio";
            case LogArea::Physics:
                // return "\033[1;35mPhysics\033[0m";
                return "Physics";
            case LogArea::Other:
                return "Other";
        }

        return std::string();
    }
}