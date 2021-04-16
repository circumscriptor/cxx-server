/**
 *
 * SpadesX
 *
 */

#pragma once

#include "manager.hxx"
#include "map.hxx"

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
     * @brief World update
     *
     */
    void world_update()
    {
        // TODO: Write world update
    }

    map& get_map()
    {
        return *m_map.get();
    }

  protected:
    std::unique_ptr<map> m_map;       //!< Map
    color3b              m_fog_color; //!< Fog color
};

} // namespace spadesx
