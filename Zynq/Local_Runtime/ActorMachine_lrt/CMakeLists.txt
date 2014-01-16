cmake_minimum_required(VERSION 2.6)
project(LRT)

set (APP_MPEG4 "ON" CACHE BOOL "")
set (APP_SOBEL "" CACHE BOOL "")

if(APP_MPEG4 AND APP_SOBEL)
    MESSAGE(FATAL_ERROR "Only one application at once is allowed!")
elseif(NOT (APP_MPEG4 OR APP_SOBEL))
    MESSAGE(FATAL_ERROR "At least one application is required!")
endif()

# Getting application' sources.
if(APP_MPEG4)    
    include_directories(applications/mpeg4Decoder/Sources/include)
    include_directories(applications/mpeg4Decoder)
    file(GLOB_RECURSE app_source_files applications/mpeg4Decoder/*)
endif()

# Getting local runtime' sources.  
include_directories(lrt)
file(GLOB_RECURSE lrt_source_files lrt
    lrt/*.h lrt/*.c
) 

# Getting platform' sources. 
set (OVPSIM "ON" CACHE BOOL "")
if(OVPSIM)
	include_directories(platform_ub_ovp)
	file(GLOB_RECURSE platform_source_files platform_ub_ovp/*)
else()
	if(CMAKE_SYSTEM_NAME STREQUAL Windows)
		include_directories(platform_desktop_win)
		file(GLOB_RECURSE platform_source_files platform_desktop_win/*)
	elseif(CMAKE_SYSTEM_NAME STREQUAL Linux)
		include_directories(platform_desktop)
		file(GLOB_RECURSE platform_source_files platform_desktop/*)
	else()
		include_directories(platform_arm)
		file(GLOB_RECURSE platform_source_files platform_arm/*)
	endif(CMAKE_SYSTEM_NAME STREQUAL Windows)
endif(OVPSIM)

# Finding libraries.
set (SDL_BUILDING_LIBRARY "ON" CACHE BOOL "" FORCE)
find_package(SDL)
if(SDL_FOUND)
else()
    set (SDL_LIB_DIR "" CACHE PATH "")
endif()

link_directories(${SDL_LIB_DIR})

add_executable(LRT ${app_source_files} ${lrt_source_files} ${platform_source_files})
target_link_libraries(LRT SDL)