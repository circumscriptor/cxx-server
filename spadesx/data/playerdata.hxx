/**
 *
 * SpadesX
 *
 */

#pragma once

#include "enums.hxx"
#include "inputdata.hxx"
#include "lastkilldata.hxx"
#include "namedata.hxx"
#include "playerstate.hxx"
#include "scoredata.hxx"
#include "transformdata.hxx"
#include "types.hxx"
#include "weapondata.hxx"

namespace spadesx {

/**
 * @brief Player data
 *
 */
class player_data :
    public input_data,
    public player_state,
    public last_kill_data,
    public transform_data,
    public weapon_data,
    public score_data,
    public name_data<16>,
    public color_data
{
  public:
    /**
     * @brief Construct a new player_data object
     *
     */
    player_data() noexcept = default;

    /**
     * @brief Destroy the player_data object
     *
     */
    ~player_data() noexcept = default;

    /**
     * @brief Reset values after death
     *
     */
    void reset_death()
    {
        reset_player_state_death();
        reset_input_data();
        m_health = 100;
        m_tool   = tool_type::gun;
    }

    /**
     * @brief Reset to initial state
     *
     */
    void reset_values()
    {
        reset_player_state();
        reset_input_data();
        reset_transform();
        reset_score();
        m_health       = 100;
        m_tool         = tool_type::gun;
        m_respawn_time = 0;
    }

    std::uint8_t m_health;          //!< Player's health
    tool_type    m_tool;            //!< Currently held tool
    team_type    m_team;            //!< Current team
    std::uint8_t m_respawn_time{0}; //!< Respawn time
};

} // namespace spadesx
