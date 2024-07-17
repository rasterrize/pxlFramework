cmake_minimum_required(VERSION 3.5.0)
project(pxlFramework VERSION 0.0.1 LANGUAGES CXX)

add_library(pxlFramework STATIC
    # CORE
    src/Core/Application.h src/Core/Application.cpp
    src/Core/Window.h src/Core/Window.cpp
    src/Core/Logging/Logger.h src/Core/Logging/Logger.cpp
    src/Core/Logging/FrameworkLog.h
    src/Core/Logging/ApplicationLog.h
    src/Core/Input.h src/Core/Input.cpp
    src/Core/KeyCodes.h
    src/Core/MouseCodes.h
    src/Core/Platform.h
    src/Core/Stopwatch.h
    src/Core/Config.h src/Core/Config.cpp
    src/Core/DiscordRPC.h src/Core/DiscordRPC.cpp

    # RENDERER
    src/Renderer/Renderer.h src/Renderer/Renderer.cpp
    src/Renderer/RendererAPI.h
    src/Renderer/RendererAPIType.h
    src/Renderer/GraphicsContext.h src/Renderer/GraphicsContext.cpp
    src/Renderer/Buffer.h src/Renderer/Buffer.cpp
    src/Renderer/VertexArray.h src/Renderer/VertexArray.cpp
    src/Renderer/BufferLayout.h
    src/Renderer/UniformLayout.h
    src/Renderer/Shader.h src/Renderer/Shader.cpp
    src/Renderer/Camera.h src/Renderer/Camera.cpp
    src/Renderer/OrthographicCamera.h src/Renderer/OrthographicCamera.cpp
    src/Renderer/PerspectiveCamera.h src/Renderer/PerspectiveCamera.cpp
    src/Renderer/Texture.h src/Renderer/Texture.cpp
    src/Renderer/ShaderLibrary.h src/Renderer/ShaderLibrary.cpp
    src/Renderer/RendererData.h
    src/Renderer/Pipeline.h src/Renderer/Pipeline.cpp
    src/Renderer/Framebuffer.h
    src/Renderer/GraphicsDevice.h
    src/Renderer/Image.h

    # OPENGL
    src/Renderer/OpenGL/OpenGLRenderer.h src/Renderer/OpenGL/OpenGLRenderer.cpp
    src/Renderer/OpenGL/OpenGLContext.h src/Renderer/OpenGL/OpenGLContext.cpp
    src/Renderer/OpenGL/OpenGLBuffer.h src/Renderer/OpenGL/OpenGLBuffer.cpp
    src/Renderer/OpenGL/OpenGLVertexArray.h src/Renderer/OpenGL/OpenGLVertexArray.cpp
    src/Renderer/OpenGL/OpenGLShader.h src/Renderer/OpenGL/OpenGLShader.cpp
    src/Renderer/OpenGL/OpenGLTexture.h src/Renderer/OpenGL/OpenGLTexture.cpp
    src/Renderer/OpenGL/OpenGLPipeline.h src/Renderer/OpenGL/OpenGLPipeline.cpp

    # AUDIO
    # src/Audio/AudioManager.h src/Audio/AudioManager.cpp
    # src/Audio/AudioTrack.h src/Audio/AudioTrack.cpp

    # VULKAN
    src/Renderer/Vulkan/VulkanInstance.h src/Renderer/Vulkan/VulkanInstance.cpp
    src/Renderer/Vulkan/VulkanRenderer.h src/Renderer/Vulkan/VulkanRenderer.cpp
    src/Renderer/Vulkan/VulkanContext.h src/Renderer/Vulkan/VulkanContext.cpp
    src/Renderer/Vulkan/VulkanShader.h src/Renderer/Vulkan/VulkanShader.cpp
    src/Renderer/Vulkan/VulkanPipeline.h src/Renderer/Vulkan/VulkanPipeline.cpp
    src/Renderer/Vulkan/VulkanRenderPass.h src/Renderer/Vulkan/VulkanRenderPass.cpp
    src/Renderer/Vulkan/VulkanFramebuffer.h src/Renderer/Vulkan/VulkanFramebuffer.cpp
    src/Renderer/Vulkan/VulkanSwapchain.h src/Renderer/Vulkan/VulkanSwapchain.cpp
    src/Renderer/Vulkan/VulkanHelpers.h src/Renderer/Vulkan/VulkanHelpers.cpp
    src/Renderer/Vulkan/VulkanImage.h src/Renderer/Vulkan/VulkanImage.cpp
    src/Renderer/Vulkan/VulkanBuffer.h src/Renderer/Vulkan/VulkanBuffer.cpp
    src/Renderer/Vulkan/VulkanDevice.h src/Renderer/Vulkan/VulkanDevice.cpp
    src/Renderer/Vulkan/VulkanAllocator.h src/Renderer/Vulkan/VulkanAllocator.cpp
    src/Renderer/Vulkan/VmaUsage.cpp

    # UTILS
    src/Utils/FileLoader.h src/Utils/FileLoader.cpp
    src/Utils/EnumStringHelper.h src/Utils/EnumStringHelper.cpp

    # DEBUG
    src/Debug/ImGui/pxl_ImGui.h src/Debug/ImGui/pxl_ImGui.cpp
    src/Debug/ImGui/ImGuiBase.h
    src/Debug/ImGui/ImGuiOpenGL.h src/Debug/ImGui/ImGuiOpenGL.cpp
    src/Debug/ImGui/ImGuiVulkan.h src/Debug/ImGui/ImGuiVulkan.cpp
    src/Debug/Profiler.h
)

