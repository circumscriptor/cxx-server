/**
 * @file connection.hxx
 * @brief This file is part of the experimental SpadesX project
 */

#pragma once

#include "peer.hxx"

namespace spadesx {

/**
 * @brief Connection - player data
 *
 */
class connection : public peer, public player_data
{
  public:

    /**
     * @brief Construct a new connection object
     *
     * @param id Connection ID
     */
    connection(std::uint8_t id)
        : player_data { id }
    {
    }

    /**
     * @brief Construct a new connection object
     *
     */
    connection(const connection &) = default;

    // TODO: Maybe add destructor (and call disconnect?)?

    /**
     * @brief Check whether this connection has valid ID
     *
     * @return true If ID is valid
     */
    [[nodiscard]] bool is_valid() const noexcept
    {
        return m_id != object_id::invalid;
    }

    /**
     * @brief Compare this connections's ID with other's ID
     *
     * @param other Other connection
     * @return true If same
     */
    bool operator==(const connection & other) const noexcept
    {
        return m_id == other.m_id;
    }

    /**
     * @brief Compare this connections's ID with other's ID
     *
     * @param other Other connection
     * @return true If different
     */
    bool operator!=(const connection & other) const noexcept
    {
        return m_id != other.m_id;
    }

    /**
     * @brief Send map start packet
     *
     * @param size Size of the map
     * @param channel Channel
     * @return true On success
     */
    bool send_map_start(std::uint32_t size, std::uint8_t channel = 0)
    {
        packet packet(nullptr, 5, false);
        auto   stream = packet.stream();
        stream.write_type(packet_type::map_start);
        stream.write_int(size);
        return send(packet, channel);
    }

    /**
     * @brief Send map chunk
     *
     * @param chunk Pointer to the memory location containing chunk
     * @param size Size of the chunk
     * @param unsequenced Unsequenced flag
     * @param channel Channel
     * @return true On success
     */
    bool send_map_chunk(void * chunk, std::uint32_t size, bool unsequenced = false, std::uint8_t channel = 0)
    {
        packet packet(nullptr, size + 1, unsequenced);
        auto   stream = packet.stream();
        stream.write_type(packet_type::map_chunk);
        stream.write_array(chunk, size);
        return send(packet, channel);
    }

    /**
     * @brief Send generic packet
     *
     * @param data Pointer to the memory location containing packet data
     * @param size Size of the packet
     * @param unsequenced Unsequenced flag
     * @param channel Channel
     * @return true On success
     */
    bool send_packet(const void * data, std::size_t size, bool unsequenced = true, std::uint8_t channel = 0)
    {
        return send({ data, size, unsequenced }, channel);
    }

    /**
     * @brief Send generic packet
     *
     * @tparam N Size of the packet
     * @param data Pointer to the memory location containing packet data
     * @param unsequenced Unsequenced flag
     * @param channel Channel
     * @return true On success
     */
    template <std::size_t N>
    bool send_packet(const std::array<std::uint8_t, N> & data, bool unsequenced = true, std::uint8_t channel = 0)
    {
        return send_packet(data.data(), data.size(), unsequenced, channel);
    }

    /**
     * @brief Disconnect
     *
     * @param reason Disconnection reason
     */
    void disconnect(reason_type reason)
    {
        peer::disconnect(static_cast<std::uint32_t>(reason));
        set_state(state_type::disconnected);
    }

    /**
     * @brief Check whether connection state is disconnected
     *
     * @return true If state is disconnected
     */
    [[nodiscard]] bool is_disconnected() const noexcept
    {
        return m_state == state_type::disconnected;
    }

    /**
     * @brief Check whether connection state is connecting
     *
     * @return true If state is connecting
     */
    [[nodiscard]] bool is_connecting() const noexcept
    {
        return m_state == state_type::connecting;
    }

    /**
     * @brief Check whether connection state is connected
     *
     * @return true If state is connected
     */
    [[nodiscard]] bool is_connected() const noexcept
    {
        return m_state == state_type::connected;
    }

    /**
     * @brief Set connection state
     *
     * @param state Connection state
     */
    void set_state(state_type state) noexcept
    {
        m_state = state;
    }

