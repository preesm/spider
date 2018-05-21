cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# *******************************************
# ************* CMake Content ***************
# *******************************************
# This CMake create a workspace containing the following projects
# 
# Programs
#  - spider

# IDE dependent config
if(${CMAKE_GENERATOR} MATCHES "Visual Studio.*")
	MESSAGE("Add definition for ${CMAKE_GENERATOR}")
	add_definitions( -D_CRT_SECURE_NO_WARNINGS )
endif()

if(${CMAKE_GENERATOR} MATCHES "MinGW Makefiles")
	MESSAGE("Add definition for ${CMAKE_GENERATOR}")
	#To prevent a redefinition conflict
	add_definitions( -D_TIMESPEC_DEFINED )
endif()

# Add definition for relative path into project
add_definitions( -DPROJECT_ROOT_PATH="${CMAKE_CURRENT_SOURCE_DIR}")

project(Spider)

# Set to 1 to activate debug compilation (0 for release)

if(NOT ${CMAKE_GENERATOR} MATCHES "Visual Studio.*")
	set(DEBUG 0)

	if(${DEBUG})
			MESSAGE("Generate Debug project")
			set(CMAKE_BUILD_TYPE Debug)
			set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Debug)
			set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -pg -Wall")
	else()
			MESSAGE("Generate Release project")
			set(CMAKE_BUILD_TYPE Release)
			set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Release)
			set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -Wall")
	endif()
else()
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

if(WIN32)
	set(LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)
endif()
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/lib/cmake_modules/")

# *******************************************
# ************ Pthread LIBRARY **************
# *******************************************
if (WIN32)
  set(THREADS_USE_PTHREADS_WIN32 true)
	# pthread included AFTER Sdl to avoid unnecessary warnings
	file(GLOB PTHREADDIR "${LIBS_DIR}/pthread-[\\.|0-9]*")
	set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${PTHREADDIR})
	Find_Package ( THREADS REQUIRED )
else()
	Find_Package ( Threads REQUIRED )
endif()

if(NOT THREADS_FOUND)
	MESSAGE(FATAL_ERROR "Pthread not found !")
endif()


if (WIN32)
	file(GLOB
		PTHREADS_DLL
		${CMAKE_PREFIX_PATH}/lib/*.dll
	)

	MESSAGE("Copy Pthreads DLLs into ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
	if(NOT ${CMAKE_GENERATOR} MATCHES "Visual Studio.*")
		file(COPY ${PTHREADS_DLL} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
	else()
		file(COPY ${PTHREADS_DLL} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug)
		file(COPY ${PTHREADS_DLL} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release)
	endif()
endif()

# *******************************************
# ************ PAPI LIBRARY *****************
# *******************************************

find_library(papi_LIBRARY papi)

if (NOT papi_LIBRARY)
	MESSAGE("papi lib NOT found")
else()
	MESSAGE("FOUND: papi lib")
    add_definitions(-DPAPI_AVAILABLE)
endif()


# *******************************************
# *************  spider.dll/a  **************
# *******************************************

include_directories(
	#${THREADS_PTHREADS_INCLUDE_DIR}
	${PTHREADDIR}/include
	./libspider/common
	./libspider/lrt
	./libspider/platforms/platform_pthread
	./libspider/spider
)

file(
	GLOB_RECURSE
	source_files
	./libspider/common/*.cpp
	./libspider/common/tools/*.cpp
	./libspider/lrt/*.cpp
	./libspider/lrt/specialActors/*.cpp
	./libspider/platforms/platform_pthread/*.cpp
	./libspider/spider/*.cpp
	./libspider/spider/graphs/Archi/*.cpp
	./libspider/spider/graphs/Bipartite/*.cpp
	./libspider/spider/graphs/PiSDF/*.cpp
	./libspider/spider/graphs/SDRAG/*.cpp
	./libspider/spider/graphTransfo/*.cpp
	./libspider/spider/launcher/*.cpp
	./libspider/spider/monitor/*.cpp
	./libspider/spider/parser/*.cpp
	./libspider/spider/scheduling/*.cpp
	./libspider/spider/scheduling/MemAlloc/*.cpp
	./libspider/spider/scheduling/Scheduler/*.cpp
)

file(
	GLOB_RECURSE
	header_files
	./libspider/common/*.h
	./libspider/common/tools/*.h
	./libspider/lrt/*.h
	./libspider/lrt/specialActors/*.h
	./libspider/platforms/platform_pthread/*.h
	./libspider/spider/platform.h
	./libspider/spider/platformPThread.h
	./libspider/spider/spider.h
	./libspider/spider/graphs/Archi/*.h
	./libspider/spider/graphs/Bipartite/*.h
	./libspider/spider/graphs/PiSDF/*.h
	./libspider/spider/graphs/SDRAG/*.h
	./libspider/spider/graphTransfo/*.h
	./libspider/spider/launcher/*.h
	./libspider/spider/monitor/*.h
	./libspider/spider/parser/*.h
	./libspider/spider/scheduling/*.h
	./libspider/spider/scheduling/MemAlloc/*.h
	./libspider/spider/scheduling/Scheduler/*.h
)


add_library(Spider SHARED ${source_files} ${header_files})
target_link_libraries(Spider ${CMAKE_THREAD_LIBS_INIT})
if (papi_LIBRARY)
    target_link_libraries(Spider ${papi_LIBRARY})
endif()
