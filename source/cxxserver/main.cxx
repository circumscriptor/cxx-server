///
/// @file main.cxx
/// @brief This file is part of the experimental SpadesX project
///

#include <cxxserver/api/server_api.hxx>
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
    if (not cxxserver::IServerApi::init())
    {
        return 1;
    }

    {
        cxxserver::IServerApi::CreateInfo      createInfo; // default
        std::unique_ptr<cxxserver::IServerApi> serverApi { cxxserver::IServerApi::create(createInfo) };
        if (serverApi)
        {
            // serverApi->registerHandler(0, [](const class Packet & packet) -> cxxserver::HandlerResult { return cxxserver::HandlerResult::VALID; });
            serverApi->run();
        }
    }

    cxxserver::IServerApi::stop();
    return 0;
}
