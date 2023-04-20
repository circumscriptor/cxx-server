
project_new_target(
    TARGET cxxserver
    NAMESPACE cxxserver
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/cxxserver
    EXECUTABLE
    HEADERS_PUBLIC
        server.hxx
        connection.hxx
        protocol.hxx
    SOURCES_COMMON
        server.cxx
        connection.cxx
        protocol.cxx
    SOURCES_TARGET
        main.cxx
    LINK_PRIVATE
        CONAN_PKG::glm
        CONAN_PKG::enet
        CONAN_PKG::bitsery
    NO_TESTS
)

# details/v75
project_target_sources(
    TARGET cxxserver
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/cxxserver/details/v75
    HEADERS_PRIVATE
        protocol_v75.hxx
    SOURCES_COMMON
        protocol_v75.cxx
)
