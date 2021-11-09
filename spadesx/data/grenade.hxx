/**
 * @file base.hxx
 * @brief This file is part of the experimental SpadesX project
 */

#pragma once

#include "map.hxx"
#include "player.hxx"

namespace spadesx {

/**
 * @brief Grenade data
 *
 */
class grenade : public entity
{
  public:
    /**
     * @brief Construct a new grenade object
     *
     * @param source Source player
     * @param velocity Initial velocity
     * @param fuse Initial fuse time
     */
    grenade(const player_data& source, const glm::vec3& position, const glm::vec3& velocity, float fuse) :
        entity{entity_type::grenade, source.id(), source.team(), position},
        m_velocity{velocity},
        m_fuse{fuse}
    {
    }

    /**
     * @brief Update grenade position
     *
     * @param map Map
     * @param delta Delta time
     */
    void update(const map& map, float delta)
    {
        // Update fuse time
        m_fuse -= delta;

        // Update position
        glm::vec3 prev = m_position;

        float dt = delta * 32.F;

        m_velocity.z += delta;
        m_position += m_velocity * dt;

        glm::ivec3 b = glm::floor(m_position);
        if (map.is_clip_world(b.x, b.y, b.z)) {
            glm::ivec3 a = glm::floor(prev);

            if (b.z != a.z && ((b.x == a.x && b.y == a.y) || !map.is_clip_box(b.x, b.y, a.z))) {
                m_velocity.z = -m_velocity.z;
            } else if (b.x != a.x && ((b.y == a.y && b.z == a.z) || !map.is_clip_box(a.x, b.y, b.z))) {
                m_velocity.x = -m_velocity.x;
            } else if (b.y != a.y && ((b.x == a.x && b.z == a.z) || !map.is_clip_box(b.x, a.y, b.z))) {
                m_velocity.y = -m_velocity.y;
            }

            // FIXME: Does not work with low TPS
            // if TPS is too low, grenade bounces back too far

            m_position = prev;
            m_velocity *= 0.36F;
        }
    }

    glm::vec3 m_velocity; //!< Current velocity
    float     m_fuse;     //!< Fuse time
};

} // namespace spadesx
