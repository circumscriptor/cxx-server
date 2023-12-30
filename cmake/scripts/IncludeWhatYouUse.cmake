cmake_minimum_required(VERSION 3.27)

#
# Set executable
#

if(NOT IWYU_TOOL_PY)
    set(IWYU_TOOL_PY iwyu_tool.py)
endif()

#
# Check options
#

if(NOT COMPILE_COMMANDS_DIR)
    message(FATAL_ERROR "COMPILE_COMMANDS_DIR not set")
endif()

#
# Execute
#

cmake_host_system_information(
    RESULT thread_count
    QUERY NUMBER_OF_LOGICAL_CORES
)

execute_process(
    COMMAND "${IWYU_TOOL_PY}" -p "${COMPILE_COMMANDS_DIR}" -o clang -j${thread_count}
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
)

if(NOT result MATCHES "0")
    message("${output}")
    message(FATAL_ERROR "Fix includes")
endif()
