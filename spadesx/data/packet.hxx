/**
 *
 * SpadesX
 *
 */

#pragma once

#include <cstdint>

namespace spadesx {

/**
 * @brief Packet sizes
 *
 */
class packet
{
  public:
    static constexpr const std::size_t world_update_size    = 769;
    static constexpr const std::size_t input_data_size      = 3;
    static constexpr const std::size_t weapon_input_size    = 3;
    static constexpr const std::size_t set_hp_size          = 15;
    static constexpr const std::size_t grenade_packet_size  = 30;
    static constexpr const std::size_t set_tool_size        = 3;
    static constexpr const std::size_t set_color_size       = 5;
    static constexpr const std::size_t existing_player_size = 28;
    static constexpr const std::size_t short_player_size    = 4;
    static constexpr const std::size_t move_object_size     = 15;
    static constexpr const std::size_t create_player_size   = 32;
    static constexpr const std::size_t block_action_size    = 15;
    static constexpr const std::size_t block_line_size      = 26;
    static constexpr const std::size_t state_data_size      = 90;
    static constexpr const std::size_t kill_action_size     = 5;
    static constexpr const std::size_t chat_message_size    = 258;
    static constexpr const std::size_t map_start_size       = 5;
    static constexpr const std::size_t player_left_size     = 2;
    static constexpr const std::size_t territory_capture    = 5;
    static constexpr const std::size_t progress_bar         = 8;
    static constexpr const std::size_t intel_capture_size   = 3;
    static constexpr const std::size_t intel_pickup_size    = 2;
    static constexpr const std::size_t intel_drop_size      = 13;
    static constexpr const std::size_t restock_size         = 2;
    static constexpr const std::size_t fog_color_size       = 5;
    static constexpr const std::size_t weapon_reload_size   = 4;
};

} // namespace spadesx
