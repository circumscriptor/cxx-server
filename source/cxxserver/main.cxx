///
/// @file main.cxx
/// @brief This file is part of the experimental SpadesX project
///

#include "server_api.hxx"
#include <memory>

///
/// @brief Main entry
///
/// @param argc Number of arguments
/// @param argv Arguments
/// @return 0 on success
///
int main([[maybe_unused]] int argc, [[maybe_unused]] char ** argv)
{
    if (not cxxserver::ServerApi::init())
    {
        return 1;
    }

    {
        cxxserver::ServerApi::CreateInfo      createInfo; // default
        std::unique_ptr<cxxserver::ServerApi> serverApi { cxxserver::ServerApi::create(createInfo) };
        if (serverApi)
        {
            // serverApi->registerHandler(0, [](const class Packet & packet) -> cxxserver::HandlerResult { return cxxserver::HandlerResult::VALID; });
            serverApi->run();
        }
    }

    cxxserver::ServerApi::stop();
    return 0;
}
