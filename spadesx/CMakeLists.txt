
#
#
# SpadesX
#
#

set(HEADERS_SERVER
        data/enums.hxx
        data/inputdata.hxx
        data/playerdata.hxx
        data/playerstate.hxx
        data/types.hxx
        data/weapondata.hxx
        baseconnection.hxx
        baseprotocol.hxx
        connection.hxx
        ctf.hxx
        datastream.hxx
        enet.hxx
        manager.hxx
        map.hxx
        peer.hxx
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

target_include_directories(server
    PRIVATE
        ${PROJECT_SOURCE_DIR}/Spades
)

target_compile_features(server
    PUBLIC
        cxx_std_17
)
