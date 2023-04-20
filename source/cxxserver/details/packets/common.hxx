///
/// @file Serialize.hpp
/// @brief ...
///
/// @copyright Copyright (c) 2023
///
///

#pragma once

#include <cstdint>
#include <glm/vec3.hpp>

namespace cxxserver::details::packets {

enum class Packet
{
    POSITION_DATA     = 0,
    ORIENTATION_DATA  = 1,
    WORLD_UPDATE      = 2,
    INPUT_DATA        = 3,
    WEAPON_INPUT      = 4,
    HIT_PACKET        = 5,
    SET_HP            = 5,
    GRENADE_PACKET    = 6,
    SET_TOOL          = 7,
    SET_COLOR         = 8,
    EXISTING_PLAYER   = 9,
    SHORT_PLAYER_DATA = 10,
    MOVE_OBJECT       = 11,
    CREATE_PLAYER     = 12,
    BLOCK_ACTION      = 13,
    BLOCK_LINE        = 14,
    STATE_DATA        = 15,
    KILL_ACTION       = 16,
    CHAT_MESSAGE      = 17,
    MAP_START         = 18,
    MAP_CHUNK         = 19,
    PLAYER_LEFT       = 20,
    TERRITORY_CAPTURE = 21,
    PROGRESS_BAR      = 22,
    INTEL_CAPTURE     = 23,
    INTEL_PICKUP      = 24,
    INTEL_DROP        = 25,
    RESTOCK           = 26,
    FOG_COLOR         = 27,
    WEAPON_RELOAD     = 28,
    CHANGE_TEAM       = 29,
    CHANGE_WEAPON     = 30,
    MAP_CACHED        = 31
};

} // namespace cxxserver::details::packets

namespace bitsery {

template <typename SerializerType>
inline void serialize(SerializerType & serializer, glm::vec3 & vector)
{
    serializer.value4b(vector.x);
    serializer.value4b(vector.y);
    serializer.value4b(vector.z);
}

template <typename SerializerType>
inline void serialize(SerializerType & serializer, glm::ivec3 & vector)
{
    serializer.value4b(vector.x);
    serializer.value4b(vector.y);
    serializer.value4b(vector.z);
}

} // namespace bitsery
