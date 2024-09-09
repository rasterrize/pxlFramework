#include "Logger.h"

namespace pxl
{
    void Logger::Init()
    {
        // Must be shared_ptr, but it's okay since spdlog's example uses it this way
        auto frameworkSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        auto applicationSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

        s_FrameworkLogger = std::make_unique<spdlog::logger>("FRAMEWORK", frameworkSink);
        PXL_ASSERT(s_FrameworkLogger);

        s_ApplicationLogger = std::make_unique<spdlog::logger>("APPLICATION", applicationSink);
        PXL_ASSERT(s_ApplicationLogger);

        s_ApplicationLogger->set_pattern("[%I:%M:%S] [%n] [%^%l%$] %v");

        // Set framework logger/sink formatter (custom flag)
        auto formatter = std::make_unique<spdlog::pattern_formatter>();
        formatter->add_flag<LogAreaFlag>('*');
        formatter->set_pattern("[%I:%M:%S] [%n] [%*] [%^%l%$] %v");
        s_FrameworkLogger->set_formatter(std::move(formatter));
    }

    std::string_view Logger::LogAreaToString(LogArea area)
    {
        switch (area)
        {
            case LogArea::None:       return "None";
            case LogArea::Core:       return "Core";
            case LogArea::Window:     return "Window";
            case LogArea::Input:      return "Input";
            case LogArea::Renderer:   return "Renderer";
            case LogArea::OpenGL:     return "OpenGL";
            case LogArea::Vulkan:     return "Vulkan";
            case LogArea::FileSystem: return "FileSystem";
            case LogArea::Audio:      return "Audio";
            case LogArea::Physics:    return "Physics";
            case LogArea::Assert:     return "Assert";
            case LogArea::Other:      return "Other";
        }

        return std::string_view();
    }
}