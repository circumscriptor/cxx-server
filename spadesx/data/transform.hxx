/**
 *
 * SpadesX
 *
 */

#pragma once

#include "position.hxx"

namespace spadesx {

/**
 * @brief Position and orientation data
 *
 */
class transform_data : public position_data
{
  public:
    /**
     * @brief Reset position (and orientation) data
     *
     */
    void reset_transform() noexcept
    {
        reset_position();
        m_orientation = {0.F, 0.F, 0.F};
    }

    glm::vec3 m_orientation; //!< Player orientation
};

} // namespace spadesx
