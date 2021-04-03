/**
 *
 * SpadesServer
 *
 */

#pragma once

#include "DataStream.hxx"

typedef struct _ENetPeer ENetPeer;

namespace Spades {

class Packet : public DataStream
{
    friend class Peer;

  public:
    Packet(uint32 size, bool sequence = false);

    ~Packet();

    bool IsValid() const noexcept
    {
        return mPacket != nullptr;
    }

    bool IsSent() const noexcept
    {
        return mIsSent;
    }

  private:
    Packet(ENetPacket* packet) : mPacket{packet}, DataStream(packet)
    {
    }

    ENetPacket* mPacket; //!< Packet pointer (created by ENet)
    bool        mIsSent; //!< True if sent by peer
};

} // namespace Spades
