#
# Function for generating special headers
#
# project_generate_version(
#   [BASE_NAME <base_name>]
#   [DIRECTORY <directory>]
#   [FROM_PROJECT]
#   [MAJOR <major_version>]
#   [MINOR <minor_version>]
#   [PATCH <patch_version>]
#   [SUFFIX <version_suffix>]
# )
#
# Generates spcieal headers. If FROM_PROJECT is set, version set by project is used.
#
#   FILE_NAME
#       ...
#
#   MAJOR
#       Major version. Ignored if FROM_PROJECT is set.
#
#   MINOR
#       Minor version. Ignored if FROM_PROJECT is set.
#
#   PATCH
#       Patch version. Ignored if FROM_PROJECT is set.
#
#   SUFFIX
#       Version suffix (added to the end of version string).
#
function(project_generate_version)
    set(options FROM_PROJECT)
    set(one_value_args
        BASE_NAME
        FILE_NAME
        MAJOR
        MINOR
        PATCH
        SUFFIX
    )
    set(multi_value_args)
    cmake_parse_arguments(arg "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

    # Check input

    if(NOT arg_BASE_NAME)
        message(FATAL_ERROR "Base name not set")
    endif()

    if(NOT arg_FILE_NAME)
        set(arg_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/version.hxx)
    endif()

    # Version
    if(arg_FROM_PROJECT)
        set(major ${PROJECT_VERSION_MAJOR})
        set(minor ${PROJECT_VERSION_MINOR})
        set(patch ${PROJECT_VERSION_PATCH})
    else()
        set(major ${arg_MAJOR})
        set(minor ${arg_MINOR})
        set(patch ${arg_PATCH})
    endif()

    # Version with suffix
    set(version "${major}.${minor}.${patch}${arg_SUFFIX}")

    # Base name
    string(TOUPPER ${arg_BASE_NAME} name)
    string(REPLACE "-" "_" name ${name})

    # Year
    string(TIMESTAMP year "%Y")

    # Configure
    configure_file(
        ${PROJECT_SOURCE_DIR}/cmake/templates/version.hxx.in
        ${arg_FILE_NAME}
        @ONLY
    )
endfunction()
