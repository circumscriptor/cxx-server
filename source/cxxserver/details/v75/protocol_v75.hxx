///
/// @file protocol_v75.hxx
/// @brief ...
///
/// @copyright Copyright (c) 2023
///
///

#pragma once

#include <cxxserver/protocol.hxx>

#include <cstdint>

namespace cxxserver::details {

class ProtocolV75 : public cxxserver::Protocol
{
  public:

    static const std::uint8_t MAX_NUM_PLAYERS = 32;

    void tryConnect(ENetPeer * peer) override;

    void tryDisconnect(ENetPeer * peer) override;

    void tryReceive(ENetPeer * peer, ENetPacket * packet) override;

  private:

    std::uint32_t mNumPlayers {};
};

} // namespace cxxserver::details
