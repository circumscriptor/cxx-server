include(GNUInstallDirs)
include(GenerateExportHeader)

# project_generate_version
include(project_version)

#
# Add target sources
#
# project_target_sources(
#   [TARGET <target_name>]
#   [DIRECTORY <source_directory>]
#   [HEADERS_PRIVATE ...]
#   [HEADERS_PUBLIC ...]
#   [HEADERS_TESTS ...]
#   [SOURCES_COMMON ...]
#   [SOURCES_TARGET ...]
#   [SOURCES_TESTS ...]
# )
#
# ...
#
function(project_target_sources)
    # Arguments
    set(options
        NO_INSTALL
    )
    set(one_value_args
        TARGET
        DIRECTORY
    )
    set(multi_value_args
        HEADERS_PRIVATE
        HEADERS_PUBLIC
        HEADERS_TESTS
        SOURCES_COMMON
        SOURCES_TARGET
        SOURCES_TESTS
    )
    cmake_parse_arguments(arg "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

    # if(NOT arg_DIRECTORY)
    #     set(arg_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    # endif()

    list(TRANSFORM arg_HEADERS_PRIVATE PREPEND ${arg_DIRECTORY}/)
    list(TRANSFORM arg_HEADERS_PUBLIC PREPEND ${arg_DIRECTORY}/)
    list(TRANSFORM arg_HEADERS_TESTS PREPEND ${arg_DIRECTORY}/)
    list(TRANSFORM arg_SOURCES_COMMON PREPEND ${arg_DIRECTORY}/)
    list(TRANSFORM arg_SOURCES_TARGET PREPEND ${arg_DIRECTORY}/)
    list(TRANSFORM arg_SOURCES_TESTS PREPEND ${arg_DIRECTORY}/)

    # Object sources
    if(arg_HEADERS_PRIVATE OR arg_SOURCES_COMMON)
        target_sources(
            ${arg_TARGET}-common-objects
            PRIVATE
                ${arg_HEADERS_PRIVATE}
                ${arg_SOURCES_COMMON}
        )
    endif()

    # Private headers
    if(arg_HEADERS_PRIVATE)
        target_precompile_headers(
            ${arg_TARGET}-common-private
            INTERFACE
                ${arg_HEADERS_PRIVATE}
        )
    endif()

    # Target sources
    if(arg_HEADERS_PUBLIC OR arg_SOURCES_TARGET)
        target_sources(
            ${arg_TARGET}
            PRIVATE
                ${arg_HEADERS_PUBLIC}
                ${arg_SOURCES_TARGET}
        )
    endif()

    # Public headers
    if(arg_HEADERS_PUBLIC)
        target_precompile_headers(
            ${arg_TARGET}-common-public
            INTERFACE
                ${arg_HEADERS_PUBLIC}
        )
    endif()

    # Tests sources
    if(arg_HEADERS_TESTS OR arg_SOURCES_TESTS)
        target_sources(
            ${arg_TARGET}-tests
            PRIVATE
                ${arg_HEADERS_TESTS}
                ${arg_SOURCES_TESTS}
        )
    endif()

    if(NOT arg_NO_INSTALL)
        install(
            FILES
                ${arg_HEADERS_PUBLIC}
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${arg_NAMESPACE}
        )
    endif()
endfunction()

#
# Add features
#
# project_target_features(
#   [TARGET <target_name>]
#   [PRIVATE ...]
#   [PUBLIC ...]
#   [TESTS ...]
# )
#
# ...
#
function(project_target_features)
    unset(options)
    set(one_value_args TARGET)
    set(multi_value_args PRIVATE PUBLIC TESTS)
    cmake_parse_arguments(arg "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

    if(arg_PRIVATE)
        target_compile_features(
            ${arg_TARGET}-common-private
            INTERFACE
                ${arg_PRIVATE}
        )
    endif()

    if(arg_PUBLIC)
        target_compile_features(
            ${arg_TARGET}-common-public
            INTERFACE
                ${arg_PUBLIC}
        )
    endif()

    if(arg_TESTS)
        target_compile_features(
            ${arg_TARGET}-tests
            PRIVATE
                ${arg_TESTS}
        )
    endif()
endfunction()

#
# Add definitions
#
# project_target_definitions(
#   [TARGET <target_name>]
#   [PRIVATE ...]
#   [PUBLIC ...]
#   [TESTS ...]
# )
#
# ...
#
function(project_target_definitions)
    unset(options)
    set(one_value_args TARGET)
    set(multi_value_args PRIVATE PUBLIC TESTS)
    cmake_parse_arguments(arg "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

    if(arg_PRIVATE)
        target_compile_definitions(
            ${arg_TARGET}-common-private
            INTERFACE
                ${arg_PRIVATE}
        )
    endif()

    if(arg_PUBLIC)
        target_compile_definitions(
            ${arg_TARGET}-common-public
            INTERFACE
                ${arg_PUBLIC}
        )
    endif()

    if(arg_TESTS)
        target_compile_definitions(
            ${arg_TARGET}-tests
            PRIVATE
                ${arg_TESTS}
        )
    endif()
endfunction()

#
# Add options
#
# project_target_options(
#   [TARGET <target_name>]
#   [PRIVATE ...]
#   [PUBLIC ...]
#   [TESTS ...]
# )
#
# ...
#
function(project_target_options)
    unset(options)
    set(one_value_args TARGET)
    set(multi_value_args PRIVATE PUBLIC TESTS)
    cmake_parse_arguments(arg "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

    if(arg_PRIVATE)
        target_compile_options(
            ${arg_TARGET}-common-private
            INTERFACE
                ${arg_PRIVATE}
        )
    endif()

    if(arg_PUBLIC)
        target_compile_options(
            ${arg_TARGET}-common-public
            INTERFACE
                ${arg_PUBLIC}
        )
    endif()

    if(arg_TESTS)
        target_compile_options(
            ${arg_TARGET}-tests
            PRIVATE
                ${arg_TESTS}
        )
    endif()
endfunction()

#
# Add link
#
# project_target_link(
#   [TARGET <target_name>]
#   [PRIVATE ...]
#   [PUBLIC ...]
#   [TESTS ...]
# )
#
# ...
#
function(project_target_link)
    unset(options)
    set(one_value_args TARGET)
    set(multi_value_args PRIVATE PUBLIC TESTS)
    cmake_parse_arguments(arg "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

    if(arg_PRIVATE)
        target_link_libraries(
            ${arg_TARGET}-common-private
            INTERFACE
                ${arg_PRIVATE}
        )
    endif()

    if(arg_PUBLIC)
        target_link_libraries(
            ${arg_TARGET}-common-public
            INTERFACE
                ${arg_PUBLIC}
        )
    endif()

    if(arg_TESTS)
        target_link_libraries(
            ${arg_TARGET}-tests
            PRIVATE
                ${arg_TESTS}
        )
    endif()
endfunction()

#
# Add include
#
# project_target_include(
#   [TARGET <target_name>]
#   [PRIVATE ...]
#   [PUBLIC ...]
#   [TESTS ...]
# )
#
# ...
#
function(project_target_include)
    unset(options)
    set(one_value_args TARGET)
    set(multi_value_args PRIVATE PUBLIC TESTS)
    cmake_parse_arguments(arg "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

    if(arg_PRIVATE)
        target_include_directories(
            ${arg_TARGET}-common-private
            INTERFACE
                ${arg_PRIVATE}
        )
    endif()

    if(arg_PUBLIC)
        target_include_directories(
            ${arg_TARGET}-common-public
            INTERFACE
                ${arg_PUBLIC}
        )
    endif()

    if(arg_TESTS)
        target_include_directories(
            ${arg_TARGET}-tests
            PRIVATE
                ${arg_TESTS}
        )
    endif()
endfunction()

#
# Add properties
#
# project_target_properties(
#   [TARGET <target_name>]
#   [PROPERTIES_COMMON ...]
#   [PROPERTIES_TARGET ...]
#   [PROPERTIES_TESTS ...]
# )
#
# ...
#
function(project_target_properties)
    unset(options)
    set(one_value_args TARGET)
    set(multi_value_args PRIVATE PUBLIC TESTS)
    cmake_parse_arguments(arg "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

    if(arg_PROPERTIES_COMMON)
        set_target_properties(
            ${arg_TARGET}-common-objects
            PROPERTIES
                ${arg_PROPERTIES_COMMON}
        )
    endif()

    if(arg_PROPERTIES_TARGET)
        set_target_properties(
            ${arg_TARGET}
            PROPERTIES
                ${arg_PROPERTIES_TARGET}
        )
    endif()

    if(arg_PROPERTIES_TESTS)
        set_target_properties(
            ${arg_TARGET}
            PROPERTIES
                ${arg_PROPERTIES_TESTS}
        )
    endif()
endfunction()

#
# Add new target
#
# project_new_target(
#   [TARGET <target_name>]
#   [NAMESPACE <namespace>]
#   [EXECUTABLE | LIBRARY | STATIC | SHARED | HEADER_ONLY]
#   [DEFINITIONS_PRIVATE ...]
#   [DEFINITIONS_PUBLIC ...]
#   [DEFINITIONS_TESTS ...]
#   [FEATURES_PRIVATE ...]
#   [FEATURES_PUBLIC ...]
#   [FEATURES_TESTS ...]
#   [HEADERS_PRIVATE ...]
#   [HEADERS_PUBLIC ...]
#   [HEADERS_TESTS ...]
#   [INCLUDE_PRIVATE ...]
#   [INCLUDE_PUBLIC ...]
#   [INCLUDE_TESTS ...]
#   [LINK_PRIVATE ...]
#   [LINK_PUBLIC ...]
#   [LINK_TESTS ...]
#   [OPTIONS_PRIVATE ...]
#   [OPTIONS_PUBLIC ...]
#   [OPTIONS_TESTS ...]
#   [PROPERTIES_COMMON ...]
#   [PROPERTIES_TARGET ...]
#   [PROPERTIES_TESTS ...]
#   [SOURCES_COMMON ...]
#   [SOURCES_TARGET ...]
#   [SOURCES_TESTS ...]
#   [NO_TESTS]
#   [NO_INSTALL]
# )
#
# ...
#
function(project_new_target)
    # Arguments
    set(options
        EXECUTABLE
        LIBRARY
        STATIC
        SHARED
        HEADER_ONLY
        VERSION_FROM_PROJECT
        NO_TESTS
        NO_INSTALL
        NO_VERSION
    )
    set(one_value_args
        TARGET
        NAMESPACE
        DIRECTORY
        VERSION_MAJOR
        VERSION_MINOR
        VERSION_PATCH
        VERSION_SUFFIX
    )
    set(multi_value_args
        DEFINITIONS_PRIVATE
        DEFINITIONS_PUBLIC
        DEFINITIONS_TESTS
        FEATURES_PRIVATE
        FEATURES_PUBLIC
        FEATURES_TESTS
        HEADERS_PRIVATE
        HEADERS_PUBLIC
        HEADERS_TESTS
        INCLUDE_PRIVATE
        INCLUDE_PUBLIC
        INCLUDE_TESTS
        LINK_PRIVATE
        LINK_PUBLIC
        LINK_TESTS
        OPTIONS_PRIVATE
        OPTIONS_PUBLIC
        OPTIONS_TESTS
        PROPERTIES_COMMON
        PROPERTIES_TARGET
        PROPERTIES_TESTS
        SOURCES_COMMON
        SOURCES_TARGET
        SOURCES_TESTS
    )
    cmake_parse_arguments(arg "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

    # Check target name
    if(NOT arg_TARGET)
        message(FATAL_ERROR "Target name cannot be empty")
    endif()

    # Set namespace
    if(NOT arg_NAMESPACE)
        set(arg_NAMESPACE ${PROJECT_NAME})
    endif()

    # Set directory
    # if(NOT arg_DIRECTORY)
    #     set(arg_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    # endif()

    # Count
    unset(count)

    if(arg_EXECUTABLE)
        set(count "${count}1")
    endif()

    if(arg_LIBRARY)
        set(count "${count}1")
    endif()

    if(arg_STATIC)
        set(count "${count}1")
    endif()

    if(arg_SHARED)
        set(count "${count}1")
    endif()

    if(arg_HEADER_ONLY)
        set(count "${count}1")
    endif()

    if(NOT count STREQUAL "1")
        message(FATAL_ERROR "Target can only be one of: executable, library, static, shared or header-only")
    endif()

    # Main target
    if(arg_EXECUTABLE)
        add_executable(${arg_TARGET})
        add_executable(${arg_NAMESPACE}::${arg_TARGET} ALIAS ${arg_TARGET})
        message(STATUS "Executable ${arg_TARGET} with alias ${arg_NAMESPACE}::${arg_TARGET}")
    else()
        if(arg_LIBRARY)
            unset(target_type)
        elseif(arg_STATIC)
            set(target_type STATIC)
        elseif(arg_SHARED)
            set(target_type STATIC)
        elseif(arg_HEADER_ONLY)
            set(target_type INTERFACE)
        endif()

        add_library(${arg_TARGET} ${target_type})
        add_library(${arg_NAMESPACE}::${arg_TARGET} ALIAS ${arg_TARGET})
        message(STATUS "Library ${arg_TARGET} with alias ${arg_NAMESPACE}::${arg_TARGET}")
    endif()

    # Common public options
    add_library(
        ${arg_TARGET}-common-public
        INTERFACE
    )

    add_library(
        ${arg_NAMESPACE}::${arg_TARGET}-common-public
        ALIAS ${arg_TARGET}-common-public
    )

    # Common private options
    add_library(
        ${arg_TARGET}-common-private
        INTERFACE
    )

    add_library(
        ${arg_NAMESPACE}::${arg_TARGET}-common-private
        ALIAS ${arg_TARGET}-common-private
    )

    target_link_libraries(
        ${arg_TARGET}-common-private
        INTERFACE
            ${arg_NAMESPACE}::${arg_TARGET}-common-public
    )

    # Objects
    if(NOT arg_HEADER_ONLY)
        add_library(
            ${arg_TARGET}-common-objects
            OBJECT
        )

        add_library(
            ${arg_NAMESPACE}::${arg_TARGET}-common-objects
            ALIAS ${arg_TARGET}-common-objects
        )

        target_link_libraries(
            ${arg_TARGET}-common-objects
            PRIVATE
                ${arg_NAMESPACE}::${arg_TARGET}-common-private
        )
    endif()

    # Tests
    if(NOT arg_NO_TESTS)
        add_executable(${arg_TARGET}-tests)

        add_executable(
            ${arg_NAMESPACE}::${arg_TARGET}-tests
            ALIAS ${arg_TARGET}-tests
        )

        target_link_libraries(
            ${arg_TARGET}-tests
            PRIVATE
                ${arg_TARGET}-common-private
        )

        target_sources(
            ${arg_TARGET}-tests
            PRIVATE
                $<TARGET_OBJECTS:${arg_TARGET}-common-objects>
        )
    endif()

    # Add common public to target
    target_link_libraries(
        ${arg_TARGET}
        PUBLIC
            ${arg_TARGET}-common-public
        PRIVATE
            ${arg_TARGET}-common-private
    )

    # Link objects with target using target_sources
    target_sources(
        ${arg_TARGET}
        PRIVATE
            $<TARGET_OBJECTS:${arg_TARGET}-common-objects>
    )

    #
    # Base name
    #

    if(NOT arg_BASE_NAME)
        set(arg_BASE_NAME ${arg_TARGET})
    endif()

    string(TOUPPER ${arg_BASE_NAME} base_name_to_upper)
    string(TOLOWER ${arg_BASE_NAME} base_name_to_lower)

    # Compile features
    project_target_features(
        TARGET ${arg_TARGET}
        PRIVATE
            ${arg_FEATURES_PRIVATE}
        PUBLIC
            ${arg_FEATURES_PUBLIC}
        TESTS
            ${arg_FEATURES_TESTS}
    )

    # Compile definitions
    project_target_definitions(
        TARGET ${arg_TARGET}
        PRIVATE
            ${arg_DEFINITIONS_PRIVATE}
        PUBLIC
            ${arg_DEFINITIONS_PUBLIC}
        TESTS
            ${arg_DEFINITIONS_TESTS}
    )

    # Compile options
    project_target_options(
        TARGET ${arg_TARGET}
        PRIVATE
            ${arg_OPTIONS_PRIVATE}
        PUBLIC
            ${arg_OPTIONS_PUBLIC}
        TESTS
            ${arg_OPTIONS_TESTS}
    )

    # Link libraries
    project_target_link(
        TARGET ${arg_TARGET}
        PRIVATE
            ${arg_LINK_PRIVATE}
        PUBLIC
            ${arg_LINK_PUBLIC}
        TESTS
            ${arg_LINK_TESTS}
    )

    # Properties
    project_target_properties(
        TARGET ${arg_TARGET}
        PROPERTIES_COMMON
            ${arg_PROPERTIES_COMMON}
        PROPERTIES_TARGET
            ${arg_PROPERTIES_TARGET}
        PROPERTIES_TESTS
            ${arg_PROPERTIES_TESTS}
    )

    # Sources
    if(NOT arg_NO_INSTALL)
        set(no_install NO_INSTALL)
    endif()

    project_target_sources(
        TARGET ${arg_TARGET}
        DIRECTORY ${arg_DIRECTORY}
        HEADERS_PRIVATE
            ${arg_HEADERS_PRIVATE}
        HEADERS_PUBLIC
            ${arg_HEADERS_PUBLIC}
        HEADERS_TESTS
            ${arg_HEADERS_TESTS}
        SOURCES_COMMON
            ${arg_SOURCES_COMMON}
        SOURCES_TARGET
            ${arg_SOURCES_TARGET}
        SOURCES_TESTS
            ${arg_SOURCES_TESTS}
        ${no_install}
    )

    # Include directories
    project_target_include(
        TARGET ${arg_TARGET}
        PRIVATE
            ${arg_LINK_PRIVATE}
        PUBLIC
            ${arg_LINK_PUBLIC}
        TESTS
            ${arg_LINK_TESTS}
    )

    # Default options
    if(NOT arg_HEADER_ONLY)
        if(COMPILER_CLANG OR COMPILER_CLANG_CL)
            target_compile_options(${arg_TARGET}-common-private
                INTERFACE
                    -Weverything
                    -Wno-c++98-compat
                    -Wno-c++98-compat-pedantic
                    -Wno-padded
            )
        elseif(COMPILER_MSVC)
            target_compile_options(${arg_TARGET}-common-private
                INTERFACE
                    /W4
            )
        elseif(COMPILER_GNU)
            target_compile_options(${arg_TARGET}-common-private
                INTERFACE
                    -Wall
                    -Wextra
                    -Wshadow
                    -Wnon-virtual-dtor
                    -Wold-style-cast
                    -Wcast-align
                    -Wunused
                    -Woverloaded-virtual
                    -Wconversion
                    -Wsign-conversion
                    -Wmisleading-indentation
                    -Wduplicated-cond
                    -Wduplicated-branches
                    -Wlogical-op
                    -Wnull-dereference
                    -Wuseless-cast
                    -Wdouble-promotion
            )
        endif()
    endif()

    # Default include directories
    target_include_directories(
        ${arg_TARGET}-common-public
        INTERFACE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )

    # Exports
    if(NOT arg_EXECUTABLE AND NOT arg_HEADER_ONLY)
        generate_export_header(
            ${arg_TARGET}
            BASE_NAME ${arg_BASE_NAME}
            EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/${base_name_to_lower}_export.hxx
        )

        project_target_sources(
            TARGET ${arg_TARGET}
            HEADERS_PUBLIC
                ${CMAKE_CURRENT_BINARY_DIR}/${base_name_to_lower}_export.hxx
        )

        if(NOT arg_NO_INSTALL)
            install(
                FILES
                    ${CMAKE_CURRENT_BINARY_DIR}/${base_name_to_lower}_export.hxx
                DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${arg_NAMESPACE}
            )
        endif()
    endif()

    # Version
    if(NOT arg_NO_VERSION)
        if(arg_VERSION_FROM_PROJECT)
            set(from_project FROM_PROJECT)
        endif()

        project_generate_version(
            BASE_NAME ${base_name_to_lower}
            FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/${base_name_to_lower}_version.hxx
            MAJOR ${arg_VERSION_MAJOR}
            MINOR ${arg_VERSION_MINOR}
            PATCH ${arg_VERSION_PATCH}
            SUFFIX ${arg_VERSION_SUFFIX}
            ${from_project}
        )

        project_target_sources(
            TARGET ${arg_TARGET}
            HEADERS_PUBLIC
                ${CMAKE_CURRENT_BINARY_DIR}/${base_name_to_lower}_version.hxx
        )

        if(NOT arg_NO_INSTALL)
            install(
                FILES
                    ${CMAKE_CURRENT_BINARY_DIR}/${base_name_to_lower}_version.hxx
                DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${arg_NAMESPACE}
            )
        endif()
    endif()

    # Install
    if(NOT arg_NO_INSTALL)
        install(
            TARGETS ${arg_TARGET}
            LIBRARY DESTINATION ${CMAKE_INSTALL_BINDIR}
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        )
    endif()
endfunction()
