/**
 * @file main.cxx
 * @brief This file is part of the experimental SpadesX project
 */

// #include "commands/kill.hxx"
// #include "commands/say.hxx"
// #include "ctf.hxx"
#include "CxxServer/Protocol.hpp"
#include "Server.hpp"

#include <enet/enet.h>
#include <memory>

/**
 * @brief Main?
 *
 * @param argc Number of arguments
 * @param argv Arguments
 * @return 0 on success
 */
int main([[maybe_unused]] int argc, [[maybe_unused]] char ** argv)

{
    if (enet_initialize() < 0)
    {
        // error
        return 1;
    }

    CxxServer::ServerCreateInfo createInfo;
    createInfo.timeout = 1;

    {
        std::unique_ptr<CxxServer::Protocol> protocol { CxxServer::Protocol::Create(CxxServer::Version::V75) };

        CxxServer::Server server { createInfo };
        server.service(*protocol);
    }

    enet_deinitialize();
    return 0;
}
