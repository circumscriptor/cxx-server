/**
 *
 * SpadesServer
 *
 */

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

    // std::cout << "reading map file...\n";
    Spades::uint32 size = 0;
    Spades::uint8* data = (Spades::uint8*) Spades::ReadFile(argv[1], size);
    if (data == nullptr || size == 0) {
        return -1;
    }

    // std::cout << "map file length: " << size << '\n';

    // std::cout << "converting map data...\n";
    map.Load(data, size);
    delete[] data;

    std::cout << "testing map...\n";

    std::vector<Spades::uint8> mapData;
    map.Save(mapData);

    std::fstream file("output.vxl", std::ios::out | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "failed to open file\n";
        return 1;
    }

    file.write(reinterpret_cast<char*>(mapData.data()), mapData.size());
    file.close();

    // std::cout << "starting server\n";
    // return Spades::Server().Run(protocol, 1);
    return 0;
}
