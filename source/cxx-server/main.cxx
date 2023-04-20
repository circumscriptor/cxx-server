/**
 * @file main.cxx
 * @brief This file is part of the experimental SpadesX project
 */

// #include "commands/kill.hxx"
// #include "commands/say.hxx"
// #include "ctf.hxx"
#include "Protocol.hpp"
#include "Server.hpp"

#include <bitsery/adapter/buffer.h>
#include <bitsery/traits/string.h>
#include <bitsery/traits/vector.h>

#include <enet/enet.h>

#include <iomanip>
#include <ios>
#include <iostream>
#include <memory>
#include <vector>

/**
 * @brief Main entry
 *
 * @param argc Number of arguments
 * @param argv Arguments
 * @return 0 on success
 */
int main([[maybe_unused]] int argc, [[maybe_unused]] char ** argv)
{
    // if (enet_initialize() < 0)
    // {
    //     // error
    //     return 1;
    // }

    // cxxserver::ServerCreateInfo createInfo;
    // createInfo.timeout = 1;

    // {
    //     std::unique_ptr<cxxserver::Protocol> protocol { cxxserver::Protocol::Create(cxxserver::Version::V75) };

    //     cxxserver::Server server { createInfo };
    //     server.service(*protocol);
    // }

    // enet_deinitialize();
    return 0;
}
