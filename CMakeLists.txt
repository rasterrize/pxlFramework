cmake_minimum_required(VERSION 3.10.0)
project(pxlFramework VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib) # Output directory for static lib (.LIB)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) # Output directory for shared lib (.DLL)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) # Output directory for executables (.EXE)

# Build options (user configurable)
option(PXL_ENABLE_LOGGING "Enable framework logging" ON)
option(PXL_ENABLE_ASSERTS "Enable framework asserts" OFF)
option(PXL_ENABLE_PROFILING "Enable profiling using tracy" OFF)
option(PXL_BUILD_TESTS "Build TestApp/Tests" OFF)

# Ensure static linking
option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)

# NOTE: This is done before pxlFramework because TestApp sets some pxlFramework options
if (PXL_BUILD_TESTS)
    add_subdirectory(TestApp)

    # Set TestApp to the startup project in Visual Studio
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT TestApp)
endif()

add_subdirectory(pxlFramework)

# Copy internal framework resources to bin directory
file(COPY pxlFramework/resources DESTINATION ${CMAKE_BINARY_DIR}/bin)

# Copy necessary framework dependency DLLs to bin directory
file(COPY pxlFramework/deps/discord-game-sdk/lib/discord_game_sdk.dll DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY pxlFramework/deps/BASS/lib/x64/bass.dll DESTINATION ${CMAKE_BINARY_DIR}/bin)