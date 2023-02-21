include(Common)
include(TargetSources)

#
# Function for adding executable target
#
# project_add_executable(<target_name>
#   [NAMESPACE <namespace>]
#   [PRECOMPILED item...]
#   [HEADERS item...]
#   [SOURCES item...]
#   [LIBRARIES item...]
#   [OBJECTS item...]
# )
#
# Adds executable target. Sets precompiled headers, headers, sources and link libraries.
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
#   LIBRARIES
#       Private link libraries.
#
#   OBJECTS
#       Object libraries.
#
function(project_add_executable target_name)
    set(pae_options)
    set(pae_one_value_args NAMESPACE)
    set(pae_multi_value_args PRECOMPILED HEADERS SOURCES LIBRARIES OBJECTS)
    cmake_parse_arguments(arg "${pae_options}" "${pae_one_value_args}" "${pae_multi_value_args}" ${ARGN})

    if(NOT arg_NAMESPACE)
        set(arg_NAMESPACE ${PROJECT_NAME})
    endif()

    add_executable(${target_name})
    add_executable(${arg_NAMESPACE}::${target_name} ALIAS ${target_name})

    project_target_sources(${target_name} NAMESPACE ${arg_NAMESPACE}
        PRECOMPILED
            ${arg_PRECOMPILED}
        HEADERS
            ${arg_HEADERS}
        SOURCES
            ${arg_SOURCES}
        OBJECTS
            ${arg_OBJECTS}
    )

    target_link_libraries(${target_name}
        PUBLIC
            Common::Public
        PRIVATE
            Common::Private
    )

    target_link_libraries(${target_name}
        PRIVATE
            ${arg_LIBRARIES}
    )
endfunction()
