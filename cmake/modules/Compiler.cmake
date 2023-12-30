include_guard(GLOBAL)

#
# Detect compiler
#

message(CHECK_START "Detecting compiler ID")

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    if(CMAKE_CXX_SIMULATE_ID STREQUAL "MSVC")
        set(CXXSERVER_COMPILER_CLANG_CL ON CACHE INTERNAL "")
        message(CHECK_PASS "found clang-cl")
    else()
        set(CXXSERVER_COMPILER_CLANG ON CACHE INTERNAL "")
        message(CHECK_PASS "found clang")
    endif()
else()
    message(CHECK_FAIL "unsupported compiler")
endif()

#
# Get compiler specific include directory
#

message(CHECK_START "Detecting compiler include directory")

if(CXXSERVER_COMPILER_CLANG OR CXXSERVER_COMPILER_CLANG_CL)
    execute_process(
        COMMAND ${CMAKE_CXX_COMPILER} --print-resource-dir
        RESULT_VARIABLE RESULT
        OUTPUT_VARIABLE OUTPUT
    )

    if(RESULT EQUAL 0)
        string(REGEX REPLACE "\n$" "" OUTPUT "${OUTPUT}")
        cmake_path(APPEND COMPILER_INCLUDE_DIR "${OUTPUT}" "include")
        get_filename_component(COMPILER_INCLUDE_DIR "${COMPILER_INCLUDE_DIR}" REALPATH)
        set(CXXSERVER_COMPILER_INCLUDE_DIR "${COMPILER_INCLUDE_DIR}" CACHE INTERNAL "Compiler-specific include directory")
        message(CHECK_PASS "found \"${COMPILER_INCLUDE_DIR}\"")
    else()
        message(CHECK_FAIL "command failed with code ${RESULT}")
    endif()
else()
    message(CHECK_FAIL "unsupported compiler")
endif()
