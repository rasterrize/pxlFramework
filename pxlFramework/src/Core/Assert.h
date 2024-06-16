#pragma once

// Asserts terminate execution if they see invalid state

// Framework Macros
#ifdef PXL_ENABLE_ASSERTS
    #ifdef PXL_ENABLE_LOGGING
        #define PXL_ASSERT(x) if (!(x)) { PXL_LOG_ERROR(pxl::LogArea::Assert, "Assertion failed in file {} at line {}", __FILE__, __LINE__); __debugbreak(); }
        #define PXL_ASSERT_MSG(x, ...) if (!(x)) { PXL_LOG_ERROR(pxl::LogArea::Assert, __VA_ARGS__); __debugbreak(); }
    #else
        #define PXL_ASSERT(x) if (!(x)) { __debugbreak(); }
        #define PXL_ASSERT_MSG(x, ...) if (!(x)) { __debugbreak(); }
    #endif
#else
    #define PXL_ASSERT(x)
    #define PXL_ASSERT_MSG(x)
#endif

// Application Macros
#ifdef PXL_ENABLE_ASSERTS
    #ifdef PXL_ENABLE_LOGGING
        #define APP_ASSERT(x) if (!(x)) { APP_LOG_ERROR("Assertion failed in file {} at line {}", __FILE__, __LINE__); __debugbreak(); }
    #else
        #define APP_ASSERT(x) if (!(x)) { __debugbreak(); }
    #endif
#else
    #define APP_ASSERT(x)
#endif