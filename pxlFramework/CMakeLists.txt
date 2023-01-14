cmake_minimum_required(VERSION 3.23)
project(pxlFramework)

add_library(pxlFramework STATIC 
# SRC
pxl.h

# CORE
src/Core/Application.h src/Core/Application.cpp
src/Core/Window.h src/Core/Window.cpp
src/Core/Logger.h src/Core/Logger.cpp
src/Core/Input.h src/Core/Input.cpp
src/Core/KeyCodes.h

# RENDERER
src/Renderer/Renderer.h src/Renderer/Renderer.cpp
src/Renderer/GraphicsContext.h
src/Renderer/VertexBuffer.h
src/Renderer/IndexBuffer.h
src/Renderer/VertexArray.h
src/Renderer/BufferLayout.h
src/Renderer/Shader.h
src/Renderer/Camera.h src/Renderer/Camera.cpp
src/Renderer/BaseCamera.h

# OPENGL
src/Renderer/OpenGL/OpenGLContext.h src/Renderer/OpenGL/OpenGLContext.cpp
src/Renderer/OpenGL/OpenGLVertexBuffer.h src/Renderer/OpenGL/OpenGLVertexBuffer.cpp
#src/Renderer/OpenGL/OpenGLIndexBuffer.h src/Renderer/OpenGL/OpenGLIndexBuffer.cpp
src/Renderer/OpenGL/OpenGLVertexArray.h src/Renderer/OpenGL/OpenGLVertexArray.cpp
src/Renderer/OpenGL/OpenGLShader.h src/Renderer/OpenGL/OpenGLShader.cpp
src/Renderer/OpenGL/OpenGLOrthographicCamera.h src/Renderer/OpenGL/OpenGLOrthographicCamera.cpp
src/Renderer/OpenGL/OpenGLPerspectiveCamera.h src/Renderer/OpenGL/OpenGLPerspectiveCamera.cpp
)

add_subdirectory(deps/GLFW)
add_subdirectory(deps/Glad)
add_subdirectory(deps/spdlog)
add_subdirectory(deps/GLM)

target_link_libraries(pxlFramework glfw glad spdlog glm)

target_precompile_headers(pxlFramework PUBLIC src/Core/Logger.h <iostream> <string> <memory> <unordered_map>)

target_compile_definitions(pxlFramework PUBLIC GLFW_INCLUDE_NONE)