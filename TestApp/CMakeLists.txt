cmake_minimum_required(VERSION 3.10.0)
project(TestApp VERSION 0.0.1 LANGUAGES CXX)

add_executable(TestApp
    src/Entrypoint.cpp
    src/TestApplication.h src/TestApplication.cpp

    # TESTS
    src/Tests/OGLVK.h
    src/Tests/OGLVK.cpp
    src/Tests/ModelViewer.h
    src/Tests/ModelViewer.cpp
    src/Tests/CubesTest.h
    src/Tests/CubesTest.cpp
    src/Tests/OGLVK.h
    src/Tests/OGLVK.cpp
    src/Tests/QuadsTest.h
    src/Tests/QuadsTest.cpp
    src/Tests/EmptyApp.h
    src/Tests/EmptyApp.cpp
    src/Tests/WindowTest.h
    src/Tests/WindowTest.cpp
    src/Tests/LinesTest.h
    src/Tests/LinesTest.cpp
)

# Set project c++ standard
target_compile_features(TestApp PRIVATE cxx_std_20)

# Set pxlFramework settings
set(PXL_ENABLE_PROFILING OFF CACHE BOOL "" FORCE)
set(PXL_ENABLE_ASSERTS $<CONFIG:Debug> CACHE BOOL "" FORCE)
set(PXL_ENABLE_LOGGING $<CONFIG:Debug> CACHE BOOL "" FORCE)

# Include and link to pxlFramework
target_include_directories(TestApp PRIVATE ${CMAKE_SOURCE_DIR}/pxlFramework/include)
target_link_libraries(TestApp pxlFramework)

# Set exe as Win32 for release builds (TODO: use only for distribution builds)
set_target_properties(TestApp PROPERTIES WIN32_EXECUTABLE $<CONFIG:Release>)

# Set Visual Studio working directory
set_target_properties(TestApp PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Set TestApp macros based on build type
target_compile_definitions(TestApp PRIVATE
    $<$<CONFIG:Debug>:TA_DEBUG>
    $<$<CONFIG:Release>:TA_RELEASE>
)

# Set MSVC multi-processor compilation
if (MSVC)
    target_compile_options(TestApp PRIVATE "/MP")
endif()

# Set MSVC runtime library based on build type
set_target_properties(TestApp PROPERTIES CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

# Copy assets to the bin folder so executables can use them along with framework resources
file(COPY assets DESTINATION "${CMAKE_BINARY_DIR}/bin")