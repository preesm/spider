cmake_minimum_required(VERSION 2.6)
project(AM_GRT)

set(CURSES_NEED_NCURSES TRUE)
find_package(Curses)

if(CURSES_FOUND)
else()
    set (NCURSES_INCLUDE_DIR "" CACHE PATH "")
    set (NCURSES_LIB_DIR "" CACHE PATH "")
    include_directories(${NCURSES_INCLUDE_DIR})
    link_directories(${NCURSES_LIB_DIR})
endif()

if(MSVC)
    add_definitions(-DMSVC)
    message(STATUS ${CMAKE_SYSTEM})
endif()



    
if(CMAKE_SYSTEM_NAME STREQUAL Windows)
    include_directories(platform_desktop_win)
    file(GLOB_RECURSE platform_source_files platform_desktop_win/*)
elseif(CMAKE_SYSTEM_NAME STREQUAL Linux)
    include_directories(platform_desktop)
    file(GLOB_RECURSE platform_source_files platform_desktop/*)
else()
    include_directories(platform_arm)
    file(GLOB_RECURSE platform_source_files platform_arm/*)
endif()

include_directories(grt)
file(GLOB_RECURSE grt_source_files
	grt/*.h grt/*.cpp 
)

add_executable(AM_GRT ${platform_source_files} ${grt_source_files})
target_link_libraries(AM_GRT ncurses)