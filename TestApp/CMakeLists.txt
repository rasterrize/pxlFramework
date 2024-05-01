cmake_minimum_required(VERSION 3.23)

project(TestApp)

add_executable(TestApp src/Entrypoint.cpp src/TestApplication.h src/TestApplication.cpp 

# TESTS
src/Tests/OGLVK.h src/Tests/OGLVK.cpp
src/Tests/ModelViewer.h src/Tests/ModelViewer.cpp
src/Tests/CubesTest.h src/Tests/CubesTest.cpp
src/Tests/OGLVK.h src/Tests/OGLVK.cpp
src/Tests/QuadsTest.h src/Tests/QuadsTest.cpp)

target_link_libraries(TestApp pxlFramework)

#set_target_properties(TestApp PROPERTIES WIN32_EXECUTABLE $<$<CONFIG:Dist>:TRUE>)

target_compile_features(TestApp PUBLIC cxx_std_20)

target_compile_definitions(TestApp PUBLIC $<$<CONFIG:Debug>:TA_DEBUG> $<$<CONFIG:Release>:TA_RELEASE>)#$<$<CONFIG:Distribute>:TA_DIST>#

set_target_properties(TestApp PROPERTIES CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")