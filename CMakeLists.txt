cmake_minimum_required(VERSION 3.24)

#
#
# cxx-server
#
#

project(CxxServer)

#
# Options
#

option(BUILD_SAMPLES "Build demo targets" OFF)
option(BUILD_TESTS "Build test targets" ON)
option(BUILD_SHARED_LIBS "Build libraries as shared if allowed" ON)
option(CMAKE_COMPILE_WARNING_AS_ERROR "Treat warnings on compile as errors" ON)
option(CMAKE_DISABLE_PRECOMPILE_HEADERS "Disable precompiling header files" OFF)
option(CMAKE_INTERPROCEDURAL_OPTIMIZATION "Enable interprocedural optimization for all targets" ON)
option(CMAKE_POSITION_INDEPENDENT_CODE "Make all targets position-independent" ON)
option(CMAKE_UNITY_BUILD "Enable batch compilation of multiple sources" ON)
option(CMAKE_VISIBILITY_INLINES_HIDDEN "Hide symbols of inline functions" ON)
option(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS "Create file with all global symbols (Windows only)" ON)

set(CMAKE_C_VISIBILITY_PRESET "hidden" CACHE STRING "Value for symbol visibility compile flags for C")
set(CMAKE_CXX_VISIBILITY_PRESET "hidden" CACHE STRING "Value for symbol visibility compile flags for C++")

#
# Include modules
#

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake/Modules")

#
# Import dependencies
#

set(CONAN_DISABLE_CHECK_COMPILER ON)
include(${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

#
# Find packages
#

find_package(Doxygen OPTIONAL_COMPONENTS dot)

#
# Add sources
#

add_subdirectory(Source)

#
# Add documentation
#

if(DOXYGEN_FOUND)
    add_subdirectory(Docs)
endif()
