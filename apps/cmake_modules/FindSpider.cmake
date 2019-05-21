SET(SPIDER_SYSTEM_SEARCH_PATHS
	~/Library/Frameworks
	/Library/Frameworks
	/usr/local
	/usr
	/sw # Fink
	/opt/local # DarwinPorts
	/opt/csw # Blastwave
	/opt
)

IF(${DEBUG})
	MESSAGE("[SPIDER] Looking for Debug version of Spider")
	SET(SPIDER_SEARCH_PATHS
	  ${SPIDER_SYSTEM_SEARCH_PATHS}
		../../../master/cmake-build-debug
		../../../master/libspider/spider
	)
ELSE()
	MESSAGE("[SPIDER] Looking for Release version of Spider")
	SET(SPIDER_SEARCH_PATHS
		${SPIDER_SYSTEM_SEARCH_PATHS}
		../../../master/cmake-build-release
		../../../master/libspider/spider
	)
ENDIF()

FIND_PATH(SPIDER_INCLUDE_DIR spider.h
	HINTS
	$ENV{SPIDERDIR}
	PATH_SUFFIXES include
	PATHS ${SPIDER_SEARCH_PATHS}
)
MESSAGE("[SPIDER] Includes dir: ${SPIDER_INCLUDE_DIR}")

FIND_LIBRARY(SPIDER_LIBRARY
	NAMES Spider
	HINTS
	$ENV{SPIDERDIR}
	PATH_SUFFIXES lib
	PATHS ${SPIDER_SEARCH_PATHS}
)
MESSAGE("[SPIDER] Library file: ${SPIDER_LIBRARY}")

INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(SPIDER REQUIRED_VARS SPIDER_LIBRARY SPIDER_INCLUDE_DIR)
