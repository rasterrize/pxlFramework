cmake_minimum_required(VERSION 3.23)
project(pxlFramework)

add_library(pxlFramework STATIC 
# SRC
pxl.h

# CORE
src/Application.h src/Application.cpp
src/Window.h src/Window.cpp
src/Logger.h src/Logger.cpp

# RENDERER
)

add_subdirectory(deps/GLFW)
add_subdirectory(deps/Glad)
add_subdirectory(deps/spdlog)

target_link_libraries(pxlFramework glfw glad spdlog)

target_precompile_headers(pxlFramework PUBLIC src/Logger.h <iostream> <string> <memory> <unordered_map>)

target_compile_definitions(pxlFramework PUBLIC GLFW_INCLUDE_NONE)