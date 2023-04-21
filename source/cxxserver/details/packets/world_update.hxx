///
/// @file WorldUpdate.cpp
/// @brief ...
///
/// @copyright Copyright (c) 2023
///
///

#pragma once

#include "common.hxx"

#include <array>
#include <glm/vec3.hpp>

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

namespace cxxserver::details::packets {

struct PlayerUpdate
{
    glm::vec3 position;
    glm::vec3 orientation;

    template <typename SerializerType>
    void serialize(SerializerType & serializer)
    {
        serializer.object(position);
        serializer.object(orientation);
    }
};

struct WorldUpdate
{
    static const std::size_t MAX_PLAYERS = 32;

    std::array<PlayerUpdate, MAX_PLAYERS> players;

    template <typename SerializerType>
    void serialize(SerializerType & serializer)
    {
        serializer.container(players);
    }
};

} // namespace cxxserver::details::packets
