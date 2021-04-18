/**
 *
 * SpadesX
 *
 */

#pragma once

#include "connection.hxx"
#include "data/enums.hxx"
#include "datastream.hxx"

#include <glm/fwd.hpp>
#include <stdexcept>
#include <string_view>
#include <vector>

namespace spadesx {

/**
 * @brief Broadcasting manager
 *
 */
class connection_manager
{
  public:
    /**
     * @brief Construct a new connection_manager object
     *
     */
    connection_manager() : connection_manager{32}
    {
    }

    /**
     * @brief Construct a new connection_manager object
     *
     * @param max_players Max number of players
     */
    connection_manager(std::uint8_t max_players) : m_max_players{max_players}
    {
        m_connections.reserve(32);
        for (std::uint8_t i = 0; i < 32; ++i) {
            m_connections.emplace_back(i);
        }
    }

    /**
     * @brief Destroy the connection_manager object
     *
     */
    ~connection_manager()
    {
        for (auto& connection : m_connections) {
            if (!connection.is_disconnected()) {
                connection.disconnect(reason_type::unknown);
            }
        }
    }

    /**
     * @brief Get players limit
     *
     * @return Limit
     */
    [[nodiscard]] std::uint8_t get_max_players() const
    {
        return m_max_players;
    }

    /**
     * @brief Check whether server is not full
     *
     * @return true If server is not full
     * @return false If serever is full
     */
    [[nodiscard]] bool available() const
    {
        return m_num_players < m_max_players;
    }

    /**
     * @brief Broadcast same data to multiple connections (except source)
     *
     * @param source Source connection
     * @param data Data
     * @param length Length of data
     * @param unsequenced If true sets unsequenced flag
     * @param channel Channel
     */
    void broadcast(const connection& source,
                   const void*       data,
                   std::size_t       length,
                   bool              unsequenced = false,
                   std::uint8_t      channel     = 0)
    {
        for (auto& connection : m_connections) {
            if (connection == source) {
                continue;
            }
            if (!connection.is_disconnected()) {
                connection.send_packet(data, length, unsequenced, channel);
            }
        }
    }

    /**
     * @brief Broadcast same data to multiple connections
     *
     * @param data Data
     * @param length Length of data
     * @param unsequenced If true sets unsequenced flag
     * @param channel Channel
     */
    void broadcast(const void* data, std::size_t length, bool unsequenced = false, std::uint8_t channel = 0)
    {
        for (auto& connection : m_connections) {
            if (!connection.is_disconnected()) {
                connection.send_packet(data, length, unsequenced, channel);
            }
        }
    }

    /**
     * @brief Broadcast input data
     *
     * @param source Source connection
     * @param unsequenced If true sets unsequenced flag
     * @param channel Channel
     */
    void broadcast_input_data(connection&  source,
                              bool         include_sender = false,
                              bool         unsequenced    = false,
                              std::uint8_t channel        = 0)
    {
        data_stream stream{m_cache, packet::input_data_size};
        source.fill_input_data(stream);
        if (!include_sender) {
            broadcast(source, m_cache, packet::input_data_size, unsequenced, channel);
        } else {
            broadcast(m_cache, packet::input_data_size, unsequenced, channel);
        }
    }

    /**
     * @brief Broadcast weapon input
     *
     * @param source Source connection
     * @param unsequenced If true sets unsequenced flag
     * @param channel Channel
     */
    void broadcast_weapon_input(connection& source, bool unsequenced = false, std::uint8_t channel = 0)
    {
        data_stream stream{m_cache, packet::weapon_input_size};
        source.fill_weapon_input(stream);
        broadcast(source, m_cache, packet::weapon_input_size, unsequenced, channel);
    }

    /**
     * @brief Broadcast create player packet
     *
     * @param connection Player to be created
     * @param unsequenced If true sets unsequenced flag
     * @param channel Channel
     */
    void broadcast_create(connection& connection, bool unsequenced = false, std::uint8_t channel = 0)
    {
        data_stream stream{m_cache, packet::create_player_size};
        auto        size = connection.fill_create_player(stream);
        broadcast(m_cache, size, unsequenced, channel);
    }

