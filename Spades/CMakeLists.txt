
#
#
# SpadesServer
#
#

target_sources(server
    PRIVATE
        Core/DataChunk.hxx
        Core/DataStream.hxx
        Core/Types.hxx
        Core/Vector.hxx
        Util/Compress.cxx
        Util/Compress.hxx
        Util/Peer.cxx
        Util/Peer.hxx
        Util/ReadFile.cxx
        Util/ReadFile.hxx
        Data/Enums.hxx
        Data/Player.hxx
        Data/Team.hxx
        Connection.hxx
        Main.cxx
        Protocol.cxx
        Protocol.hxx
        Server.cxx
        Server.hxx
)

target_include_directories(server
    PRIVATE
        ${PROJECT_SOURCE_DIR}/Spades
)
