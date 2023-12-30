add_executable(cxxserver)

target_compile_features(cxxserver
    PRIVATE
        cxx_std_23
)

target_include_directories(cxxserver
    PRIVATE
        ${CXXSERVER_SOURCE_DIRECTORY}
        ${CXXSERVER_BINARY_DIRECTORY}
)

target_sources(cxxserver
    PRIVATE
        Main.cpp
        Map.cpp
        Map.hpp
        Protocol.hpp
        Server.cpp
        Server.hpp
)

target_link_libraries(cxxserver
    PRIVATE
        cxxserver::coverage
        cxxserver::sanitize
        cxxserver::strict
        third-party::bitsery
        third-party::doctest
        third-party::enet
        third-party::glm
)

#
# Configure
#

configure_file(
    ${CXXSERVER_TEMPLATES_DIRECTORY}/Configuration.h
    ${CMAKE_CURRENT_BINARY_DIR}/Configuration.h
    @ONLY
)

target_sources(cxxserver
    PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}/Configuration.h
)

#
# Tests
#

if(CXXSERVER_WITH_TESTS)
    include(${CXXSERVER_THIRD_PARTY_DIRECTORY}/doctest/scripts/doctest.cmake)
    doctest_discover_tests(cxxserver)
    add_subdirectory(tests)
endif()

#
# Extra commands
#

get_target_property(CXXSERVER_OUTPUT_NAME cxxserver OUTPUT_NAME)

add_custom_target(strip
    COMMAND ${CMAKE_STRIP} --strip-all $<TARGET_FILE:cxxserver>
    DEPENDS cxxserver
    COMMENT "Stripping symbols from ${CXXSERVER_OUTPUT_NAME}"
)

if(CXXSERVER_WITH_SCOV AND LLVM_PROFDATA AND LLVM_COV)
    set(CXXSERVER_COVERAGE_DIRECTORY ${CXXSERVER_TOP_BINARY_DIR}/coverage)
    set(CXXSERVER_COVERAGE_BASE_PATH ${CXXSERVER_COVERAGE_DIRECTORY}/${CXXSERVER_OUTPUT_NAME})
    set(CXXSERVER_COVERAGE_RAW_FILE ${CXXSERVER_COVERAGE_BASE_PATH}.profraw)
    set(CXXSERVER_COVERAGE_DATA_FILE ${CXXSERVER_COVERAGE_BASE_PATH}.profdata)

    add_custom_command(
        OUTPUT ${CXXSERVER_COVERAGE_RAW_FILE}
        COMMAND LLVM_PROFILE_FILE=${CXXSERVER_COVERAGE_RAW_FILE} $<TARGET_FILE:cxxserver>
        DEPENDS cxxserver
        WORKING_DIRECTORY ${CXXSERVER_COVERAGE_DIRECTORY}
        COMMENT "Running coverage for ${CXXSERVER_OUTPUT_NAME}"
    )

    add_custom_command(
        OUTPUT ${CXXSERVER_COVERAGE_DATA_FILE}
        COMMAND
            ${LLVM_PROFDATA} merge
            -sparse
            -o ${CXXSERVER_COVERAGE_DATA_FILE}
            ${CXXSERVER_COVERAGE_RAW_FILE}
        DEPENDS ${CXXSERVER_COVERAGE_RAW_FILE}
        WORKING_DIRECTORY ${CXXSERVER_COVERAGE_DIRECTORY}
        COMMENT "Parsing coverage for ${CXXSERVER_OUTPUT_NAME}"
    )

    add_custom_target(coverage
        COMMAND
            ${LLVM_COV} show
            $<TARGET_FILE:cxxserver>
            -format "html"
            -instr-profile ${CXXSERVER_COVERAGE_DATA_FILE}
            -output-dir ${CXXSERVER_COVERAGE_DIRECTORY}
            ${CXXSERVER_SOURCE_DIRECTORY}
        DEPENDS ${CXXSERVER_COVERAGE_DATA_FILE}
        WORKING_DIRECTORY ${CXXSERVER_COVERAGE_DIRECTORY}
        COMMENT "Generating coverage for ${CXXSERVER_OUTPUT_NAME}"
    )
endif()

#
# Install
#

install(
    TARGETS cxxserver
    RUNTIME DESTINATION ${CXXSERVER_INSTALL_BINARY_DIRECTORY}
)
