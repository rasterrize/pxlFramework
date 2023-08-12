cmake_minimum_required(VERSION 3.23)

project(TestApp)

add_executable(TestApp src/Entrypoint.cpp src/TestApplication.h src/TestApplication.cpp)

target_include_directories(TestApp PUBLIC ${CMAKE_SOURCE_DIR}/pxlFramework/include)

target_link_libraries(TestApp pxlFramework)

set_target_properties(TestApp PROPERTIES WIN32_EXECUTABLE TA_DIST)

target_compile_definitions(TestApp PUBLIC $<$<CONFIG:Debug>:TA_DEBUG> $<$<CONFIG:Release>:TA_RELEASE>) # TA_DIST

set_target_properties(TestApp PROPERTIES CMAKE_MSVC_RUNTIME_LIBRARY "MultithreadedDLL")