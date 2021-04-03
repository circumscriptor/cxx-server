/**
 *
 * SpadesServer
 *
 */

#pragma once

#include "Packet.hxx"

typedef struct _ENetPeer ENetPeer;

namespace Spades {

class Peer
{
    friend class Player;

  public:
    Peer()  = default;
    ~Peer() = default;

    bool IsValid() const noexcept
    {
        return mPeer != nullptr;
    }

    uint32 EventData();

    void Disconnect(Reason reason);

    bool Send(Packet& packet, uint8 channel = 0);

  private:
    void Set(ENetPeer* peer) noexcept
    {
        mPeer = peer;
    }

    ENetPeer* mPeer{nullptr};
};

} // namespace Spades
