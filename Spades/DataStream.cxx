/**
 *
 * SpadesServer
 *
 */

#include "DataStream.hxx"

#include <enet/enet.h>

Spades::DataStream::DataStream(ENetPacket* packet) :
    mData{packet->data},
    mLength{static_cast<uint32>(packet->dataLength)},
    mPos{0}
{
}
