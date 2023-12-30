add_library(glm INTERFACE)
add_library(third-party::glm ALIAS glm)

target_compile_features(glm
    INTERFACE
        cxx_std_17
)

target_include_directories(glm
    SYSTEM
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources(glm
    PRIVATE
        glm/glm.hpp
)

target_precompile_headers(glm
    INTERFACE
        glm/glm.hpp
)
