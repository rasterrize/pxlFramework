cmake_minimum_required(VERSION 3.30)

# Glob source and header files
file(GLOB_RECURSE sources CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE headers CONFIGURE_DEPENDS src/*.h)

add_library(${PROJECT_NAME} STATIC)

target_sources(${PROJECT_NAME}
    PRIVATE
    ${sources}
    PUBLIC
    ${headers}
)

# Set project c++ standard
target_compile_features(pxl PRIVATE cxx_std_20)

# Enable logging if desired
if(PXL_ENABLE_LOGGING)
    target_compile_definitions(pxl PRIVATE PXL_ENABLE_LOGGING)
endif()

# Enable asserts if desired
if(PXL_ENABLE_ASSERTS)
    target_compile_definitions(pxl PRIVATE PXL_ENABLE_ASSERTS)
endif()

# Set MSVC settings
if(MSVC)
    # Set MSVC runtime library based on build type
    set_target_properties(pxl PROPERTIES CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

    # Set MSBuild multi-processor compilation
    target_compile_options(pxl PRIVATE "/MP")

    # Set compiler warning level
    target_compile_options(pxl PRIVATE "/W3")

    # Treat warnings as errors
    target_compile_options(pxl PRIVATE "/WX")

    # Ignore missing PDB warnings (currently because of shaderc)
    target_link_options(pxl PUBLIC "/ignore:4099")
endif()

# download CPM.cmake
file(
    DOWNLOAD
    https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.40.8/CPM.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
    EXPECTED_HASH SHA256=78ba32abdf798bc616bab7c73aac32a17bbd7b06ad9e26a6add69de8f3ae4791
)

include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)

if(DEFINED ENV{CPM_CACHE})
    set(CPM_SOURCE_CACHE $ENV{CPM_CACHE})
else()
    set(CPM_SOURCE_CACHE ${CMAKE_BINARY_DIR}/cpm_cache)
endif()

# Set GLFW settings
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# Set GLM settings
set(GLM_ENABLE_CXX_20 ON CACHE BOOL "" FORCE)

# Set ASSIMP settings
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_FBX_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_GLTF_IMPORTER ON CACHE BOOL "" FORCE)

# Set YAMLCPP settings
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(YAML_CPP_FORMAT_SOURCE OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_CONTRIB OFF CACHE BOOL "" FORCE)

# Set ImGui settings
set(IMGUI_USE_BACKEND_GLFW ON CACHE BOOL "" FORCE)
set(IMGUI_USE_BACKEND_VULKAN ON CACHE BOOL "" FORCE)
set(IMGUI_USE_STDLIB ON CACHE BOOL "" FORCE)
set(IMGUI_USE_VOLK ON CACHE BOOL "" FORCE)

# Add Dependencies (CMake projects)
message(STATUS "PXL: Restoring dependencies")

CPMAddPackage("gh:glfw/glfw#3.4")
CPMAddPackage("gh:gabime/spdlog#v1.14.1")
CPMAddPackage("gh:g-truc/glm#1.0.2")
CPMAddPackage("gh:rasterrize/imgui#0b6e01053c60c828b5625bd2eb80a9b9525e31d1")
CPMAddPackage("gh:assimp/assimp#v6.0.2")
CPMAddPackage("gh:jbeder/yaml-cpp#yaml-cpp-0.9.0")

if(PXL_ENABLE_PROFILING)
    option(TRACY_ENABLE "" ON)
    option(TRACY_ON_DEMAND "" ON)

    CPMAddPackage("gh:wolfpld/tracy#v0.11.0")

    target_compile_definitions(pxlFramework PRIVATE PXL_ENABLE_PROFILING)
    target_link_libraries(pxlFramework Tracy::TracyClient)
    message(STATUS "PXL: Profiling enabled")
else()
    option(TRACY_ENABLE "" OFF)
endif()

add_subdirectory(deps/stb)

target_include_directories(pxl PUBLIC deps/BASS/include)
target_link_directories(pxl PUBLIC deps/BASS/lib/x64)

# Set libraries to build with multi-processor compilation
if(MSVC)
    target_compile_options(glfw PUBLIC "/MP")
    target_compile_options(imgui PUBLIC "/MP")
    target_compile_options(stb PUBLIC "/MP")
    target_compile_options(yaml-cpp PUBLIC "/MP")
endif()

# Find Vulkan SDK
find_package(Vulkan REQUIRED COMPONENTS volk shaderc_combined)

message(STATUS "PXL: Finished restoring dependencies")

target_link_libraries(pxl
    glfw
    spdlog
    glm
    imgui
    stb
    assimp
    yaml-cpp
    Vulkan::volk
    Vulkan::shaderc_combined
    $<$<PLATFORM_ID:Windows>:dwmapi.lib>
)

target_precompile_headers(pxl PUBLIC src/PCH.h)

# Framework Modules
if(PXL_MODULE_DISCORD)
    message(STATUS "PXL: Discord module enabled")
    add_subdirectory(modules/discord)
    target_link_libraries(pxl pxlFramework-discord)
    target_compile_definitions(pxl PUBLIC PXL_ENABLE_MODULE_DISCORD)
endif()

set(PXL_ABSOLUTE_SOURCE_RESOURCE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/resources)
target_compile_definitions(${PROJECT_NAME} PUBLIC
    $<$<PLATFORM_ID:Windows>:GLFW_EXPOSE_NATIVE_WIN32>
    $<$<PLATFORM_ID:Windows>:WIN32_LEAN_AND_MEAN> # Tell Win32 API to use less headers
    GLFW_INCLUDE_NONE # Tell GLFW to not include it's own OpenGL headers
    VK_NO_PROTOTYPES # Tell vulkan.h to not use compile-time function prototypes
    $<$<CONFIG:Debug>:PXL_DEBUG>
    $<$<CONFIG:Release>:PXL_RELEASE>
    PXL_SOURCE_RESOURCE_DIRECTORY="${PXL_ABSOLUTE_SOURCE_RESOURCE_DIRECTORY}"
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