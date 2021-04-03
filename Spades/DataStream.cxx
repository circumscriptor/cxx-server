/**
 *
 * SpadesServer
 *
 */

#include "DataStream.hxx"

#include <cassert>
#include <cstring>
#include <enet/enet.h>

Spades::DataStream::DataStream(ENetPacket* packet) :
    mData{packet->data},
    mLength{static_cast<uint32>(packet->dataLength)},
    mPos{0}
{
}

void Spades::DataStream::ReadArray(void* buffer, uint32 length)
{
    assert(mPos + length <= mLength);
    std::memcpy(buffer, mData + mPos, length);
    mPos += length;
}

void Spades::DataStream::WriteArray(const void* data, uint32 length)
{
    assert(mPos + length <= mLength);
    std::memcpy(mData + mPos, data, length);
    mPos += length;
}
