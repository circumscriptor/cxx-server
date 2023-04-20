///
/// @file Connection.hpp
/// @brief ...
///
/// @copyright Copyright (c) 2023
///
///

#pragma once

#include <cstdint>
#include <cxxserver/Generated/ClassDefinitions.hpp>

#include <enet/enet.h>

namespace cxxserver {

class Connection
{
  public:

    static const std::uint8_t INVALID = 0xFF;

    CXXSERVER_CLASS_DEFAULT_ALL(Connection)

    void apply(ENetPeer * peer);

    [[nodiscard]] std::uint8_t id() const noexcept;

    bool send(ENetPacket * packet);

  private:

    ENetPeer * mPeer {};
};

} // namespace cxxserver
