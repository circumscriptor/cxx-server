/**
 *
 * SpadesX
 *
 */

#pragma once

#include "entity.hxx"
#include "enums.hxx"
#include "input.hxx"
#include "name.hxx"
#include "score.hxx"
#include "state.hxx"
#include "types.hxx"
#include "weapon.hxx"

namespace spadesx {

/**
 * @brief Player data
 *
 */
class player_data :
    public entity,
    public input_data,
    public player_state,
    public weapon_data,
    public score_data,
    public name_data<16>,
    public color_data
{
  public:
    /**
     * @brief Construct a new player_data object
     *
     * @param id ID
     */
    constexpr player_data(std::uint8_t id) noexcept : entity{entity_type::player, id, team_type::spectator}
    {
    }

    /**
     * @brief Destroy the player_data object
     *
     */
    ~player_data() noexcept = default;

    /**
     * @brief Reset values after death
     *
     */
    constexpr void reset_death() noexcept
    {
        reset_player_state_death();
        reset_input_data();
        m_health       = 100;
        m_tool         = tool_type::gun;
        m_restock_time = 0;
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
        m_restock_time = 0;
    }

    /**
     * @brief Reset position (and orientation) data
     *
     */
    void reset_transform() noexcept
    {
        reset_position();
        m_orientation = {0.F, 0.F, 0.F};
        m_velocity    = {0.F, 0.F, 0.F};
    }

    glm::vec3 m_orientation{0.F, 0.F, 0.F}; //!< Player orientation
    glm::vec3 m_velocity{0.F, 0.F, 0.F};    //!< Player velocity

    std::uint8_t m_health{100};          //!< Player's health
    tool_type    m_tool{tool_type::gun}; //!< Currently held tool
    std::uint8_t m_respawn_time{0};      //!< Respawn time
    std::uint8_t m_restock_time{0};      //!< Restock timer

    /**
     * @brief Set kill data
     *
     * @param killer Killer
     * @param type Kill type
     */
    void set_last_kill(std::uint8_t killer, kill_type type) noexcept
    {
        m_last_kill_killer = killer;
        m_last_kill_type   = type;
    }

  protected:
    std::uint8_t m_last_kill_killer{object_id::invalid};   //!< Last death killer
    kill_type    m_last_kill_type{kill_type::team_change}; //!< Last death type
};

} // namespace spadesx
