#pragma once

// A current side affect of these macros is they take up 2 lines, causing issues with one line if statemtents
#ifdef PXL_ENABLE_LOGGING
    #include "Logger.h"

    #define PXL_LOG_TRACE(...)    Logger::GetFrameworkLogger()->trace(__VA_ARGS__)
    #define PXL_LOG_INFO(...)     Logger::GetFrameworkLogger()->info(__VA_ARGS__)
    #define PXL_LOG_WARN(...)     Logger::GetFrameworkLogger()->warn(__VA_ARGS__)
    #define PXL_LOG_ERROR(...)    Logger::GetFrameworkLogger()->error(__VA_ARGS__)
    #define PXL_LOG_CRITICAL(...) Logger::GetFrameworkLogger()->critical(__VA_ARGS__)
    #define PXL_LOG_DEBUG(...)    Logger::GetFrameworkLogger()->debug(__VA_ARGS__)

    #define PXL_INIT_LOGGING pxl::Logger::Init()
#else
    #define PXL_LOG_TRACE(...)
    #define PXL_LOG_INFO(...)
    #define PXL_LOG_WARN(...)
    #define PXL_LOG_ERROR(...)
    #define PXL_LOG_CRITICAL(...)
    #define PXL_LOG_DEBUG(...)

    #define PXL_INIT_LOGGING
#endif