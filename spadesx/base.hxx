/**
 *
 * SpadesX
 *
 */

#pragma once

#include "data/enums.hxx"
#include "data/player.hxx"
#include "data/position.hxx"

#include <glm/glm.hpp>

namespace spadesx {

/**
 * @brief Base data
 *
 */
class base : public position_data
{
  public:
    /**
     * @brief Construct a new base object
     *
     */
    base() noexcept = default;

    /**
     * @brief Construct a new base object
     *
     * @param team Team
     */
    base(team_type team) noexcept : m_team{team}
    {
    }

    /**
     * @brief Destroy the base object
     *
     */
    ~base() noexcept = default;

    /**
     * @brief Get team
     *
     * @return Team
     */
    [[nodiscard]] team_type get_team() const noexcept
    {
        return m_team;
    }

    /**
     * @brief Set team (spectator = neutral)
     *
     * @param team Team
     */
    void set_team(team_type team) noexcept
    {
        m_team = team;
    }

    /**
     * @brief Set player's team
     *
     * @param player Player
     */
    void set_team(const player_data& player) noexcept
    {
        set_team(player.m_team);
    }

    /**
     * @brief Check whether player is in same team as the base is
     *
     * @param player Player
     * @return true If player is in the same team as the base is
     */
    [[nodiscard]] bool is_same_team(const player_data& player) const noexcept
    {
        return m_team == player.m_team;
    }

  private:
    team_type m_team; //!< Team, spectator = neutral
};

} // namespace spadesx
