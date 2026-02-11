project(stb)

add_library(stb STATIC stb_image.h stb_image.cpp stb_image_write.h stb_image_write.cpp)

target_include_directories(stb PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# To fix sprintf security warning
if(WIN32)
    target_compile_definitions(stb PRIVATE __STDC_LIB_EXT1__)
endif()