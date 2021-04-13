/**
 *
 * SpadesX
 *
 */

#pragma once

#include "datastream.hxx"
#include "enums.hxx"
#include "peer.hxx"

#include <string>

namespace spadesx {

class connection : public peer
{
    friend class protocol;

  public:
    static constexpr const std::uint8_t invalid_id = 0xFF;

    connection() = default;

    bool send_map_start(std::uint32_t size, std::uint8_t channel = 0)
    {
        ENetPacket* packet = enet_packet_create(nullptr, 5, ENET_PACKET_FLAG_RELIABLE);
        data_stream stream = packet;
        stream.write_type(packet_type::map_start);
        stream.write_int(size);
        return send(packet, channel);
    }

    bool send_map_chunk(void* chunk, std::uint32_t size, std::uint8_t channel = 0)
    {
        ENetPacket* packet = enet_packet_create(nullptr, size + 1, ENET_PACKET_FLAG_RELIABLE);
        data_stream stream = packet;
        stream.write_type(packet_type::map_chunk);
        stream.write_array(chunk, size);
        return send(packet, channel);
    }

    bool send_packet(const void* data, std::size_t size, bool reliable = true, std::uint8_t channel = 0)
    {
        std::uint32_t flags  = reliable ? ENET_PACKET_FLAG_RELIABLE : ENET_PACKET_FLAG_UNSEQUENCED;
        ENetPacket*   packet = enet_packet_create(data, size, flags);
        return send(packet, channel);
    }

    bool send_existing_player(connection& other, std::uint8_t channel = 0)
    {
        ENetPacket* packet = enet_packet_create(nullptr, 28, ENET_PACKET_FLAG_RELIABLE);
        data_stream stream = packet;

        stream.write_type(packet_type::existing_player);
        stream.write_byte(other.m_id);
        stream.write_type(other.m_team);
        stream.write_type(other.m_weapon);
        stream.write_type(other.m_tool);
        stream.write_int(other.m_kills);
        stream.write_color3b(other.m_color);
        stream.write_array(other.m_name.c_str(), other.m_name.length());
        return send(packet, channel);
    }

    bool send_kill_action(connection& other, std::uint8_t channel = 0)
    {
        ENetPacket* packet = enet_packet_create(nullptr, 5, ENET_PACKET_FLAG_RELIABLE);
        data_stream stream = packet;

        stream.write_type(packet_type::kill_action);
        stream.write_byte(other.m_id);
        stream.write_byte(other.m_last_kill_killer);
        stream.write_type(other.m_last_kill_type);
        stream.write_byte(other.m_respawn_time);
        return send(packet, channel);
    }

    [[nodiscard]] bool is_disconnected() const noexcept
    {
        return m_state == state_type::disconnected;
    }

    [[nodiscard]] bool is_connecting() const noexcept
    {
        return m_state == state_type::connecting;
    }

    [[nodiscard]] bool is_connected() const noexcept
    {
        return m_state == state_type::connected;
    }

    operator bool() const noexcept
    {
        return m_state != state_type::disconnected;
    }

    bool operator==(const connection& other) const noexcept
    {
        return m_id == other.m_id;
    }

    bool operator!=(const connection& other) const noexcept
    {
        return m_id != other.m_id;
    }

  private:
    std::uint8_t m_id{invalid_id};
    state_type   m_state{state_type::disconnected};

  public:
    glm::vec3 m_position;
    glm::vec3 m_orientation;
    color3b   m_color;

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

    bool m_gliding{false};   //!< In the air (airborne)
    bool m_crouching{false}; //!< Crouching state
    bool m_alive{false};     //!< Is player alive
    bool m_can_spawn{false}; //!< Is player allowed to spawn
    bool m_can_break{true};  //!< Is player allowed to break blocks
    bool m_can_kill{true};   //!< Is player allowed to kill players

    std::uint32_t m_kills{0};
    std::uint32_t m_deaths{0};
    std::uint8_t  m_last_kill_killer;
    kill_type     m_last_kill_type;
    std::uint8_t  m_respawn_time{0};
    std::uint8_t  m_clip_ammo;
    std::uint8_t  m_reserve_ammo;
    std::uint8_t  m_intel_captures{0};

    std::string m_name;

    tool_type   m_tool{tool_type::gun};
    team_type   m_team;
    weapon_type m_weapon;

    /**
     * @brief Reset to initial state
     *
     */
    void reset_values()
    {
        m_state          = state_type::disconnected;
        m_position       = {0.F, 0.F, 0.F};
        m_orientation    = {0.F, 0.F, 0.F};
        m_tool           = tool_type::gun;
        m_up             = false;
        m_down           = false;
        m_left           = false;
        m_right          = false;
        m_jump           = false;
        m_crouch         = false;
        m_sneak          = false;
        m_sprint         = false;
        m_primary        = false;
        m_secondary      = false;
        m_crouching      = false;
        m_gliding        = false;
        m_alive          = false;
        m_can_spawn      = false;
        m_can_break      = true;
        m_can_kill       = true;
        m_kills          = 0;
        m_deaths         = 0;
        m_respawn_time   = 0;
        m_intel_captures = 0;
    }
};

} // namespace spadesx
