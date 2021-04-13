/**
 *
 * SpadesX
 *
 */

#pragma once

#include "baseprotocol.hxx"
#include "enet.hxx"

namespace spadesx {

class server
{
  public:
    server() : server(32887)
    {
    }

    server(std::uint16_t port) : server(port, 32)
    {
    }

    server(std::uint16_t port, std::uint8_t connections) : server(port, connections, 0, 0)
    {
    }

    server(std::uint16_t port, std::uint8_t connections, std::uint32_t bandwidth_in, std::uint32_t bandwidth_out)
    {
        m_host = enet::get().host(port, connections, 2, bandwidth_in, bandwidth_out);
    }

    int run(base_protocol& protocol, std::uint32_t timeout)
    {
        protocol.start();
        m_running = true;

        ENetEvent event;
        while (m_running) {
            while (enet_host_service(m_host, &event, timeout) > 0) {
                switch (event.type) {
                    case ENET_EVENT_TYPE_NONE:
                        break;
                    case ENET_EVENT_TYPE_CONNECT:
                        protocol.try_connect(event.peer);
                        break;
                    case ENET_EVENT_TYPE_DISCONNECT:
                        protocol.on_disconnect(event.peer);
                        break;
                    case ENET_EVENT_TYPE_RECEIVE:
                        protocol.on_receive(event.peer, event.packet);
                        enet_packet_destroy(event.packet);
                        break;
                }
            }
            protocol.update();
        }

        return 0;
    }

  private:
    ENetHost* m_host; //!< Host
    bool      m_running{false};
};

} // namespace spadesx
