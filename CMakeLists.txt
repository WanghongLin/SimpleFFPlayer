cmake_minimum_required(VERSION 3.5.1)
project(SimpleFFPlayer)

set(CMAKE_CXX_STANDARD 11)

include(FindPkgConfig)
if (PkgConfig_FOUND)

    set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:$ENV{HOME}/Applications/ffmpeg/lib/pkgconfig:$ENV{HOME}/Applications/SDL2-2.0.4/lib/pkgconfig")
    message("$ENV{PKG_CONFIG_PATH}")
    pkg_check_modules(FFMPEG REQUIRED libavformat libavcodec libavutil libswscale libswresample)
    if (FFMPEG_FOUND)
        include_directories(BEFORE SYSTEM ${FFMPEG_INCLUDE_DIRS})
        link_directories(${FFMPEG_LIBRARY_DIRS})
        link_libraries(${FFMPEG_LIBRARIES})
    endif (FFMPEG_FOUND)
    pkg_search_module(SDL2 REQUIRED sdl2)
    if (SDL2_FOUND)
        include_directories(BEFORE SYSTEM ${SDL2_INCLUDE_DIRS})
        link_directories(${SDL2_LIBRARY_DIRS})
        link_libraries(${SDL2_LIBRARIES})
    endif (SDL2_FOUND)
endif (PkgConfig_FOUND)

set(SOURCE_FILES main.cpp MediaFormat.cpp MediaInput.cpp MediaRender.cpp MediaUtils.cpp MediaUtils.h MediaFrame.cpp MediaFrame.h MediaFrameAvailableListener.cpp MediaFrameAvailableListener.h)
add_executable(SimpleFFPlayer ${SOURCE_FILES})
target_link_libraries(SimpleFFPlayer "pthread")

# audio player example
file(GLOB AUDIO_PLAYER_SOURCE_FILES
        ${PROJECT_SOURCE_DIR}/audioplayer/*.h
        ${PROJECT_SOURCE_DIR}/audioplayer/*.cpp)
add_executable(audioplayer ${AUDIO_PLAYER_SOURCE_FILES})
target_link_libraries(audioplayer "pthread")
