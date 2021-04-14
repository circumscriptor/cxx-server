/**
 *
 * SpadesX
 *
 */

#pragma once

#include "enums.hxx"
#include "inputdata.hxx"
#include "playerstate.hxx"
#include "types.hxx"

#include <glm/vec3.hpp>
#include <string>
#include <string_view>

namespace spadesx {

/**
 * @brief Player data
 *
 */
class player_data : public input_data, public player_state
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
     * @brief Set player's name
     *
     * @param name New name
     */
    void set_name(std::string_view name)
    {
        auto size = std::min(name.size(), std::size_t(15));
        m_name.assign(name.data(), size);
    }

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
        m_health       = 100;
        m_tool         = tool_type::gun;
        m_position     = {0.F, 0.F, 0.F};
        m_orientation  = {0.F, 0.F, 0.F};
        m_kills        = 0;
        m_deaths       = 0;
        m_respawn_time = 0;
    }

    std::string   m_name;             //!< Player's name
    std::uint8_t  m_health;           //!< Player's health
    std::uint32_t m_kills{0};         //!< Kill score
    std::uint32_t m_deaths{0};        //!< Death counter
    std::uint8_t  m_respawn_time{0};  //!< Respawn time
    glm::vec3     m_position;         //!< Player position
    glm::vec3     m_orientation;      //!< Player orientation
    color3b       m_color;            //!< Block color
    std::uint8_t  m_clip_ammo;        //!< Clip ammo
    std::uint8_t  m_reserve_ammo;     //!< Reserve ammo
    std::uint8_t  m_last_kill_killer; //!< Last death killer
    kill_type     m_last_kill_type;   //!< Last death type
    tool_type     m_tool;             //!< Currently held tool
    team_type     m_team;             //!< Current team
    weapon_type   m_weapon;           //!< Current weapon
};

} // namespace spadesx
