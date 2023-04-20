#
# Add target
#

project_new_target(
    TARGET cxxserver
    NAMESPACE cxxserver
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
