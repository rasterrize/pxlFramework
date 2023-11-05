cmake_minimum_required(VERSION 3.23)

project(TestApp)

add_executable(TestApp src/Entrypoint.cpp src/TestApplication.h src/TestApplication.cpp)

target_link_libraries(TestApp pxlFramework)

set_target_properties(TestApp PROPERTIES WIN32_EXECUTABLE $<$<CONFIG:Release>:TRUE>)

target_compile_features(TestApp PUBLIC cxx_std_20)

target_compile_definitions(TestApp PUBLIC $<$<CONFIG:Debug>:TA_DEBUG> $<$<CONFIG:Release>:TA_RELEASE>)#$<$<CONFIG:Distribute>:TA_DIST>#

set_target_properties(TestApp PROPERTIES CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")