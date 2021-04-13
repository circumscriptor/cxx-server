
#
#
# SpadesX
#
#

target_sources(server
    PRIVATE
    	baseconnection.hxx
	baseprotocol.hxx
        connection.hxx
        ctf.hxx
        datastream.hxx
        enet.hxx
        enums.hxx
        main.cxx
        map.hxx
        peer.hxx
	playerdata.hxx
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
