/**
 *
 * SpadesServer
 *
 */

#include "Data/Enums.hxx"
#include "Protocol.hxx"
#include "Server.hxx"
#include "Util/ReadFile.hxx"

#include <fstream>
#include <ios>
#include <iostream>

auto main(int argc, char** argv) -> int
{
    if (argc != 2) {
        return 1;
    }
    Spades::Protocol protocol(32);
    auto&            map = protocol.GetMap();

    std::cout << "reading map file...\n";
    Spades::uint32 size = 0;
    auto*          data = (Spades::uint8*) Spades::ReadFile(argv[1], size);
    if (data == nullptr || size == 0) {
        return -1;
    }

    // std::cout << "map file length: " << size << '\n';

    std::cout << "converting map data...\n";
    map.Load(data, size);
    delete[] data;

    protocol.SetTeamName(0, "Team A");
    protocol.SetTeamName(1, "Team B");

    protocol.GetTeam(0).mColor = {0xff, 0x00, 0x00};
    protocol.GetTeam(1).mColor = {0x00, 0xff, 0x00};

    protocol.SetSpawn(Spades::TeamType::A, {{64.F, 224.F}, 64.F});
    protocol.SetSpawn(Spades::TeamType::B, {{384.F, 224.F}, 64.F});
    protocol.SetSpawn(Spades::TeamType::SPECTATOR, {{256.F, 256.F}, 0.F});

    std::cout << "starting server\n";
    return Spades::Server().Run(protocol, 1);
}
