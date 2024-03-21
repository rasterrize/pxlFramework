#pragma once

#include "Logger.h"

#ifndef PXL_DISABLE_LOGGING
    #define APP_LOG_INFO(...)     pxl::Logger::GetApplicationLogger()->info(__VA_ARGS__)
    #define APP_LOG_WARN(...)     pxl::Logger::GetApplicationLogger()->warn(__VA_ARGS__)
    #define APP_LOG_ERROR(...)    pxl::Logger::GetApplicationLogger()->error(__VA_ARGS__)
    #define APP_LOG_CRITICAL(...) pxl::Logger::GetApplicationLogger()->critical(__VA_ARGS__)
#else
    #define APP_LOG_INFO(...)
    #define APP_LOG_WARN(...)
    #define APP_LOG_ERROR(...)
    #define APP_LOG_CRITICAL(...)
#endif