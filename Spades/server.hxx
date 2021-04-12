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
