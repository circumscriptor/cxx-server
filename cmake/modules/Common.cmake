include_guard(GLOBAL)

#
# Includes
#

include(Options)

#
# Project sanitizers
#

add_library(cxxserver-sanitize INTERFACE)
add_library(cxxserver::sanitize ALIAS cxxserver-sanitize)

# Ignore list file
target_compile_options(cxxserver-sanitize
    INTERFACE
        -fsanitize-ignorelist=${CXXSERVER_SUPPRESSIONS_DIRECTORY}/sanitize-ignorelist.txt
)

# Address sanitizer
if(CXXSERVER_WITH_ASAN)
    target_compile_options(cxxserver-sanitize
        INTERFACE
            -fsanitize=address
            -fsanitize-address-use-after-return=always
            -fsanitize-address-use-after-scope
            -fno-omit-frame-pointer
    )

    target_link_options(cxxserver-sanitize
        INTERFACE
            -fsanitize=address
    )
endif()

# Leak sanitizer
if(CXXSERVER_WITH_LSAN)
    target_compile_options(cxxserver-sanitize
        INTERFACE
            -fsanitize=leak
    )

    target_link_options(cxxserver-sanitize
        INTERFACE
            -fsanitize=leak
    )
endif()

# Thread sanitizer
if(CXXSERVER_WITH_TSAN)
    target_compile_options(cxxserver-sanitize
        INTERFACE
            -fsanitize=thread
    )

    target_link_options(cxxserver-sanitize
        INTERFACE
            -fsanitize=thread
    )
endif()

# Memory sanitizer
if(CXXSERVER_WITH_MSAN)
    target_compile_options(cxxserver-sanitize
        INTERFACE
            -fsanitize=memory
            -fno-omit-frame-pointer
    )

    target_link_options(cxxserver-sanitize
        INTERFACE
            -fsanitize=memory
    )
endif()

# Undefined behavior sanitizer
if(CXXSERVER_WITH_UBSAN)
    target_compile_options(cxxserver-sanitize
        INTERFACE
            -fsanitize=undefined
    )

    target_link_options(cxxserver-sanitize
        INTERFACE
            -fsanitize=undefined
    )
endif()

#
# Project coverage
#

add_library(cxxserver-coverage INTERFACE)
add_library(cxxserver::coverage ALIAS cxxserver-coverage)

# Coverage
if(CXXSERVER_WITH_SCOV)
    target_compile_options(cxxserver-coverage
        INTERFACE
            -fprofile-instr-generate
            -fcoverage-mapping
    )

    target_link_libraries(cxxserver-coverage
        INTERFACE
            -fprofile-instr-generate
            -fcoverage-mapping
    )
endif()

#
# Project common build options
#

add_library(cxxserver-common INTERFACE)
add_library(cxxserver::common ALIAS cxxserver-common)

target_compile_definitions(cxxserver-common
    INTERFACE
        $<$<PLATFORM_ID:Windows>:_CRT_SECURE_NO_WARNINGS _CRT_NONSTDC_NO_DEPRECATE>
        $<$<CONFIG:Debug>:_ITERATOR_DEBUG_LEVEL=0>
        _LARGEFILE64_SOURCE=1
        _FILE_OFFSET_BITS=64
        # $<$<CONFIG:Debug>:_GLIBCXX_DEBUG>
)

set(IS_CXX $<COMPILE_LANGUAGE:CXX>)
set(WITH_LIBCXX $<BOOL:${CXXSERVER_WITH_LIBCXX}>)
set(LIBCXX_REQUIREMENTS $<AND:${IS_CXX},${WITH_LIBCXX}>)

target_compile_options(cxxserver-common
    INTERFACE
        -Wall
        -Wextra
        -ferror-limit=0
        $<$<BOOL:${CXXSERVER_WITH_FAST_OPTIMIZATION}>:-Ofast>
        $<IF:$<BOOL:${CXXSERVER_WITH_NATIVE_ARCHITECTURE}>,-march=native,-msse4.2> # At least sse4.2
        $<${LIBCXX_REQUIREMENTS}:-stdlib=libc++>
)

target_link_options(cxxserver-common
    INTERFACE
        $<${LIBCXX_REQUIREMENTS}:-lc++ -lc++abi>
)

# Stack protection
if(CXXSERVER_WITH_STACK_PROTECTOR)
    target_compile_options(cxxserver-common
        INTERFACE
            -fstack-protector-all
            -fstack-check
    )
endif()

#
# Project strict build options
#

add_library(cxxserver-strict INTERFACE)
add_library(cxxserver::strict ALIAS cxxserver-strict)

# Warnings
target_compile_options(cxxserver-strict
    INTERFACE
        -Weverything
        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic
        -Wno-padded
        -fsafe-buffer-usage-suggestions
)

# Include common settings
target_link_libraries(cxxserver-strict
    INTERFACE
        cxxserver::common
)
