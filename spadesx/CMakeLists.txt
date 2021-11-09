
#
#
# SpadesX
#
#

add_subdirectory(data)
add_subdirectory(commands)

set(HEADERS_SERVER
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
)

target_sources(server
    PRIVATE
        ${HEADERS_SERVER}
        main.cxx
)

target_precompile_headers(server
    PRIVATE
        ${HEADERS_SERVER}
)
