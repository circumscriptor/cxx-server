cmake_minimum_required(VERSION 3.27)

#
#
# cxx-server
#
#

project(
    cxx-server
    VERSION 0.1.0
    LANGUAGES C CXX
)

#
# Include modules
#

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/modules")
include(CxxServer)

#
# Add third-party
#

add_subdirectory(third-party)

#
# Add sources
#

add_subdirectory(source)
