/**
 * @file intel.hxx
 * @brief This file is part of the experimental SpadesX project
 */

#pragma once

#include "connection.hxx"
#include "data/enums.hxx"

#include <cstdint>

namespace spadesx {

/**
 * @brief Intel data
 *
 */
class intel_data : public entity
{
  public:
    /**
     * @brief Construct a new intel object
     *
     * @param team Team
     */
    constexpr intel_data(team_type team) noexcept : entity{entity_type::intel, object_id::intel_id(team), team}
    {
    }

    /**
     * @brief Reset intel (same as drop)
     *
     */
    constexpr void reset() noexcept
    {
        m_taken  = false;
        m_holder = object_id::invalid;
    }

    /**
     * @brief Drop intel (return back to previous location)
     *
     */
    constexpr void drop() noexcept
    {
        m_taken  = false;
        m_holder = object_id::invalid;
    }

    /**
     * @brief Drop intel
     *
     */
    constexpr void drop(const glm::vec3& position) noexcept
    {
        m_position = position;
        m_taken    = false;
        m_holder   = object_id::invalid;
    }

    /**
     * @brief Pick intel
     *
     * @param player Player
     */
    constexpr void pick(std::uint8_t player) noexcept
    {
        m_holder = player;
        m_taken  = true;
    }

    /**
     * @brief Pick intel
     *
     * @param connection Connection (player)
     */
    constexpr void pick(const connection& connection) noexcept
    {
        m_holder = connection.id();
        m_taken  = true;
    }

    /**
     * @brief Is intel taken
     *
     * @return true If taken
     */
    [[nodiscard]] constexpr bool is_taken() const noexcept
    {
        return m_taken;
    }

    /**
     * @brief Get intel holder
     *
     * @return Holder
     */
    [[nodiscard]] constexpr std::uint8_t holder() const noexcept
    {
        return m_holder;
    }

    /**
     * @brief Check whether the intel taken by the given player
     *
     * @param id Player ID
     * @return true If is taken by the given player
     */
    [[nodiscard]] constexpr bool is_held_by(std::uint8_t id) const noexcept
    {
        return m_taken && m_holder == id;
    }

    /**
     * @brief Check whether the intel taken by the given player
     *
     * @param connection Connection (player)
     * @return true If is taken by the given player
     */
    [[nodiscard]] constexpr bool is_held_by(const connection& connection) const noexcept
    {
        return m_taken && m_holder == connection.id();
    }

  private:
    std::uint8_t m_holder{object_id::invalid}; //!< Intel holder
    bool         m_taken{false};               //!< Is intel taken
};

} // namespace spadesx
