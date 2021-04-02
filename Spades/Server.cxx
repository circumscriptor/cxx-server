/**
 *
 * SpadesServer
 *
 */

#include "Server.hxx"

#include <enet/enet.h>
#include <iostream>

Spades::Server::Server(uint16 port, uint8 connections, uint32 inBandwidth, uint32 outBandwidth)
{
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
    enet_host_destroy(mHost);
}

int Spades::Server::Run(uint32 timeout)
{
    return 0;
}
