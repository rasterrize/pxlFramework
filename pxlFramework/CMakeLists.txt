cmake_minimum_required(VERSION 3.23)
project(pxlFramework)

add_library(pxlFramework STATIC 
pxl.h 
src/Application.h src/Application.cpp 
src/Window.h src/Window.cpp)

add_subdirectory(deps/GLFW)

target_link_libraries(pxlFramework glfw)

target_precompile_headers(pxlFramework PUBLIC <string>)