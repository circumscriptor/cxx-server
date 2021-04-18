/**
 *
 * SpadesX
 *
 */

#pragma once

#include "manager.hxx"
#include "map.hxx"

#include <cmath>
#include <glm/fwd.hpp>
#include <memory>

namespace spadesx {

/**
 * @brief World manager
 *
 */
class world_manager : public connection_manager
{
  public:
    static constexpr const float diagonal_multiplier = float(0.7071067811865475244);
    static constexpr const float jump_velocity       = -0.36F; //!< Jump velocity

    /**
     * @brief Construct a new world_manager object
     *
     */
    world_manager() : world_manager{32}
    {
    }

    /**
     * @brief Construct a new world_manager object
     *
     * @param max_players Max number of players
     */
    world_manager(std::uint8_t max_players) : connection_manager{max_players}, m_map{new map()}
    {
    }

    /**
     * @brief Destroy the world_manager object
     *
     */
    ~world_manager() = default;

    /**
     * @brief Read map from file
     *
     * @param string File path
     */
    void load_map(std::string_view string)
    {
        m_map->read_from_file(string);
    }

    /**
     * @brief Set fog color
     *
     * @param color Color (RGB)
     */
    void set_fog_color(const color3b& color)
    {
        m_fog_color = color;
    }

    /**
     * @brief Move player
     *
     * @param player Player
     * @param delta Delta time
     */
    void move_player(connection& player, float delta)
    {
        if (player.m_jump && !player.m_gliding) {
            player.m_gliding    = true;
            player.m_velocity.z = jump_velocity;
        }

        float multiplier = delta;

        if (player.m_gliding) {
            multiplier *= 0.1F;
        } else if (player.m_crouching) {
            multiplier *= 0.3F;
        } else if (player.m_secondary && player.m_tool == tool_type::gun) {
            multiplier *= 0.5F;
        } else if (player.m_sprint) {
            multiplier *= 1.3F;
        }

        if ((player.m_up || player.m_down) && (player.m_left || player.m_right)) {
            multiplier *= diagonal_multiplier;
        }

        // Get vertical slowdown (from orientation)
        float slowdown = std::max(std::abs(player.m_orientation.z) - 0.65F, 0.0F) / (1.F - 0.65F) * 0.9F;
        // Get normalized orientation in 2D (front)
        glm::vec3 front = glm::normalize(glm::vec3{player.m_orientation.x, player.m_orientation.y, 0.F});
        front *= (1.F - slowdown);
        // Get normalized orientation in 2D (left)
        glm::vec3 left = glm::normalize(glm::cross(glm::vec3{0.F, 0.F, -1.F}, front));

        if (player.m_up) {
            player.m_velocity.x += front.x * multiplier;
            player.m_velocity.y += front.y * multiplier;
        } else if (player.m_down) {
            player.m_velocity.x -= front.x * multiplier;
            player.m_velocity.y -= front.y * multiplier;
        }

        if (player.m_left) {
            player.m_velocity.x += left.x * multiplier;
            player.m_velocity.y += left.y * multiplier;
        } else if (player.m_right) {
            player.m_velocity.x -= left.x * multiplier;
            player.m_velocity.y -= left.y * multiplier;
        }

        // Air friction?
        multiplier = delta + 1.F;
        player.m_velocity.z += delta;
        player.m_velocity.z /= multiplier;

        // apply friction if in water or on ground
        if (player.m_wade) {
            multiplier = delta * 6.F + 1.F;
        } else if (!player.m_gliding) {
            multiplier = delta * 4.F + 1.F;
        }

        player.m_velocity.x /= multiplier;
        player.m_velocity.y /= multiplier;

        // TODO: Return fallback damage

        multiplier          = delta * 32.F;
        player.m_position.x = player.m_position.x + player.m_velocity.x * multiplier;
        player.m_position.y = player.m_position.y + player.m_velocity.y * multiplier;
    }

    /**
     * @brief Update a player
     *
     * @param connection Connection
     * @param delta Delta time
     */
    void world_update_player(connection& player, float delta)
    {
        if (player.m_crouching != player.m_crouch) {
            if (player.m_crouch) {
                player.m_position.z += 0.9F;
            } else {
                player.m_position.z -= 0.9F;
            }
            player.m_crouching = player.m_crouch;
        }

        move_player(player, delta);
    }

    /**
     * @brief World update
     *
     * @param delta Delta time
     */
    void world_update(float delta)
    {
        for (auto& connection : m_connections) {
            if (connection.m_alive) {
                world_update_player(connection, delta);
            }
        }
    }

    /**
     * @brief Get the map object
     *
     * @return Map
     */
    map& get_map()
    {
        return *m_map;
    }

  protected:
    std::unique_ptr<map> m_map;       //!< Map
    color3b              m_fog_color; //!< Fog color
};

} // namespace spadesx
