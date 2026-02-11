#pragma once
// ------------------------------------------------------------------------------------------------
// Asserts check for a true statement otherwise they terminate execution if they catch invalid state
// ------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
    #define PXL_DEBUG_BREAK __debugbreak()
#else
    #define PXL_DEBUG_BREAK
#endif

// Framework Macros
#ifdef PXL_ENABLE_ASSERTS
    #ifdef PXL_ENABLE_LOGGING
        #define PXL_ASSERT(x) if (!(x)) { PXL_LOG_ERROR(pxl::LogArea::Assert, "Assertion failed in file {} at line {}", __FILE__, __LINE__); PXL_DEBUG_BREAK; }
        #define PXL_ASSERT_MSG(x, ...) if (!(x)) { PXL_LOG_ERROR(pxl::LogArea::Assert, __VA_ARGS__); PXL_DEBUG_BREAK; }
    #else
        #define PXL_ASSERT(x) if (!(x)) { PXL_DEBUG_BREAK; }
        #define PXL_ASSERT_MSG(x, ...) if (!(x)) { PXL_DEBUG_BREAK; }
    #endif
#else
    #define PXL_ASSERT(x)
    #define PXL_ASSERT_MSG(x, ...)
#endif

// Application Macros
#ifdef PXL_ENABLE_ASSERTS
    #ifdef PXL_ENABLE_LOGGING
        #define APP_ASSERT(x) if (!(x)) { APP_LOG_ERROR("Assertion failed in file {} at line {}", __FILE__, __LINE__); PXL_DEBUG_BREAK; }
    #else
        #define APP_ASSERT(x) if (!(x)) { PXL_DEBUG_BREAK; }
    #endif
#else
    #define APP_ASSERT(x)
#endif