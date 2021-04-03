/**
 *
 * SpadesServer
 *
 */

#include "Peer.hxx"

#include <enet/enet.h>

Spades::uint32 Spades::Peer::EventData()
{
    if (IsValid()) {
        return mPeer->eventData;
    }
    return 0;
}

void Spades::Peer::Disconnect(Reason reason)
{
    if (IsValid()) {
        enet_peer_disconnect(mPeer, static_cast<uint32>(reason));
        mPeer = nullptr;
    }
}

bool Spades::Peer::Send(Packet& packet, uint8 channel)
{
    if (!IsValid()) {
        return false;
    }
    packet.mIsSent = true;
    return enet_peer_send(mPeer, channel, packet.mPacket) == 0;
}
