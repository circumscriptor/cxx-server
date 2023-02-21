
project_generate_version(
    ${CMAKE_CURRENT_BINARY_DIR}/Generated/Version.hpp
    FROM_PROJECT
)

project_add_executable(cxx-server
    HEADERS
        connection.hxx
        ctf.hxx
        datastream.hxx
        enet.hxx
        handler.hxx
        manager.hxx
        map.hxx
        peer.hxx
        protocol.hxx
        server.hxx
    SOURCES
        main.cxx
    LIBRARIES
        CONAN_PKG::glm
        CONAN_PKG::enet
)

#
# Add subdirectories
#

add_subdirectory(data)
add_subdirectory(commands)
