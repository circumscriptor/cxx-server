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

int main(int argc, char** argv)
{
    if (argc != 2) {
        return 1;
    }
    Spades::Protocol protocol(32);
    auto&            map = protocol.GetMap();

    std::cout << "reading map file...\n";
    Spades::uint32 size = 0;
    Spades::uint8* data = (Spades::uint8*) Spades::ReadFile(argv[1], size);
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
    protocol.GetTeam(0).mBase  = {256.f, 256.f, 40.f};
    protocol.GetTeam(1).mBase  = {256.f, 256.f, 40.f};

    std::cout << "starting server\n";
    return Spades::Server().Run(protocol, 1);
    return 0;
}