# Set project c++ standard
target_compile_features(pxlFramework PRIVATE cxx_std_20)

# Set compiler warning level - NOTE: This likely only works with MSVC
target_compile_options(pxlFramework PRIVATE "/W3")

# pxlFramework CMake options
option(PXL_ENABLE_PROFILING "Enable profiling using tracy" OFF)

# Set GLFW settings
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# Set ASSIMP settings
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)

# Set YAMLCPP settings
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "" FORCE)

# Enable profiling if desired
if (PXL_ENABLE_PROFILING)
    message("Enabling tracy profiling for pxlFramework")
    option(TRACY_ENABLE "" ON)
    option(TRACY_ON_DEMAND "" ON)
    add_subdirectory(deps/tracy)
    target_compile_definitions(pxlFramework PRIVATE PXL_ENABLE_PROFILING)
else()
    option(TRACY_ENABLE "" OFF)
endif()

# Add Dependencies (CMake projects)
add_subdirectory(deps/GLFW)
add_subdirectory(deps/Glad)
add_subdirectory(deps/spdlog)
add_subdirectory(deps/GLM)
add_subdirectory(deps/ImGui)
add_subdirectory(deps/stb)
add_subdirectory(deps/assimp)
add_subdirectory(deps/yaml-cpp)
add_subdirectory(deps/discord-game-sdk)

#target_include_directories(pxlFramework PUBLIC deps/BASS/include)
#target_link_directories(pxlFramework PUBLIC deps/BASS/lib/x64)

# Find Vulkan SDK using its environment variable
set(CMAKE_MODULE_PATH $ENV{VULKAN_SDK})
find_package(Vulkan REQUIRED)

if (${Vulkan_FOUND})
    message("Found Vulkan SDK")
    message("-- Vulkan SDK Path: $ENV{VULKAN_SDK}")
    message("-- Vulkan Include: ${Vulkan_INCLUDE_DIR}")
    message("-- Vulkan Library: ${Vulkan_LIBRARY}")
else()
    message("Failed to find Vulkan SDK")
endif()

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
    Vulkan::Vulkan
    Tracy::TracyClient
    discordgamesdk
)

target_precompile_headers(pxlFramework PUBLIC 
    src/Core/Logging/Logger.h
    src/Core/Logging/FrameworkLog.h
    src/Core/Assert.h
    src/Debug/Profiler.h
    <iostream> # TODO: remove this?
    <string>
    <memory>
    <unordered_map>
    <map>
    <algorithm>
    <optional>
    <functional>
    <array>
    <deque>
    <filesystem>
)

target_compile_definitions(pxlFramework PRIVATE 
    GLFW_INCLUDE_NONE
    GLFW_EXPOSE_NATIVE_WIN32
    GLFW_INCLUDE_VULKAN
    $<$<CONFIG:Debug>:PXL_ENABLE_LOGGING>
    $<$<CONFIG:Debug>:PXL_ENABLE_ASSERTS> # Should these be a part of the application cmake file so users can actually change them
    $<$<CONFIG:Debug>:PXL_DEBUG_VULKAN>
    _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING # temporary fix for warnings
    IMGUI_IMPL_OPENGL_LOADER_GLAD
    IMGUI_IMPL_VULKAN_NO_PROTOTYPES
)

# Specify source directory as an include folder for simplified '#include's in code
target_include_directories(pxlFramework PRIVATE src)

# Specify include directories for projects using pxlFramework via add_subdirectory()
target_include_directories(pxlFramework PUBLIC include)

# Copy internal framework resources to bin directory
file(COPY resources/shaders DESTINATION ${CMAKE_BINARY_DIR}/bin/resources)

# Copy necessary framework dependency DLLs to bin directory
file(COPY deps/discord-game-sdk/lib/discord_game_sdk.dll DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY deps/BASS/lib/x64/bass.dll DESTINATION ${CMAKE_BINARY_DIR}/bin)