    /**
     * @brief Broadcast player left
     *
     * @param connection Connection
     * @param unsequenced If true sets unsequenced flag
     * @param channel Channel
     */
    void broadcast_leave(connection& connection, bool unsequenced = false, std::uint8_t channel = 0)
    {
        data_stream stream{m_cache, packet::player_left_size};
        connection.fill_player_left(stream);
        broadcast(connection, m_cache, packet::player_left_size, unsequenced, channel);
    }

    /**
     * @brief Broadcast kill action
     *
     * @param killer Killer
     * @param victim Victim
     * @param type Kill type
     * @param respawn_time Respawn time
     * @param unsequenced If true sets unsequenced flag
     * @param channel Channel
     */
    void kill_and_broadcast(connection&  killer,
                            connection&  victim,
                            kill_type    type,
                            std::uint8_t respawn_time,
                            bool         unsequenced = false,
                            std::uint8_t channel     = 0)
    {
        victim.set_last_kill(killer.id(), type);
        victim.m_respawn_time = respawn_time;
        victim.reset_death();

        data_stream stream{m_cache, packet::kill_action_size};
        victim.fill_kill_action(stream);
        broadcast(m_cache, packet::kill_action_size, unsequenced, channel);
    }

    /**
     * @brief Throw grenade
     *
     * @param source Source connection
     * @param position Initial position
     * @param velocity Initial velocity
     * @param fuse Fuse time
     * @param unsequenced If true sets unsequenced flag
     * @param channel Channel
     */
    void broadcast_grenade(connection&      source,
                           const glm::vec3& position,
                           const glm::vec3& velocity,
                           float            fuse,
                           bool             unsequenced = false,
                           std::uint8_t     channel     = 0)
    {
        data_stream stream{m_cache, packet::grenade_packet_size};
        source.fill_grenade_packet(stream, position, velocity, fuse);
        broadcast(source, m_cache, packet::grenade_packet_size, unsequenced, channel);
    }

    /**
     * @brief Broadcast block action
     *
     * @param source Source connection
     * @param x The x-coordinate of the block (or grenade)
     * @param y The y-coordinate of the block (or grenade)
     * @param z The z-coordinate of the block (or grenade)
     * @param action Block action
     * @param unsequenced If true sets unsequenced flag
     * @param channel Channel
     */
    void broadcast_block_action(connection&       source,
                                std::uint32_t     x,
                                std::uint32_t     y,
                                std::uint32_t     z,
                                block_action_type action,
                                bool              unsequenced = false,
                                std::uint8_t      channel     = 0)
    {
        data_stream stream{m_cache, packet::block_action_size};
        source.fill_block_action(stream, x, y, z, action);
        broadcast(m_cache, packet::block_action_size, unsequenced, channel);
    }

    /**
     * @brief Broadcast block line
     *
     * @param source Source connection
     * @param start Start position
     * @param end End position
     * @param unsequenced If true sets unsequenced flag
     * @param channel Channel
     */
    void broadcast_block_line(connection&       source,
                              const glm::ivec3& start,
                              const glm::ivec3& end,
                              bool              unsequenced = false,
                              std::uint8_t      channel     = 0)
    {
        data_stream stream{m_cache, packet::block_line_size};
        source.fill_block_line(stream, start, end);
        broadcast(m_cache, packet::block_line_size, unsequenced, channel);
    }

    /**
     * @brief Broadcast set tool
     *
     * @param source Source connection
     * @param unsequenced If true sets unsequenced flag
     * @param channel Channel
     */
    void broadcast_set_tool(connection& source, bool unsequenced = false, std::uint8_t channel = 0)
    {
        data_stream stream{m_cache, packet::set_tool_size};
        source.fill_set_tool(stream);
        broadcast(source, m_cache, packet::set_tool_size, unsequenced, channel);
    }

