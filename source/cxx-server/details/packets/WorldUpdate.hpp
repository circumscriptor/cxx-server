///
/// @file WorldUpdate.cpp
/// @brief ...
///
/// @copyright Copyright (c) 2023
///
///

#pragma once

#include "Common.hpp"

#include <bitsery/traits/array.h>

#include <glm/vec3.hpp>

#include <array>

namespace cxxserver::Details::Packets {

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

} // namespace cxxserver::Details::Packets
