#pragma once

#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace pxl
{
    enum class LogLevel
    {
        Info,
        Warn,
        Error,
        Critical,
    };

    enum class LogArea
    {
        None,
        Core,
        Window,
        Input,
        Renderer,
        OpenGL,
        Vulkan,
        FileSystem,
        Audio,
        Physics,
        Assert,
        Other,
    };

    class Logger
    {
    public:
        static void Init();

        static std::unique_ptr<spdlog::logger>& GetFrameworkLogger() { return s_FrameworkLogger; }
        static std::unique_ptr<spdlog::logger>& GetApplicationLogger() { return s_ApplicationLogger; }

        static void SetCurrentArea(LogArea area) { s_CurrentArea = area; }
    private:
        static std::string_view LogAreaToString(LogArea area);
    private:
        class LogAreaFlag : public spdlog::custom_flag_formatter
        {
        public:
            void format(const spdlog::details::log_msg&, const std::tm&, spdlog::memory_buf_t& dest) override
            {
                auto area = LogAreaToString(s_CurrentArea);
                dest.append(area.data(), area.data() + area.size());
            }

            std::unique_ptr<custom_flag_formatter> clone() const override
            {
                return spdlog::details::make_unique<LogAreaFlag>();
            }
        };
    private:
        static inline std::unique_ptr<spdlog::logger> s_FrameworkLogger = nullptr; // NOTE: uses special flag for log areas
        static inline std::unique_ptr<spdlog::logger> s_ApplicationLogger = nullptr;

        static inline LogArea s_CurrentArea = LogArea::None;
    };
}