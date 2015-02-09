cmake_minimum_required(VERSION 2.8)

project(ederc_nvar)

set(GIT_REPO /home/jheulot/git/COMPA-Runtime/GRT)

# Getting global runtime's sources.
include_directories(${GIT_REPO}/grt)
file(GLOB_RECURSE grt_source_files
	${GIT_REPO}/grt/*.h ${GIT_REPO}/grt/*.cpp 
)

# Getting platform' sources.
include_directories(${GIT_REPO}/platform_include)
include_directories(${GIT_REPO}/platform_desktop_linux)
file(GLOB_RECURSE platform_source_files
	${GIT_REPO}/platform_desktop_linux/*.h ${GIT_REPO}/platform_desktop_linux/*.cpp 
)

include_directories(.)
file(GLOB applications_source_files
	./*.h ./*.cpp
)

add_executable(ederc_nvar ${grt_source_files} ${platform_source_files} ${applications_source_files})
