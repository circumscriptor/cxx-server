include_guard(GLOBAL)

#
# Add variables
#

set(compiler_definitions
    COMPILER_CLANG_CL
    COMPILER_CLANG
    COMPILER_MSVC
    COMPILER_GNU
)

foreach(opt ${compiler_definitions})
    set(${opt} OFF CACHE INTERNAL "")
endforeach()

#
# Detect compiler
#

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    if(CMAKE_CXX_SIMULATE_ID STREQUAL "MSVC")
        set(COMPILER_CLANG_CL ON CACHE INTERNAL "")
    else()
        set(COMPILER_CLANG ON CACHE INTERNAL "")
    endif(CMAKE_CXX_SIMULATE_ID STREQUAL "MSVC")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(COMPILER_MSVC ON CACHE INTERNAL "")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(COMPILER_GNU ON CACHE INTERNAL "")
else()
    set(COMPILER_GNU ON CACHE INTERNAL "")
    message("WARN: Other compiler, using GNU compiler settings")
endif()

#
# Add definitions target
#

add_library(CompilerDefinitions INTERFACE)
add_library(Definitions::Compiler ALIAS CompilerDefinitions)

foreach(opt ${compiler_definitions})
    target_compile_definitions(CompilerDefinitions
        INTERFACE
            $<$<BOOL:${${opt}}>:${opt}>
    )
endforeach()
