cmake_minimum_required(VERSION 3.10.0)
project(pxlFramework VERSION 0.0.1 LANGUAGES CXX)

# Add source and header files
file(GLOB_RECURSE core_source_files CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/src/Core/*.cpp ${PROJECT_SOURCE_DIR}/src/Core/*.h)
file(GLOB_RECURSE renderer_source_files CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/src/Renderer/*.cpp ${PROJECT_SOURCE_DIR}/src/Renderer/*.h)
file(GLOB_RECURSE debug_source_files CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/src/Debug/*.cpp ${PROJECT_SOURCE_DIR}/src/Debug/*.h)
file(GLOB_RECURSE utils_source_files CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/src/Utils/*.cpp ${PROJECT_SOURCE_DIR}/src/Utils/*.h)
file(GLOB root_source_files CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/src/*.h)

# Exclude audio for now
if(FALSE)
    file(GLOB_RECURSE audio_source_files CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/src/Audio/*.cpp ${PROJECT_SOURCE_DIR}/src/Audio/*.h)
endif()

add_library(pxlFramework STATIC
    ${core_source_files}
    ${renderer_source_files}
    ${debug_source_files}
    ${utils_source_files}
    ${root_source_files}
)

# Set project c++ standard
target_compile_features(pxlFramework PRIVATE cxx_std_20)

# Enable logging if desired
if(PXL_ENABLE_LOGGING)
    target_compile_definitions(pxlFramework PUBLIC PXL_ENABLE_LOGGING)
endif()

# Enable asserts if desired
if(PXL_ENABLE_ASSERTS)
    target_compile_definitions(pxlFramework PUBLIC PXL_ENABLE_ASSERTS)
endif()

# Enable profiling if desired
if(PXL_ENABLE_PROFILING)
    option(TRACY_ENABLE "" ON)
    option(TRACY_ON_DEMAND "" ON)

    FetchContent_Declare(
        tracy
        GIT_REPOSITORY https://github.com/wolfpld/tracy/
        GIT_TAG 075395620a504c0cdcaf9bab3d196db16a043de7 # v0.11.0
    )

    FetchContent_MakeAvailable(tracy)

    target_compile_definitions(pxlFramework PRIVATE PXL_ENABLE_PROFILING)
    target_link_libraries(pxlFramework Tracy::TracyClient)
    message("pxlFramework profiling enabled")
else()
    option(TRACY_ENABLE "" OFF)
endif()

# Set MSVC settings
if(MSVC)
    # Set MSVC runtime library based on build type
    set_target_properties(pxlFramework PROPERTIES CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

    # Set MSBuild multi-processor compilation
    target_compile_options(pxlFramework PRIVATE "/MP")

    # Set compiler warning level
    target_compile_options(pxlFramework PRIVATE "/W3")

    # Treat warnings as errors
    target_compile_options(pxlFramework PRIVATE "/WX")

    # Ignore missing PDB warnings (currently because of shaderc)
    target_link_options(pxlFramework PUBLIC "/ignore:4099")
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

# Add Dependencies (CMake projects)
include(FetchContent)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw
    GIT_TAG 7b6aead9fb88b3623e3b3725ebb42670cbe4c579 # 3.4
)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog
    GIT_TAG 27cb4c76708608465c413f6d0e6b8d99a4d84302 # v1.14.1
)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm

    # GIT_TAG 0af55ccecd98d4e5a8d1fad7de25ba429d60e863 # 1.0.1
)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/rasterrize/imgui
    GIT_TAG b34fef5aabb7fc280379c901161cb09eb34138a6
)

FetchContent_Declare(
    assimp
    GIT_REPOSITORY https://github.com/assimp/assimp
    GIT_TAG fb375dd8c0a032106a2122815fb18dffe0283721 # v6.0.2
)

FetchContent_Declare(
    yaml-cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp
    GIT_TAG c2680200486572baf8221ba052ef50b58ecd816e # v0.11.0
)

FetchContent_Declare(
    volk
    GIT_REPOSITORY https://github.com/zeux/volk
    GIT_TAG 0b17a763ba5643e32da1b2152f8140461b3b7345 # 1.4.304
)

message("Restoring pxlFramework dependencies...")

FetchContent_MakeAvailable(GLFW spdlog glm imgui assimp yaml-cpp volk)

add_subdirectory(deps/Glad)
add_subdirectory(deps/stb)

# BASS audio library
target_include_directories(pxlFramework PUBLIC deps/BASS/include)
target_link_directories(pxlFramework PUBLIC deps/BASS/lib/x64)

# Set libraries to build with multi-processor compilation
if(MSVC)
    target_compile_options(glfw PUBLIC "/MP")
    target_compile_options(glad PUBLIC "/MP")
    target_compile_options(imgui PUBLIC "/MP")
    target_compile_options(stb PUBLIC "/MP")
    target_compile_options(yaml-cpp PUBLIC "/MP")
endif()

# Find Vulkan SDK
find_package(Vulkan REQUIRED COMPONENTS shaderc_combined)

message("pxlFramework dependencies restored")

# Add Vulkan include directory
target_include_directories(pxlFramework PRIVATE Vulkan::Headers)

target_link_libraries(pxlFramework
    glfw
    glad
    spdlog
    glm
    imgui
    stb
    assimp
    yaml-cpp
    Vulkan::shaderc_combined
    Winmm.lib # For FPS limiter. I consider this temporary.
    dwmapi.lib
    volk
)

target_precompile_headers(pxlFramework PUBLIC src/PCH.h)

# Framework Modules
if(PXL_MODULE_DISCORD)
    message("pxlFramework discord module enabled")
    add_subdirectory(modules/discord)
    target_link_libraries(pxlFramework pxlFramework-discord)
    target_compile_definitions(pxlFramework PUBLIC PXL_ENABLE_MODULE_DISCORD)
endif()

target_compile_definitions(pxlFramework PUBLIC
    GLFW_EXPOSE_NATIVE_WIN32
    GLFW_INCLUDE_NONE # Tell GLFW to not include it's own OpenGL headers
    GLFW_INCLUDE_VULKAN # Tell GLFW to include the Vulkan header
    VK_NO_PROTOTYPES # Tell vulkan.h to not use compile-time function prototypes
    WIN32_LEAN_AND_MEAN # Tell Win32 API to use less headers
    $<$<CONFIG:Debug>:PXL_DEBUG>
    $<$<CONFIG:Release>:PXL_RELEASE>
)

# Specify source directory as an include folder for simplified '#include's in code
# TODO: This currently causes the problem that applications now have access to all source files, which isn't ideal
# and should instead only expose pxl.h (I am unsure if this is even possible)
target_include_directories(pxlFramework PUBLIC src)

# Specify include directories for projects using pxlFramework via add_subdirectory()
target_include_directories(pxlFramework PUBLIC include)