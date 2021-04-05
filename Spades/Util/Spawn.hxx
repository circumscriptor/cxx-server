/**
 *
 * SpadesServer
 *
 */

#pragma once

#include "Core/Random.hxx"
#include "Map.hxx"

namespace Spades {

class Spawn
{
  public:
    Spawn() = default;

    Spawn(const Vector2f& pos, float side) : mFrom{pos}, mTo{pos.x + side, pos.y + side}
    {
    }

    Spawn(const Vector2f& from, const Vector2f& to) : mFrom{from}, mTo{to}
    {
    }

    Spawn(const Spawn& other) : mFrom{other.mFrom}, mTo{other.mTo}
    {
    }

    Spawn& operator=(const Spawn& other)
    {
        mFrom = other.mFrom;
        mTo   = other.mTo;
        return *this;
    }

    void GetLocation(Random& random, Map& map, Vector3f& out)
    {
        random.Get(mFrom, mTo, out);
        out.z = map.GetHeight((uint32) out.x, (uint32) out.y) - 1;
    }

    Vector2f mFrom;
    Vector2f mTo;
};

} // namespace Spades
