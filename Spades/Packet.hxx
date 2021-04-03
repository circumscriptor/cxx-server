/**
 *
 * SpadesServer
 *
 */

#pragma once

#include "DataStream.hxx"

typedef struct _ENetPeer ENetPeer;

namespace Spades {

struct GrenadeEvent
{
    uint8    player;
    float    fuseLength;
    Vector3f position;
    Vector3f velocity;
};

struct HitEvent
{
    uint8   player;
    uint8   target;
    HitType type;
};

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
