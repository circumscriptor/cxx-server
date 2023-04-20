///
/// @file Protocol.hpp
/// @brief ...
///
/// @copyright Copyright (c) 2023
///
///

#pragma once

#include <cxxserver/Protocol.hpp>

#include <cstdint>

namespace cxxserver::Details::V75 {

class Protocol : public cxxserver::Protocol
{
  public:

    CXXSERVER_CLASS_DEFAULT_VIRTUAL_OUT_OF_LINE_OVERRIDE(Protocol)

    static const std::uint8_t MAX_NUM_PLAYERS = 32;

    void try_connect(ENetPeer * peer) override;

    void try_disconnect(ENetPeer * peer) override;

    void try_receive(ENetPeer * peer, ENetPacket * packet) override;

  private:

    std::uint32_t mNumPlayers {};
};

} // namespace cxxserver::Details::V75
