add_library(doctest STATIC)
add_library(third-party::doctest ALIAS doctest)

target_compile_definitions(doctest
    PUBLIC
        $<$<NOT:$<BOOL:${CXXSERVER_WITH_TESTS}>>:DOCTEST_CONFIG_DISABLE>
        # DOCTEST_CONFIG_USE_STD_HEADERS
    PRIVATE
        DOCTEST_CONFIG_IMPLEMENT
)

target_compile_features(doctest
    PUBLIC
        cxx_std_11
)

target_include_directories(doctest
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_include_directories(doctest
    SYSTEM
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(doctest
    PRIVATE
        cxxserver::common
        cxxserver::sanitize
)

target_sources(doctest
    PRIVATE
        doctest_fwd.h
        doctest.cpp
)

target_precompile_headers(doctest
    INTERFACE
        doctest_fwd.h
)

set_target_properties(doctest
    PROPERTIES
        UNITY_BUILD OFF # Not needed
)
