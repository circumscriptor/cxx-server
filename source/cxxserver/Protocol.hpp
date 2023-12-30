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

class Protocol {
public:

    Protocol()                           = default;
    Protocol(const Protocol&)            = delete;
    Protocol(Protocol&&)                 = delete;
    Protocol& operator=(const Protocol&) = delete;
    Protocol& operator=(Protocol&&)      = delete;
    virtual ~Protocol()                  = default;

    ///
    /// @brief ...
    ///
    /// @param peer ..
    ///
    virtual void try_connect(ENetPeer* peer) = 0;

    ///
    /// @brief ...
    ///
    /// @param peer ...
    ///
    virtual void try_disconnect(ENetPeer* peer) = 0;

    ///
    /// @brief ...
    ///
    /// @param peer ...
    /// @param packet ...
    ///
    virtual void try_receive(ENetPeer* peer, ENetPacket* packet) = 0;
};

} // namespace cxxserver
