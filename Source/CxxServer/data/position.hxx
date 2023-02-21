/**
 * @file position.hxx
 * @brief This file is part of the experimental SpadesX project
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

    constexpr position_data() noexcept = default;

    /**
     * @brief Construct a new position data object
     *
     * @param position Initial possition
     */
    constexpr position_data(const glm::vec3 & position) noexcept
        : m_position { position }
    {
    }

    /**
     * @brief Reset position data
     *
     */
    void reset_position() noexcept
    {
        m_position = { 0.F, 0.F, 0.F };
    }

    /**
     * @brief Get distance
     *
     * @param position Another position data
     * @return Distance
     */
    [[nodiscard]] float distance(const position_data & position) const noexcept
    {
        return glm::distance(m_position, position.m_position);
    }

    glm::vec3 m_position { 0.F, 0.F, 0.F }; //!< Player position
};

} // namespace spadesx
