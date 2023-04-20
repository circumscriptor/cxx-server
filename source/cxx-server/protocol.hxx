///
/// @file protocol.hxx
/// @brief ...
///
/// @copyright Copyright (c) 2023
///
///

#pragma once

#include <enet/enet.h>

namespace cxxserver {

enum class Version
{
    V75 = 3,
    V76 = 4
};

/**
 * @brief Disconnect reason
 *
 */
enum class DisconnectReason
{
    UNKNOWN                = 0,
    BANNED                 = 1,
    IP_LIMIT_EXCEEDED      = 2,
    WRONG_PROTOCOL_VERSION = 3,
    SERVER_FULL            = 4,
    KICKED                 = 10
};

class Protocol
{
  public:

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
    virtual void try_connect(ENetPeer * peer) = 0;

    ///
    /// @brief ...
    ///
    /// @param peer ...
    ///
    virtual void try_disconnect(ENetPeer * peer) = 0;

    ///
    /// @brief ...
    ///
    /// @param peer ...
    /// @param packet ...
    ///
    virtual void try_receive(ENetPeer * peer, ENetPacket * packet) = 0;

    ///
    /// @brief ...
    ///
    /// @param version ...
    /// @return ...
    ///
    static Protocol * Create(Version version);
};

} // namespace cxxserver
