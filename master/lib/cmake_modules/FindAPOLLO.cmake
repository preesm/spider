# Try to find APOLLO headers and libraries.
#
# Usage of this module as follows:
#
#     find_package(APOLLO)
#
# Variables used by this module, they can change the default behaviour and need
# to be set before calling find_package:
#
#  APOLLO_PREFIX       Set this variable to the root installation of
#                      libapolloRuntime if the module has problems finding the
#                      proper installation path.
#
# Variables defined by this module:
#
#  APOLLO_FOUND              System has APOLLO libraries and headers
#  APOLLO_LIBRARIES          The APOLLO library
#  APOLLO_INCLUDE_DIRS       The location of APOLLO headers

find_path(APOLLO_PREFIX
    NAMES include/apolloAPI.h
)

find_library(APOLLO_LIBRARIES
    NAMES libapolloRuntime.so apolloRuntime
    HINTS ${APOLLO_PREFIX}/lib ${HILTIDEPS}/lib
)

find_path(APOLLO_INCLUDE_DIRS
    NAMES apolloAPI.h
    HINTS ${APOLLO_PREFIX}/include ${HILTIDEPS}/include
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(APOLLO DEFAULT_MSG
    APOLLO_LIBRARIES
    APOLLO_INCLUDE_DIRS
)

mark_as_advanced(
    APOLLO_PREFIX_DIRS
    APOLLO_LIBRARIES
    APOLLO_INCLUDE_DIRS
)
