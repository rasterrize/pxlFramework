project(discordgamesdk)

add_library(discordgamesdk STATIC 
src/achievement_manager.h src/achievement_manager.cpp
src/activity_manager.h src/activity_manager.cpp
src/application_manager.h src/application_manager.cpp
src/core.h src/core.cpp
src/event.h
src/ffi.h
src/image_manager.h src/image_manager.cpp
src/lobby_manager.h src/lobby_manager.cpp
src/network_manager.h src/network_manager.cpp
src/overlay_manager.h src/overlay_manager.cpp
src/relationship_manager.h src/relationship_manager.cpp
src/storage_manager.h src/storage_manager.cpp
src/store_manager.h src/store_manager.cpp
src/types.h src/types.cpp
src/user_manager.h src/user_manager.cpp
src/voice_manager.h src/voice_manager.cpp
)

target_include_directories(discordgamesdk PUBLIC include)
target_link_directories(discordgamesdk PUBLIC lib)

# Link project to the DLL lib
target_link_libraries(discordgamesdk discord_game_sdk.dll.lib)