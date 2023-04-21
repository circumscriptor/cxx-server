/**
 * @file server.hxx
 * @brief This file is part of the experimental SpadesX project
 */

#pragma once

#include "server_api.hxx"

#include <cstdint>
#include <enet/enet.h>

namespace cxxserver {

class Protocol;

/**
 * @brief Server (ENetHost wrapper)
 *
 */
class Server
{
  public:

    Server(const Server &)             = delete;
    Server(Server &&)                  = delete;
    Server & operator=(const Server &) = delete;
    Server & operator=(Server &&)      = delete;

    ///
    /// @brief Construct a new Server object
    ///
    /// @param config Create information
    ///
    explicit Server(const ServerApi::CreateInfo & config);

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

} // namespace cxxserver
