/**
 *
 * SpadesX
 *
 */

#pragma once

#include <glm/glm.hpp>
#include <glm/vec3.hpp>

namespace spadesx {

/**
 * @brief Position data
 *
 */
class position_data
{
  public:
    /**
     * @brief Reset position data
     *
     */
    void reset_position() noexcept
    {
        m_position = {0.F, 0.F, 0.F};
    }

    /**
     * @brief Get distance
     *
     * @param position Another position data
     * @return Distance
     */
    [[nodiscard]] float distance(const position_data& position) const noexcept
    {
        return glm::distance(m_position, position.m_position);
    }

    glm::vec3 m_position; //!< Player position
};

} // namespace spadesx
