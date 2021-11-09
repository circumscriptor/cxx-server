
#
#
# SpadesX
#
#

add_subdirectory(data)
add_subdirectory(commands)

set(HEADERS_SERVER
        base.hxx
        connection.hxx
        ctf.hxx
        datastream.hxx
        enet.hxx
        handler.hxx
        intel.hxx
        manager.hxx
        map.hxx
        peer.hxx
        protocol.hxx
        server.hxx
        spawn.hxx
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
