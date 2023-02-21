include_guard(GLOBAL)

include(DetectCompiler)

#
# Add common targets
#

add_library(CommonPublic INTERFACE)
add_library(CommonPrivate INTERFACE)

add_library(Common::Public ALIAS CommonPublic)
add_library(Common::Private ALIAS CommonPrivate)

#
# Include directories
#

target_include_directories(CommonPublic
    INTERFACE
        "${PROJECT_SOURCE_DIR}/Source"
        "${PROJECT_BINARY_DIR}/Source" # For generated files
)

#
# Compile definitions
#

target_compile_definitions(CommonPublic
    INTERFACE
        BUILD_SHARED_LIBS=$<BOOL:${BUILD_SHARED_LIBS}>
)

target_compile_definitions(CommonPrivate
    INTERFACE
        _CRT_SECURE_NO_WARNINGS
)

#
# Compile features
#

target_compile_features(CommonPublic
    INTERFACE
        cxx_std_20
)

#
# Compile options
#

if(COMPILER_CLANG OR COMPILER_CLANG_CL)
    target_compile_options(CommonPrivate
        INTERFACE
            -Weverything
            -Wno-c++98-compat
            -Wno-c++98-compat-pedantic
            -Wno-error=padded
    )
elseif(COMPILER_MSVC)
    target_compile_options(CommonPrivate
        INTERFACE
            /W4
    )
elseif(COMPILER_GNU)
    target_compile_options(CommonPrivate
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
            -Wno-comment # GCC does not like comments ending with backslash ('\')
    )
endif()

#
# Link libraries
#

target_link_libraries(CommonPublic
    INTERFACE
        Definitions::Compiler
)

target_link_libraries(CommonPrivate
    INTERFACE
        Common::Public
)
