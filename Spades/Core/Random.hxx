/**
 *
 * SpadesServer
 *
 */

#pragma once

#include "Vector.hxx"

#include <algorithm>
#include <cmath>
#include <random>

namespace Spades {

/**
 * @brief Random number generator
 *
 */
class Random
{
  public:
    /**
     * @brief Construct a new Random object
     *
     */
    Random() : mGenerator(std::random_device()()), mDistribution(0.F, 1.F)
    {
    }

    /**
     * @brief Get random float in range
     *
     * @param from First position
     * @param to Second position
     * @return Random value in range
     */
    [[nodiscard]] auto Get(float from, float to) -> float
    {
        // return std::min(std::abs(from), std::abs(to)) + std::abs(std::abs(to) - std::abs(from)) *
        // mDistribution(mGenerator);
        // return std::min(from, to) + std::abs(to - from) * mDistribution(mGenerator);
        return from + (to - from) * mDistribution(mGenerator);
    }

    /**
     * @brief Get random horizontal position from range (vertical position is 0)
     *
     * @param from First position
     * @param to Second position
     * @param out Output vector
     */
    void Get(const Vector2& from, const Vector2& to, Vector3& out)
    {
        out.x = Get(from.x, to.x);
        out.y = Get(from.y, to.y);
        out.z = 0.F;
    }

  private:
    std::mt19937                          mGenerator;
    std::uniform_real_distribution<float> mDistribution;
};

} // namespace Spades
