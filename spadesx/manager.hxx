/**
 *
 * SpadesX
 *
 */

#pragma once

#include "command.hxx"
#include "connection.hxx"
#include "data/enums.hxx"

#include <iostream>
#include <stdexcept>
#include <string_view>
#include <vector>

namespace spadesx {

/**
 * @brief Packet cache
 *
 */
class packet_cache
{
  public:
    std::array<std::uint8_t, 3>   m_cache_input_data;
    std::array<std::uint8_t, 3>   m_cache_weapon_input;
    std::array<std::uint8_t, 5>   m_cache_kill_action;
    std::array<std::uint8_t, 32>  m_cache_create_player;
    std::array<std::uint8_t, 769> m_cache_world_update;
    std::array<std::uint8_t, 90>  m_cache_state_data;
    std::array<std::uint8_t, 2>   m_cache_player_left;
    std::array<std::uint8_t, 30>  m_cache_grenade_packet;
    std::array<std::uint8_t, 3>   m_cache_set_tool;
    std::array<std::uint8_t, 5>   m_cache_set_color;
    std::array<std::uint8_t, 15>  m_cache_block_action;
    std::array<std::uint8_t, 4>   m_cache_weapon_reload;
    std::array<std::uint8_t, 258> m_cache_chat_message;
    std::array<std::uint8_t, 2>   m_cache_restock;
};

/**
 * @brief Broadcasting manager
 *
 */
class connection_manager : protected packet_cache
{
  public:
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
    ~connection_manager() = default;

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
     * @brief Broadcast same data to multiple connections (except source) - cached array
     *
     * @tparam N Length of data
     * @param source Source connection
     * @param data Data
     * @param unsequenced If true sets unsequenced flag
     * @param channel Channel
     */
    template<std::size_t N>
    void broadcast(const connection&                  source,
                   const std::array<std::uint8_t, N>& data,
                   bool                               unsequenced = false,
                   std::uint8_t                       channel     = 0)
    {
        broadcast(source, data.data(), data.size(), unsequenced, channel);
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
     * @brief Broadcast same data to multiple connections - cached array
     *
     * @tparam N Length of data
     * @param data Data
     * @param unsequenced If true sets unsequenced flag
     * @param channel Channel
     */
    template<std::size_t N>
    void broadcast(const std::array<std::uint8_t, N>& data, bool unsequenced = false, std::uint8_t channel = 0)
    {
        broadcast(data.data(), data.size(), unsequenced, channel);
    }

    /**
     * @brief Broadcast input data
     *
     * @param source Source connection
     */
    void broadcast_input_data(connection& source)
    {
        data_stream stream{m_cache_input_data};
        source.fill_input_data(stream);
        broadcast(source, m_cache_input_data);
    }

    /**
     * @brief Broadcast weapon input
     *
     * @param source Source connection
     */
    void broadcast_weapon_input(connection& source)
    {
        data_stream output{m_cache_weapon_input};
        source.fill_weapon_input(output);
        broadcast(source, m_cache_weapon_input);
    }

    /**
     * @brief Broadcast kill action
     *
     * @param killer Killer
     * @param victim Victim
     * @param type Kill type
     * @param respawn_time Respawn time
     */
    void kill_and_broadcast(connection& killer, connection& victim, kill_type type, std::uint8_t respawn_time)
    {
        victim.m_last_kill_killer = killer.get_id();
        victim.m_last_kill_type   = type;
        victim.m_respawn_time     = respawn_time;
        victim.reset_death();

        data_stream stream{m_cache_kill_action};
        victim.fill_kill_action(stream);
        broadcast(m_cache_kill_action);
    }

    /**
     * @brief Throw grenade
     *
     * @param source Source connection
     * @param position Initial position
     * @param velocity Initial velocity
     * @param fuse Fuse time
     */
    void broadcast_grenade(connection& source, const glm::vec3& position, const glm::vec3& velocity, float fuse)
    {
        data_stream stream{m_cache_grenade_packet};
        source.fill_grenade_packet(stream, position, velocity, fuse);
        broadcast(source, m_cache_grenade_packet);
    }

    /**
     * @brief Broadcast block action
     *
     * @param source Source connection
     * @param x The x-coordinate of the block (or grenade)
     * @param y The y-coordinate of the block (or grenade)
     * @param z The z-coordinate of the block (or grenade)
     * @param action Block action
     */
    void broadcast_block_action(connection&       source,
                                std::uint32_t     x,
                                std::uint32_t     y,
                                std::uint32_t     z,
                                block_action_type action)
    {
        data_stream stream{m_cache_block_action};
        source.fill_block_action(stream, x, y, z, action);
        broadcast(m_cache_block_action);
    }

    /**
     * @brief Broadcast world update
     *
     */
    void broadcast_world_update()
    {
        data_stream stream{m_cache_world_update};
        stream.write_type(packet_type::world_update);
        for (auto& connection : m_connections) {
            stream.write_vec3(connection.m_position);
            stream.write_vec3(connection.m_orientation);
        }
        for (auto& connection : m_connections) {
            if (connection.is_connected()) {
                connection.send_packet(m_cache_world_update.data(), m_cache_world_update.size(), true);
            }
        }
    }

    /**
     * @brief Broadcast create player packet
     *
     * @param connection Player to be created
     */
    void broadcast_create(connection& connection)
    {
        data_stream stream{m_cache_create_player};
        auto        size = connection.fill_create_player(stream);
        broadcast(m_cache_create_player.data(), size);
    }

    /**
     * @brief Broadcast player left
     *
     * @param connection Connection
     */
    void broadcast_leave(connection& connection)
    {
        data_stream stream(m_cache_player_left);
        stream.write_type(packet_type::player_left);
        stream.write_byte(connection.get_id());
        broadcast(connection, m_cache_player_left);
    }

    /**
     * @brief Broadcast restock
     *
     * @param target Target to be restocked
     */
    void broadcast_restock(connection& target)
    {
        data_stream stream{m_cache_restock};
        target.fill_restock(stream);
        broadcast(m_cache_restock);
    }

    /**
     * @brief Broadcast chat message
     *
     * @param source Source connection
     * @param type Chat type
     * @param message Message
     */
    void broadcast_message(connection& source, chat_type type, std::string_view message)
    {
        data_stream stream{m_cache_chat_message};
        auto        size = source.fill_chat_message(stream, type, message);

        switch (type) {
            case chat_type::all:
                if (!source.m_muted) {
                    for (auto& connection : m_connections) {
                        if (!connection.is_disconnected() && !connection.m_deaf) {
                            connection.send_packet(m_cache_chat_message.data(), size);
                        }
                    }
                }
                break;
            case chat_type::team:
                if (!source.m_muted) {
                    for (auto& connection : m_connections) {
                        if (connection.m_team != source.m_team) {
                            continue;
                        }
                        if (!connection.is_disconnected() && !connection.m_deaf) {
                            connection.send_packet(m_cache_chat_message.data(), size);
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
     */
    void broadcast_system_message(std::string_view message)
    {
        data_stream stream{m_cache_chat_message};
        auto        size = fill_system_message(stream, message);
        broadcast(m_cache_chat_message.data(), size);
    }

    /**
     * @brief Send system message to single connection
     *
     * @param target Target connection
     * @param message Message
     */
    void system_message(connection& target, std::string_view message)
    {
        data_stream stream{m_cache_chat_message};
        auto        size = fill_system_message(stream, message);
        target.send_packet(m_cache_chat_message.data(), size);
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

    std::uint8_t            m_max_players;    //!< Maximal number of players
    std::uint8_t            m_num_players{0}; //!< Current number of players
    std::vector<connection> m_connections;    //!< Player connections
};

/**
 * @brief Register commands
 *
 */
class command_manager
{
  public:
    /**
     * @brief Register command
     *
     * @tparam T Command type
     * @param name Command name
     */
    template<typename T>
    void register_command(const std::string& name, bool enable = true)
    {
        auto it = m_commands.find(name);
        if (it == m_commands.end()) {
            std::shared_ptr<command> shared = std::make_shared<T>();
            if (enable) {
                shared->enable();
            }
            m_commands.insert({name, shared});
        }
    }

    /**
     * @brief Get command
     *
     * @param name Command name
     */
    command& get_command(const std::string& name)
    {
        auto it = m_commands.find(name);
        if (it == m_commands.end()) {
            throw std::runtime_error("command not found");
        }
        return *(it->second.get());
    }

    /**
     * @brief Parse command
     *
     * @param command Command
     * @return true True on success
     */
    bool execute_command(std::string_view command, connection& connection, base_protocol& protocol)
    {
        if (command.empty() || command[0] != '/') {
            return false;
        }

        auto n    = command.find(' ', 1);
        auto name = command.substr(1, n - 1);
        auto size = name.size();
        if (name.back() == 0) {
            size -= 1;
        }

        if (!name.empty()) {
            auto it = m_commands.find(std::string{name.data(), size});
            if (it != m_commands.end()) {
                auto& cmd = *(it->second.get());
                if (cmd.is_enabled()) {
                    auto args = command.substr(n + 1);
                    cmd.on_execute(args, connection, protocol);
                    return true;
                }
            }
        }
        return false;
    }

  private:
    std::unordered_map<std::string, std::shared_ptr<command>> m_commands; //!< Command
};

} // namespace spadesx
