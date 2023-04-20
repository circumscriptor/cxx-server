#include "server.hxx"

#include "protocol.hxx"

namespace cxxserver {

Server::Server(const ServerCreateInfo & config)
    : mTimeout { config.timeout }
{
    ENetAddress address;
    address.host = ENET_HOST_ANY;
    address.port = config.port;
    mHost        = enet_host_create(&address, config.connections, 2, config.bandwidthIncoming, config.bandwidthOutgoing);

    if (mHost == nullptr)
    {
        // error
    }

    if (enet_host_compress_with_range_coder(mHost) != 0)
    {
        // error
    }
}

Server::~Server()
{
    enet_host_destroy(mHost);
}

bool Server::service(Protocol & protocol)
{
    ENetEvent event;
    while (true)
    {
        int result = enet_host_service(mHost, &event, mTimeout);
        if (result <= 0)
        {
            return result == 0;
        }

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
                protocol.try_receive(event.peer, event.packet);
                enet_packet_destroy(event.packet);
                break;
        }
    }
}

} // namespace cxxserver
