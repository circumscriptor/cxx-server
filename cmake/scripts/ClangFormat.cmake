cmake_minimum_required(VERSION 3.27)

#
# Set executable
#

if(NOT CLANG_FORMAT)
    set(CLANG_FORMAT clang-format)
endif()

#
# Find sources
#

file(
    GLOB_RECURSE SOURCES
    source/*.hpp
    source/*.cpp
)

#
# Execute
#

foreach(SOURCE IN LISTS SOURCES)
    execute_process(
        COMMAND "${CLANG_FORMAT}" --style=file -i "${SOURCE}"
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        RESULT_VARIABLE result
        OUTPUT_VARIABLE output
    )

    if(NOT result EQUAL "0")
        file(RELATIVE_PATH RELATIVE_SOURCE ${CMAKE_SOURCE_DIR} ${SOURCE})
        message(FATAL_ERROR "'${RELATIVE_SOURCE}': formatter returned with ${result}")
    endif()
endforeach()
