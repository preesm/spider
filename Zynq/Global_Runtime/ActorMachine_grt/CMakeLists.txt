cmake_minimum_required(VERSION 2.6)
project(AM_GRT)

# Selecting the application to be built.
set (APP_MPEG4 "ON" CACHE BOOL "")
set (APP_SOBEL "" CACHE BOOL "")

if(APP_MPEG4 AND APP_SOBEL)
    MESSAGE(FATAL_ERROR "Only one application at once is allowed!")
elseif(NOT (APP_MPEG4 OR APP_SOBEL))
    MESSAGE(FATAL_ERROR "At least one application is required!")
endif()

# Getting the application' sources.
if(APP_MPEG4)    
    #include_directories(applications/mpeg4Decoder)
    file(GLOB_RECURSE app_source_files applications/mpeg4Decoder/*)
endif()

# Getting global runtime' sources.
include_directories(grt)
file(GLOB_RECURSE grt_source_files
	grt/*.h grt/*.cpp 
)

# Getting platform' sources.
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

# Compiling and linking.
add_executable(AM_GRT ${app_source_files} ${grt_source_files} ${platform_source_files})
target_link_libraries(AM_GRT)