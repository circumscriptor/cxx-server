/**
 *
 * SpadesServer
 *
 */

#include "Server.hxx"

#include "Vector.hxx"

#include <enet/enet.h>
#include <iostream>

Spades::Server::Server(uint16 port, uint8 connections, uint32 inBandwidth, uint32 outBandwidth) : mProtocol(connections)
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

    mHost = enet_host_create(&address, connections, 2, inBandwidth, outBandwidth);
    if (mHost == NULL) {
        std::cerr << "failed to create server\n";
        std::exit(EXIT_FAILURE);
    }

    if (enet_host_compress_with_range_coder(mHost) != 0) {
        std::cerr << "failed to set compress with range coder\n";
        std::exit(EXIT_FAILURE);
    }
}

Spades::Server::~Server()
{
    std::cout << "destroying server\n";
    enet_host_destroy(mHost);
}

int Spades::Server::Run(uint32 timeout)
{
    std::cout << "loading map\n";
    mProtocol.LoadMap("hallway.vxl");
    std::cout << "running serer\n";

    mProtocol.SetSpawn(Team::A, Quad2f({64.f, 224.f}, 64.f, 64.f));
    mProtocol.SetSpawn(Team::B, Quad2f({384.f, 224.f}, 64.f, 64.f));

    mProtocol.SetColor(Team::A, {0xff, 0x00, 0x00});
    mProtocol.SetColor(Team::B, {0x00, 0xff, 0x00});

    mProtocol.Setup();

    ENetEvent event;
    while (true) {
        while (enet_host_service(mHost, &event, timeout)) {
            switch (event.type) {
                case ENET_EVENT_TYPE_NONE:
                    break;
                case ENET_EVENT_TYPE_CONNECT:
                    mProtocol.TryConnect(event.peer);
                    break;
                case ENET_EVENT_TYPE_DISCONNECT:
                    mProtocol.TryDisconnect(event.peer);
                    break;
                case ENET_EVENT_TYPE_RECEIVE:
                    DataStream stream(event.packet);
                    Player*    player = reinterpret_cast<Player*>(event.peer->data);
                    mProtocol.ProcessInput(*player, stream);
                    enet_packet_destroy(event.packet);
                    break;
            }
        }
        mProtocol.Update(double(1) / 20);
        // enet_host_flush(mHost);
    }
    return 0;
}
