/**
 *
 * SpadesServer
 *
 */

#include "Server.hxx"

auto main(int argc, char** argv) -> int
{
    if (argc != 2) {
        return 1;
    }

    Spades::Protocol protocol(32);
    if (!protocol.LoadFromFile(argv[1])) {
        std::cerr << "failed to load map\n";
        return 1;
    }
    protocol.SetTeamName(Spades::TeamType::A, "Team A");
    protocol.SetTeamName(Spades::TeamType::B, "Team B");
    protocol.SetTeamColor(Spades::TeamType::A, {0xFF, 0x00, 0x00});
    protocol.SetTeamColor(Spades::TeamType::B, {0x00, 0xFF, 0x00});
    protocol.SetSpawn(Spades::TeamType::A, {{64.F, 224.F}, 64.F});
    protocol.SetSpawn(Spades::TeamType::B, {{384.F, 224.F}, 64.F});
    protocol.SetSpawn(Spades::TeamType::Spectator, {{256.F, 256.F}, 0.F});

    std::cout << "starting server\n";
    return Spades::Server().Run(protocol, 1);
}
