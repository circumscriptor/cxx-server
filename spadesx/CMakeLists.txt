
#
#
# SpadesX
#
#

set(HEADERS_SERVER_DATA
    data/color.hxx
    data/enums.hxx
    data/input.hxx
    data/intel.hxx
    data/lastkill.hxx
    data/name.hxx
    data/player.hxx
    data/state.hxx
    data/position.hxx
    data/score.hxx
    data/transform.hxx
    data/types.hxx
    data/weapon.hxx
)

set(HEADERS_SERVER
        ${HEADERS_SERVER_DATA}
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
        world.hxx
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
