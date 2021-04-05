/**
 *
 * SpadesServer
 *
 */

#pragma once

typedef struct _ENetHost   ENetHost;
typedef struct _ENetPeer   ENetPeer;
typedef struct _ENetPacket ENetPacket;

namespace Spades {

typedef unsigned char  uint8;
typedef unsigned short uint16;
typedef unsigned int   uint32;

struct Color3b
{
    uint8 r;
    uint8 g;
    uint8 b;
};

constexpr uint32 ColorToU32(Color3b color) noexcept
{
    uint32 v = 0;
    // v |= 0x00 << 24;
    v |= uint32(color.r) << 16;
    v |= uint32(color.g) << 8;
    v |= uint32(color.b);
    return v;
}

constexpr void U32ToColor(uint32 v, Color3b& color) noexcept
{
    color.r = uint8(v >> 16);
    color.g = uint8(v >> 8);
    color.b = uint8(v);
}

} // namespace Spades
