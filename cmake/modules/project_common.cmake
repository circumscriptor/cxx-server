include_guard(GLOBAL)

include(project_compiler)

#
# Add common targets
#

add_library(common-public INTERFACE)
add_library(common-private INTERFACE)

add_library(${PROJECT_NAME}::common-public ALIAS common-public)
add_library(${PROJECT_NAME}::common-private ALIAS common-private)

target_compile_definitions(common-private
    INTERFACE
        _CRT_SECURE_NO_WARNINGS
)

#
# Compile options
#

if(COMPILER_CLANG OR COMPILER_CLANG_CL)
    target_compile_options(common-private
        INTERFACE
            -Weverything
            -Wno-c++98-compat
            -Wno-c++98-compat-pedantic
            -Wno-padded
    )
elseif(COMPILER_MSVC)
    target_compile_options(common-private
        INTERFACE
            /W4
    )
elseif(COMPILER_GNU)
    target_compile_options(common-private
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

#
# Link libraries
#

target_link_libraries(common-public
    INTERFACE
        Definitions::Compiler
)

target_link_libraries(common-private
    INTERFACE
        Common::common-public
)
