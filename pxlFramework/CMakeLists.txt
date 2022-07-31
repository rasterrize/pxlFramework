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

# RENDERER
)

add_subdirectory(deps/GLFW)
add_subdirectory(deps/Glad)
add_subdirectory(deps/spdlog)

target_link_libraries(pxlFramework glfw glad spdlog)

target_precompile_headers(pxlFramework PUBLIC src/Core/Logger.h <iostream> <string> <memory> <unordered_map>)

target_compile_definitions(pxlFramework PUBLIC GLFW_INCLUDE_NONE)