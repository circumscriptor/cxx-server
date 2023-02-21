#
# Function for generating version header
#
# project_generate_version(<output_path>
#   [FROM_PROJECT]
#   [NAMESPACE <namespace>]
#   [MAJOR <major_version>]
#   [MINOR <minor_version>]
#   [PATCH <patch_version>]
#   [SUFFIX <version_suffix>]
# )
#
# Generates version header. If FROM_PROJECT is set, version set by project is used.
#
#   NAMESPACE
#       Namespace for macros. (By default is set to PROJECT_NAME)
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
function(project_generate_version output)
    set(pgv_options FROM_PROJECT)
    set(pgv_one_value_args MAJOR MINOR PATCH SUFFIX NAMESPACE)
    set(pgv_multi_value_args)
    cmake_parse_arguments(PARSE_ARGV 1 arg "${pgv_options}" "${pgv_one_value_args}" "${pgv_multi_value_args}")

    if(arg_NAMESPACE)
        set(GENERATED_NAMESPACE ${arg_NAMESPACE})
    else()
        set(GENERATED_NAMESPACE ${PROJECT_NAME})
    endif()

    string(TOUPPER ${GENERATED_NAMESPACE} GENERATED_NAMESPACE)

    if(arg_FROM_PROJECT)
        set(GENERATED_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
        set(GENERATED_VERSION_MINOR ${PROJECT_VERSION_MINOR})
        set(GENERATED_VERSION_PATCH ${PROJECT_VERSION_PATCH})
    else()
        set(GENERATED_VERSION_MAJOR ${arg_MAJOR})
        set(GENERATED_VERSION_MINOR ${arg_MINOR})
        set(GENERATED_VERSION_PATCH ${arg_PATCH})
    endif()

    set(GENERATED_VERSION "${GENERATED_VERSION_MAJOR}.${GENERATED_VERSION_MINOR}.${GENERATED_VERSION_PATCH}${arg_SUFFIX}")

    get_filename_component(GENERATED_FILENAME ${output} NAME)

    string(TIMESTAMP GENERATED_YEAR "%Y")

    configure_file(
        ${PROJECT_SOURCE_DIR}/CMake/Templates/Version.hpp.in
        ${output}
        @ONLY
    )
endfunction()
