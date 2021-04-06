/**
 *
 * SpadesServer
 *
 */

#pragma once

using ENetHost   = struct _ENetHost;
using ENetPeer   = struct _ENetPeer;
using ENetPacket = struct _ENetPacket;

namespace Spades {

using uint8  = unsigned char;  //!< Unsigned 8-bit word
using uint16 = unsigned short; //!< Unsigned 16-bit word
using uint32 = unsigned int;   //!< Unsigned 32-bit word

/**
 * @brief RGB color
 *
 */
struct Color3
{
    uint8 r; //!< Red component
    uint8 g; //!< Green component
    uint8 b; //!< Blue component
};

/**
 * @brief Convert Color3 to ARGB 32bit format
 *
 * @param color Color3 object
 * @return Color in ARGB format
 */
constexpr auto ColorToU32(const Color3& color) noexcept -> uint32
{
    uint32 v = 0;
    // v |= 0x00 << 24;
    v |= uint32(color.r) << 16;
    v |= uint32(color.g) << 8;
    v |= uint32(color.b);
    return v;
}

/**
 * @brief Convert color in ARGB format to Color3
 *
 * @param v Color in ARGB 32bit format
 * @param color Color3 object
 */
constexpr void U32ToColor(uint32 v, Color3& color) noexcept
{
    color.r = uint8(v >> 16);
    color.g = uint8(v >> 8);
    color.b = uint8(v);
}

} // namespace Spades
