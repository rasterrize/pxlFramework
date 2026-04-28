cmake_minimum_required(VERSION 3.31)

# Glob source and header files
file(GLOB_RECURSE core-sources CONFIGURE_DEPENDS src/Core/*.cpp)
file(GLOB_RECURSE core-headers CONFIGURE_DEPENDS src/Core/*.h)

file(GLOB_RECURSE debug-sources CONFIGURE_DEPENDS src/Debug/*.cpp)
file(GLOB_RECURSE debug-headers CONFIGURE_DEPENDS src/Debug/*.h)

file(GLOB_RECURSE input-sources CONFIGURE_DEPENDS src/Input/*.cpp)
file(GLOB_RECURSE input-headers CONFIGURE_DEPENDS src/Input/*.h)

file(GLOB platform-sources CONFIGURE_DEPENDS src/Platform/*.cpp)
file(GLOB platform-headers CONFIGURE_DEPENDS src/Platform/*.h)

file(GLOB_RECURSE renderer-sources CONFIGURE_DEPENDS src/Renderer/*.cpp)
file(GLOB_RECURSE renderer-headers CONFIGURE_DEPENDS src/Renderer/*.h)

file(GLOB_RECURSE utils-sources CONFIGURE_DEPENDS src/Utils/*.cpp)
file(GLOB_RECURSE utils-headers CONFIGURE_DEPENDS src/Utils/*.h)

add_library(${PROJECT_NAME} STATIC)

target_sources(${PROJECT_NAME}
    PRIVATE
    ${core-sources}
    ${debug-sources}
    ${input-sources}
    ${platform-sources}
    ${renderer-sources}
    ${utils-sources}
    PUBLIC
    ${core-headers}
    ${debug-headers}
    ${input-headers}
    ${platform-sources}
    ${renderer-headers}
    ${utils-headers}
)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    file(GLOB_RECURSE platform-windows-sources CONFIGURE_DEPENDS src/Platform/Windows/*.cpp)
    file(GLOB_RECURSE platform-windows-headers CONFIGURE_DEPENDS src/Platform/Windows/*.h)
endif()

# Set library c++ standard
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20)

# Enable logging if desired
if(PXL_ENABLE_LOGGING)
    target_compile_definitions(${PROJECT_NAME} PUBLIC PXL_ENABLE_LOGGING)
endif()

# Enable asserts if desired
if(PXL_ENABLE_ASSERTS)
    target_compile_definitions(${PROJECT_NAME} PUBLIC PXL_ENABLE_ASSERTS)
endif()

# Set MSVC settings
if(MSVC)
    # Set MSVC runtime library based on build type
    set_target_properties(${PROJECT_NAME} PROPERTIES CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

    # Set MSBuild multi-processor compilation
    target_compile_options(${PROJECT_NAME} PRIVATE "/MP")

    # Set compiler warning level
    target_compile_options(${PROJECT_NAME} PRIVATE "/W4")

    # Treat warnings as errors
    target_compile_options(${PROJECT_NAME} PRIVATE "/WX")

    # Ignore missing PDB warnings (currently because of shaderc)
    target_link_options(${PROJECT_NAME} PUBLIC "/ignore:4099")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # Due to certain variables being named their own type, we must ignore this warning for now
    target_compile_options(${PROJECT_NAME} PUBLIC "-Wno-changes-meaning")
endif()

# download CPM.cmake
file(
    DOWNLOAD
    https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.40.8/CPM.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
    EXPECTED_HASH SHA256=78ba32abdf798bc616bab7c73aac32a17bbd7b06ad9e26a6add69de8f3ae4791
)

include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)

# Use external CPM cache if defined
if(DEFINED ENV{CPM_CACHE})
    set(CPM_SOURCE_CACHE $ENV{CPM_CACHE})
else()
    set(CPM_SOURCE_CACHE ${CMAKE_BINARY_DIR}/cpm_cache)
endif()

# Add dependencies (CMake projects)
message(STATUS "PXL: Restoring dependencies")

CPMAddPackage("gh:glfw/glfw#3.4")
CPMAddPackage("gh:gabime/spdlog#v1.14.1")

CPMAddPackage(
    NAME glm
    GITHUB_REPOSITORY g-truc/glm
    GIT_TAG 1.0.2
    OPTIONS
    "GLM_ENABLE_CXX_20 ON"
)

if(PXL_ENABLE_IMGUI)
    CPMAddPackage(
        NAME imgui
        GITHUB_REPOSITORY rasterrize/imgui
        GIT_TAG 1ba07ec10c7e143bf37c7d565bec90524bbb96d8
        OPTIONS
        "IMGUI_USE_BACKEND_GLFW ON"
        "IMGUI_USE_BACKEND_VULKAN ON"
        "IMGUI_USE_STDLIB ON"
        "IMGUI_USE_VOLK ON"
    )

    target_compile_definitions(${PROJECT_NAME} PUBLIC PXL_ENABLE_IMGUI)
    target_link_libraries(${PROJECT_NAME} imgui)
    message(STATUS "PXL: ImGui support enabled")
endif()

CPMAddPackage(
    NAME assimp
    GITHUB_REPOSITORY assimp/assimp
    GIT_TAG v6.0.2
    OPTIONS
    "ASSIMP_BUILD_TESTS OFF"
    "ASSIMP_INSTALL OFF"
    "ASSIMP_NO_EXPORT ON"
    "ASSIMP_BUILD_ZLIB ON"
    "ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF"
    "ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF"
    "ASSIMP_BUILD_FBX_IMPORTER ON"
    "ASSIMP_BUILD_OBJ_IMPORTER ON"
    "ASSIMP_BUILD_GLTF_IMPORTER ON"
)

CPMAddPackage(
    NAME simpleini
    GITHUB_REPOSITORY brofield/simpleini
    GIT_TAG v4.25
)

if(PXL_ENABLE_PROFILING)
    CPMAddPackage(
        NAME tracy
        GITHUB_REPOSITORY wolfpld/tracy
        GIT_TAG v0.13.1
        OPTIONS
        "TRACY_ENABLE ON"
        "TRACY_ON_DEMAND ON"
    )

    target_compile_definitions(${PROJECT_NAME} PRIVATE PXL_ENABLE_PROFILING)
    target_link_libraries(${PROJECT_NAME} Tracy::TracyClient)
    message(STATUS "PXL: Profiling enabled")
endif()

add_subdirectory(deps/stb)

target_include_directories(${PROJECT_NAME} PUBLIC deps/BASS/include)
target_link_directories(${PROJECT_NAME} PUBLIC deps/BASS/lib/x64)

# Find Vulkan SDK
find_package(Vulkan REQUIRED COMPONENTS volk shaderc_combined)

message(STATUS "PXL: Finished restoring dependencies")

# Link dependencies
target_link_libraries(${PROJECT_NAME}
    glfw
    spdlog
    glm
    stb
    assimp
    SimpleIni::SimpleIni
    Vulkan::volk
    Vulkan::shaderc_combined
    $<$<PLATFORM_ID:Windows>:dwmapi.lib>
    $<$<PLATFORM_ID:Windows>:winmm.lib>
)

# TODO: find out how to make this private (pxlFramework consumers should not have access to this)
target_precompile_headers(${PROJECT_NAME} PUBLIC src/PCH.h)

# Framework Modules
if(PXL_MODULE_DISCORD)
    message(STATUS "PXL: Discord module enabled")
    add_subdirectory(modules/discord)
    target_link_libraries(${PROJECT_NAME} pxlFramework-discord)
    target_compile_definitions(${PROJECT_NAME} PUBLIC PXL_ENABLE_MODULE_DISCORD)
endif()

set(PXL_ABSOLUTE_SOURCE_RESOURCE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/resources)
target_compile_definitions(${PROJECT_NAME} PUBLIC
    $<$<PLATFORM_ID:Windows>:GLFW_EXPOSE_NATIVE_WIN32>
    $<$<PLATFORM_ID:Windows>:WIN32_LEAN_AND_MEAN> # Tell Win32 API to use less headers
    $<$<PLATFORM_ID:Windows>:NOMINMAX>
    GLFW_INCLUDE_NONE # Tell GLFW to not include it's own OpenGL headers
    VK_NO_PROTOTYPES # Tell vulkan.h to not use compile-time function prototypes
    $<$<CONFIG:Debug>:PXL_DEBUG>
    $<$<CONFIG:Release>:PXL_RELEASE>
    $<IF:$<CONFIG:Debug>, PXL_RESOURCE_DIRECTORY="${PXL_ABSOLUTE_SOURCE_RESOURCE_DIRECTORY}", PXL_RESOURCE_DIRECTORY="resources">
)

# Specify source directory as an include folder for simplified '#include's in code
# TODO: This currently causes the problem that applications now have access to all source files, which isn't ideal
# and should instead only expose pxl.h (I am unsure if this is even possible)
target_include_directories(${PROJECT_NAME}
    PUBLIC src
    INTERFACE include
)

# Copy framework resources to bin directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# Copy necessary framework dependency DLLs to bin directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/deps/BASS/lib/x64/bass.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})