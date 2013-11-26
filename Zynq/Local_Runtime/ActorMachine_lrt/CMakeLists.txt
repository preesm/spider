cmake_minimum_required(VERSION 2.6)
project(AM_LRT)

set (APP_MPEG4 "ON" CACHE BOOL "")
set (APP_SOBEL "" CACHE BOOL "")

if(APP_MPEG4 AND APP_SOBEL)
    MESSAGE(FATAL_ERROR "Only one application at once is allowed!")
elseif(NOT (APP_MPEG4 OR APP_SOBEL))
    MESSAGE(FATAL_ERROR "At least one application is required!")
endif()

if(CMAKE_SYSTEM_NAME STREQUAL Windows)
    if(APP_MPEG4)    
        include_directories(actors_decod_mpeg_desktop_win)
        include_directories(actors_decod_mpeg_desktop_win/Sources/include)
        file(GLOB_RECURSE app_source_files actors_decod_mpeg_desktop_win/*)
    endif()

    include_directories(platform_desktop_win)
    file(GLOB_RECURSE platform_source_files platform_desktop_win/*)
elseif(CMAKE_SYSTEM_NAME STREQUAL Linux)
    include_directories(platform_desktop)
    file(GLOB_RECURSE platform_source_files platform_desktop/*)
else()
    include_directories(platform_arm)
    file(GLOB_RECURSE platform_source_files platform_arm/*)
endif(CMAKE_SYSTEM_NAME STREQUAL Windows)
    
include_directories(lrt)
file(GLOB_RECURSE lrt_source_files lrt
    lrt/*.h lrt/*.c
)

set (SDL_BUILDING_LIBRARY "ON" CACHE BOOL "")
find_package(SDL)
if(SDL_FOUND)
else()
    set (SDL_LIB_DIR "" CACHE PATH "")
    link_directories(${SDL_LIB_DIR})
endif()

add_executable(AM_LRT ${app_source_files} ${platform_source_files} ${lrt_source_files})
target_link_libraries(AM_LRT SDL)