/**
 *
 * SpadesX
 *
 */

#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace spadesx {

class spawn
{
  public:
    void set(const glm::vec2& from, const glm::vec2& to)
    {
        m_from = from;
        m_diff = to - from;
    }

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
