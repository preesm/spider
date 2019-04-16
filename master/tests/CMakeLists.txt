cmake_minimum_required(VERSION 3.1)

# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -pg -Wall -Wextra -coverage")

# Link runTests with what we want to test and the GTest and pthread library
file(GLOB_RECURSE
     test_files
     ./tests/*.cpp
     ./tests/*.h)

add_executable(runTests main.cpp ${test_files})
target_link_libraries(runTests ${GTEST_LIBRARIES} pthread)
