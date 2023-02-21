#
# Function for adding sources and headers to a target
#
# project_target_sources(<target_name>
#   [NAMESPACE <namespace>]
#   [PRECOMPILED item...]
#   [HEADERS item...]
#   [SOURCES item...]
#   [OBJECTS item...]
# )
#
# Adds sources and headers to a target <target_name>.
#
#   NAMESPACE
#       Namespace for target alias. (By default set to PROJECT_NAME)
#
#   PRECOMPILED
#       Precompiles headers for the target and dependent targets.
#
#   HEADERS
#       Headers not to be precompiled.
#
#   SOURCES
#       Source files.
#
#   OBJECTS
#       Object libraries.
#
function(project_target_sources target_name)
    set(pts_options)
    set(pts_one_value_args NAMESPACE)
    set(pts_multi_value_args PRECOMPILED HEADERS SOURCES OBJECTS)
    cmake_parse_arguments(PARSE_ARGV 1 arg "${pts_options}" "${pts_one_value_args}" "${pts_multi_value_args}")

    list(TRANSFORM arg_PRECOMPILED PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/)
    list(TRANSFORM arg_HEADERS PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/)
    list(TRANSFORM arg_SOURCES PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/)

    if(NOT arg_NAMESPACE)
        set(arg_NAMESPACE ${PROJECT_NAME})
    endif()

    set(full_target_name ${arg_NAMESPACE}${target_name})

    target_sources(${full_target_name}
        PRIVATE
            ${arg_PRECOMPILED}
            ${arg_HEADERS}
            ${arg_SOURCES}
    )

    target_precompile_headers(${full_target_name}
        PUBLIC
            $<$<COMPILE_LANGUAGE:CXX>:${arg_PRECOMPILED}>
    )

    foreach(object_target ${arg_OBJECTS})
        target_sources(${full_target_name}
            PRIVATE
                $<TARGET_OBJECTS:${object_target}>
        )
    endforeach()
endfunction()
