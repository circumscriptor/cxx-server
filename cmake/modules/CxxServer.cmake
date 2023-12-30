include_guard(GLOBAL)

#
# Prevent in-source builds
#

get_filename_component(CXXSERVER_TOP_SOURCE_DIR "${CMAKE_SOURCE_DIR}" REALPATH)
get_filename_component(CXXSERVER_TOP_BINARY_DIR "${CMAKE_BINARY_DIR}" REALPATH)

if(CXXSERVER_TOP_SOURCE_DIR STREQUAL CXXSERVER_TOP_BINARY_DIR)
    message(FATAL_ERROR "In-source builds are not allowed")
endif()

#
# Includes
#

include(Common)
include(Compiler)
include(Tools)
include(Options)

if(CXXSERVER_WITH_TESTS)
    include(CTest)
endif()

if(CXXSERVER_ENABLE_INSTALL_FHS)
    include(GNUInstallDirs)
endif()

#
# Packages
#

find_package(Threads REQUIRED)

if(CXXSERVER_ENABLE_DOCUMENTATION)
    find_package(Doxygen OPTIONAL_COMPONENTS dot)
endif()

#
# Install directories
#

if(CXXSERVER_ENABLE_INSTALL_FHS)
    set(CXXSERVER_INSTALL_BINARY_DIRECTORY "${CMAKE_INSTALL_BINDIR}")
else()
    set(CXXSERVER_INSTALL_BINARY_DIRECTORY ".")
endif()

#
# Configure CPack
#

set(CPACK_RESOURCE_FILE_LICENSE ${CXXSERVER_TOP_SOURCE_DIR}/LICENSE)
set(CPACK_RESOURCE_FILE_README ${CXXSERVER_TOP_SOURCE_DIR}/README.md)
set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")

# Include CPack
include(CPack)
