#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/pattern_formatter.h>

namespace pxl
{
    enum class LogLevel
    {
        Info, Warn, Error, Critical
    };

    enum class LogArea
    {
        None, Core, Window, Input, Renderer, OpenGL, Vulkan, Audio, Physics, Other
    };

    class Logger
    {
    public:
        static void Init();

        static std::shared_ptr<spdlog::logger>& GetFrameworkLogger() { return s_FrameworkLogger; }
        static std::shared_ptr<spdlog::logger>& GetApplicationLogger() { return s_ApplicationLogger; }

        static void SetCurrentArea(LogArea area) { s_CurrentArea = area; }
    private:
        static std::string LogAreaToString(LogArea area);
    private:
        class SystemAreaFlag : public spdlog::custom_flag_formatter
        {
        public:
            void format(const spdlog::details::log_msg&, const std::tm&, spdlog::memory_buf_t& dest) override
            {
                std::string area = LogAreaToString(s_CurrentArea); // TODO: not use strings here if possible
                dest.append(area.data(), area.data() + area.size());
            }

            std::unique_ptr<custom_flag_formatter> clone() const override
            {
                return spdlog::details::make_unique<SystemAreaFlag>();
            }
        };
    private:
        static std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> s_FrameworkSink; // uses special flag for system areas
        static std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> s_ApplicationSink;
        static std::shared_ptr<spdlog::logger> s_FrameworkLogger;
        static std::shared_ptr<spdlog::logger> s_ApplicationLogger;

        static LogArea s_CurrentArea;
    };
}