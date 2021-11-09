/**
 * @file player.hxx
 * @brief This file is part of the experimental SpadesX project
 */

#pragma once

#include "entity.hxx"
#include "score.hxx"
#include "weapon.hxx"

namespace spadesx {

/**
 * @brief Player data
 *
 */
class player_data : public entity, public name_data<16>
{

  public:
    /**
     * @brief Construct a new player_data object
     *
     * @param id ID
     */
    player_data(std::uint8_t id) noexcept : entity{entity_type::player, id, team_type::spectator}
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
    void reset_death() noexcept
    {
        reset_player_state_death();
        reset_input();
        m_health       = 100;
        m_tool         = tool_type::gun;
        m_restock_time = 0;
    }

    /**
     * @brief Reset to initial state
     *
     */
    void reset_values() noexcept
    {
        reset_state();
        reset_input();
        reset_transform();
        m_score.reset();
        m_health       = 100;
        m_tool         = tool_type::gun;
        m_respawn_time = 0;
        m_restock_time = 0;
    }

    /**
     * @brief Set input
     *
     * @param input Input
     */
    void input_from_byte(std::uint8_t input) noexcept
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
    [[nodiscard]] std::uint8_t input_to_byte() const noexcept
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
    void weapon_input_from_byte(std::uint8_t input) noexcept
    {
        m_primary   = ((input & 0x01) != 0);
        m_secondary = ((input & 0x02) != 0);
    }

    /**
     * @brief Get weapon input
     *
     * @return Weapon input
     */
    [[nodiscard]] std::uint8_t weapon_input_to_byte() const noexcept
    {
        std::uint8_t input = 0;
        input |= m_primary ? 0x01 : 0x00;
        input |= m_secondary ? 0x02 : 0x00;
        return input;
    }

    /**
     * @brief Get reference to score data
     *
     * @return Reference to score data
     */
    score_data& score() noexcept
    {
        return m_score;
    }

    /**
     * @brief Get reference to score data
     *
     * @return Reference to score data
     */
    [[nodiscard]] const score_data& score() const noexcept
    {
        return m_score;
    }

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

    /**
     * @brief Set weapon data
     *
     * @param weapon Weapon type
     */
    void set_weapon(weapon_type weapon)
    {
        const auto& w  = weapons::get(weapon);
        m_clip_ammo    = w.get_clip();
        m_reserve_ammo = w.get_stock();
        m_weapon       = weapon;
    }

    /**
     * @brief Get weapon damage
     *
     * @param hit Hit type
     * @return Weapon damage
     */
    [[nodiscard]] std::uint8_t get_weapon_damage(hit_type hit) const
    {
        return weapons::get(m_weapon).get_damage(hit);
    }

    /**
     * @brief Get current weapon type
     *
     * @return Weapon type
     */
    [[nodiscard]] weapon_type weapon() const noexcept
    {
        return m_weapon;
    }

    /**
     * @brief Change weapon type
     *
     * @param weapon New weapon type
     */
    void change_weapon(weapon_type weapon) noexcept
    {
        m_weapon = weapon;
    }

    //
    // Weapon
    //

    std::uint8_t m_clip_ammo{0};    //!< Clip ammo
    std::uint8_t m_reserve_ammo{0}; //!< Reserve ammo

    //
    // Transformation
    //

    glm::vec3 m_orientation{0.F, 0.F, 0.F}; //!< Player orientation
    glm::vec3 m_velocity{0.F, 0.F, 0.F};    //!< Player velocity

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

    //
    // Input
    //

    bool m_up{false};        //!< Input forward
    bool m_down{false};      //!< Input backward
    bool m_left{false};      //!< Input left
    bool m_right{false};     //!< Input right
    bool m_jump{false};      //!< Input jump
    bool m_crouch{false};    //!< Input crouch
    bool m_sneak{false};     //!< Input sneak
    bool m_sprint{false};    //!< Input sprint
    bool m_primary{false};   //!< Primary action
    bool m_secondary{false}; //!< Secondary action

    /**
     * @brief Reset input
     *
     */
    void reset_input() noexcept
    {
        m_up        = false;
        m_down      = false;
        m_left      = false;
        m_right     = false;
        m_jump      = false;
        m_crouch    = false;
        m_sneak     = false;
        m_sprint    = false;
        m_primary   = false;
        m_secondary = false;
    }

    //
    // State
    //

    bool m_deaf{false};       //!< Chat (deaf)
    bool m_muted{false};      //!< Chat (muted)
    bool m_alive{false};      //!< Is player alive
    bool m_wade{false};       //!< Is player in water
    bool m_gliding{false};    //!< In the air (airborne)
    bool m_crouching{false};  //!< Crouching state
    bool m_jumping{false};    //!< Is mid jump?
    bool m_has_joined{false}; //!< Has player joined
    bool m_can_spawn{false};  //!< Is player allowed to spawn
    bool m_can_build{true};   //!< Is player allowed to break blocks
    bool m_can_kill{true};    //!< Is player allowed to kill players

    std::uint8_t m_health{100};          //!< Player's health
    tool_type    m_tool{tool_type::gun}; //!< Currently held tool
    std::uint8_t m_respawn_time{0};      //!< Respawn time
    std::uint8_t m_restock_time{0};      //!< Restock timer

    //
    // Color
    //

    color3b m_color; //!< Block color

    /**
     * @brief Reset player's state
     *
     */
    void reset_state() noexcept
    {
        m_muted      = false;
        m_deaf       = false;
        m_alive      = false;
        m_wade       = false;
        m_gliding    = false;
        m_crouching  = false;
        m_jumping    = false;
        m_has_joined = false;
        m_can_spawn  = false;
        m_can_build  = true;
        m_can_kill   = true;
    }

    /**
     * @brief Reset player's state (on death)
     *
     */
    void reset_player_state_death() noexcept
    {
        m_alive     = false;
        m_gliding   = false;
        m_crouching = false;
    }

  protected:
    score_data   m_score;                                  //!< Score
    weapon_type  m_weapon{weapon_type::rifle};             //!< Current weapon
    std::uint8_t m_last_kill_killer{object_id::invalid};   //!< Last death killer
    kill_type    m_last_kill_type{kill_type::team_change}; //!< Last death type
};

} // namespace spadesx
