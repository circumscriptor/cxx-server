/**
 *
 * SpadesX
 *
 */

#pragma once

#include "connection.hxx"

#include <cstdint>

namespace spadesx {

/**
 * @brief Intel data
 *
 */
class intel : public position_data
{
  public:
    /**
     * @brief Reset intel
     *
     */
    void reset()
    {
        m_taken = false;
    }

    /**
     * @brief Drop intel
     *
     */
    void drop()
    {
        m_taken = false;
    }

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
     * @brief Pick intel
     *
     * @param connection Connection (player)
     */
    void pick(const connection& connection)
    {
        pick(connection.get_id());
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

    /**
     * @brief Check whether the intel taken by the given player
     *
     * @param id Player ID
     * @return true If is taken by the given player
     */
    [[nodiscard]] bool is_held_by(std::uint8_t id) const noexcept
    {
        return m_taken && m_holder == id;
    }

    /**
     * @brief Check whether the intel taken by the given player
     *
     * @param connection Connection (player)
     * @return true If is taken by the given player
     */
    [[nodiscard]] bool is_held_by(const connection& connection) const noexcept
    {
        return is_held_by(connection.get_id());
    }

  private:
    std::uint8_t m_holder;       //!< Intel holder
    bool         m_taken{false}; //!< Is intel taken
};

} // namespace spadesx
