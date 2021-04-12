/**
 *
 * SpadesX
 *
 */

#include "protocol.hxx"
#include "server.hxx"

auto main(int argc, char** argv) -> int
{
    if (argc != 2) {
        return 1;
    }

    spadesx::protocol protocol(32);
    try {
        protocol.get_map().read_from_file(argv[1]);
    } catch (...) {
        std::cerr << "failed to read map" << std::endl;
        return 1;
    }

    protocol.get_spawn(spadesx::team_type::a).set({64.F, 224.F}, {128.F, 290.F});
    protocol.get_spawn(spadesx::team_type::b).set({384.F, 224.F}, {448.F, 290.F});
    protocol.get_spawn(spadesx::team_type::spectator).set({256.F, 256.F}, {256.F, 256.F});
    protocol.get_team(spadesx::team_type::a).set_name("Team A");
    protocol.get_team(spadesx::team_type::b).set_name("Team B");
    protocol.get_team(spadesx::team_type::a).m_color = {0xFF, 0x00, 0x00};
    protocol.get_team(spadesx::team_type::b).m_color = {0x00, 0xFF, 0x00};

    return spadesx::server().run(protocol, 1);
}
