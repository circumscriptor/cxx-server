/**
 *
 * SpadesServer
 *
 */

#include "Packet.hxx"

#include <enet/enet.h>

Spades::Packet::Packet(uint32 size, bool sequence) :
    Packet(enet_packet_create(nullptr, size, (sequence) ? ENET_PACKET_FLAG_UNSEQUENCED : ENET_PACKET_FLAG_RELIABLE))
{
}

Spades::Packet::~Packet()
{
    if (mPacket && !mIsSent) {
        enet_packet_destroy(mPacket);
    }
}
