/**
 * @file server.hxx
 * @brief This file is part of the experimental SpadesX project
 */

#pragma once

#include "protocol.hxx"

#include <enet/enet.h>

namespace spadesx {

/**
 * @brief Server (ENetHost wrapper)
 *
 */
class server
{
  public:

    /**
     * @brief Construct a new server object
     *
     */
    server()
        : server(32887)
    {
    }

    /**
     * @brief Construct a new server object
     *
     * @param port Port
     */
    server(std::uint16_t port)
        : server(port, 32)
    {
    }

    /**
     * @brief Construct a new server object
     *
     * @param port Port
     * @param connections Max number of connections
     */
    server(std::uint16_t port, std::uint8_t connections)
        : server(port, connections, 0, 0)
    {
    }

    /**
     * @brief Construct a new server object
     *
     * @param port Port
     * @param connections Max number of connections
     * @param bandwidth_in Incoming bandwidth
     * @param bandwidth_out Outcoming bandwidth
     */
    server(std::uint16_t port, std::uint8_t connections, std::uint32_t bandwidth_in, std::uint32_t bandwidth_out)
    {
        m_host = enet::get().host(port, connections, 2, bandwidth_in, bandwidth_out);
    }

    /**
     * @brief Destroy the server object
     *
     */
    ~server()
    {
        enet::get().destroy(m_host);
    }

    /**
     * @brief Run protocol
     *
     * @param protocol Protocol
     * @param timeout Timeout
     * @return 0 on success
     */
    int run(protocol & protocol, std::uint32_t timeout)
    {
        protocol.start();
        m_running = true;

        ENetEvent event;
        while (m_running)
        {
            while (enet_host_service(m_host, &event, timeout) > 0)
            {
                switch (event.type)
                {
                    case ENET_EVENT_TYPE_NONE:
                        break;
                    case ENET_EVENT_TYPE_CONNECT:
                        protocol.try_connect(event.peer);
                        break;
                    case ENET_EVENT_TYPE_DISCONNECT:
                        protocol.try_disconnect(event.peer);
                        break;
                    case ENET_EVENT_TYPE_RECEIVE:
                        auto &      connection = protocol::peer_to_connection(event.peer);
                        data_stream stream     = event.packet;
                        protocol.on_receive(connection, stream);
                        enet_packet_destroy(event.packet);
                        break;
                }
            }
            protocol.update();
        }

        return 0;
    }

  private:

    ENetHost * m_host; //!< Host
    bool       m_running { false };
};

} // namespace spadesx
