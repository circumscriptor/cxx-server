/**
 * @file update.hxx
 * @brief This file is part of the experimental SpadesX project
 */

#pragma once

#include "data/player.hxx"
#include "map.hxx"

namespace spadesx {

/**
 * @brief Player movement
 *
 */
class player_update : public player_data
{
  public:
    /**
     * @brief Construct a new player_update object
     *
     * @param id ID
     */
    player_update(std::uint8_t id) : player_data{id}
    {
    }

    static constexpr const float diagonal_multiplier = float(0.7071067811865475244); //!< Diagonal movement multiplier

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
    void box_clip_move(map& map, float delta)
    {
        bool  climb    = false;
        float modifier = delta * 32.F;
        float offset;
        float m;
        float z;

        if (m_crouching) {
            offset = 0.45F;
            m      = 0.9F;
        } else {
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

        if (m_velocity.x < 0.F) {
            modifier = -0.45F;
        } else {
            modifier = 0.45F;
        }

        while (z >= -1.36F && //
               !map.is_clip_box(next.x + modifier, m_position.y - 0.45F, next.z + z) &&
               !map.is_clip_box(next.x + modifier, m_position.y + 0.45F, next.z + z))
        {
            z -= 0.9F;
        }

        if (z < -1.36F) {
            m_position.x = next.x;
        } else if (!m_crouching && m_orientation.z < 0.5F && !m_sprint) {
            z = 0.35F;
            while (z >= -2.36F && //
                   !map.is_clip_box(next.x + modifier, m_position.y - 0.45F, next.z + z) &&
                   !map.is_clip_box(next.x + modifier, m_position.y + 0.45F, next.z + z))
            {
                z -= 0.9F;
            }
            if (z < -2.36F) {
                m_position.x = next.x;
                climb        = true;
            } else {
                m_velocity.x = 0.F;
            }
        } else {
            m_velocity.x = 0.F;
        }

        //
        // check for Y
        //

        z = m;

        if (m_velocity.y < 0.F) {
            modifier = -0.45F;
        } else {
            modifier = 0.45F;
        }

        while (z >= -1.36F && //
               !map.is_clip_box(m_position.x - 0.45F, next.y + modifier, next.z + z) &&
               !map.is_clip_box(m_position.x + 0.45F, next.y + modifier, next.z + z))
        {
            z -= 0.9F;
        }

        if (z < -1.36F) {
            m_position.y = next.y;
        } else if (!m_crouching && m_orientation.z < 0.5F && !m_sprint && !climb) {
            z = 0.35F;
            while (z >= -2.36F && //
                   !map.is_clip_box(m_position.x - 0.45F, next.y + modifier, next.z + z) &&
                   !map.is_clip_box(m_position.x + 0.45F, next.y + modifier, next.z + z))
            {
                z -= 0.9F;
            }
            if (z < -2.36F) {
                m_position.y = next.y;
                climb        = true;
            } else {
                m_velocity.y = 0.F;
            }
        } else if (!climb) {
            m_velocity.y = 0.F;
        }

        //
        // move z
        //

        if (climb) {
            m_velocity.x *= 0.5F;
            m_velocity.y *= 0.5F;
            next.z -= 1.F;
            m = -1.35F;
        } else {
            if (m_velocity.z < 0.F) {
                m = -m;
            }
            next.z += m_velocity.z * delta * 32.F;
        }

        //
        // check ground
        //

        m_gliding = true;

        if (map.is_clip_box(m_position.x - 0.45F, m_position.y - 0.45F, next.z + m) ||
            map.is_clip_box(m_position.x - 0.45F, m_position.y + 0.45F, next.z + m) ||
            map.is_clip_box(m_position.x + 0.45F, m_position.y - 0.45F, next.z + m) ||
            map.is_clip_box(m_position.x + 0.45F, m_position.y + 0.45F, next.z + m))
        {
            if (m_velocity.z >= 0.F) {
                m_wade    = m_position.z > 61.F;
                m_gliding = false;
            }
            m_velocity.z = 0.F;
        } else {
            m_position.z = next.z - offset;
        }

        // reposition(m_position);
    }

    /**
     * @brief Is on ground or in water
     *
     * @return true If is ou ground or in water
     */
    bool is_on_ground_or_wade() const
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
        if (jump) {
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
        if (m_crouching != crouch) {
            if (crouch) {
                m_position.z += 0.9F;
            } else {
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
    int move_player(map& map, float delta)
    {
        if (m_jumping && is_on_ground_or_wade()) {
            m_jumping    = false;
            m_velocity.z = -0.36F;
        }

        float modifier = delta;

        if (m_gliding) {
            modifier *= 0.1F;
        } else if (m_crouching) {
            modifier *= 0.3F;
        } else if (m_secondary && m_tool == tool_type::gun) {
            modifier *= 0.5F;
        } else if (m_sprint) {
            modifier *= 1.3F;
        }

        if ((m_up || m_down) && (m_left || m_right)) {
            modifier *= diagonal_multiplier;
        }

        // Get vertical slowdown (from orientation)
        float slowdown = std::max(std::abs(m_orientation.z) - 0.65F, 0.0F) / (1.F - 0.65F) * 0.9F;
        // Get normalized orientation in 2D (front)
        glm::vec3 front = glm::normalize(glm::vec3{m_orientation.x, m_orientation.y, 0.F});
        front *= (1.F - slowdown);
        // Get normalized orientation in 2D (left)
        glm::vec3 left = glm::normalize(glm::cross(glm::vec3{0.F, 0.F, -1.F}, front));

        if (m_up) {
            m_velocity.x += front.x * modifier;
            m_velocity.y += front.y * modifier;
        } else if (m_down) {
            m_velocity.x -= front.x * modifier;
            m_velocity.y -= front.y * modifier;
        }

        if (m_left) {
            m_velocity.x += left.x * modifier;
            m_velocity.y += left.y * modifier;
        } else if (m_right) {
            m_velocity.x -= left.x * modifier;
            m_velocity.y -= left.y * modifier;
        }

        // Air friction?
        modifier = delta + 1.F;
        m_velocity.z += delta;
        m_velocity.z /= modifier;

        // Apply friction if in water or on ground
        if (m_wade) {
            modifier = delta * 6.F + 1.F;
        } else if (!m_gliding) {
            modifier = delta * 4.F + 1.F;
        }

        m_velocity.x /= modifier;
        m_velocity.y /= modifier;

        float vel_z = m_velocity.z;

        box_clip_move(map, delta);

        // check fall - slow
        if (m_velocity.z == 0.F && vel_z > 0.24F) {
            // Slow down on fall
            m_velocity.x *= 0.5F;
            m_velocity.y *= 0.5F;

            // check fall - damage
            if (vel_z > 0.58F) {
                vel_z -= 0.58F;
                return static_cast<int>(vel_z * vel_z * 4096); // get fall damage
            }
            return -1; // no fall damage, but sound
        }
        return 0; // no fall damage
    }
};

} // namespace spadesx
