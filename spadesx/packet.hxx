/**
 * @file packet.hxx
 * @brief This file is part of the experimental SpadesX project
 */

#pragma once

#include "datastream.hxx"

#include <enet/enet.h>

namespace spadesx {

class peer;

/**
 * @brief Packet sizes
 *
 */
class packet
{
    friend peer;

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

    /**
     * @brief Construct a new packet object
     *
     * @param source Source memory
     * @param capacity Packet capacity
     * @param unsequenced Unsequenced flag
     */
    packet(const void* source, std::size_t capacity, bool unsequenced = false) :
        m_packet{enet_packet_create(source, capacity, unsequenced_to_flag(unsequenced))}
    {
    }

    /**
     * @brief Create data stream
     *
     * @return Data stream
     */
    [[nodiscard]] data_stream stream() noexcept
    {
        return {m_packet};
    }

  protected:
    /**
     * @brief Convert bool to ENet packet flag
     *
     * @param unsequenced Unsequenced flag
     * @return ENet packet flag
     */
    static std::uint32_t unsequenced_to_flag(bool unsequenced)
    {
        return unsequenced ? ENET_PACKET_FLAG_RELIABLE : ENET_PACKET_FLAG_UNSEQUENCED;
    }

    ENetPacket* m_packet; //!< ENet packet
};

} // namespace spadesx
