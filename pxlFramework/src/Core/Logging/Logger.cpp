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
        formatter->add_flag<LogAreaFlag>('*');
        formatter->set_pattern("[%I:%M:%S] [%n] [%*] [%^%l%$] %v");
        s_FrameworkLogger->set_formatter(std::move(formatter));
    }

    std::string Logger::LogAreaToString(LogArea area)
    {
        switch (area)
        {
            case LogArea::None:     return "None";
            case LogArea::Core:     return "Core";
            case LogArea::Window:   return "Window";
            case LogArea::Input:    return "Input";
            case LogArea::Renderer: return "Renderer";
            case LogArea::OpenGL:   return "OpenGL";
            case LogArea::Vulkan:   return "Vulkan";
            case LogArea::Audio:    return "Audio";
            case LogArea::Physics:  return "Physics";
            case LogArea::Assert:   return "Assert";
            case LogArea::Other:    return "Other"; 
        }

        return std::string();
    }
}