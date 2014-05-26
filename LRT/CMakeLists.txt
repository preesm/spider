cmake_minimum_required(VERSION 2.6)
project(LRT)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

set (APP_DOUBLE_LOOP "ON" CACHE BOOL "")
set (APP_MPEG4 "" CACHE BOOL "")
set (APP_SOBEL "" CACHE BOOL "")
set (APP_TEST_OVP "" CACHE BOOL "")

# if(APP_MPEG4 AND APP_SOBEL)
    # MESSAGE(FATAL_ERROR "Only one application at once is allowed!")
# elseif(NOT (APP_MPEG4 OR APP_SOBEL))
    # MESSAGE(FATAL_ERROR "At least one application is required!")
# endif()

# Getting application' sources.
if(APP_MPEG4)    
    include_directories(applications/mpeg4Decoder/Sources/include)
    include_directories(applications/mpeg4Decoder)
    file(GLOB_RECURSE app_source_files applications/mpeg4Decoder/*)
elseif(APP_TEST_OVP)
    file(GLOB_RECURSE app_source_files applications/testForOVPSim/*)
elseif(APP_DOUBLE_LOOP)
    file(GLOB_RECURSE app_source_files applications/DoubleLoop/*)
endif()

# Getting local runtime' sources.  
include_directories(lrt)
file(GLOB_RECURSE lrt_source_files lrt
    lrt/*.h lrt/*.c
) 

# Getting platform' sources. 
set (ARCH_OVPSIM "" CACHE BOOL "")
set (ARCH_Zynq "" CACHE BOOL "")
if(ARCH_OVPSIM)
	include_directories(platform_ub_ovp)
	file(GLOB_RECURSE platform_source_files platform_ub_ovp/*)
elseif(ARCH_Zynq)
	include_directories(platform_arm)
	file(GLOB_RECURSE platform_source_files platform_arm/*)
	
	find_package(BSPlibs REQUIRED)
	include_directories(${BSPlibs_INCLUDE_DIRS})
	set(LIBS ${LIBS} ${BSPlibs_LIBRARIES})
else()
	if(CMAKE_SYSTEM_NAME STREQUAL Windows)
		SET(CMAKE_MFC_FLAG 2)
		#SET (CMAKE_INSTALL_MFC_LIBRARIES "ON" CACHE BOOL "") 
		#include (InstallRequiredSystemLibraries)
		#include (FindMFC)
		include_directories(platform_desktop_win)
		file(GLOB_RECURSE platform_source_files platform_desktop_win/*)
	elseif(CMAKE_SYSTEM_NAME STREQUAL Linux)
		include_directories(platform_desktop_linux)
		file(GLOB_RECURSE platform_source_files platform_desktop/*)
	endif(CMAKE_SYSTEM_NAME STREQUAL Windows)
endif(ARCH_OVPSIM)

if(APP_MPEG4)  
	# Finding libraries.
	set (SDL_BUILDING_LIBRARY "ON" CACHE BOOL "" FORCE)
	find_package(SDL)
	if(SDL_FOUND)
	else()
		set (SDL_LIB_DIR "" CACHE PATH "")
	endif(SDL_FOUND)
	link_directories(${SDL_LIB_DIR})
endif(APP_MPEG4) 

add_executable(LRT WIN32 ${app_source_files} ${lrt_source_files} ${platform_source_files})

#add_library(xil STATIC IMPORTED)
#set_property(TARGET xil PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/../../xilinx/standalone_bsp_0/microblaze_0/lib/libxil.a)
#set(CMAKE_EXE_LINKER_FLAGS "-L${CMAKE_SOURCE_DIR}/../../xilinx/standalone_bsp_0/microblaze_0/lib")
#add_library(c STATIC IMPORTED)
#set_property(TARGET c PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/../../xilinx/standalone_bsp_0/microblaze_0/lib/libc.a)

if(APP_MPEG4)
	target_link_libraries(LRT SDL)
else()
	target_link_libraries(LRT ${LIBS})
endif(APP_MPEG4)  