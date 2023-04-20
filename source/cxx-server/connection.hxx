///
/// @file connection.hxx
/// @brief ...
///
/// @copyright Copyright (c) 2023
///
///

#pragma once

#include <cstdint>

using ENetPeer   = struct _ENetPeer;
using ENetPacket = struct _ENetPacket;

namespace cxxserver {

class Connection
{
  public:

    static const std::uint8_t INVALID = 0xFF;

    void apply(ENetPeer * peer);

    [[nodiscard]] std::uint8_t id() const noexcept;

    bool send(ENetPacket * packet);

  private:

    ENetPeer * mPeer {};
};

} // namespace cxxserver
