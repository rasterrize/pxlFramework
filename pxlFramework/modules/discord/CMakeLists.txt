cmake_minimum_required(VERSION 3.10.0)
project(pxlFramework-discord VERSION 0.0.1 LANGUAGES CXX)

add_library(pxlFramework-discord STATIC src/DiscordRPC.h src/DiscordRPC.cpp)

target_compile_features(pxlFramework-discord PRIVATE cxx_std_20)

add_subdirectory(deps/discord-game-sdk)

target_include_directories(pxlFramework-discord PUBLIC src)
target_link_libraries(pxlFramework-discord discordgamesdk)

target_precompile_headers(pxlFramework-discord REUSE_FROM pxlFramework)

if(MSVC)
    target_compile_options(discordgamesdk PUBLIC "/MP")
endif()

file(COPY deps/discord-game-sdk/lib/discord_game_sdk.dll DESTINATION ${CMAKE_BINARY_DIR}/bin)