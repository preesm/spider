cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (${CROSS_COMPILE_MINGW})
    set(CMAKE_SYSTEM_NAME Windows)
    set(TOOLCHAIN_PREFIX x86_64-w64-mingw32)

    # cross compilers to use for C and C++
    set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
    set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++)
    set(CMAKE_Fortran_COMPILER ${TOOLCHAIN_PREFIX}-gfortran)
    set(CMAKE_RC_COMPILER ${TOOLCHAIN_PREFIX}-windres)

    # target environment on the build host system
    #   set 1st to dir with the cross compiler's C/C++ headers/libs
    set(CMAKE_FIND_ROOT_PATH /usr/${TOOLCHAIN_PREFIX})

    # modify default behavior of FIND_XXX() commands to
    # search for headers/libs in the target environment and
    # search for programs in the build host environment
    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
endif ()


# Macro to get short file name (vs full path) at compile time
# The additional / is important to remove the last character from the path.
# Note that it does not matter if the OS uses / or \, because we are only
# saving the path size.
string(LENGTH "${CMAKE_SOURCE_DIR}/" SOURCE_PATH_SIZE)
add_definitions("-DSOURCE_PATH_SIZE=${SOURCE_PATH_SIZE}")
add_definitions("-D__FILENAME__=(__FILE__ + ${SOURCE_PATH_SIZE})")

# *******************************************
# ************* CMake Content ***************
# *******************************************
# This CMake create a workspace containing the following projects
# 
# Programs
#  - spider

# IDE dependent config
if (${CMAKE_GENERATOR} MATCHES "Visual Studio.*")
    MESSAGE("Add definition for ${CMAKE_GENERATOR}")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif ()

if (${CMAKE_GENERATOR} MATCHES "MinGW Makefiles")
    MESSAGE("Add definition for ${CMAKE_GENERATOR}")
    #To prevent a redefinition conflict
    add_definitions(-D_TIMESPEC_DEFINED)
endif ()

# Add definition for relative path into project
add_definitions(-DPROJECT_ROOT_PATH="${CMAKE_CURRENT_SOURCE_DIR}")

project(Spider)

# Set to 1 to activate debug compilation (0 for release)

if (NOT ${CMAKE_GENERATOR} MATCHES "Visual Studio.*")
    set(DEBUG 1)

    if (${DEBUG})
        MESSAGE("Generate Debug project")
        set(CMAKE_BUILD_TYPE Debug)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Debug)
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g3 -pg -Wall -Wextra")
        MESSAGE("CXX_FLAGS: " ${CMAKE_CXX_FLAGS_DEBUG})
    else ()
        MESSAGE("Generate Release project")
        set(CMAKE_BUILD_TYPE Release)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Release)
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -Wextra")
        if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
            set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto")
        endif ()
        MESSAGE("CXX_FLAGS: " ${CMAKE_CXX_FLAGS_RELEASE})
    endif ()
else ()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif ()

if (WIN32)
    set(LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)
endif ()
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/lib/cmake_modules/")

# *******************************************
# ************ Pthread LIBRARY **************
# *******************************************
if (WIN32 AND NOT MINGW)
    set(THREADS_USE_PTHREADS_WIN32 true)
    # pthread included AFTER Sdl to avoid unnecessary warnings
    file(GLOB PTHREADDIR "${LIBS_DIR}/pthread-[\\.|0-9]*")
    set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${PTHREADDIR})
    Find_Package(Threads REQUIRED)
else ()
    Find_Package(Threads REQUIRED)
endif ()

if (NOT THREADS_FOUND)
    MESSAGE(FATAL_ERROR "Pthread not found !")
endif ()


if (WIN32)
    file(GLOB
            PTHREADS_DLL
            ${CMAKE_PREFIX_PATH}/lib/*.dll
            )

    MESSAGE("Copy Pthreads DLLs into ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    if (NOT ${CMAKE_GENERATOR} MATCHES "Visual Studio.*")
        file(COPY ${PTHREADS_DLL} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    else ()
        file(COPY ${PTHREADS_DLL} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug)
        file(COPY ${PTHREADS_DLL} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release)
    endif ()
endif ()

# *******************************************
# ************ PAPI LIBRARY *****************
# *******************************************

if (${SKIP_PAPI})
    MESSAGE("Skipping Papi")
else ()
    find_package(PAPI)
endif ()

if (PAPI_FOUND)
    add_definitions(-DPAPI_AVAILABLE)
    set(papify_sources
            libspider/papify/PapifyAction.cpp
            libspider/papify/PapifyAction.h
            libspider/papify/PapifyEventLib.cpp
            libspider/papify/PapifyEventLib.h)
else ()
    set(PAPI_LIBRARIES "")
    set(PAPI_INCLUDE_DIRS "")
endif ()


# *******************************************
# *************  spider.dll/a  **************
# *******************************************

include_directories(
        #${THREADS_PTHREADS_INCLUDE_DIR}
        ${PAPI_INCLUDE_DIRS}
        ${PTHREADDIR}/include
        ./libspider/
        ./libspider/common
        ./libspider/lrt
        ./libspider/platforms/platform_pthread
#        ./libspider/platforms/platform_mppa
        ./libspider/spider
)

file(
        GLOB_RECURSE
        source_files
        libspider/spider-api/spider/*.cpp
        ./libspider/common/*.cpp
        ./libspider/common/tools/*.cpp
        ./libspider/lrt/*.cpp
        ./libspider/lrt/specialActors/*.cpp
        ./libspider/platforms/platform_pthread/*.cpp
#        ./libspider/platforms/platform_mppa/*.cpp
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
        libspider/spider-api/user/*.h
        ./libspider/common/*.h
        ./libspider/common/tools/*.h
        ./libspider/lrt/*.h
        ./libspider/lrt/specialActors/*.h
        ./libspider/platforms/platform_pthread/*.h
#        ./libspider/platforms/platform_mppa/*.h
        ./libspider/spider/platform.h
        ./libspider/spider/platformPThread.h
#        ./libspider/spider/platformMPPA.h
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


add_library(Spider SHARED ${source_files} ${header_files} ${papify_sources})
target_link_libraries(Spider ${CMAKE_THREAD_LIBS_INIT} ${PAPI_LIBRARIES})
set(CMAKE_CXX_FLAGS "-fpermissive")

if (NOT MINGW AND NOT ${CMAKE_GENERATOR} MATCHES "Visual Studio.*")
    set(CMAKE_CXX_FLAGS "-fPIC")
    if (${32BITS})
        set_target_properties(Spider PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
    else ()
        if (${64BITS})
            set_target_properties(Spider PROPERTIES COMPILE_FLAGS "-m64" LINK_FLAGS "-m64")
        endif ()
    endif ()
endif ()
