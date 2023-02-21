/**
 * @file player.hxx
 * @brief This file is part of the experimental SpadesX project
 */

#pragma once

#include "entity.hxx"
#include "name.hxx"
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

    static constexpr const float diagonal_multiplier = float(0.7071067811865475244); //!< Diagonal movement multiplier

    /**
     * @brief Construct a new player_data object
     *
     * @param id ID
     */
    player_data(std::uint8_t id) noexcept
        : entity { entity_type::player, id, team_type::spectator }
    {
    }

    /**
     * @brief Destroy the player_data object
     *
     */
    ~player_data() noexcept = default;

    // void reposition(const glm::vec3& position, float time)
    // {
    //     // eye = m_position = position; ?
    //     m_position = position;
    //     // float timediff = m_last_climb - time;
    //     // if (timediff > -0.25F) {
    //     //     eye.z += (timediff + 0.25F) / 0.25F;
    //     // }
    // }

    /**
     * @brief Calculate clip move
     *
     * @param map Map
     * @param delta Delta time
     */
    void box_clip_move(map & map, float delta)
    {
        bool  climb    = false;
        float modifier = delta * 32.F;
        float offset;
        float m;
        float z;

        if (m_crouching)
        {
            offset = 0.45F;
            m      = 0.9F;
        }
        else
        {
            offset = 0.9F;
            m      = 1.35F;
        }

        glm::vec3 next;
        next.x = m_position.x + m_velocity.x * modifier;
        next.y = m_position.y + m_velocity.y * modifier;
        next.z = m_position.z + offset;

        //
        // check for X
        //

        z = m;

        if (m_velocity.x < 0.F)
        {
            modifier = -0.45F;
        }
        else
        {
            modifier = 0.45F;
        }

        while (z >= -1.36F && //
               !map.is_clip_box(next.x + modifier, m_position.y - 0.45F, next.z + z) && !map.is_clip_box(next.x + modifier, m_position.y + 0.45F, next.z + z))
        {
            z -= 0.9F;
        }

        if (z < -1.36F)
        {
            m_position.x = next.x;
        }
        else if (!m_crouching && m_orientation.z < 0.5F && !m_sprint)
        {
            z = 0.35F;
            while (z >= -2.36F && //
                   !map.is_clip_box(next.x + modifier, m_position.y - 0.45F, next.z + z)
                   && !map.is_clip_box(next.x + modifier, m_position.y + 0.45F, next.z + z))
            {
                z -= 0.9F;
            }
            if (z < -2.36F)
            {
                m_position.x = next.x;
                climb        = true;
            }
            else
            {
                m_velocity.x = 0.F;
            }
        }
        else
        {
            m_velocity.x = 0.F;
        }

        //
        // check for Y
        //

        z = m;

        if (m_velocity.y < 0.F)
        {
            modifier = -0.45F;
        }
        else
        {
            modifier = 0.45F;
        }

        while (z >= -1.36F && //
               !map.is_clip_box(m_position.x - 0.45F, next.y + modifier, next.z + z) && !map.is_clip_box(m_position.x + 0.45F, next.y + modifier, next.z + z))
        {
            z -= 0.9F;
        }

        if (z < -1.36F)
        {
            m_position.y = next.y;
        }
        else if (!m_crouching && m_orientation.z < 0.5F && !m_sprint && !climb)
        {
            z = 0.35F;
            while (z >= -2.36F && //
                   !map.is_clip_box(m_position.x - 0.45F, next.y + modifier, next.z + z)
                   && !map.is_clip_box(m_position.x + 0.45F, next.y + modifier, next.z + z))
            {
                z -= 0.9F;
            }
            if (z < -2.36F)
            {
                m_position.y = next.y;
                climb        = true;
            }
            else
            {
                m_velocity.y = 0.F;
            }
        }
        else if (!climb)
        {
            m_velocity.y = 0.F;
        }

        //
        // move z
        //

        if (climb)
        {
            m_velocity.x *= 0.5F;
            m_velocity.y *= 0.5F;
            next.z       -= 1.F;
            m            = -1.35F;
        }
        else
        {
            if (m_velocity.z < 0.F)
            {
                m = -m;
            }
            next.z += m_velocity.z * delta * 32.F;
        }

        //
        // check ground
        //

        m_gliding = true;

        if (map.is_clip_box(m_position.x - 0.45F, m_position.y - 0.45F, next.z + m) || map.is_clip_box(m_position.x - 0.45F, m_position.y + 0.45F, next.z + m)
            || map.is_clip_box(m_position.x + 0.45F, m_position.y - 0.45F, next.z + m)
            || map.is_clip_box(m_position.x + 0.45F, m_position.y + 0.45F, next.z + m))
        {
            if (m_velocity.z >= 0.F)
            {
                m_wade    = m_position.z > 61.F;
                m_gliding = false;
            }
            m_velocity.z = 0.F;
        }
        else
        {
            m_position.z = next.z - offset;
        }

        // reposition(m_position);
    }

    /**
     * @brief Is on ground or in water
     *
     * @return true If is ou ground or in water
     */
    [[nodiscard]] bool is_on_ground_or_wade() const
    {
        return (m_velocity.z >= 0.F && m_velocity.z < 0.017F) && !m_gliding;
    }

    /**
     * @brief Set jump
     *
     * @param jump Jump value
     */
    void set_jump(bool jump)
    {
        if (jump)
        {
            m_jumping = !m_jumping && is_on_ground_or_wade();
            // m_jumping = !(m_jumping && !(m_velocity.z >= 0.F && m_velocity.z < 0.017F));
        }
    }

    /**
     * @brief Set crouch
     *
     * @param crouch Crouch value
     */
    void set_crouch(bool crouch)
    {
        if (m_crouching != crouch)
        {
            if (crouch)
            {
                m_position.z += 0.9F;
            }
            else
            {
                m_position.z -= 0.9F;
            }
            m_crouching = crouch;
        }
    }

    /**
     * @brief Move player (and return fall damage)
     *
     * @param map Map
     * @param delta Delta time
     * @return Fall damage (if > 0)
     */
    int move_player(map & map, float delta)
    {
        if (m_jumping && is_on_ground_or_wade())
        {
            m_jumping    = false;
            m_velocity.z = -0.36F;
        }

        float modifier = delta;

        if (m_gliding)
        {
            modifier *= 0.1F;
        }
        else if (m_crouching)
        {
            modifier *= 0.3F;
        }
        else if (m_secondary && m_tool == tool_type::gun)
        {
            modifier *= 0.5F;
        }
        else if (m_sprint)
        {
            modifier *= 1.3F;
        }

        if ((m_up || m_down) && (m_left || m_right))
        {
            modifier *= diagonal_multiplier;
        }

        // Get vertical slowdown (from orientation)
        float slowdown = std::max(std::abs(m_orientation.z) - 0.65F, 0.0F) / (1.F - 0.65F) * 0.9F;
        // Get normalized orientation in 2D (front)
        glm::vec3 front = glm::normalize(glm::vec3 { m_orientation.x, m_orientation.y, 0.F });
        front           *= (1.F - slowdown);
        // Get normalized orientation in 2D (left)
        glm::vec3 left = glm::normalize(glm::cross(glm::vec3 { 0.F, 0.F, -1.F }, front));

        if (m_up)
        {
            m_velocity.x += front.x * modifier;
            m_velocity.y += front.y * modifier;
        }
        else if (m_down)
        {
            m_velocity.x -= front.x * modifier;
            m_velocity.y -= front.y * modifier;
        }

        if (m_left)
        {
            m_velocity.x += left.x * modifier;
            m_velocity.y += left.y * modifier;
        }
        else if (m_right)
        {
            m_velocity.x -= left.x * modifier;
            m_velocity.y -= left.y * modifier;
        }

        // Air friction?
        modifier     = delta + 1.F;
        m_velocity.z += delta;
        m_velocity.z /= modifier;

        // Apply friction if in water or on ground
        if (m_wade)
        {
            modifier = delta * 6.F + 1.F;
        }
        else if (!m_gliding)
        {
            modifier = delta * 4.F + 1.F;
        }

        m_velocity.x /= modifier;
        m_velocity.y /= modifier;

        float vel_z  = m_velocity.z;

        box_clip_move(map, delta);

        // check fall - slow
        if (m_velocity.z == 0.F && vel_z > 0.24F)
        {
            // Slow down on fall
            m_velocity.x *= 0.5F;
            m_velocity.y *= 0.5F;

            // check fall - damage
            if (vel_z > 0.58F)
            {
                vel_z -= 0.58F;
                return static_cast<int>(vel_z * vel_z * 4096); // get fall damage
            }
            return -1; // no fall damage, but sound
        }
        return 0; // no fall damage
    }

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
        m_grenades     = 3;
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
        m_grenades     = 3;
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
        input              |= m_up ? 0x01 : 0x00;
        input              |= m_down ? 0x02 : 0x00;
        input              |= m_left ? 0x04 : 0x00;
        input              |= m_right ? 0x08 : 0x00;
        input              |= m_jump ? 0x10 : 0x00;
        input              |= m_crouch ? 0x20 : 0x00;
        input              |= m_sneak ? 0x40 : 0x00;
        input              |= m_sprint ? 0x80 : 0x00;
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
        input              |= m_primary ? 0x01 : 0x00;
        input              |= m_secondary ? 0x02 : 0x00;
        return input;
    }

    /**
     * @brief Get reference to score data
     *
     * @return Reference to score data
     */
    score_data & score() noexcept
    {
        return m_score;
    }

    /**
     * @brief Get reference to score data
     *
     * @return Reference to score data
     */
    [[nodiscard]] const score_data & score() const noexcept
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
        const auto & w = weapons::get(weapon);
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

    std::uint8_t m_clip_ammo { 0 };    //!< Clip ammo
    std::uint8_t m_reserve_ammo { 0 }; //!< Reserve ammo

    //
    // Grenade
    //

    std::uint8_t m_grenades { 3 }; //!< Grenades count

    //
    // Transformation
    //

    glm::vec3 m_orientation { 0.F, 0.F, 0.F }; //!< Player orientation
    glm::vec3 m_velocity { 0.F, 0.F, 0.F };    //!< Player velocity

    /**
     * @brief Reset position (and orientation) data
     *
     */
    void reset_transform() noexcept
    {
        reset_position();
        m_orientation = { 0.F, 0.F, 0.F };
        m_velocity    = { 0.F, 0.F, 0.F };
    }

    //
    // Input
    //

    bool m_up { false };        //!< Input forward
    bool m_down { false };      //!< Input backward
    bool m_left { false };      //!< Input left
    bool m_right { false };     //!< Input right
    bool m_jump { false };      //!< Input jump
    bool m_crouch { false };    //!< Input crouch
    bool m_sneak { false };     //!< Input sneak
    bool m_sprint { false };    //!< Input sprint
    bool m_primary { false };   //!< Primary action
    bool m_secondary { false }; //!< Secondary action

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

    bool m_deaf { false };       //!< Chat (deaf)
    bool m_muted { false };      //!< Chat (muted)
    bool m_alive { false };      //!< Is player alive
    bool m_wade { false };       //!< Is player in water
    bool m_gliding { false };    //!< In the air (airborne)
    bool m_crouching { false };  //!< Crouching state
    bool m_jumping { false };    //!< Is mid jump?
    bool m_has_joined { false }; //!< Has player joined
    bool m_can_spawn { false };  //!< Is player allowed to spawn
    bool m_can_build { true };   //!< Is player allowed to break blocks
    bool m_can_kill { true };    //!< Is player allowed to kill players

    std::uint8_t m_health { 100 };          //!< Player's health
    tool_type    m_tool { tool_type::gun }; //!< Currently held tool
    std::uint8_t m_respawn_time { 0 };      //!< Respawn time
    std::uint8_t m_restock_time { 0 };      //!< Restock timer

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

    score_data   m_score;                                     //!< Score
    weapon_type  m_weapon { weapon_type::rifle };             //!< Current weapon
    std::uint8_t m_last_kill_killer { object_id::invalid };   //!< Last death killer
    kill_type    m_last_kill_type { kill_type::team_change }; //!< Last death type
};

} // namespace spadesx
