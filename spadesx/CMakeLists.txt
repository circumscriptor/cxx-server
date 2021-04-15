
#
#
# SpadesX
#
#

set(HEADERS_SERVER_DATA
    data/colordata.hxx
    data/enums.hxx
    data/inputdata.hxx
    data/inteldata.hxx
    data/lastkilldata.hxx
    data/namedata.hxx
    data/playerdata.hxx
    data/playerstate.hxx
    data/positiondata.hxx
    data/scoredata.hxx
    data/transformdata.hxx
    data/types.hxx
    data/weapondata.hxx
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
