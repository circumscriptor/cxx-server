/**
 * @file main.cxx
 * @brief This file is part of the experimental SpadesX project
 */

#include "commands/kill.hxx"
#include "commands/say.hxx"
#include "ctf.hxx"
#include "server.hxx"

/**
 * @brief Main?
 *
 * @param argc Number of arguments
 * @param argv Arguments
 * @return 0 on success
 */
int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cerr << "no map specified" << std::endl;
        return 1;
    }

    spadesx::ctf_protocol protocol(32);
    try {
        protocol.load_map(argv[1]);
    } catch (const std::exception& ex) {
        std::cerr << "exception: " << ex.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "unknown exception" << std::endl;
        return 1;
    }

    protocol.get_spawn(spadesx::team_type::a).set({64.F, 224.F}, {128.F, 290.F});
    protocol.get_spawn(spadesx::team_type::b).set({64.F, 224.F}, {128.F, 290.F});
    // protocol.get_spawn(spadesx::team_type::b).set({384.F, 224.F}, {448.F, 290.F});
    protocol.get_spawn(spadesx::team_type::spectator).set({256.F, 256.F}, {256.F, 256.F});
    protocol.get_team(spadesx::team_type::a).set_name("Team A");
    protocol.get_team(spadesx::team_type::b).set_name("Team B");
    protocol.get_team(spadesx::team_type::a).color() = {0xFF, 0x00, 0x00};
    protocol.get_team(spadesx::team_type::b).color() = {0x00, 0xFF, 0x00};
    protocol.set_fog_color({0xFF, 0xFF, 0xFF});
    protocol.set_respawn_time(3);

    protocol.register_command<spadesx::say_command>("say");
    protocol.register_command<spadesx::kill_command>("kill");

    return spadesx::server().run(protocol, 1);
}
