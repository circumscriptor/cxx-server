/**
 *
 * SpadesX
 *
 */

#pragma once

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

    glm::vec3 m_position; //!< Player position
};

} // namespace spadesx
