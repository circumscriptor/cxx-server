/**
 * @file world.hxx
 * @brief This file is part of the experimental SpadesX project
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
     * @brief On fall damage event
     *
     * @param source Source connection (player)
     * @param damage Damage
     */
    virtual void on_fall_damage(connection& source, int damage)
    {
    }

    /**
     * @brief Update a player
     *
     * @param player Target connection
     * @param delta Delta time
     */
    void world_update_player(connection& player, float delta)
    {
        auto fall_damage = player.move_player(*m_map, delta);
        if (fall_damage > 0) {
            on_fall_damage(player, fall_damage);
        }
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
