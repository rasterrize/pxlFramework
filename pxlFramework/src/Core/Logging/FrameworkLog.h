#pragma once

// A current side affect of these macros is they take up 2 lines, causing issues with one line if statemtents
#ifdef PXL_ENABLE_LOGGING
    #include "Logger.h"

    #define PXL_LOG_INFO(area, ...)     { pxl::Logger::SetCurrentArea(area); pxl::Logger::GetFrameworkLogger()->info(__VA_ARGS__); }
    #define PXL_LOG_WARN(area, ...)     { pxl::Logger::SetCurrentArea(area); pxl::Logger::GetFrameworkLogger()->warn(__VA_ARGS__); }
    #define PXL_LOG_ERROR(area, ...)    { pxl::Logger::SetCurrentArea(area); pxl::Logger::GetFrameworkLogger()->error(__VA_ARGS__); }
    #define PXL_LOG_CRITICAL(area, ...) { pxl::Logger::SetCurrentArea(area); pxl::Logger::GetFrameworkLogger()->critical(__VA_ARGS__); }

    #define PXL_INIT_LOGGING pxl::Logger::Init()
#else
    #define PXL_LOG_INFO(area, ...)     {}
    #define PXL_LOG_WARN(area, ...)     {}
    #define PXL_LOG_ERROR(area, ...)    {}
    #define PXL_LOG_CRITICAL(area, ...) {}
#endif