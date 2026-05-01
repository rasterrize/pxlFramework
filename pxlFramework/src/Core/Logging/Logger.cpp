#include "Logger.h"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace pxl
{
    void Logger::Init()
    {
        auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        consoleSink->set_pattern("[%D %H:%M:%S.%e] [%n] %^[%l]%$ %v");
        consoleSink->set_level(spdlog::level::trace);

        std::vector<spdlog::sink_ptr> frameworkSinks = { consoleSink };
        std::vector<spdlog::sink_ptr> applicationSinks = { consoleSink };

        const std::string fileSinkPattern = "[%D %H:%M:%S.%e] [%l] %v";
        auto frameworkFileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/framework-log.txt", true);
        frameworkFileSink->set_pattern(fileSinkPattern);
        frameworkFileSink->set_level(spdlog::level::info);
        frameworkSinks.push_back(frameworkFileSink);

        auto applicationFileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/application-log.txt", true);
        applicationFileSink->set_pattern(fileSinkPattern);
        applicationSinks.push_back(applicationFileSink);

        s_FrameworkLogger = std::make_unique<spdlog::logger>("pxl", frameworkSinks.begin(), frameworkSinks.end());
        s_FrameworkLogger->set_level(spdlog::level::trace);
        s_ApplicationLogger = std::make_unique<spdlog::logger>("app", applicationSinks.begin(), applicationSinks.end());
    }
}