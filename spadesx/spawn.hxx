/**
 * @file spawn.hxx
 * @brief This file is part of the experimental SpadesX project
 */

#pragma once

#include "connection.hxx"

namespace spadesx {

/**
 * @brief Helper class for calculating spawn location
 *
 */
class spawn
{
  public:
    /**
     * @brief Set spawn area
     *
     * @param from Start of spawn area
     * @param to End of spawn area
     */
    void set(const glm::vec2& from, const glm::vec2& to)
    {
        m_from = from;
        m_diff = to - from;
    }

    /**
     * @brief Get spawn location
     *
     * @param t1 X-axis parameter
     * @param t2 Y-axis parameter
     * @param result Result position
     */
    void get(float t1, float t2, glm::vec3& result) const
    {
        result.x = m_from.x + (m_diff.x * t1);
        result.y = m_from.y + (m_diff.y * t2);
    }

  private:
    glm::vec2 m_from;
    glm::vec2 m_diff;
};

} // namespace spadesx
