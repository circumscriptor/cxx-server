/**
 *
 * SpadesX
 *
 */

#pragma once

#include "baseconnection.hxx"
#include "enums.hxx"
#include "playerdata.hxx"

#include <cstring>

namespace spadesx {

class connection : public base_connection, public player_data
{
    friend class base_protocol;

  public:
    connection(std::uint8_t id) : base_connection(id)
    {
    }

    connection(const connection&) = default;

    /**
     * @brief Send existing player packet
     *
     * @param other Other connection
     * @param channel Channel
     * @return true On success
     */
    bool send_existing_player(connection& other, std::uint8_t channel = 0)
    {
        ENetPacket* packet = enet_packet_create(nullptr, 12 + m_name.length(), ENET_PACKET_FLAG_RELIABLE);
        data_stream stream = packet;

        stream.write_type(packet_type::existing_player);
        stream.write_byte(other.m_id);
        stream.write_type(other.m_team);
        stream.write_type(other.m_weapon);
        stream.write_type(other.m_tool);
        stream.write_int(other.m_kills);
        stream.write_color3b(other.m_color);
        stream.write_array(other.m_name.data(), m_name.length());
        return send(packet, channel);
    }

    /**
     * @brief Send kill action packet
     *
     * @param other Other connection
     * @param channel Channel
     * @return true On success
     */
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

    /**
     * @brief Send set hp packet
     *
     * @param source Source (enemy or block?)
     * @param weapon
     * @return true
     * @return false
     */
    bool send_set_hp(const glm::vec3& source, bool weapon, std::uint8_t channel = 0)
    {
        ENetPacket* packet = enet_packet_create(nullptr, 15, ENET_PACKET_FLAG_RELIABLE);
        data_stream stream = packet;

        stream.write_type(packet_type::set_hp);
        stream.write_byte(m_health);
        stream.write_byte(weapon ? 1 : 0);
        stream.write_vec3(source);
        return send(packet, channel);
    }

    /**
     * @brief Fill create player packet
     *
     * @param stream Packet stream
     * @return Length of the packet
     */
    std::uint32_t fill_create_player(data_stream& stream) const
    {
        stream.write_type(packet_type::create_player);
        stream.write_byte(m_id);
        stream.write_type(m_weapon);
        stream.write_type(m_team);
        stream.write_vec3(m_position);
        stream.write_array(m_name.data(), m_name.length());
        return 18 + m_name.length();
    }

    /**
     * @brief Fill kill action (from victim's perspective)
     *
     * @param stream Packet stream
     */
    void fill_kill_action(data_stream& stream) const
    {
        stream.write_type(packet_type::kill_action);
        stream.write_byte(m_id);
        stream.write_byte(m_last_kill_killer);
        stream.write_type(m_last_kill_type);
        stream.write_byte(m_respawn_time);
    }

    /**
     * @brief Fill input data
     *
     * @param stream Packet stream
     */
    void fill_input_data(data_stream& stream)
    {
        stream.write_type(packet_type::input_data);
        stream.write_byte(m_id);
        stream.write_byte(get_input_data());
    }

    /**
     * @brief Fill input data
     *
     * @param stream Packet stream
     */
    void fill_weapon_input(data_stream& stream)
    {
        stream.write_type(packet_type::weapon_input);
        stream.write_byte(m_id);
        stream.write_byte(get_weapon_input());
    }

    /**
     * @brief Fill grenade packet
     *
     * @param stream Packet stream
     * @param position Initial grenade position
     * @param velocity Initial grenade velocity
     * @param fuse Grenade fuse time
     */
    void fill_grenade_packet(data_stream& stream, const glm::vec3& position, const glm::vec3& velocity, float fuse)
    {
        stream.write_type(packet_type::grenade_packet);
        stream.write_byte(m_id);
        stream.write_float(fuse);
        stream.write_vec3(position);
        stream.write_vec3(velocity);
    }
};

} // namespace spadesx
