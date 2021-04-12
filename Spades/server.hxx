/**
 *
 * SpadesX
 *
 */

#pragma once

#include "protocol.hxx"

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
        std::cout << "initializing server\n";

        if (enet_initialize() != 0) {
            std::cerr << "failed to initalize ENet\n";
            std::exit(EXIT_FAILURE);
        }
        atexit(enet_deinitialize);

        ENetAddress address;
        address.host = ENET_HOST_ANY;
        address.port = port;

        m_host = enet_host_create(&address, connections, 2, bandwidth_in, bandwidth_out);
        if (m_host == nullptr) {
            std::cerr << "failed to create server\n";
            std::exit(EXIT_FAILURE);
        }

        if (enet_host_compress_with_range_coder(m_host) != 0) {
            std::cerr << "failed to set compress with range coder\n";
            std::exit(EXIT_FAILURE);
        }
    }

    int run(protocol& prot, std::uint32_t timeout)
    {
        prot.on_start();

        ENetEvent event;
        while (true) {
            while (enet_host_service(m_host, &event, timeout) > 0) {
                switch (event.type) {
                    case ENET_EVENT_TYPE_NONE:
                        break;
                    case ENET_EVENT_TYPE_CONNECT:
                        prot.on_connect(event.peer);
                        break;
                    case ENET_EVENT_TYPE_DISCONNECT:
                        prot.on_disconnect(event.peer);
                        break;
                    case ENET_EVENT_TYPE_RECEIVE:
                        prot.on_receive(event.peer, event.packet);
                        enet_packet_destroy(event.packet);
                        break;
                }
            }
            prot.update();
        }

        return 0;
    }

  private:
    ENetHost* m_host; //!< Host
};

} // namespace spadesx
