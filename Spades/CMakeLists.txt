
#
#
# SpadesServer
#
#

target_sources(server
    PRIVATE
        connection.hxx
        datastream.hxx
        enums.hxx
        main.cxx
        map.hxx
        peer.hxx
        protocol.hxx
        server.hxx
        spawn.hxx
        types.hxx
)

target_include_directories(server
    PRIVATE
        ${PROJECT_SOURCE_DIR}/Spades
)

target_compile_features(server
    PUBLIC
        cxx_std_17
)
