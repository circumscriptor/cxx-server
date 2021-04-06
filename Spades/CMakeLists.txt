
#
#
# SpadesServer
#
#

target_sources(server
    PRIVATE
        Core/Math/Math.hxx
        Core/Math/Vector2.hxx
        Core/Math/Vector3.hxx
        Core/DataChunk.hxx
        Core/DataStream.hxx
        Core/Random.hxx
        Core/Types.hxx
        Core/Vector.hxx
        Util/Compress.cxx
        Util/Compress.hxx
        Util/Map.cxx
        Util/Map.hxx
        Util/Peer.cxx
        Util/Peer.hxx
        Util/File.hxx
        Util/Spawn.hxx
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

target_compile_features(server
    PUBLIC
        cxx_std_17
)
