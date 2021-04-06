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

class Random
{
  public:
    Random() : mDistribution(0.f, 1.f)
    {
        std::random_device random;
        mGenerator.seed(random());
    }

    /**
     * @brief Get random float in range
     *
     * @param from First position
     * @param to Second position
     * @return Random value in range
     */
    float Get(float from, float to) const
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
    void Get(const Vector2f& from, const Vector2f& to, Vector3f& out) const
    {
        out.x = Get(from.x, to.x);
        out.y = Get(from.y, to.y);
        out.z = 0.f;
    }

  private:
    std::mt19937                          mGenerator;
    std::uniform_real_distribution<float> mDistribution;
};

} // namespace Spades
