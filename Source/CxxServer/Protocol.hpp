///
/// @file Protocol.hpp
/// @brief ...
///
/// @copyright Copyright (c) 2023
///
///

#pragma once

#include <CxxServer/Generated/ClassDefinitions.hpp>

#include <enet/enet.h>

namespace CxxServer {

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

    CXXSERVER_CLASS_DEFAULT_VIRTUAL_OUT_OF_LINE(Protocol)

    virtual void try_connect(ENetPeer * peer)                      = 0;

    virtual void try_disconnect(ENetPeer * peer)                   = 0;

    virtual void try_receive(ENetPeer * peer, ENetPacket * packet) = 0;

    static Protocol * Create(Version version);
};

} // namespace CxxServer
