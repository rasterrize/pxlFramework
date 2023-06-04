cmake_minimum_required(VERSION 3.23)

project(TestApp)

add_executable(TestApp src/Entrypoint.cpp src/TestApplication.h src/TestApplication.cpp) #[[WIN32]]

target_include_directories(TestApp PUBLIC ${CMAKE_SOURCE_DIR}/pxlFramework/include)

target_link_libraries(TestApp pxlFramework)

#target_compile_definitions(TestApp PUBLIC PXL_RELEASE)