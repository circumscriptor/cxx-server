/**
 * @file packet.hxx
 * @brief This file is part of the experimental SpadesX project
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
    static constexpr const std::size_t world_update_size    = 769; //!< World update packet size
    static constexpr const std::size_t input_data_size      = 3;   //!< Input data packet size
    static constexpr const std::size_t weapon_input_size    = 3;   //!< Weapon input packet size
    static constexpr const std::size_t set_hp_size          = 15;  //!< Set hp packet size
    static constexpr const std::size_t grenade_packet_size  = 30;  //!< Grenade packet size
    static constexpr const std::size_t set_tool_size        = 3;   //!< Set tool packet size
    static constexpr const std::size_t set_color_size       = 5;   //!< Set color packet size
    static constexpr const std::size_t existing_player_size = 28;  //!< Existing player packet max size
    static constexpr const std::size_t short_player_size    = 4;   //!< Short player packet size
    static constexpr const std::size_t move_object_size     = 15;  //!< Move object packet size
    static constexpr const std::size_t create_player_size   = 32;  //!< Create player packet size
    static constexpr const std::size_t block_action_size    = 15;  //!< Block action packet size
    static constexpr const std::size_t block_line_size      = 26;  //!< Block line packet size
    static constexpr const std::size_t state_data_size      = 90;  //!< State data packet size
    static constexpr const std::size_t kill_action_size     = 5;   //!< Kill action packet size
    static constexpr const std::size_t chat_message_size    = 258; //!< Chat message packet max size
    static constexpr const std::size_t map_start_size       = 5;   //!< Map start packet size
    static constexpr const std::size_t player_left_size     = 2;   //!< Player left packet size
    static constexpr const std::size_t territory_capture    = 5;   //!< Territroy capture packet size
    static constexpr const std::size_t progress_bar         = 8;   //!< Progress bar packet size
    static constexpr const std::size_t intel_capture_size   = 3;   //!< Intel capture packet size
    static constexpr const std::size_t intel_pickup_size    = 2;   //!< Intel pickup packet size
    static constexpr const std::size_t intel_drop_size      = 14;  //!< Intel drop packet size
    static constexpr const std::size_t restock_size         = 2;   //!< Restock packet size
    static constexpr const std::size_t fog_color_size       = 5;   //!< Fog color packet size
    static constexpr const std::size_t weapon_reload_size   = 4;   //!< Weapon reload packet size
};

} // namespace spadesx
