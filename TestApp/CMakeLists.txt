cmake_minimum_required(VERSION 3.23)

project(TestApp)

add_executable(TestApp src/Entrypoint.cpp src/TestApplication.h src/TestApplication.cpp)

target_include_directories(TestApp PUBLIC ${CMAKE_SOURCE_DIR}/pxlFramework)

target_link_libraries(TestApp pxlFramework)