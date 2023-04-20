///
/// @file Connection.cpp
/// @brief ...
///
/// @copyright Copyright (c) 2023
///
///

#pragma once

#include "Connection.hpp"

#include <cassert>

namespace cxxserver {

void Connection::apply(ENetPeer * peer)
{
    mPeer      = peer;
    peer->data = this;
}

std::uint8_t Connection::id() const noexcept
{
    return mPeer != nullptr ? mPeer->incomingSessionID : INVALID;
}

bool Connection::send(ENetPacket * packet)
{
    assert(mPeer != nullptr);
    assert(packet != nullptr);
    return enet_peer_send(mPeer, 0, packet) == 0;
}

} // namespace cxxserver
