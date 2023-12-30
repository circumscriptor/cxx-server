add_library(bitsery INTERFACE)
add_library(third-party::bitsery ALIAS bitsery)

target_compile_features(bitsery
    INTERFACE
        cxx_std_17
)

target_include_directories(bitsery
    SYSTEM
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_sources(bitsery
    PRIVATE
        include/bitsery/bitsery.h
)

target_precompile_headers(bitsery
    INTERFACE
        include/bitsery/bitsery.h
)
