///
/// @file server_api.cxx
/// @brief Server API
///
/// @copyright Copyright (c) 2023
///
///

#include "server_api.hxx"

#include <cxxserver/server.hxx>
#include <memory>

namespace cxxserver {

class ServerApi : public IServerApi
{
  public:

    ServerApi(const ServerApi &)             = delete;
    ServerApi(ServerApi &&)                  = delete;
    ServerApi & operator=(const ServerApi &) = delete;
    ServerApi & operator=(ServerApi &&)      = delete;

    explicit ServerApi(const CreateInfo & createInfo);

    ~ServerApi() override = default;

    int  run() override;
    void registerHandler(std::uint8_t packetType, HandlerType handler) override;

  private:

    std::unique_ptr<Server>   mServer {};
    std::unique_ptr<Protocol> mProtocol {};
};

ServerApi::ServerApi(const CreateInfo & createInfo)
    : mServer { std::make_unique<Server>(createInfo) }
    , mProtocol { Protocol::create(createInfo.protocol) }
{
}

int ServerApi::run()
{
    return mServer->service(*mProtocol) ? 0 : 1;
}

void ServerApi::registerHandler([[maybe_unused]] std::uint8_t packetType, [[maybe_unused]] HandlerType handler) { }

IServerApi * IServerApi::create(const CreateInfo & createInfo)
{
    return new ServerApi(createInfo);
}

bool IServerApi::init()
{
    return enet_initialize() < 0;
}

void IServerApi::stop()
{
    enet_deinitialize();
}

std::string_view IServerApi::version()
{
    return CXXSERVER_VERSION;
}

} // namespace cxxserver
