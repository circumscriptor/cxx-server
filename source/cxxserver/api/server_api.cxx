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

class ServerApiImpl : public ServerApi
{
  public:

    ServerApiImpl(const ServerApiImpl &)             = delete;
    ServerApiImpl(ServerApiImpl &&)                  = delete;
    ServerApiImpl & operator=(const ServerApiImpl &) = delete;
    ServerApiImpl & operator=(ServerApiImpl &&)      = delete;

    explicit ServerApiImpl(const CreateInfo & createInfo);

    ~ServerApiImpl() override = default;

    int  run() override;
    void registerHandler(std::uint8_t packetType, HandlerType handler) override;

  private:

    std::unique_ptr<Server>   mServer {};
    std::unique_ptr<Protocol> mProtocol {};
};

ServerApiImpl::ServerApiImpl(const CreateInfo & createInfo)
    : mServer { std::make_unique<Server>(createInfo) }
    , mProtocol { Protocol::create(createInfo.protocol) }
{
}

int ServerApiImpl::run()
{
    return mServer->service(*mProtocol) ? 0 : 1;
}

void ServerApiImpl::registerHandler([[maybe_unused]] std::uint8_t packetType, [[maybe_unused]] HandlerType handler) { }

ServerApi * ServerApi::create(const CreateInfo & createInfo)
{
    return new ServerApiImpl(createInfo);
}

bool ServerApi::init()
{
    return enet_initialize() < 0;
}

void ServerApi::stop()
{
    enet_deinitialize();
}

std::string_view ServerApi::version()
{
    return CXXSERVER_VERSION;
}

} // namespace cxxserver