    /**
     * @brief Broadcast set color
     *
     * @param source Source connection
     * @param unsequenced If true sets unsequenced flag
     * @param channel Channel
     */
    void broadcast_set_color(connection& source, bool unsequenced = false, std::uint8_t channel = 0)
    {
        data_stream stream{m_cache, packet::set_color_size};
        source.fill_set_color(stream);
        broadcast(source, m_cache, packet::set_color_size, unsequenced, channel);
    }

    /**
     * @brief Broadcast weapon reload
     *
     * @param source Source connection
     * @param unsequenced If true sets unsequenced flag
     * @param channel Channel
     */
    void broadcast_weapon_reload(connection& source, bool unsequenced = false, std::uint8_t channel = 0)
    {
        data_stream stream{m_cache, packet::weapon_reload_size};
        source.fill_weapon_reload(stream);
        broadcast(source, m_cache, packet::weapon_reload_size, unsequenced, channel);
    }

    /**
     * @brief Broadcast world update
     *
     * @param unsequenced If true sets unsequenced flag
     * @param channel Channel
     */
    void broadcast_world_update(bool unsequenced = true, std::uint8_t channel = 0)
    {
        data_stream stream{m_cache, packet::world_update_size};
        stream.write_type(packet_type::world_update);
        for (auto& connection : m_connections) {
            stream.write_vec3(connection.m_position);
            stream.write_vec3(connection.m_orientation);
        }
        for (auto& connection : m_connections) {
            if (connection.is_connected()) {
                connection.send_packet(m_cache, packet::world_update_size, unsequenced, channel);
            }
        }
    }

    /**
     * @brief Broadcast restock
     *
     * @param target Target to be restocked
     * @param unsequenced If true sets unsequenced flag
     * @param channel Channel
     */
    void broadcast_restock(connection& target, bool unsequenced = false, std::uint8_t channel = 0)
    {
        data_stream stream{m_cache, packet::restock_size};
        target.fill_restock(stream);
        broadcast(m_cache, packet::restock_size, unsequenced, channel);
    }

    /**
     * @brief Broadcast chat message
     *
     * @param source Source connection
     * @param type Chat type
     * @param message Message
     * @param unsequenced If true sets unsequenced flag
     * @param channel Channel
     */
    void broadcast_message(connection&      source,
                           chat_type        type,
                           std::string_view message,
                           bool             unsequenced = false,
                           std::uint8_t     channel     = 0)
    {
        data_stream stream{m_cache, packet::chat_message_size};
        auto        size = source.fill_chat_message(stream, type, message);

        switch (type) {
            case chat_type::all:
                if (!source.m_muted) {
                    for (auto& connection : m_connections) {
                        if (!connection.is_disconnected() && !connection.m_deaf) {
                            connection.send_packet(m_cache, size, unsequenced, channel);
                        }
                    }
                }
                break;
            case chat_type::team:
                if (!source.m_muted) {
                    for (auto& connection : m_connections) {
                        if (connection.team() != source.team()) {
                            continue;
                        }
                        if (!connection.is_disconnected() && !connection.m_deaf) {
                            connection.send_packet(m_cache, size, unsequenced, channel);
                        }
                    }
                }
            default:
                return; // invalid type
        }
    }

    /**
     * @brief Send system message
     *
     * @param message Message
     * @param unsequenced If true sets unsequenced flag
     * @param channel Channel
     */
    void broadcast_system_message(std::string_view message, bool unsequenced = false, std::uint8_t channel = 0)
    {
        data_stream stream{m_cache, packet::chat_message_size};
        auto        size = fill_system_message(stream, message);
        broadcast(m_cache, size, unsequenced, channel);
    }

    /**
     * @brief Send system message to single connection
     *
     * @param target Target connection
     * @param message Message
     * @param unsequenced If true sets unsequenced flag
     * @param channel Channel
     */
    void
    system_message(connection& target, std::string_view message, bool unsequenced = false, std::uint8_t channel = 0)
    {
        data_stream stream{m_cache, packet::chat_message_size};
        auto        size = fill_system_message(stream, message);
        target.send_packet(m_cache, size, unsequenced, channel);
    }

