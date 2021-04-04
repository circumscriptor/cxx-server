/**
 *
 * SpadesServer
 *
 */

#pragma once

#include "Protocol.hxx"

namespace Spades {

/**
 * @brief Server class
 *
 */
class Server
{
  public:
    /**
     * @brief Construct a new Server object
     *
     */
    Server() : Server(32887)
    {
    }

    /**
     * @brief Construct a new Server object
     *
     * @param port Server port
     */
    Server(uint16 port) : Server(port, 32)
    {
    }

    /**
     * @brief Construct a new Server object
     *
     * @param port Server port
     * @param connections Maximal number of connections
     */
    Server(uint16 port, uint8 connections) : Server(port, connections, 0, 0)
    {
    }

    /**
     * @brief Construct a new Server object
     *
     * @param port Server port
     * @param connections Maximal number of connections
     * @param inBandwidth Input bandwidth (0 = no limit)
     * @param outBandwidth Output bandwidth (0 = no limit)
     */
    Server(uint16 port, uint8 connections, uint32 inBandwidth, uint32 outBandwidth);

    /**
     * @brief Destroy the Server object
     *
     */
    ~Server();

    /**
     * @brief Run server
     *
     * @param timeout ...
     * @return 0 on sucess
     */
    int Run(Protocol& protocol, uint32 timeout);

  private:
    ENetHost* mHost; //!< Host
};

} // namespace Spades
