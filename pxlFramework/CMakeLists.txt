cmake_minimum_required(VERSION 3.23)
project(pxlFramework)

add_library(pxlFramework STATIC 
# SRC
pxl.h

# CORE
src/Application.h src/Application.cpp
src/Window.h src/Window.cpp

# RENDERER
)

add_subdirectory(deps/GLFW)
add_subdirectory(deps/Glad)

target_link_libraries(pxlFramework glfw glad)

target_precompile_headers(pxlFramework PUBLIC <iostream> <string> <memory>)

target_compile_definitions(pxlFramework PUBLIC GLFW_INCLUDE_NONE)