    /**
     * @brief Fill system mesage
     *
     * @param stream Packet stream
     * @param message Message
     */
    static std::size_t fill_system_message(data_stream& stream, std::string_view message)
    {
        auto size = std::min(message.size(), std::size_t(256));
        stream.write_type(packet_type::chat_message);
        stream.write_byte(33);
        stream.write_type(chat_type::system);
        stream.write_array(message.data(), size);
        return size + 3;
    }

    /**
     * @brief
     *
     * @param source Source connection
     * @param winning If true, then it's winning capture
     * @param unsequenced If true sets unsequenced flag
     * @param channel Channel
     */
    void
    broadcast_intel_capture(const connection& source, bool winning, bool unsequenced = false, std::uint8_t channel = 0)
    {
        data_stream stream{m_cache, packet::intel_capture_size};
        source.fill_intel_capture(stream, winning);
        broadcast(m_cache, packet::intel_capture_size, unsequenced, channel);
    }

    /**
     * @brief Broadcast intel pickup
     *
     * @param source Source connection
     * @param unsequenced If true sets unsequenced flag
     * @param channel Channel
     */
    void broadcast_intel_pickup(const connection& source, bool unsequenced = false, std::uint8_t channel = 0)
    {
        data_stream stream{m_cache, packet::intel_pickup_size};
        source.fill_intel_pickup(stream);
        broadcast(m_cache, packet::intel_pickup_size, unsequenced, channel);
    }

    /**
     * @brief Broadcast intel drop
     *
     * @param source Source connection
     * @param position New intel position
     * @param unsequenced If true sets unsequenced flag
     * @param channel Channel
     */
    void broadcast_intel_drop(const connection& source,
                              const glm::uvec3& position,
                              bool              unsequenced = false,
                              std::uint8_t      channel     = 0)
    {
        data_stream stream{m_cache, packet::intel_drop_size};
        source.fill_intel_drop(stream, position);
        broadcast(m_cache, packet::intel_drop_size, unsequenced, channel);
    }

    /**
     * @brief Broadcasst move object
     *
     * @param object_id Object ID
     * @param team Team
     * @param position Position
     * @param unsequenced If true sets unsequenced flag
     * @param channel Channel
     */
    void broadcast_move_object(std::uint8_t     object_id,
                               team_type        team,
                               const glm::vec3& position,
                               bool             unsequenced = false,
                               std::uint8_t     channel     = 0)
    {
        data_stream stream{m_cache, packet::move_object_size};
        stream.write_type(packet_type::move_object);
        stream.write_byte(object_id);
        stream.write_type(team);
        stream.write_vec3(position);
        broadcast(m_cache, packet::move_object_size, unsequenced, channel);
    }

    /**
     * @brief Assign connection to peer
     *
     * @param peer Peer to be assigned
     * @param connection Connection
     */
    void assign_connection(ENetPeer* peer, connection& connection)
    {
        connection.set_peer(peer);
        peer->data = &connection;
        connection.set_state(state_type::connecting);
        m_num_players++;
    }

    /**
     * @brief Detach connection from peer (reset)
     *
     * @param connection Connection
     */
    void detach_connection(connection& connection)
    {
        connection.reset_values();
        connection.set_state(state_type::disconnected);
        connection.set_peer(nullptr);
        m_num_players--;
    }

    /**
     * @brief Extract connection from peer
     *
     * @param peer Peer
     * @return Reference to connection
     */
    static connection& peer_to_connection(ENetPeer* peer)
    {
        return *reinterpret_cast<connection*>(peer->data);
    }

  protected:
    /**
     * @brief Get next free connection
     *
     * @return Connection
     */
    connection& next_free_connection()
    {
        for (auto& connection : m_connections) {
            if (connection.is_disconnected()) {
                return connection;
            }
        }
        throw std::runtime_error("failed to get next free connection");
    }

    std::uint8_t m_cache[769]; //!< Packet stream buffer

    std::uint8_t            m_max_players;    //!< Maximal number of players
    std::uint8_t            m_num_players{0}; //!< Current number of players
    std::vector<connection> m_connections;    //!< Player connections
};

} // namespace spadesx
