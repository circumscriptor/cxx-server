///
/// @file protocol.hxx
/// @brief ...
///
/// @copyright Copyright (c) 2023
///
///

#pragma once

#include <cxxserver/api/server_api.hxx>
#include <enet/enet.h>

namespace cxxserver {

class Protocol
{
  public:

    Protocol()                             = default;
    Protocol(const Protocol &)             = delete;
    Protocol(Protocol &&)                  = delete;
    Protocol & operator=(const Protocol &) = delete;
    Protocol & operator=(Protocol &&)      = delete;
    virtual ~Protocol()                    = default;

    ///
    /// @brief ...
    ///
    /// @param peer ..
    ///
    virtual void tryConnect(ENetPeer * peer) = 0;

    ///
    /// @brief ...
    ///
    /// @param peer ...
    ///
    virtual void tryDisconnect(ENetPeer * peer) = 0;

    ///
    /// @brief ...
    ///
    /// @param peer ...
    /// @param packet ...
    ///
    virtual void tryReceive(ENetPeer * peer, ENetPacket * packet) = 0;

    ///
    /// @brief ...
    ///
    /// @param version ...
    /// @return ...
    ///
    static Protocol * create(ProtocolVersion version);
};

} // namespace cxxserver
