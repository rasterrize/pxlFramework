cmake_minimum_required(VERSION 3.23)
project(pxlFramework)

add_library(pxlFramework STATIC 
# INCLUDE
inc/pxl.h

# CORE
src/Core/Application.h src/Core/Application.cpp
src/Core/Window.h src/Core/Window.cpp
src/Core/WindowMode.h
src/Core/Logger.h src/Core/Logger.cpp
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
src/Renderer/VertexBuffer.h
src/Renderer/IndexBuffer.h
src/Renderer/VertexArray.h
src/Renderer/BufferLayout.h
src/Renderer/Shader.h src/Renderer/Shader.cpp
src/Renderer/Camera.h src/Renderer/Camera.cpp
src/Renderer/OrthographicCamera.h src/Renderer/OrthographicCamera.cpp
src/Renderer/PerspectiveCamera.h src/Renderer/PerspectiveCamera.cpp
src/Renderer/Texture.h src/Renderer/Texture.cpp
src/Renderer/ShaderLibrary.h src/Renderer/ShaderLibrary.cpp
src/Renderer/RendererData.h
src/Renderer/Pipeline.h

# OPENGL
src/Renderer/OpenGL/OpenGLRenderer.h src/Renderer/OpenGL/OpenGLRenderer.cpp
src/Renderer/OpenGL/OpenGLContext.h src/Renderer/OpenGL/OpenGLContext.cpp
src/Renderer/OpenGL/OpenGLVertexBuffer.h src/Renderer/OpenGL/OpenGLVertexBuffer.cpp
src/Renderer/OpenGL/OpenGLIndexBuffer.h src/Renderer/OpenGL/OpenGLIndexBuffer.cpp
src/Renderer/OpenGL/OpenGLVertexArray.h src/Renderer/OpenGL/OpenGLVertexArray.cpp
src/Renderer/OpenGL/OpenGLShader.h src/Renderer/OpenGL/OpenGLShader.cpp
src/Renderer/OpenGL/OpenGLTexture2D.h src/Renderer/OpenGL/OpenGLTexture2D.cpp

# AUDIO
# src/Audio/AudioManager.h src/Audio/AudioManager.cpp
# src/Audio/AudioTrack.h src/Audio/AudioTrack.cpp

# VULKAN
src/Renderer/Vulkan/VulkanErrorCheck.h src/Renderer/Vulkan/VulkanErrorCheck.cpp
src/Renderer/Vulkan/VulkanRenderer.h src/Renderer/Vulkan/VulkanRenderer.cpp
src/Renderer/Vulkan/VulkanContext.h src/Renderer/Vulkan/VulkanContext.cpp
src/Renderer/Vulkan/VulkanShader.h src/Renderer/Vulkan/VulkanShader.cpp
src/Renderer/Vulkan/VulkanPipeline.h src/Renderer/Vulkan/VulkanPipeline.cpp

# UTILS
src/Utils/FileLoader.h src/Utils/FileLoader.cpp

# DEBUG
src/Debug/ImGui/pxl_ImGui.h src/Debug/ImGui/pxl_ImGui.cpp
src/Debug/ImGui/ImGuiBase.h
src/Debug/ImGui/ImGuiOpenGL.h src/Debug/ImGui/ImGuiOpenGL.cpp
)

#target_compile_options(pxlFramework PUBLIC "/P") # preprocess to a file

target_compile_features(pxlFramework PUBLIC cxx_std_20)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE) 

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
#target_link_directories(pxlFramework PUBLIC deps/BASS/lib/x64)

target_include_directories(pxlFramework PUBLIC C:/VulkanSDK/1.3.246.1/Include)
target_link_directories(pxlFramework PUBLIC C:/VulkanSDK/1.3.246.1/Lib)

target_link_libraries(pxlFramework glfw glad spdlog glm imgui stb assimp yaml-cpp vulkan-1)

target_precompile_headers(pxlFramework PUBLIC src/Core/Logger.h <iostream> <string> <memory> <unordered_map> <algorithm> <optional>)

target_compile_definitions(pxlFramework PUBLIC 
GLFW_INCLUDE_NONE
GLFW_EXPOSE_NATIVE_WIN32
GLFW_INCLUDE_VULKAN) #IMGUI_IMPL_OPENGL_LOADER_GLAD

target_include_directories(pxlFramework PUBLIC inc)