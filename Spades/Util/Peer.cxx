/**
 *
 * SpadesServer
 *
 */

#include "Peer.hxx"

#include <enet/enet.h>

void Spades::Peer::Disconnect(Reason reason)
{
    if (mPeer != nullptr) {
        enet_peer_disconnect(mPeer, static_cast<uint32>(reason));
        mPeer = nullptr;
    }
}

auto Spades::Peer::Send(DataStream& data, uint8 channel, bool unsequenced) -> bool
{
    if (mPeer != nullptr) {
        uint32      flag   = (unsequenced) ? ENET_PACKET_FLAG_UNSEQUENCED : ENET_PACKET_FLAG_RELIABLE;
        ENetPacket* packet = enet_packet_create(data.Data(), data.Length(), flag);
        if (packet != nullptr) {
            return enet_peer_send(mPeer, channel, packet) == 0;
        }
        return false;
    }
    return false;
}
