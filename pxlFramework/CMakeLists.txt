cmake_minimum_required(VERSION 3.23)
project(pxlFramework VERSION 0.0.1)

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

# RENDERER
src/Renderer/Renderer.h src/Renderer/Renderer.cpp
src/Renderer/RendererAPI.h
src/Renderer/RendererAPIType.h
src/Renderer/GraphicsContext.h src/Renderer/GraphicsContext.cpp
src/Renderer/Buffer.h src/Renderer/Buffer.cpp
src/Renderer/VertexArray.h src/Renderer/VertexArray.cpp
src/Renderer/BufferLayout.h
src/Renderer/Shader.h src/Renderer/Shader.cpp
src/Renderer/Camera.h src/Renderer/Camera.cpp
src/Renderer/OrthographicCamera.h src/Renderer/OrthographicCamera.cpp
src/Renderer/PerspectiveCamera.h src/Renderer/PerspectiveCamera.cpp
src/Renderer/Texture.h src/Renderer/Texture.cpp
src/Renderer/ShaderLibrary.h src/Renderer/ShaderLibrary.cpp
src/Renderer/RendererData.h
src/Renderer/Pipeline.h src/Renderer/Pipeline.cpp
src/Renderer/Framebuffer.h
src/Renderer/Device.h

# OPENGL
src/Renderer/OpenGL/OpenGLRenderer.h src/Renderer/OpenGL/OpenGLRenderer.cpp
src/Renderer/OpenGL/OpenGLContext.h src/Renderer/OpenGL/OpenGLContext.cpp
src/Renderer/OpenGL/OpenGLBuffer.h src/Renderer/OpenGL/OpenGLBuffer.cpp
src/Renderer/OpenGL/OpenGLVertexArray.h src/Renderer/OpenGL/OpenGLVertexArray.cpp
src/Renderer/OpenGL/OpenGLShader.h src/Renderer/OpenGL/OpenGLShader.cpp
src/Renderer/OpenGL/OpenGLTexture2D.h src/Renderer/OpenGL/OpenGLTexture2D.cpp
src/Renderer/OpenGL/OpenGLPipeline.h src/Renderer/OpenGL/OpenGLPipeline.cpp

# AUDIO
# src/Audio/AudioManager.h src/Audio/AudioManager.cpp
# src/Audio/AudioTrack.h src/Audio/AudioTrack.cpp

# VULKAN
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
src/Renderer/Vulkan/VulkanVertexArray.h

# UTILS
src/Utils/FileLoader.h src/Utils/FileLoader.cpp

# DEBUG
src/Debug/ImGui/pxl_ImGui.h src/Debug/ImGui/pxl_ImGui.cpp
src/Debug/ImGui/ImGuiBase.h
src/Debug/ImGui/ImGuiOpenGL.h src/Debug/ImGui/ImGuiOpenGL.cpp
src/Debug/ImGui/ImGuiVulkan.h src/Debug/ImGui/ImGuiVulkan.cpp
)


target_compile_features(pxlFramework PUBLIC cxx_std_20)
# Set highest warning level (highest detection)
target_compile_options(pxlFramework PUBLIC "/W3")

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)

set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "" FORCE)

# Add Dependencies (CMake projects)
add_subdirectory(deps/GLFW)
add_subdirectory(deps/Glad)
add_subdirectory(deps/spdlog)
add_subdirectory(deps/GLM)
add_subdirectory(deps/ImGui)
add_subdirectory(deps/stb)
add_subdirectory(deps/assimp)
add_subdirectory(deps/yaml-cpp)

#target_include_directories(pxlFramework PUBLIC deps/BASS/include)
target_link_directories(pxlFramework PUBLIC deps/BASS/lib/x64)

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

target_include_directories(pxlFramework PUBLIC Vulkan::Headers)

# Link Libraries
target_link_libraries(pxlFramework glfw glad spdlog glm imgui stb assimp yaml-cpp Vulkan::Vulkan)

target_precompile_headers(pxlFramework PUBLIC src/Core/Logging/Logger.h src/Core/Logging/FrameworkLog.h src/Core/Assert.h src/Core/Stopwatch.h <iostream> <string> <memory> <unordered_map> <algorithm> <optional> <functional>)

target_compile_definitions(pxlFramework PUBLIC 
GLFW_INCLUDE_NONE
GLFW_EXPOSE_NATIVE_WIN32
GLFW_INCLUDE_VULKAN
$<$<CONFIG:Release>:PXL_DISABLE_LOGGING> # TODO: switch this to distribution builds
$<$<CONFIG:Debug>:PXL_ENABLE_ASSERTS>
$<$<CONFIG:Debug>:PXL_DEBUG>
_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING # temporary fix for warnings
IMGUI_IMPL_VULKAN_NO_PROTOTYPES) 
#IMGUI_IMPL_OPENGL_LOADER_GLAD

target_include_directories(pxlFramework PUBLIC include)