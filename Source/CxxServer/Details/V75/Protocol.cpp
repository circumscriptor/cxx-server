///
/// @file Protocol.cpp
/// @brief ...
///
/// @copyright Copyright (c) 2023
///
///

#include "Protocol.hpp"

namespace CxxServer::Details::V75 {

Protocol::~Protocol() = default;

void Protocol::try_connect(ENetPeer * peer)
{
    if (peer->eventData != static_cast<std::uint32_t>(Version::V75))
    {
        enet_peer_disconnect(peer, static_cast<std::uint32_t>(DisconnectReason::WRONG_PROTOCOL_VERSION));
        return;
    }

    if (mNumPlayers >= MAX_NUM_PLAYERS)
    {
        enet_peer_disconnect(peer, static_cast<std::uint32_t>(DisconnectReason::SERVER_FULL));
        return;
    }

    // TODO: Connect
}

void Protocol::try_disconnect(ENetPeer * peer)
{
    if (peer->data != nullptr)
    {
        // TODO: Disconnect
    }
}

void Protocol::try_receive(ENetPeer * peer, [[maybe_unused]] ENetPacket * packet)
{
    [[maybe_unused]] auto * connection = peer->data;
    // TODO: Receive
}

} // namespace CxxServer::Details::V75
