
#
#
# SpadesServer
#
#

target_sources(server
    PRIVATE
        Core/DataStream.hxx
        Core/Types.hxx
        Core/Vector.hxx
        Util/Compress.cxx
        Util/Compress.hxx
        Data/Enums.hxx
        Data/Player.hxx
        Data/Team.hxx
        Protocol.cxx
        Protocol.hxx
        Server.cxx
        Server.hxx
        Main.cxx
)

target_include_directories(server
    PRIVATE
        ${PROJECT_SOURCE_DIR}/Spades
)
