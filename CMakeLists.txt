cmake_minimum_required(VERSION 3.31)

project(pxl VERSION 0.0.1)

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
    message(
        FATAL_ERROR
        "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
    )
endif()

# Output directory for executables (.EXE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# User configurable build options
option(PXL_ENABLE_LOGGING "Enable framework logging" ON)
option(PXL_ENABLE_ASSERTS "Enable framework asserts" ON)
option(PXL_ENABLE_PROFILING "Enable profiling using tracy" OFF)
option(PXL_ENABLE_IMGUI "Enable ImGui support" ON)
option(PXL_BUILD_TESTS "Build TestApp/Tests" ${PROJECT_IS_TOP_LEVEL})

# User optional framework modules
option(PXL_MODULE_DISCORD "Enable discord rpc module" OFF)

# NOTE: This is done before pxlFramework because TestApp sets some pxlFramework options
if(PXL_BUILD_TESTS)
    message(STATUS "PXL: Build TestApp enabled")
    add_subdirectory(TestApp)

    # Set TestApp to the startup project in Visual Studio
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT TestApp)
endif()

add_subdirectory(pxlFramework)