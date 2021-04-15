/**
 *
 * SpadesX
 *
 */

#pragma once

#include "positiondata.hxx"

#include <cstdint>
#include <glm/glm.hpp>

namespace spadesx {

/**
 * @brief Intel data
 *
 */
class intel_data : public position_data
{
  public:
    /**
     * @brief Drop intel
     *
     */
    void drop(const glm::vec3& position)
    {
        m_position = position;
        m_taken    = false;
    }

    /**
     * @brief Pick intel
     *
     * @param player Player
     */
    void pick(std::uint8_t player)
    {
        m_holder = player;
        m_taken  = true;
    }

    /**
     * @brief Get distance between player's position and intel's position
     *
     * @param position Player's position
     * @return Distance
     */
    [[nodiscard]] float distance(const glm::vec3& position) const noexcept
    {
        return glm::distance(m_position, position);
    }

    /**
     * @brief Is intel taken
     *
     * @return true If taken
     */
    [[nodiscard]] bool is_taken() const noexcept
    {
        return m_taken;
    }

    /**
     * @brief Get intel holder
     *
     * @return Holder
     */
    [[nodiscard]] std::uint8_t holder() const noexcept
    {
        return m_holder;
    }

  private:
    std::uint8_t m_holder;       //!< Intel holder
    bool         m_taken{false}; //!< Is intel taken
};

} // namespace spadesx
