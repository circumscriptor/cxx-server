///
/// @file protocol_v75.cxx
/// @brief ...
///
/// @copyright Copyright (c) 2023
///
///

#include "protocol_v75.hxx"

#include <cxxserver/connection.hxx>

namespace cxxserver::details {

void ProtocolV75::tryConnect(ENetPeer * peer)
{
    if (peer->eventData != static_cast<std::uint32_t>(ProtocolVersion::V75))
    {
        enet_peer_disconnect(peer, static_cast<std::uint32_t>(DisconnectReason::WRONG_PROTOCOL_VERSION));
        return;
    }

    if (mNumPlayers >= MAX_NUM_PLAYERS)
    {
        enet_peer_disconnect(peer, static_cast<std::uint32_t>(DisconnectReason::SERVER_FULL));
        return;
    }

    // TODO(none): Connect
}

void ProtocolV75::tryDisconnect(ENetPeer * peer)
{
    if (peer->data != nullptr)
    {
        [[maybe_unused]] auto * connection = static_cast<Connection *>(peer->data);
        // TODO(none): Notify disconnect
    }
}

void ProtocolV75::tryReceive(ENetPeer * peer, [[maybe_unused]] ENetPacket * packet)
{
    [[maybe_unused]] auto * connection = static_cast<Connection *>(peer->data);
    // TODO(none): Receive
}

} // namespace cxxserver::details
