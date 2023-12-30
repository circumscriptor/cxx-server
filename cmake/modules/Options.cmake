include_guard(GLOBAL)

#
# Include modules
#

include(CMakeDependentOption)

#
# Project features
#

option(CXXSERVER_WITH_DEV_VALUES   "Build with dev values"          OFF)
option(CXXSERVER_WITH_TESTS        "Build with tests"               OFF)

#
# Project dependent build options
#

cmake_dependent_option(CXXSERVER_WITH_ASAN   "Build with address sanitizer"            OFF "NOT WIN32" OFF)
cmake_dependent_option(CXXSERVER_WITH_LSAN   "Build with leak sanitizer"               OFF "NOT WIN32" OFF)
cmake_dependent_option(CXXSERVER_WITH_MSAN   "Build with memory sanitizer"             OFF "NOT WIN32" OFF)
cmake_dependent_option(CXXSERVER_WITH_SCOV   "Build with source coverage"              OFF "NOT WIN32" OFF)
cmake_dependent_option(CXXSERVER_WITH_TSAN   "Build with thread sanitizer"             OFF "NOT WIN32" OFF)
cmake_dependent_option(CXXSERVER_WITH_UBSAN  "Build with undefined behavior sanitizer" OFF "NOT WIN32" OFF)
cmake_dependent_option(CXXSERVER_WITH_LIBCXX "Build with libc++"                       OFF "NOT WIN32" OFF)

#
# Project build options
#

option(CXXSERVER_ENABLE_CLANG_TIDY           "Enable Clang-Tidy code analysis tool"         OFF)
option(CXXSERVER_ENABLE_CPPCHECK             "Enable Cppcheck code analysis tool"           OFF)
option(CXXSERVER_ENABLE_DOCUMENTATION        "Enable doxygen documentation generator"       OFF)
option(CXXSERVER_ENABLE_INCLUDE_WHAT_YOU_USE "Enable Include What You Use include analyzer" OFF)
option(CXXSERVER_ENABLE_INSTALL_FHS          "Install with standard hierarchy layout"       OFF)
option(CXXSERVER_EXPORT_THIRD_PARTY_COMMANDS "Export third-party targets compile commands"  OFF)
option(CXXSERVER_WITH_FAST_OPTIMIZATION      "Build with -Ofast flag"                       OFF)
option(CXXSERVER_WITH_NATIVE_ARCHITECTURE    "Build with host native extensions"            OFF)
option(CXXSERVER_WITH_SECURE_ALLOCATOR       "Build with secure allocator"                  OFF)
option(CXXSERVER_WITH_STACK_PROTECTOR        "Build with stack protector"                   OFF)

#
# Forced project options
#

set(CXXSERVER_APPLICATION_NAME "CxxServer"              CACHE STRING   "Internal application name" FORCE)
set(CXXSERVER_FRAMEWORK_NAME   "CxxServer"              CACHE STRING   "Internal framework name"   FORCE)
set(CXXSERVER_VERSION          ${PROJECT_VERSION}       CACHE INTERNAL "Project version"           FORCE)
set(CXXSERVER_VERSION_MAJOR    ${PROJECT_VERSION_MAJOR} CACHE INTERNAL "Project version"           FORCE)
set(CXXSERVER_VERSION_MINOR    ${PROJECT_VERSION_MINOR} CACHE INTERNAL "Project version"           FORCE)
set(CXXSERVER_VERSION_PATCH    ${PROJECT_VERSION_PATCH} CACHE INTERNAL "Project version"           FORCE)

#
# CMake build options
#

option(CMAKE_DISABLE_PRECOMPILE_HEADERS    "Disable precompiling headers"         OFF)
option(CMAKE_EXPORT_COMPILE_COMMANDS       "Export compile database"              OFF)
option(CMAKE_INTERPROCEDURAL_OPTIMIZATION  "Enable LTO for all targets"           OFF)
option(CMAKE_POSITION_INDEPENDENT_CODE     "Enable PIC for all targets"           OFF)
option(CMAKE_UNITY_BUILD                   "Enable unity build"                   OFF)

