#pragma once

// Asserts check for a true statement otherwise they terminate execution if they see invalid state

// Framework Macros
#ifdef PXL_ENABLE_ASSERTS
    #ifdef PXL_ENABLE_LOGGING
        // TODO: re-add debugbreaks (both windows and linux versions __raise(SIGTRAP);)
        #define PXL_ASSERT(x) if (!(x)) { PXL_LOG_ERROR(pxl::LogArea::Assert, "Assertion failed in file {} at line {}", __FILE__, __LINE__); }
        #define PXL_ASSERT_MSG(x, ...) if (!(x)) { PXL_LOG_ERROR(pxl::LogArea::Assert, __VA_ARGS__); }
    #else
        #define PXL_ASSERT(x) if (!(x)) {  }
        #define PXL_ASSERT_MSG(x, ...) if (!(x)) {  }
    #endif
#else
    #define PXL_ASSERT(x)
    #define PXL_ASSERT_MSG(x, ...)
#endif

// Application Macros
#ifdef PXL_ENABLE_ASSERTS
    #ifdef PXL_ENABLE_LOGGING
        #define APP_ASSERT(x) if (!(x)) { APP_LOG_ERROR("Assertion failed in file {} at line {}", __FILE__, __LINE__); }
    #else
        #define APP_ASSERT(x) if (!(x)) { }
    #endif
#else
    #define APP_ASSERT(x)
#endif