    /**
     * @brief Send existing player packet
     *
     * @param other Other connection
     * @param channel Channel
     * @return true On success
     */
    bool send_existing_player(connection & other, std::uint8_t channel = 0)
    {
        packet packet(nullptr, 12 + other.m_name_length, false);
        auto   stream = packet.stream();
        other.fill_existing_player(stream);
        return send(packet, channel);
    }

    /**
     * @brief Send kill action packet
     *
     * @param other Other connection
     * @param channel Channel
     * @return true On success
     */
    bool send_kill_action(connection & other, std::uint8_t channel = 0)
    {
        packet packet(nullptr, packet::kill_action_size, false);
        auto   stream = packet.stream();
        other.fill_kill_action(stream);
        return send(packet, channel);
    }

    /**
     * @brief Send set hp packet
     *
     * @param source Source (enemy or block?)
     * @param weapon If true, source of the damage is a weapon
     * @param channel Channel
     * @return true on success
     */
    bool send_set_hp(const glm::vec3 & source, bool weapon, std::uint8_t channel = 0)
    {
        packet packet(nullptr, packet::set_hp_size, false);
        auto   stream = packet.stream();
        fill_set_hp(stream, source, weapon);
        return send(packet, channel);
    }

    /**
     * @brief Set hp packet
     *
     * @param stream Packet stream
     * @param source Location of the source of the damage
     * @param weapon Was weapon used?
     */
    void fill_set_hp(data_stream & stream, const glm::vec3 & source, bool weapon)
    {
        stream.write_type(packet_type::set_hp);
        stream.write_byte(m_health);
        stream.write_byte(weapon ? 1 : 0);
        stream.write_vec3(source);
    }

    /**
     * @brief Fill existing player packet
     *
     * @param stream Packet stream
     * @return Packet size
     */
    std::uint32_t fill_existing_player(data_stream & stream) const
    {
        stream.write_type(packet_type::existing_player);
        stream.write_byte(m_id);
        stream.write_type(m_team);
        stream.write_type(m_weapon);
        stream.write_type(m_tool);
        stream.write_int(m_score.kills());
        stream.write_color3b(m_color);
        stream.write_array(m_name, m_name_length);
        return 12 + m_name_length;
    }

    /**
     * @brief Fill create player packet
     *
     * @param stream Packet stream
     * @return Length of the packet
     */
    std::uint32_t fill_create_player(data_stream & stream) const
    {
        stream.write_type(packet_type::create_player);
        stream.write_byte(m_id);
        stream.write_type(m_weapon);
        stream.write_type(m_team);
        stream.write_vec3(m_position);
        stream.write_array(m_name, m_name_length);
        return 16 + m_name_length;
    }

    /**
     * @brief Fill player left packet
     *
     * @param stream Packet stream
     */
    void fill_player_left(data_stream & stream) const
    {
        stream.write_type(packet_type::player_left);
        stream.write_byte(m_id);
    }

    /**
     * @brief Fill kill action (from victim's perspective)
     *
     * @param stream Packet stream
     */
    void fill_kill_action(data_stream & stream) const
    {
        stream.write_type(packet_type::kill_action);
        stream.write_byte(m_id);
        stream.write_byte(m_last_kill_killer);
        stream.write_type(m_last_kill_type);
        stream.write_byte(m_respawn_time);
    }

    /**
     * @brief Fill set tool
     *
     * @param stream Packet stream
     */
    void fill_set_tool(data_stream & stream) const
    {
        stream.write_type(packet_type::set_tool);
        stream.write_byte(m_id);
        stream.write_type(m_tool);
    }

    /**
     * @brief Fill set color
     *
     * @param stream Packet stream
     */
    void fill_set_color(data_stream & stream) const
    {
        stream.write_type(packet_type::set_color);
        stream.write_byte(m_id);
        stream.write_color3b(m_color);
    }

    /**
     * @brief Fill input data
     *
     * @param stream Packet stream
     */
    void fill_input_data(data_stream & stream) const
    {
        stream.write_type(packet_type::input_data);
        stream.write_byte(m_id);
        stream.write_byte(input_to_byte());
    }

