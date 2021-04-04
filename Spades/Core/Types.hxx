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

} // namespace Spades
