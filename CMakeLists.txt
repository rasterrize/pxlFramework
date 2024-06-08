cmake_minimum_required(VERSION 3.23)
project(pxlFramework)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib) # Output directory for static lib (.LIB)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) # Output directory for shared lib (.DLL)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) # Output directory for executables (.EXE)

option(PXL_BUILD_TESTS "Build pxlFramework TestApp/Tests" ON)

if (PXL_BUILD_TESTS)
    add_subdirectory(TestApp)
endif()

add_subdirectory(pxlFramework)