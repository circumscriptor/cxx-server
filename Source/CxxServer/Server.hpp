/**
 * @file Server.hpp
 * @brief This file is part of the experimental SpadesX project
 */

#pragma once

#include <CxxServer/Generated/ClassDefinitions.hpp>

#include <enet/enet.h>

#include <cstdint>

namespace CxxServer {

class Protocol;

struct ServerCreateInfo
{
    static constexpr std::uint16_t DEFAULT_PORT              = 32887;
    static constexpr std::uint8_t  DEFAULT_CONNECTIONS_LIMIT = 32;

    std::uint32_t bandwidthIncoming { 0 };
    std::uint32_t bandwidthOutgoing { 0 };
    std::uint32_t timeout { 0 };
    std::uint16_t port { DEFAULT_PORT };
    std::uint8_t  connections { DEFAULT_CONNECTIONS_LIMIT };
};

/**
 * @brief Server (ENetHost wrapper)
 *
 */
class Server
{
  public:

    CXXSERVER_CLASS_DELETE_COPY_AND_MOVE(Server)

    ///
    /// @brief Construct a new Server object
    ///
    /// @param config Create information
    ///
    explicit Server(const ServerCreateInfo & config);

    /**
     * @brief Destroy the Server object
     *
     */
    ~Server();

    /**
     * @brief Run protocol
     *
     * @param protocol Protocol
     * @return 0 on success
     */
    bool service(Protocol & protocol);

  private:

    ENetHost *    mHost;
    std::uint32_t mTimeout;
};

} // namespace CxxServer