set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE INTERNAL "Installation prefix")

#
# Forced CMake build options
#

set(CMAKE_COLOR_DIAGNOSTICS          ON                        CACHE INTERNAL "Enable make/compiler colors"         FORCE)
set(CMAKE_COMPILE_WARNING_AS_ERROR   ON                        CACHE INTERNAL "Treat warnings on compile as errors" FORCE)
set(CMAKE_CXX_EXTENSIONS             OFF                       CACHE INTERNAL "Allow C++ extensions"                FORCE)
set(CMAKE_CXX_STANDARD_REQUIRED      ON                        CACHE INTERNAL "Require C++ standard"                FORCE)
set(CMAKE_VISIBILITY_INLINES_HIDDEN  ON                        CACHE INTERNAL "Hide symbols of inline functions"    FORCE)
set(CMAKE_C_VISIBILITY_PRESET        "hidden"                  CACHE INTERNAL "Symbol visibility C compiler"        FORCE)
set(CMAKE_CXX_VISIBILITY_PRESET      "hidden"                  CACHE INTERNAL "Symbol visibility C++ compiler"      FORCE)
set(CMAKE_DEBUG_POSTFIX              "-debug"                  CACHE INTERNAL "Debug postfix"                       FORCE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY   "${CMAKE_BINARY_DIR}/lib" CACHE INTERNAL "(.lib .a  ) output directory"        FORCE)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY   "${CMAKE_BINARY_DIR}/bin" CACHE INTERNAL "(.so      ) output directory"        FORCE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY   "${CMAKE_BINARY_DIR}/bin" CACHE INTERNAL "(.exe .dll) output directory"        FORCE)
set(CMAKE_PDB_OUTPUT_DIRECTORY       "${CMAKE_BINARY_DIR}/pdb" CACHE INTERNAL "(.pdb     ) output directory"        FORCE)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS OFF                       CACHE INTERNAL "Create file with all global symbols" FORCE)

#
# Forced project directories
#

set(CXXSERVER_BUILD_TOOLS_DIRECTORY   "${CMAKE_BINARY_DIR}/build-tools"           CACHE INTERNAL "Compiled build-tools output directory"    FORCE)
set(CXXSERVER_DOCUMENTATION_DIRECTORY "${CMAKE_BINARY_DIR}/docs"                  CACHE INTERNAL "Generated documentation output directory" FORCE)
set(CXXSERVER_CMAKE_DIRECTORY         "${PROJECT_SOURCE_DIR}/cmake"               CACHE INTERNAL "Project cmake sources directory"          FORCE)
set(CXXSERVER_SCRIPTS_DIRECTORY       "${CXXSERVER_CMAKE_DIRECTORY}/scripts"      CACHE INTERNAL "Project cmake scripts directory"          FORCE)
set(CXXSERVER_SUPPRESSIONS_DIRECTORY  "${CXXSERVER_CMAKE_DIRECTORY}/suppressions" CACHE INTERNAL "Project suppressions directory"           FORCE)
set(CXXSERVER_TEMPLATES_DIRECTORY     "${CXXSERVER_CMAKE_DIRECTORY}/templates"    CACHE INTERNAL "Project code templates directory"         FORCE)
set(CXXSERVER_SOURCE_DIRECTORY        "${PROJECT_SOURCE_DIR}/source"              CACHE INTERNAL "Project source directory"                 FORCE)
set(CXXSERVER_BINARY_DIRECTORY        "${PROJECT_BINARY_DIR}/source"              CACHE INTERNAL "Project binary directory"                 FORCE)
set(CXXSERVER_THIRD_PARTY_DIRECTORY   "${PROJECT_SOURCE_DIR}/third-party"         CACHE INTERNAL "Project third-party source directory"     FORCE)
