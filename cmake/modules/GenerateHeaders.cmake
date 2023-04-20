#
# Function for generating special headers
#
# project_generate_headers(<target_name>
#   [FROM_PROJECT]
#   [NAMESPACE <namespace>]
#   [MAJOR <major_version>]
#   [MINOR <minor_version>]
#   [PATCH <patch_version>]
#   [SUFFIX <version_suffix>]
# )
#
# Generates spcieal headers. If FROM_PROJECT is set, version set by project is used.
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
function(project_generate_headers target_name)
    set(pgh_options FROM_PROJECT)
    set(pgh_one_value_args MAJOR MINOR PATCH SUFFIX NAMESPACE)
    set(pgh_multi_value_args)
    cmake_parse_arguments(PARSE_ARGV 1 arg "${pgh_options}" "${pgh_one_value_args}" "${pgh_multi_value_args}")

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
    set(GENERATED_TARGET ${target_name})

    string(TOUPPER ${GENERATED_TARGET} GENERATED_NAME)
    string(REPLACE "-" "_" GENERATED_NAME ${GENERATED_NAME})
    string(TIMESTAMP GENERATED_YEAR "%Y")

    get_target_property(output ${target_name} BINARY_DIR)
    set(output ${output}/Generated)

    set(GENERATED_FILENAME Version.hpp)
    configure_file(
        ${PROJECT_SOURCE_DIR}/CMake/Templates/Version.hpp.in
        ${output}/${GENERATED_FILENAME}
        @ONLY
    )

    set(GENERATED_FILENAME ClassDefinitions.hpp)
    configure_file(
        ${PROJECT_SOURCE_DIR}/CMake/Templates/ClassDefinitions.hpp.in
        ${output}/${GENERATED_FILENAME}
        @ONLY
    )

    set(GENERATED_FILENAME Exports.hpp)
    configure_file(
        ${PROJECT_SOURCE_DIR}/CMake/Templates/Exports.hpp.in
        ${output}/${GENERATED_FILENAME}
        @ONLY
    )
endfunction()
