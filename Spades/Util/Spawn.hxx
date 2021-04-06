/**
 *
 * SpadesServer
 *
 */

#pragma once

#include "Core/Random.hxx"
#include "Map.hxx"

namespace Spades {

/**
 * @brief Spawn position (area)
 *
 */
class Spawn
{
  public:
    /**
     * @brief Construct a new Spawn object
     *
     */
    constexpr Spawn() noexcept = default;

    /**
     * @brief Construct a new Spawn object
     *
     * @param pos Starting position (top-left corner, closer to word origin)
     * @param side The size of the edge of the square spawn area
     */
    constexpr Spawn(const Vector2& pos, float side) : mFrom{pos}, mTo{pos.x + side, pos.y + side}
    {
    }

    /**
     * @brief Construct a new Spawn object
     *
     * @param from Starting position
     * @param to End position
     */
    constexpr Spawn(const Vector2& from, const Vector2& to) : mFrom{from}, mTo{to}
    {
    }

    // Default copy constructor
    constexpr Spawn(const Spawn& other) = default;

    // Default assign operator
    constexpr auto operator=(const Spawn& other) -> Spawn& = default;

    /**
     * @brief Set spawn area
     *
     * @param from Starting position
     * @param to End position
     */
    constexpr void Set(const Vector2& from, const Vector2& to) noexcept
    {
        mFrom = from;
        mTo   = to;
    }

    /**
     * @brief Get spawn location (2D-vector)
     *
     * @param random Random number generator
     * @param map Map object
     * @param out Output vector
     */
    void GetLocation2(Random& random, Vector3& out)
    {
        random.Get(mFrom, mTo, out);
    }

    /**
     * @brief Get spawn location (3D-vector)
     *
     * @param random Random number generator
     * @param map Map object
     * @param out Output vector
     */
    void GetLocation3(Random& random, const Map& map, Vector3& out)
    {
        random.Get(mFrom, mTo, out);
        out.z = static_cast<float>(map.GetHeight((uint32) out.x, (uint32) out.y) - 1);
    }

  private:
    Vector2 mFrom; //!< Starting position
    Vector2 mTo;   //!< End position
};

} // namespace Spades