    /**
     * @brief Fill input data
     *
     * @param stream Packet stream
     */
    void fill_weapon_input(data_stream & stream) const
    {
        stream.write_type(packet_type::weapon_input);
        stream.write_byte(m_id);
        stream.write_byte(weapon_input_to_byte());
    }

    /**
     * @brief Fill grenade packet
     *
     * @param stream Packet stream
     * @param position Initial grenade position
     * @param velocity Initial grenade velocity
     * @param fuse Grenade fuse time
     */
    void fill_grenade_packet(data_stream & stream, const glm::vec3 & position, const glm::vec3 & velocity, float fuse) const
    {
        stream.write_type(packet_type::grenade_packet);
        stream.write_byte(m_id);
        stream.write_float(fuse);
        stream.write_vec3(position);
        stream.write_vec3(velocity);
    }

    /**
     * @brief Fill block action packet
     *
     * @param stream Packet stream
     * @param x The x-coordinate of the block (or grenade)
     * @param y The y-coordinate of the block (or grenade)
     * @param z The z-coordinate of the block (or grenade)
     * @param action Block action
     */
    void fill_block_action(data_stream & stream, std::uint32_t x, std::uint32_t y, std::uint32_t z, block_action_type action) const
    {
        stream.write_type(packet_type::block_action);
        stream.write_byte(m_id);
        stream.write_type(action);
        stream.write_int(x);
        stream.write_int(y);
        stream.write_int(z);
    }

    /**
     * @brief Fill block line packet
     *
     * @param stream Packet stream
     * @param start Start position
     * @param end End position
     */
    void fill_block_line(data_stream & stream, const glm::uvec3 & start, const glm::uvec3 & end) const
    {
        stream.write_type(packet_type::block_line);
        stream.write_byte(m_id);
        stream.write_uvec3(start);
        stream.write_uvec3(end);
    }

    /**
     * @brief Fill weapon reload packet
     *
     * @param stream Packet stream
     */
    void fill_weapon_reload(data_stream & stream) const
    {
        stream.write_type(packet_type::weapon_reload);
        stream.write_byte(m_id);
        stream.write_byte(m_clip_ammo);
        stream.write_byte(m_reserve_ammo);
    }

    /**
     * @brief Fill chat message packet
     *
     * @param stream Packet stream
     * @param type Chat type
     * @param message Message
     */
    std::size_t fill_chat_message(data_stream & stream, chat_type type, std::string_view message) const
    {
        auto size = std::min(message.size(), std::size_t(256));
        stream.write_type(packet_type::chat_message);
        stream.write_byte(m_id);
        stream.write_type(type);
        stream.write_array(message.data(), size);
        return size + 3;
    }

    /**
     * @brief Fill restock packet
     *
     * @param stream Packet stream
     */
    void fill_restock(data_stream & stream) const
    {
        stream.write_type(packet_type::restock);
        stream.write_byte(m_id);
    }

    /**
     * @brief Fill intel capture packet
     *
     * @param stream Packet stream
     * @param winning If true, then it's winning capture
     */
    void fill_intel_capture(data_stream & stream, bool winning) const
    {
        stream.write_type(packet_type::intel_capture);
        stream.write_byte(m_id);
        stream.write_byte(winning ? 1 : 0);
    }

    /**
     * @brief Fill intel pickup packet
     *
     * @param stream Packet stream
     */
    void fill_intel_pickup(data_stream & stream) const
    {
        stream.write_type(packet_type::intel_pickup);
        stream.write_byte(m_id);
    }

    /**
     * @brief Fill intel drop
     *
     * @param stream Packet stream
     * @param position New position
     */
    void fill_intel_drop(data_stream & stream, const glm::uvec3 & position) const
    {
        stream.write_type(packet_type::intel_drop);
        stream.write_byte(m_id);
        stream.write_uvec3(position);
    }

  protected:

    state_type m_state { state_type::disconnected }; //!< Connection state (used by protocol)
};

} // namespace spadesx
