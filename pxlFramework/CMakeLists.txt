cmake_minimum_required(VERSION 3.23)
project(pxlFramework)

add_library(pxlFramework STATIC 
# INCLUDE
inc/pxl.h

# CORE
src/Core/Application.h src/Core/Application.cpp
src/Core/Window.h src/Core/Window.cpp
src/Core/Logger.h src/Core/Logger.cpp
src/Core/Input.h src/Core/Input.cpp
src/Core/KeyCodes.h
src/Core/Timestep.h
src/Core/Math.h

# RENDERER
src/Renderer/Renderer.h src/Renderer/Renderer.cpp
src/Renderer/RendererAPI.h
src/Renderer/GraphicsContext.h
src/Renderer/VertexBuffer.h
src/Renderer/IndexBuffer.h
src/Renderer/VertexArray.h
src/Renderer/BufferLayout.h
src/Renderer/Shader.h src/Renderer/Shader.cpp
src/Renderer/Camera.h src/Renderer/Camera.cpp
src/Renderer/BaseCamera.h
src/Renderer/Texture.h

# OPENGL
src/Renderer/OpenGL/OpenGLRenderer.h src/Renderer/OpenGL/OpenGLRenderer.cpp
src/Renderer/OpenGL/OpenGLContext.h src/Renderer/OpenGL/OpenGLContext.cpp
src/Renderer/OpenGL/OpenGLVertexBuffer.h src/Renderer/OpenGL/OpenGLVertexBuffer.cpp
src/Renderer/OpenGL/OpenGLIndexBuffer.h src/Renderer/OpenGL/OpenGLIndexBuffer.cpp
src/Renderer/OpenGL/OpenGLVertexArray.h src/Renderer/OpenGL/OpenGLVertexArray.cpp
src/Renderer/OpenGL/OpenGLShader.h src/Renderer/OpenGL/OpenGLShader.cpp
src/Renderer/OpenGL/OpenGLOrthographicCamera.h src/Renderer/OpenGL/OpenGLOrthographicCamera.cpp
src/Renderer/OpenGL/OpenGLPerspectiveCamera.h src/Renderer/OpenGL/OpenGLPerspectiveCamera.cpp
src/Renderer/OpenGL/OpenGLTexture.h src/Renderer/OpenGL/OpenGLTexture.cpp

# UTILS
src/Utils/TextureLoader.h src/Utils/TextureLoader.cpp

# DEBUG
src/Debug/ImGui/ImGuiOpenGL.h src/Debug/ImGui/ImGuiOpenGL.cpp
)

target_compile_features(pxlFramework PUBLIC cxx_std_17)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(deps/GLFW)
add_subdirectory(deps/Glad)
add_subdirectory(deps/spdlog)
add_subdirectory(deps/GLM)
add_subdirectory(deps/ImGui)
add_subdirectory(deps/stb)
#add_subdirectory(deps/assimp)

target_link_libraries(pxlFramework glfw glad spdlog glm imgui stb)

target_precompile_headers(pxlFramework PUBLIC src/Core/Logger.h <iostream> <string> <memory> <unordered_map> <algorithm>)

target_compile_definitions(pxlFramework PUBLIC GLFW_INCLUDE_NONE) #IMGUI_IMPL_OPENGL_LOADER_CUSTOM

target_include_directories(pxlFramework PUBLIC inc)