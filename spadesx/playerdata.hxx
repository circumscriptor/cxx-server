/**
 *
 * SpadesX
 *
 */

#pragma once

#include "enums.hxx"
#include "types.hxx"

#include <algorithm>
#include <glm/vec3.hpp>
#include <string>
#include <string_view>

namespace spadesx {

/**
 * @brief Player data
 *
 */
struct player_data
{
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
    bool          m_up{false};        //!< Input forward
    bool          m_down{false};      //!< Input backward
    bool          m_left{false};      //!< Input left
    bool          m_right{false};     //!< Input right
    bool          m_jump{false};      //!< Input jump
    bool          m_crouch{false};    //!< Input crouch
    bool          m_sneak{false};     //!< Input sneak
    bool          m_sprint{false};    //!< Input sprint
    bool          m_primary{false};   //!< Primary action
    bool          m_secondary{false}; //!< Secondary action
    bool          m_gliding{false};   //!< In the air (airborne)
    bool          m_crouching{false}; //!< Crouching state
    bool          m_alive{false};     //!< Is player alive
    bool          m_can_spawn{false}; //!< Is player allowed to spawn
    bool          m_can_break{true};  //!< Is player allowed to break blocks
    bool          m_can_kill{true};   //!< Is player allowed to kill players

    /**
     * @brief Set player's name
     *
     * @param name New name
     */
    void set_name(const std::string_view& name)
    {
        auto length = name.length();
        if (length > 15) {
            length = 15;
        }
        m_name.assign(name.data(), length);
    }

    /**
     * @brief Set input
     *
     * @param input Input
     */
    void set_input(std::uint8_t input)
    {
        m_up     = ((input & 0x01) != 0);
        m_down   = ((input & 0x02) != 0);
        m_left   = ((input & 0x04) != 0);
        m_right  = ((input & 0x08) != 0);
        m_jump   = ((input & 0x10) != 0);
        m_crouch = ((input & 0x20) != 0);
        m_sneak  = ((input & 0x40) != 0);
        m_sprint = ((input & 0x80) != 0);
    }

    /**
     * @brief Get input data
     *
     * @return Input data
     */
    [[nodiscard]] std::uint8_t get_input_data() const
    {
        std::uint8_t input = 0;
        input |= m_up ? 0x01 : 0x00;
        input |= m_down ? 0x02 : 0x00;
        input |= m_left ? 0x04 : 0x00;
        input |= m_right ? 0x08 : 0x00;
        input |= m_jump ? 0x10 : 0x00;
        input |= m_crouch ? 0x20 : 0x00;
        input |= m_sneak ? 0x40 : 0x00;
        input |= m_sprint ? 0x80 : 0x00;
        return input;
    }

    /**
     * @brief Set weapon input
     *
     * @param input Weapon input
     */
    void set_weapon_input(std::uint8_t input)
    {
        m_primary   = ((input & 0x01) != 0);
        m_secondary = ((input & 0x02) != 0);
    }

    /**
     * @brief Get weapon input
     *
     * @return Weapon input
     */
    [[nodiscard]] std::uint8_t get_weapon_input() const
    {
        std::uint8_t input = 0;
        input |= m_primary ? 0x01 : 0x00;
        input |= m_secondary ? 0x02 : 0x00;
        return input;
    }

    /**
     * @brief Reset to initial state
     *
     */
    void reset_values()
    {
        m_health       = 100;
        m_position     = {0.F, 0.F, 0.F};
        m_orientation  = {0.F, 0.F, 0.F};
        m_tool         = tool_type::gun;
        m_crouching    = false;
        m_gliding      = false;
        m_alive        = false;
        m_can_spawn    = false;
        m_can_break    = true;
        m_can_kill     = true;
        m_kills        = 0;
        m_deaths       = 0;
        m_respawn_time = 0;
        set_input(0);
        set_weapon_input(0);
    }

    /**
     * @brief Reset values after death
     *
     */
    void reset_death()
    {
        m_health    = 100;
        m_tool      = tool_type::gun;
        m_crouching = false;
        m_gliding   = false;
        m_alive     = false;
        set_input(0);
        set_weapon_input(0);
    }
};

} // namespace spadesx
